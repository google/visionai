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

	lvagpb "google3/google/cloud/visionai/v1alpha1_lva_go_grpc_proto"
	lvapb "google3/google/cloud/visionai/v1alpha1_lva_go_proto"
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
	ListAnalyses(parent string) ([]*lvapb.Analysis, error)
	GetAnalysis(name string) (*lvapb.Analysis, error)
	CreateAnalysis(parent, id string, analysis *lvapb.Analysis) error
	DeleteAnalysis(name string) error

	ListProcesses(parent string) ([]*lvapb.Process, error)
	GetProcess(name string) (*lvapb.Process, error)
	CreateProcess(parent, id string, process *lvapb.Process) error
	DeleteProcess(name string) error
	BatchRunProcess(parent string, createProcessRequests []*lvapb.CreateProcessRequest) (*lropb.Operation, error)
}

// NewManager creates a LVA manager.
func NewManager(serviceEndpoint string) (Interface, error) {
	return newManager(serviceEndpoint, transport.DialGRPC)
}

// NewTestManager creates a test LVA manager.
func NewTestManager(serviceEndpoint string) (Interface, error) {
	return newManager(serviceEndpoint, transport.DialGRPCInsecure)
}

func newManager(serviceEndpoint string, dial func(ctx context.Context, opts ...option.ClientOption) (*grpc.ClientConn, error)) (Interface, error) {
	conn, err := dial(context.Background(), option.WithEndpoint(serviceEndpoint))
	if err != nil {
		return nil, err
	}

	c := lvagpb.NewLiveVideoAnalyticsClient(conn)
	opC := lro.NewOperationsClient(conn)
	return &manager{
		client:   c,
		opClient: opC,
	}, nil
}

type manager struct {
	client   lvagpb.LiveVideoAnalyticsClient
	opClient lro.OperationsClient
}

func (m *manager) ListAnalyses(parent string) ([]*lvapb.Analysis, error) {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("parent=%s", parent)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	resp, err := m.client.ListAnalyses(ctx, lvapb.ListAnalysesRequest_builder{
		Parent: parent,
	}.Build())
	if err != nil {
		return nil, err
	}
	return resp.GetAnalyses(), nil
}

func (m *manager) GetAnalysis(name string) (*lvapb.Analysis, error) {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("name=%s", name)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	return m.client.GetAnalysis(ctx, lvapb.GetAnalysisRequest_builder{Name: name}.Build())
}

func (m *manager) CreateAnalysis(parent, id string, analysis *lvapb.Analysis) error {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("parent=%s", parent)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	op, err := m.client.CreateAnalysis(ctx, lvapb.CreateAnalysisRequest_builder{Parent: parent, AnalysisId: id, Analysis: analysis}.Build())
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
	op, err := m.client.DeleteAnalysis(ctx, lvapb.DeleteAnalysisRequest_builder{Name: name}.Build())
	if err != nil {
		return err
	}
	newCtx, cancel := context.WithTimeout(ctx, 60*time.Second)
	defer cancel()
	return util.WaitForOperation(newCtx, m.opClient, op)
}

func (m *manager) ListProcesses(parent string) ([]*lvapb.Process, error) {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("parent=%s", parent)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	resp, err := m.client.ListProcesses(ctx, lvapb.ListProcessesRequest_builder{
		Parent: parent,
	}.Build())
	if err != nil {
		return nil, err
	}
	return resp.GetProcesses(), nil
}

func (m *manager) GetProcess(name string) (*lvapb.Process, error) {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("name=%s", name)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	return m.client.GetProcess(ctx, lvapb.GetProcessRequest_builder{Name: name}.Build())
}

func (m *manager) CreateProcess(parent, id string, process *lvapb.Process) error {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("parent=%s", parent)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	op, err := m.client.CreateProcess(ctx, lvapb.CreateProcessRequest_builder{Parent: parent, ProcessId: id, Process: process}.Build())
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
	op, err := m.client.DeleteProcess(ctx, lvapb.DeleteProcessRequest_builder{Name: name}.Build())
	if err != nil {
		return err
	}
	newCtx, cancel := context.WithTimeout(ctx, 60*time.Second)
	defer cancel()
	return util.WaitForOperation(newCtx, m.opClient, op)
}

func (m *manager) BatchRunProcess(parent string, createProcessRequests []*lvapb.CreateProcessRequest) (*lropb.Operation, error) {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("parent=%s", parent)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	op, err := m.client.BatchRunProcess(ctx, lvapb.BatchRunProcessRequest_builder{
		Parent:   parent,
		Requests: createProcessRequests,
	}.Build())
	if err != nil {
		return nil, err
	}
	return op, nil
}
