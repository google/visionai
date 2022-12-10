// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package util

import (
	"context"
	"fmt"
	"net"
	"testing"
	"time"

	"google3/base/go/log"
	lropb "google3/google/longrunning/operations_go_proto"
	lro "google3/google/longrunning/operations_grpc_go"
	spb "google3/google/rpc/status_go_proto"
	"google3/third_party/golang/grpc/credentials/insecure/insecure"
	"google3/third_party/golang/grpc/grpc"
	"google3/util/task/go/status"
)

type fakeLroService struct {
	lro.UnimplementedOperationsServer
	getOpration func(*lropb.GetOperationRequest) (*lropb.Operation, error)
}

func (s *fakeLroService) GetOperation(ctx context.Context, req *lropb.GetOperationRequest) (*lropb.Operation, error) {
	return s.getOpration(req)
}

func TestPollingLRO(t *testing.T) {
	svc := &fakeLroService{}
	id := 0
	svc.getOpration = func(req *lropb.GetOperationRequest) (*lropb.Operation, error) {
		id++
		if id < 10 {
			return &lropb.Operation{
				Done: false,
			}, nil
		}
		return &lropb.Operation{
			Done: true,
		}, nil
	}

	s, c, conn := mustCreateLROServerClient(svc)
	defer s.GracefulStop()
	defer conn.Close()

	err := WaitForOperation(context.Background(), c, &lropb.Operation{})
	if err != nil {
		t.Fatalf("want nil, got %v", err)
	}
}

func TestPollingLROFailure(t *testing.T) {
	svc := &fakeLroService{}
	svc.getOpration = func(req *lropb.GetOperationRequest) (*lropb.Operation, error) {
		return nil, fmt.Errorf("foo")
	}

	s, c, conn := mustCreateLROServerClient(svc)
	defer s.GracefulStop()
	defer conn.Close()

	err := WaitForOperation(context.Background(), c, &lropb.Operation{})
	if err == nil {
		t.Fatal("want error, got nil")
	}
}

func TestLROCompleteWithError(t *testing.T) {
	svc := &fakeLroService{}
	svc.getOpration = func(req *lropb.GetOperationRequest) (*lropb.Operation, error) {
		return &lropb.Operation{
			Done: true,
			Result: &lropb.Operation_Error{
				Error: &spb.Status{
					Message: "foo",
					Code:    int32(status.Internal),
				},
			},
		}, nil
	}

	s, c, conn := mustCreateLROServerClient(svc)
	defer s.GracefulStop()
	defer conn.Close()

	l := &lropb.Operation{}
	err := WaitForOperation(context.Background(), c, l)
	if err == nil {
		t.Fatal("want error, got nil")
	}
}

func TestWaitForLROTimeout(t *testing.T) {
	svc := &fakeLroService{}
	svc.getOpration = func(req *lropb.GetOperationRequest) (*lropb.Operation, error) {
		return &lropb.Operation{
			Done: false,
		}, nil
	}

	s, c, conn := mustCreateLROServerClient(svc)
	defer s.GracefulStop()
	defer conn.Close()

	ctx, cancel := context.WithTimeout(context.Background(), time.Second*3)
	defer cancel()
	err := WaitForOperation(ctx, c, &lropb.Operation{})
	if err == nil {
		t.Fatal("want error, got nil")
	}
}

func mustCreateLROServerClient(svc *fakeLroService) (*grpc.Server, lro.OperationsClient, *grpc.ClientConn) {
	listener, err := net.Listen("tcp", ":0")
	if err != nil {
		log.Fatal(err)
	}
	s := grpc.NewServer()

	lro.RegisterOperationsServer(s, svc)

	go func() {
		if err := s.Serve(listener); err != nil {
			log.Fatal(err)
		}
	}()

	conn, err := grpc.Dial(listener.Addr().String(), grpc.WithTransportCredentials(insecure.NewCredentials()))
	if err != nil {
		log.Fatal(err)
	}
	client := lro.NewOperationsClient(conn)
	return s, client, conn
}
