// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package client

import (
	"context"
	"fmt"
	"net"
	"strings"
	"testing"

	"google3/base/go/log"
	lvagpb "google3/google/cloud/visionai/v1_lva_go_grpc_proto"
	lvapb "google3/google/cloud/visionai/v1_lva_go_proto"
	lropb "google3/google/longrunning/operations_go_proto"
	lro "google3/google/longrunning/operations_grpc_go"

	"google3/third_party/golang/cmp/cmp"
	"google3/third_party/golang/grpc/codes/codes"
	"google3/third_party/golang/grpc/grpc"
	"google3/third_party/golang/grpc/status/status"
	"google3/third_party/golang/protobuf/v2/testing/protocmp/protocmp"
)

var (
	testParent    = "projects/test-project/locations/test-locations/clusters/test-cluster"
	testParentPrj = "projects/test-project/locations/test-locations"
)

type fakeLVAService struct {
	lvagpb.UnimplementedLiveVideoAnalyticsServer
	lro.UnimplementedOperationsServer

	analyses        []*lvapb.Analysis
	processes       []*lvapb.Process
	operators       []*lvapb.Operator
	publicOperators []*lvapb.Operator
}

func (s *fakeLVAService) ListAnalyses(ctx context.Context, req *lvapb.ListAnalysesRequest) (*lvapb.ListAnalysesResponse, error) {
	anas := make([]*lvapb.Analysis, 0)
	for _, a := range s.analyses {
		if strings.HasPrefix(a.GetName(), req.GetParent()) {
			anas = append(anas, a)
		}
	}
	return lvapb.ListAnalysesResponse_builder{Analyses: anas}.Build(), nil
}

func (s *fakeLVAService) GetAnalysis(ctx context.Context, req *lvapb.GetAnalysisRequest) (*lvapb.Analysis, error) {
	for _, a := range s.analyses {
		if a.GetName() == req.GetName() {
			return a, nil
		}
	}
	return nil, status.Error(codes.NotFound, "not found")
}

func (s *fakeLVAService) GetOperator(ctx context.Context, req *lvapb.GetOperatorRequest) (*lvapb.Operator, error) {
	for _, a := range s.operators {
		if a.GetName() == req.GetName() {
			return a, nil
		}
	}
	return nil, status.Error(codes.NotFound, "not found")
}

func (s *fakeLVAService) CreateAnalysis(ctx context.Context, req *lvapb.CreateAnalysisRequest) (*lropb.Operation, error) {
	ana := req.GetAnalysis()
	ana.SetName(fmt.Sprintf("%s/analyses/%s", req.GetParent(), req.GetAnalysisId()))
	s.analyses = append(s.analyses, ana)
	return &lropb.Operation{
		Name: "foo",
	}, nil
}

func (s *fakeLVAService) DeleteAnalysis(ctx context.Context, req *lvapb.DeleteAnalysisRequest) (*lropb.Operation, error) {
	for i, a := range s.analyses {
		if a.GetName() == req.GetName() {
			s.analyses = append(s.analyses[:i], s.analyses[i+1:]...)
		}
	}
	return &lropb.Operation{}, nil
}

func (s *fakeLVAService) ListProcesses(ctx context.Context, req *lvapb.ListProcessesRequest) (*lvapb.ListProcessesResponse, error) {
	procs := make([]*lvapb.Process, 0)
	for _, p := range s.processes {
		if strings.HasPrefix(p.GetName(), req.GetParent()) {
			procs = append(procs, p)
		}
	}
	return lvapb.ListProcessesResponse_builder{Processes: procs}.Build(), nil
}

func (s *fakeLVAService) GetProcess(ctx context.Context, req *lvapb.GetProcessRequest) (*lvapb.Process, error) {
	for _, p := range s.processes {
		if p.GetName() == req.GetName() {
			return p, nil
		}
	}
	return nil, status.Error(codes.NotFound, "not found")
}

func (s *fakeLVAService) CreateProcess(ctx context.Context, req *lvapb.CreateProcessRequest) (*lropb.Operation, error) {
	proc := req.GetProcess()
	proc.SetName(fmt.Sprintf("%s/processes/%s", req.GetParent(), req.GetProcessId()))
	s.processes = append(s.processes, proc)
	return &lropb.Operation{
		Name: "foo",
	}, nil
}

func (s *fakeLVAService) DeleteProcess(ctx context.Context, req *lvapb.DeleteProcessRequest) (*lropb.Operation, error) {
	for i, p := range s.processes {
		if p.GetName() == req.GetName() {
			s.processes = append(s.processes[:i], s.processes[i+1:]...)
		}
	}
	return &lropb.Operation{}, nil
}

func (s *fakeLVAService) BatchRunProcess(ctx context.Context, req *lvapb.BatchRunProcessRequest) (*lropb.Operation, error) {
	for _, createProcessRequest := range req.GetRequests() {
		proc := createProcessRequest.GetProcess()
		proc.SetName(fmt.Sprintf("%s/processes/%s", req.GetParent(), createProcessRequest.GetProcessId()))
		s.processes = append(s.processes, proc)
	}
	return &lropb.Operation{}, nil
}

func (s *fakeLVAService) ListOperators(ctx context.Context, req *lvapb.ListOperatorsRequest) (*lvapb.ListOperatorsResponse, error) {
	ops := make([]*lvapb.Operator, 0)
	for _, p := range s.operators {
		if strings.HasPrefix(p.GetName(), req.GetParent()) {
			ops = append(ops, p)
		}
	}
	return lvapb.ListOperatorsResponse_builder{Operators: ops}.Build(), nil
}

func (s *fakeLVAService) ListPublicOperators(ctx context.Context, req *lvapb.ListPublicOperatorsRequest) (*lvapb.ListPublicOperatorsResponse, error) {
	return lvapb.ListPublicOperatorsResponse_builder{Operators: s.publicOperators}.Build(), nil
}

func (s *fakeLVAService) DeleteOperator(ctx context.Context, req *lvapb.DeleteOperatorRequest) (*lropb.Operation, error) {
	for i, p := range s.operators {
		if p.GetName() == req.GetName() {
			s.operators = append(s.operators[:i], s.operators[i+1:]...)
		}
	}
	return &lropb.Operation{}, nil
}

func (s *fakeLVAService) GetOperation(ctx context.Context, req *lropb.GetOperationRequest) (*lropb.Operation, error) {
	return &lropb.Operation{
		Done: true,
	}, nil
}

func TestListLiveVideoAnalyticsAnalyses(t *testing.T) {
	tests := []struct {
		name     string
		analyses []*lvapb.Analysis
		parent   string
		want     []*lvapb.Analysis
	}{
		{
			name:     "successfully list empty analyses",
			analyses: []*lvapb.Analysis{},
			parent:   testParent,
			want:     []*lvapb.Analysis{},
		},
		{
			name: "successfully list analyses",
			analyses: []*lvapb.Analysis{
				lvapb.Analysis_builder{
					Name: fmt.Sprintf("%s/analyses/analysis-1", testParent),
				}.Build(),
				lvapb.Analysis_builder{
					Name: fmt.Sprintf("%s/analyses/analysis-2", testParent),
				}.Build(),
			},
			parent: testParent,
			want: []*lvapb.Analysis{
				lvapb.Analysis_builder{
					Name: fmt.Sprintf("%s/analyses/analysis-1", testParent),
				}.Build(),
				lvapb.Analysis_builder{
					Name: fmt.Sprintf("%s/analyses/analysis-2", testParent),
				}.Build(),
			},
		},
		{
			name: "successfully partially list analyses",
			analyses: []*lvapb.Analysis{
				lvapb.Analysis_builder{
					Name: fmt.Sprintf("%s/analyses/analysis-1", testParent),
				}.Build(),
				lvapb.Analysis_builder{
					Name: fmt.Sprintf("%s/analyses/analysis-2", "foo"),
				}.Build(),
			},
			parent: testParent,
			want: []*lvapb.Analysis{
				lvapb.Analysis_builder{
					Name: fmt.Sprintf("%s/analyses/analysis-1", testParent),
				}.Build(),
			},
		},
	}

	for _, test := range tests {
		t.Run(test.name, func(t *testing.T) {
			manager := mustCreateManager(managerOptions{analyses: test.analyses})
			got, err := manager.ListAnalyses(test.parent)
			if err != nil {
				t.Fatalf("want nil, got error %v", err)
			}
			if len(got) != len(test.want) {
				t.Errorf("want %d analyses, got %d analyses", len(test.want), len(got))
			}
			for id := range test.want {
				if diff := cmp.Diff(got[id], test.want[id], protocmp.Transform()); diff != "" {
					t.Errorf("want %s, got %s, (-want +got)\n %s", test.want[id], got[id], diff)
				}
			}
		})
	}
}

func TestGetLiveVideoAnalyticsAnalysis(t *testing.T) {
	tests := []struct {
		name     string
		analyses []*lvapb.Analysis
		analysis string
		want     *lvapb.Analysis
		wantErr  bool
	}{
		{
			name:     "analysis not found",
			analyses: []*lvapb.Analysis{},
			analysis: testParent + "/analyses/analysis-1",
			wantErr:  true,
		},
		{
			name: "successfully get analysis",
			analyses: []*lvapb.Analysis{
				lvapb.Analysis_builder{
					Name: fmt.Sprintf("%s/analyses/analysis-1", testParent),
				}.Build(),
				lvapb.Analysis_builder{
					Name: fmt.Sprintf("%s/analyses/analysis-2", testParent),
				}.Build(),
			},
			analysis: testParent + "/analyses/analysis-1",
			want: lvapb.Analysis_builder{
				Name: fmt.Sprintf("%s/analyses/analysis-1", testParent),
			}.Build(),
		},
	}

	for _, test := range tests {
		t.Run(test.name, func(t *testing.T) {
			manager := mustCreateManager(managerOptions{analyses: test.analyses})
			got, err := manager.GetAnalysis(test.analysis)
			if test.wantErr {
				if err == nil {
					t.Fatal("want error, got nil")
				}
			} else {
				if err != nil {
					t.Fatalf("want nil, got error %v", err)
				}
				if diff := cmp.Diff(got, test.want, protocmp.Transform()); diff != "" {
					t.Errorf("want %v, got %v, (-want +got)\n %s", test.want, got, diff)
				}
			}
		})
	}
}

func TestCreateLiveVideoAnalyticsAnalysis(t *testing.T) {
	analyses := []*lvapb.Analysis{
		lvapb.Analysis_builder{
			Name: fmt.Sprintf("%s/analyses/analysis-1", testParent),
		}.Build(),
	}
	analysis := lvapb.Analysis_builder{}.Build()
	manager := mustCreateManager(managerOptions{analyses: analyses})
	_, err := manager.GetAnalysis(fmt.Sprintf("%s/analyses/analysis-2", testParent))
	if !is(err, codes.NotFound) {
		t.Fatalf("want NotFound, got %v", err)
	}
	err = manager.CreateAnalysis(testParent, "analysis-2", analysis)
	if err != nil {
		t.Fatalf("want nil, got error %v", err)
	}
	_, err = manager.GetAnalysis(fmt.Sprintf("%s/analyses/analysis-2", testParent))
	if err != nil {
		t.Fatalf("want nil, got error %v", err)
	}
}

func TestDeleteLiveVideoAnalyticsAnalysis(t *testing.T) {
	analyses := []*lvapb.Analysis{
		lvapb.Analysis_builder{
			Name: fmt.Sprintf("%s/analyses/analysis-1", testParent),
		}.Build(),
		lvapb.Analysis_builder{
			Name: fmt.Sprintf("%s/analyses/analysis-2", testParent),
		}.Build(),
	}
	manager := mustCreateManager(managerOptions{analyses: analyses})
	_, err := manager.GetAnalysis(fmt.Sprintf("%s/analyses/analysis-1", testParent))
	if err != nil {
		t.Fatalf("want nil, got %v", err)
	}

	err = manager.DeleteAnalysis(fmt.Sprintf("%s/analyses/analysis-1", testParent))
	if err != nil {
		t.Fatalf("want nil, got error %v", err)
	}

	_, err = manager.GetAnalysis(fmt.Sprintf("%s/analyses/analysis-1", testParent))
	if !is(err, codes.NotFound) {
		t.Fatalf("want NotFound, got %v", err)
	}
}

type managerOptions struct {
	analyses        []*lvapb.Analysis
	processes       []*lvapb.Process
	operators       []*lvapb.Operator
	publicOperators []*lvapb.Operator
}

func mustCreateManager(options managerOptions) Interface {
	listener, err := net.Listen("tcp", ":0")
	if err != nil {
		log.Fatal(err)
	}
	s := grpc.NewServer()

	server := &fakeLVAService{
		analyses:        options.analyses,
		processes:       options.processes,
		operators:       options.operators,
		publicOperators: options.publicOperators,
	}
	lvagpb.RegisterLiveVideoAnalyticsServer(s, server)
	lro.RegisterOperationsServer(s, server)

	go func() {
		if err := s.Serve(listener); err != nil {
			log.Fatal(err)
		}
	}()

	manager, err := NewTestManager(listener.Addr().String())
	if err != nil {
		log.Fatal(err)
	}
	return manager
}

func TestListLiveVideoAnalyticsOperators(t *testing.T) {
	tests := []struct {
		name      string
		operators []*lvapb.Operator
		parent    string
		want      []*lvapb.Operator
	}{
		{
			name:   "successfully list operators",
			parent: testParentPrj,
			operators: []*lvapb.Operator{
				lvapb.Operator_builder{
					Name: fmt.Sprintf("%s/operators/operator-1", testParentPrj),
				}.Build(),
			},
			want: []*lvapb.Operator{
				lvapb.Operator_builder{
					Name: fmt.Sprintf("%s/operators/operator-1", testParentPrj),
				}.Build(),
			},
		},
	}

	for _, test := range tests {
		t.Run(test.name, func(t *testing.T) {
			manager := mustCreateManager(managerOptions{operators: test.operators})
			got, err := manager.ListOperators(test.parent)
			if err != nil {
				t.Fatalf("want nil, got error %v", err)
			}
			if len(got) != len(test.want) {
				t.Fatalf("want %d operators, got %d operators", len(test.want), len(got))
			}
			for id := range test.want {
				if diff := cmp.Diff(got[id], test.want[id], protocmp.Transform()); diff != "" {
					t.Errorf("want %s, got %s, (-want +got)\n %s", test.want[id], got[id], diff)
				}
			}
		})
	}
}

func TestListLiveVideoAnalyticsPublicOperators(t *testing.T) {
	tests := []struct {
		name            string
		publicOperators []*lvapb.Operator
		parent          string
		want            []*lvapb.Operator
	}{
		{
			name:   "successfully list operators",
			parent: testParentPrj,
			publicOperators: []*lvapb.Operator{
				lvapb.Operator_builder{
					Name: fmt.Sprintf("%s/operators/operator-2", testParentPrj),
				}.Build(),
			},
			want: []*lvapb.Operator{
				lvapb.Operator_builder{
					Name: fmt.Sprintf("%s/operators/operator-2", testParentPrj),
				}.Build(),
			},
		},
	}

	for _, test := range tests {
		t.Run(test.name, func(t *testing.T) {
			manager := mustCreateManager(managerOptions{publicOperators: test.publicOperators})
			got, err := manager.ListPublicOperators(test.parent)
			if err != nil {
				t.Fatalf("want nil, got error %v", err)
			}
			if len(got) != len(test.want) {
				t.Fatalf("want %d operators, got %d operators", len(test.want), len(got))
			}
			for id := range test.want {
				if diff := cmp.Diff(got[id], test.want[id], protocmp.Transform()); diff != "" {
					t.Errorf("want %s, got %s, (-want +got)\n %s", test.want[id], got[id], diff)
				}
			}
		})
	}
}

func TestListLiveVideoAnalyticsProcesses(t *testing.T) {
	tests := []struct {
		name      string
		processes []*lvapb.Process
		parent    string
		want      []*lvapb.Process
	}{
		{
			name:      "successfully list empty processes",
			processes: []*lvapb.Process{},
			parent:    testParent,
			want:      []*lvapb.Process{},
		},
		{
			name: "successfully list processes",
			processes: []*lvapb.Process{
				lvapb.Process_builder{
					Name: fmt.Sprintf("%s/processes/process-1", testParent),
				}.Build(),
				lvapb.Process_builder{
					Name: fmt.Sprintf("%s/processes/process-2", testParent),
				}.Build(),
			},
			parent: testParent,
			want: []*lvapb.Process{
				lvapb.Process_builder{
					Name: fmt.Sprintf("%s/processes/process-1", testParent),
				}.Build(),
				lvapb.Process_builder{
					Name: fmt.Sprintf("%s/processes/process-2", testParent),
				}.Build(),
			},
		},
		{
			name: "successfully partially list processes",
			processes: []*lvapb.Process{
				lvapb.Process_builder{
					Name: fmt.Sprintf("%s/processes/process-1", testParent),
				}.Build(),
				lvapb.Process_builder{
					Name: fmt.Sprintf("%s/processes/process-2", "foo"),
				}.Build(),
			},
			parent: testParent,
			want: []*lvapb.Process{
				lvapb.Process_builder{
					Name: fmt.Sprintf("%s/processes/process-1", testParent),
				}.Build(),
			},
		},
	}

	for _, test := range tests {
		t.Run(test.name, func(t *testing.T) {

			manager := mustCreateManager(managerOptions{processes: test.processes})
			got, err := manager.ListProcesses(test.parent)
			if err != nil {
				t.Fatalf("want nil, got error %v", err)
			}
			if len(got) != len(test.want) {
				t.Errorf("want %d processes, got %d processes", len(test.want), len(got))
			}
			for id := range test.want {
				if diff := cmp.Diff(got[id], test.want[id], protocmp.Transform()); diff != "" {
					t.Errorf("want %s, got %s, (-want +got)\n %s", test.want[id], got[id], diff)
				}
			}
		})
	}
}

func TestGetLiveVideoAnalyticsProcess(t *testing.T) {
	tests := []struct {
		name      string
		processes []*lvapb.Process
		process   string
		want      *lvapb.Process
		wantErr   bool
	}{
		{
			name:      "process not found",
			processes: []*lvapb.Process{},
			process:   testParent + "/processes/process-1",
			wantErr:   true,
		},
		{
			name: "successfully get process",
			processes: []*lvapb.Process{
				lvapb.Process_builder{
					Name: fmt.Sprintf("%s/processes/process-1", testParent),
				}.Build(),
				lvapb.Process_builder{
					Name: fmt.Sprintf("%s/processes/process-2", testParent),
				}.Build(),
			},
			process: testParent + "/processes/process-1",
			want: lvapb.Process_builder{
				Name: fmt.Sprintf("%s/processes/process-1", testParent),
			}.Build(),
		},
	}

	for _, test := range tests {
		t.Run(test.name, func(t *testing.T) {
			manager := mustCreateManager(managerOptions{processes: test.processes})
			got, err := manager.GetProcess(test.process)
			if test.wantErr {
				if err == nil {
					t.Fatal("want error, got nil")
				}
			} else {
				if err != nil {
					t.Fatalf("want nil, got error %v", err)
				}
				if diff := cmp.Diff(got, test.want, protocmp.Transform()); diff != "" {
					t.Errorf("want %v, got %v, (-want +got)\n %s", test.want, got, diff)
				}
			}
		})
	}
}

func TestCreateLiveVideoAnalyticsProcess(t *testing.T) {
	processes := []*lvapb.Process{
		lvapb.Process_builder{
			Name: fmt.Sprintf("%s/processes/process-1", testParent),
		}.Build(),
	}
	process := lvapb.Process_builder{}.Build()
	manager := mustCreateManager(managerOptions{processes: processes})
	_, err := manager.GetProcess(fmt.Sprintf("%s/processes/process-2", testParent))
	if !is(err, codes.NotFound) {
		t.Fatalf("want NotFound, got %v", err)
	}
	err = manager.CreateProcess(testParent, "process-2", process)
	if err != nil {
		t.Fatalf("want nil, got error %v", err)
	}
	_, err = manager.GetProcess(fmt.Sprintf("%s/processes/process-2", testParent))
	if err != nil {
		t.Fatalf("want nil, got error %v", err)
	}
}

func TestDeleteLiveVideoAnalyticsProcess(t *testing.T) {
	processes := []*lvapb.Process{
		lvapb.Process_builder{
			Name: fmt.Sprintf("%s/processes/process-1", testParent),
		}.Build(),
		lvapb.Process_builder{
			Name: fmt.Sprintf("%s/processes/process-2", testParent),
		}.Build(),
	}
	manager := mustCreateManager(managerOptions{processes: processes})
	_, err := manager.GetProcess(fmt.Sprintf("%s/processes/process-1", testParent))
	if err != nil {
		t.Fatalf("want nil, got %v", err)
	}

	err = manager.DeleteProcess(fmt.Sprintf("%s/processes/process-1", testParent))
	if err != nil {
		t.Fatalf("want nil, got error %v", err)
	}

	_, err = manager.GetProcess(fmt.Sprintf("%s/processes/process-1", testParent))
	if !is(err, codes.NotFound) {
		t.Fatalf("want NotFound, got %v", err)
	}
}

func TestDeleteLiveVideoAnalyticsOperator(t *testing.T) {
	operators := []*lvapb.Operator{
		lvapb.Operator_builder{
			Name: fmt.Sprintf("%s/operators/operator-1", testParentPrj),
		}.Build(),
		lvapb.Operator_builder{
			Name: fmt.Sprintf("%s/operators/operator-2", testParentPrj),
		}.Build(),
	}
	manager := mustCreateManager(managerOptions{operators: operators})
	_, err := manager.GetOperator(fmt.Sprintf("%s/operators/operator-1", testParentPrj))
	if err != nil {
		t.Fatalf("want nil, got %v", err)
	}

	err = manager.DeleteOperator(fmt.Sprintf("%s/operators/operator-1", testParentPrj))
	if err != nil {
		t.Fatalf("want nil, got error %v", err)
	}

	_, err = manager.GetOperator(fmt.Sprintf("%s/operators/operator-1", testParentPrj))
	if !is(err, codes.NotFound) {
		t.Fatalf("want NotFound, got %v", err)
	}
}

func TestBatchRunLiveVideoAnalyticsProcess(t *testing.T) {
	processes := []*lvapb.Process{
		lvapb.Process_builder{
			Name: fmt.Sprintf("%s/processes/process-1", testParent),
		}.Build(),
	}
	manager := mustCreateManager(managerOptions{processes: processes})
	for i := 2; i < 4; i++ {
		_, err := manager.GetProcess(fmt.Sprintf("%s/processes/process-%d", testParent, i))
		if !is(err, codes.NotFound) {
			t.Fatalf("want NotFound, got %v", err)
		}
	}

	createProcessRequests := []*lvapb.CreateProcessRequest{
		lvapb.CreateProcessRequest_builder{
			Parent:    testParent,
			ProcessId: "process-2",
			Process:   lvapb.Process_builder{}.Build(),
		}.Build(),
		lvapb.CreateProcessRequest_builder{
			Parent:    testParent,
			ProcessId: "process-3",
			Process:   lvapb.Process_builder{}.Build(),
		}.Build(),
	}
	_, err := manager.BatchRunProcess(testParent, createProcessRequests)
	if err != nil {
		t.Fatalf("want nil, got error %v", err)
	}

	for i := 2; i < 4; i++ {
		_, err := manager.GetProcess(fmt.Sprintf("%s/processes/process-%d", testParent, i))
		if err != nil {
			t.Fatalf("want nil, got error %v", err)
		}
	}
}

func is(err error, code codes.Code) bool {
	st, ok := status.FromError(err)
	if !ok {
		return false
	}
	return st.Code() == code
}
