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

	wgpb "google3/google/cloud/visionai/v1alpha1_warehouse_go_grpc_proto"
	wpb "google3/google/cloud/visionai/v1alpha1_warehouse_go_proto"
	lro "google3/google/longrunning/operations_grpc_go"
	"google3/third_party/golang/google_api/option/option"
	"google3/third_party/golang/google_api/transport/transport"
	"google3/third_party/golang/grpc/grpc"
	"google3/third_party/golang/grpc/metadata/metadata"
	"google3/third_party/visionai/golang/pkg/lva/util/util"
)

// Interface defines the warehouse management interface.
type Interface interface {
	ListCorpora(parent string) ([]*wpb.Corpus, error)
	GetCorpus(name string) (*wpb.Corpus, error)
	CreateCorpus(parent string, Corpus *wpb.Corpus) error
	DeleteCorpus(name string) error

	ListAssets(parent string) ([]*wpb.Asset, error)
	GetAsset(name string) (*wpb.Asset, error)
	CreateAsset(parent string, Asset *wpb.Asset) error
	DeleteAsset(name string) error
}

// NewManager creates a warehouse manager.
func NewManager(serviceEndpoint string) (Interface, error) {
	return newManager(serviceEndpoint, transport.DialGRPC)
}

// NewTestManager creates a test warehouse manager.
func NewTestManager(serviceEndpoint string) (Interface, error) {
	return newManager(serviceEndpoint, transport.DialGRPCInsecure)
}

func newManager(serviceEndpoint string, dial func(ctx context.Context, opts ...option.ClientOption) (*grpc.ClientConn, error)) (Interface, error) {
	conn, err := dial(context.Background(), option.WithEndpoint(serviceEndpoint))
	if err != nil {
		return nil, err
	}

	c := wgpb.NewWarehouseClient(conn)
	opC := lro.NewOperationsClient(conn)
	return &manager{
		client:   c,
		opClient: opC,
	}, nil
}

type manager struct {
	client   wgpb.WarehouseClient
	opClient lro.OperationsClient
}

func (m *manager) ListCorpora(parent string) ([]*wpb.Corpus, error) {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("parent=%s", parent)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	resp, err := m.client.ListCorpora(ctx, wpb.ListCorporaRequest_builder{
		Parent: parent,
	}.Build())
	if err != nil {
		return nil, err
	}
	return resp.GetCorpora(), nil
}

func (m *manager) GetCorpus(name string) (*wpb.Corpus, error) {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("name=%s", name)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	return m.client.GetCorpus(ctx, wpb.GetCorpusRequest_builder{Name: name}.Build())
}

func (m *manager) CreateCorpus(parent string, corpus *wpb.Corpus) error {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("parent=%s", parent)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	op, err := m.client.CreateCorpus(ctx, wpb.CreateCorpusRequest_builder{Parent: parent, Corpus: corpus}.Build())
	if err != nil {
		return err
	}
	newCtx, cancel := context.WithTimeout(ctx, 60*time.Second)
	defer cancel()
	err = util.WaitForOperation(newCtx, m.opClient, op)
	if err != nil {
		return err
	}
	fmt.Print("Created corpus is : ", op.GetResponse())
	return nil
}

func (m *manager) DeleteCorpus(name string) error {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("name=%s", name)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	_, err := m.client.DeleteCorpus(ctx, wpb.DeleteCorpusRequest_builder{Name: name}.Build())
	if err != nil {
		return err
	}
	return nil
}

func (m *manager) ListAssets(parent string) ([]*wpb.Asset, error) {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("parent=%s", parent)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	resp, err := m.client.ListAssets(ctx, wpb.ListAssetsRequest_builder{
		Parent: parent,
	}.Build())
	if err != nil {
		return nil, err
	}
	return resp.GetAssets(), nil
}

func (m *manager) GetAsset(name string) (*wpb.Asset, error) {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("name=%s", name)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	return m.client.GetAsset(ctx, wpb.GetAssetRequest_builder{Name: name}.Build())
}

func (m *manager) CreateAsset(parent string, asset *wpb.Asset) error {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("parent=%s", parent)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	response, err := m.client.CreateAsset(ctx, wpb.CreateAssetRequest_builder{Parent: parent, Asset: asset}.Build())
	if err != nil {
		return err
	}
	fmt.Print("Created asset is : ", response)
	return nil
}

func (m *manager) DeleteAsset(name string) error {
	md := metadata.New(map[string]string{"x-goog-request-params": fmt.Sprintf("name=%s", name)})
	ctx := metadata.NewOutgoingContext(context.Background(), md)
	_, err := m.client.DeleteAsset(ctx, wpb.DeleteAssetRequest_builder{Name: name}.Build())
	if err != nil {
		return err
	}
	return nil
}
