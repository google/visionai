// Copyright (c) 2023 Google LLC All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Package client supplies a programmatic client to the App Platform.
package client

import (
	"context"
	"fmt"

	"google3/third_party/golang/google_api/option/option"
	"google3/third_party/golang/google_api/transport/transport"
	"google3/third_party/golang/grpc/grpc"
	"google3/third_party/golang/grpc/metadata/metadata"

	rpcpb "google3/google/cloud/visionai/v1alpha1_platform_go_grpc_proto"
	pb "google3/google/cloud/visionai/v1alpha1_platform_go_proto"
	lro "google3/google/longrunning/operations_grpc_go"
)

// Interface defines the app platform management interface.
type Interface interface {
	ListApplications(parent string) ([]*pb.Application, error)
	ListInstances(parent string) ([]*pb.Instance, error)
}

// NewManager creates an app platform manager.
func NewManager(serviceEndpoint string) (Interface, error) {
	return newManager(serviceEndpoint, transport.DialGRPC)
}

// NewTestManager creates a test app platform manager.
func NewTestManager(serviceEndpoint string) (Interface, error) {
	return newManager(serviceEndpoint, transport.DialGRPCInsecure)
}

func newManager(serviceEndpoint string, dial func(ctx context.Context, opts ...option.ClientOption) (*grpc.ClientConn, error)) (Interface, error) {
	conn, err := dial(context.Background(), option.WithEndpoint(serviceEndpoint))
	if err != nil {
		return nil, err
	}

	cl := rpcpb.NewAppPlatformClient(conn)
	opCl := lro.NewOperationsClient(conn)
	return &manager{
		client:   cl,
		opClient: opCl,
	}, nil
}

type manager struct {
	client   rpcpb.AppPlatformClient
	opClient lro.OperationsClient
}

func (m *manager) ListApplications(parent string) ([]*pb.Application, error) {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("parent=%s", parent)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	resp, err := m.client.ListApplications(ctx, pb.ListApplicationsRequest_builder{
		Parent: parent,
	}.Build())
	if err != nil {
		return nil, err
	}
	return resp.GetApplications(), nil
}

func (m *manager) ListInstances(parent string) ([]*pb.Instance, error) {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("parent=%s", parent)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	resp, err := m.client.ListInstances(ctx, pb.ListInstancesRequest_builder{
		Parent: parent,
	}.Build())
	if err != nil {
		return nil, err
	}
	return resp.GetInstances(), nil
}
