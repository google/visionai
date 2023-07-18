// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package client

import (
	"context"
	"fmt"
	"time"

	v1pb "google3/google/cloud/visionai/v1_lva_go_grpc_proto"
	v1apipb "google3/google/cloud/visionai/v1_lva_go_proto"

	lropb "google3/google/longrunning/operations_go_proto"
	lro "google3/google/longrunning/operations_grpc_go"
	"google3/third_party/golang/google_api/option/option"
	"google3/third_party/golang/google_api/transport/transport"
	"google3/third_party/golang/grpc/grpc"
	"google3/third_party/golang/grpc/metadata/metadata"
	"google3/third_party/visionai/golang/pkg/lva/util/util"
)

// Interface defines the LVA management interface.
type Interface interface {
	ListAnalyses(parent string) ([]*v1apipb.Analysis, error)
	GetAnalysis(name string) (*v1apipb.Analysis, error)
	CreateAnalysis(parent, id string, analysis *v1apipb.Analysis) error
	DeleteAnalysis(name string) error

	ListProcesses(parent string) ([]*v1apipb.Process, error)
	GetProcess(name string) (*v1apipb.Process, error)
	CreateProcess(parent, id string, process *v1apipb.Process) error
	DeleteProcess(name string) error
	BatchRunProcess(parent string, createProcessRequests []*v1apipb.CreateProcessRequest) (*lropb.Operation, error)

	CreateOperator(parent, id string, operator *v1apipb.Operator) error
	UpdateOperator(name string, operator *v1apipb.Operator) error
	GetOperator(name string) (*v1apipb.Operator, error)
	ListOperators(parent string) ([]*v1apipb.Operator, error)
	ListPublicOperators(parent string) ([]*v1apipb.Operator, error)
	DeleteOperator(name string) error
}

// NewManager creates a LVA manager.
func NewManager(serviceEndpoint string, opts ...option.ClientOption) (Interface, error) {
	opts = append(opts, option.WithEndpoint(serviceEndpoint))
	return newManager(transport.DialGRPC, opts...)
}

// NewTestManager creates a test LVA manager.
func NewTestManager(serviceEndpoint string) (Interface, error) {
	return newManager(transport.DialGRPCInsecure, option.WithEndpoint(serviceEndpoint))
}

func newManager(dial func(ctx context.Context, opts ...option.ClientOption) (*grpc.ClientConn, error), opts ...option.ClientOption) (Interface, error) {
	conn, err := dial(context.Background(), opts...)
	if err != nil {
		return nil, err
	}

	c := v1pb.NewLiveVideoAnalyticsClient(conn)
	opC := lro.NewOperationsClient(conn)
	return &manager{
		client:   c,
		opClient: opC,
	}, nil
}

type manager struct {
	client   v1pb.LiveVideoAnalyticsClient
	opClient lro.OperationsClient
}

func (m *manager) ListAnalyses(parent string) ([]*v1apipb.Analysis, error) {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("parent=%s", parent)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	resp, err := m.client.ListAnalyses(ctx, v1apipb.ListAnalysesRequest_builder{
		Parent: parent,
	}.Build())
	if err != nil {
		return nil, err
	}
	return resp.GetAnalyses(), nil
}

func (m *manager) GetAnalysis(name string) (*v1apipb.Analysis, error) {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("name=%s", name)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	return m.client.GetAnalysis(ctx, v1apipb.GetAnalysisRequest_builder{Name: name}.Build())
}

func (m *manager) CreateAnalysis(parent, id string, analysis *v1apipb.Analysis) error {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("parent=%s", parent)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	op, err := m.client.CreateAnalysis(ctx, v1apipb.CreateAnalysisRequest_builder{Parent: parent, AnalysisId: id, Analysis: analysis}.Build())
	if err != nil {
		return err
	}
	newCtx, cancel := context.WithTimeout(ctx, 60*time.Second)
	defer cancel()
	return util.WaitForOperation(newCtx, m.opClient, op)
}

func (m *manager) DeleteAnalysis(name string) error {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("name=%s", name)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	op, err := m.client.DeleteAnalysis(ctx, v1apipb.DeleteAnalysisRequest_builder{Name: name}.Build())
	if err != nil {
		return err
	}
	newCtx, cancel := context.WithTimeout(ctx, 60*time.Second)
	defer cancel()
	return util.WaitForOperation(newCtx, m.opClient, op)
}

func (m *manager) ListProcesses(parent string) ([]*v1apipb.Process, error) {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("parent=%s", parent)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	resp, err := m.client.ListProcesses(ctx, v1apipb.ListProcessesRequest_builder{
		Parent: parent,
	}.Build())
	if err != nil {
		return nil, err
	}
	return resp.GetProcesses(), nil
}

func (m *manager) GetProcess(name string) (*v1apipb.Process, error) {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("name=%s", name)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	return m.client.GetProcess(ctx, v1apipb.GetProcessRequest_builder{Name: name}.Build())
}

func (m *manager) CreateProcess(parent, id string, process *v1apipb.Process) error {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("parent=%s", parent)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	op, err := m.client.CreateProcess(ctx, v1apipb.CreateProcessRequest_builder{Parent: parent, ProcessId: id, Process: process}.Build())
	if err != nil {
		return err
	}
	newCtx, cancel := context.WithTimeout(ctx, 60*time.Second)
	defer cancel()
	return util.WaitForOperation(newCtx, m.opClient, op)
}

func (m *manager) DeleteProcess(name string) error {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("name=%s", name)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	op, err := m.client.DeleteProcess(ctx, v1apipb.DeleteProcessRequest_builder{Name: name}.Build())
	if err != nil {
		return err
	}
	newCtx, cancel := context.WithTimeout(ctx, 60*time.Second)
	defer cancel()
	return util.WaitForOperation(newCtx, m.opClient, op)
}

func (m *manager) BatchRunProcess(parent string, createProcessRequests []*v1apipb.CreateProcessRequest) (*lropb.Operation, error) {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("parent=%s", parent)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	op, err := m.client.BatchRunProcess(ctx, v1apipb.BatchRunProcessRequest_builder{
		Parent:   parent,
		Requests: createProcessRequests,
	}.Build())
	if err != nil {
		return nil, err
	}
	return op, nil
}

func (m *manager) CreateOperator(parent, id string, operator *v1apipb.Operator) error {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("parent=%s", parent)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	op, err := m.client.CreateOperator(ctx, v1apipb.CreateOperatorRequest_builder{
		Parent:     parent,
		OperatorId: id,
		Operator:   operator,
	}.Build())
	if err != nil {
		return err
	}
	newCtx, cancel := context.WithTimeout(ctx, 60*time.Second)
	defer cancel()
	return util.WaitForOperation(newCtx, m.opClient, op)
}

func (m *manager) UpdateOperator(name string, operator *v1apipb.Operator) error {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("name=%s", name)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	op, err := m.client.UpdateOperator(ctx, v1apipb.UpdateOperatorRequest_builder{Operator: operator}.Build())
	if err != nil {
		return err
	}
	newCtx, cancel := context.WithTimeout(ctx, 60*time.Second)
	defer cancel()
	util.WaitForOperation(newCtx, m.opClient, op)
	return nil
}

func (m *manager) GetOperator(name string) (*v1apipb.Operator, error) {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("name=%s", name)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	return m.client.GetOperator(ctx, v1apipb.GetOperatorRequest_builder{Name: name}.Build())
}

func (m *manager) ListOperators(parent string) ([]*v1apipb.Operator, error) {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("parent=%s", parent)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	var ops []*v1apipb.Operator
	pageToken := ""

	for {
		opresp, err := m.client.ListOperators(ctx, v1apipb.ListOperatorsRequest_builder{
			Parent:    parent,
			PageToken: pageToken,
		}.Build())
		if err != nil {
			return nil, err
		}
		ops = append(ops, opresp.GetOperators()...)
		pageToken = opresp.GetNextPageToken()
		if pageToken == "" {
			break
		}
	}

	return ops, nil
}

func (m *manager) ListPublicOperators(parent string) ([]*v1apipb.Operator, error) {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("parent=%s", parent)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	resp, err := m.client.ListPublicOperators(ctx, v1apipb.ListPublicOperatorsRequest_builder{
		Parent: parent,
	}.Build())
	if err != nil {
		return nil, err
	}
	return resp.GetOperators(), nil
}

func (m *manager) DeleteOperator(name string) error {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("name=%s", name)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	op, err := m.client.DeleteOperator(ctx, v1apipb.DeleteOperatorRequest_builder{Name: name}.Build())
	if err != nil {
		return err
	}
	newCtx, cancel := context.WithTimeout(ctx, 60*time.Second)
	defer cancel()
	return util.WaitForOperation(newCtx, m.opClient, op)
}
