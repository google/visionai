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
	lvagpb "google3/google/cloud/visionai/v1alpha1_lva_go_grpc_proto"
	lvapb "google3/google/cloud/visionai/v1alpha1_lva_go_proto"
	lropb "google3/google/longrunning/operations_go_proto"
	lro "google3/google/longrunning/operations_grpc_go"

	"google3/third_party/golang/cmp/cmp"
	"google3/third_party/golang/grpc/codes/codes"
	"google3/third_party/golang/grpc/grpc"
	"google3/third_party/golang/grpc/status/status"
	"google3/third_party/golang/protobuf/v2/testing/protocmp/protocmp"
)

var (
	testParent = "projects/test-project/locations/test-locations/clusters/test-cluster"
)

type fakeLVAService struct {
	lvagpb.UnimplementedLiveVideoAnalyticsServer
	lro.UnimplementedOperationsServer

	analyses []*lvapb.Analysis
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

func (s *fakeLVAService) GetOperation(ctx context.Context, req *lropb.GetOperationRequest) (*lropb.Operation, error) {
	return &lropb.Operation{
		Done: true,
	}, nil
}

func TestListLiveVideoAnalytics(t *testing.T) {
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
			manager := mustCreateManager(test.analyses)
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

func TestGetLiveVideoAnalytics(t *testing.T) {
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
			manager := mustCreateManager(test.analyses)
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

func TestCreateLiveVideoAnalytics(t *testing.T) {
	analyses := []*lvapb.Analysis{
		lvapb.Analysis_builder{
			Name: fmt.Sprintf("%s/analyses/analysis-1", testParent),
		}.Build(),
	}
	analysis := lvapb.Analysis_builder{}.Build()
	manager := mustCreateManager(analyses)
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

func TestDeleteLiveVideoAnalytics(t *testing.T) {
	analyses := []*lvapb.Analysis{
		lvapb.Analysis_builder{
			Name: fmt.Sprintf("%s/analyses/analysis-1", testParent),
		}.Build(),
		lvapb.Analysis_builder{
			Name: fmt.Sprintf("%s/analyses/analysis-2", testParent),
		}.Build(),
	}
	manager := mustCreateManager(analyses)
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

func mustCreateManager(analyses []*lvapb.Analysis) Interface {
	listener, err := net.Listen("tcp", ":0")
	if err != nil {
		log.Fatal(err)
	}
	s := grpc.NewServer()

	server := &fakeLVAService{
		analyses: analyses,
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

func is(err error, code codes.Code) bool {
	st, ok := status.FromError(err)
	if !ok {
		return false
	}
	return st.Code() == code
}
