// Copyright (c) 2023 Google LLC All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package client

import (
	"context"
	"fmt"
	"net"
	"strings"
	"testing"

	"google3/base/go/log"
	rpcpb "google3/google/cloud/visionai/v1alpha1_platform_go_grpc_proto"
	pb "google3/google/cloud/visionai/v1alpha1_platform_go_proto"
	lropb "google3/google/longrunning/operations_go_proto"
	lro "google3/google/longrunning/operations_grpc_go"
	"google3/third_party/golang/cmp/cmp"
	"google3/third_party/golang/grpc/grpc"
	"google3/third_party/golang/protobuf/v2/testing/protocmp/protocmp"
)

var (
	testParent = "projects/test-project/locations/test-locations/clusters/test-cluster"
	testApp    = testParent + "/applications/test-app"
)

type fakeAppPlatformService struct {
	rpcpb.UnimplementedAppPlatformServer
	lro.UnimplementedOperationsServer

	applications []*pb.Application
	instances    []*pb.Instance
}

func (s *fakeAppPlatformService) ListApplications(ctx context.Context, req *pb.ListApplicationsRequest) (*pb.ListApplicationsResponse, error) {
	apps := make([]*pb.Application, 0)
	for _, a := range s.applications {
		if strings.HasPrefix(a.GetName(), req.GetParent()) {
			apps = append(apps, a)
		}
	}
	return pb.ListApplicationsResponse_builder{Applications: apps}.Build(), nil
}

func (s *fakeAppPlatformService) ListInstances(ctx context.Context, req *pb.ListInstancesRequest) (*pb.ListInstancesResponse, error) {
	instances := make([]*pb.Instance, 0)
	for _, i := range s.instances {
		if strings.HasPrefix(i.GetName(), req.GetParent()) {
			instances = append(instances, i)
		}
	}
	return pb.ListInstancesResponse_builder{Instances: instances}.Build(), nil
}

func (s *fakeAppPlatformService) GetOperation(ctx context.Context, req *lropb.GetOperationRequest) (*lropb.Operation, error) {
	return &lropb.Operation{
		Done: true,
	}, nil
}

func TestListApplications(t *testing.T) {
	tests := []struct {
		name         string
		applications []*pb.Application
		parent       string
		want         []*pb.Application
	}{
		{
			name:         "successfully list empty applications",
			applications: []*pb.Application{},
			parent:       testApp,
			want:         []*pb.Application{},
		},
		{
			name: "successfully list applications",
			applications: []*pb.Application{
				pb.Application_builder{
					Name: fmt.Sprintf("%s/applications/app-1", testParent),
				}.Build(),
				pb.Application_builder{
					Name: fmt.Sprintf("%s/applications/app-2", testParent),
				}.Build(),
			},
			parent: testParent,
			want: []*pb.Application{
				pb.Application_builder{
					Name: fmt.Sprintf("%s/applications/app-1", testParent),
				}.Build(),
				pb.Application_builder{
					Name: fmt.Sprintf("%s/applications/app-2", testParent),
				}.Build(),
			},
		},
		{
			name: "successfully partially list applications",
			applications: []*pb.Application{
				pb.Application_builder{
					Name: fmt.Sprintf("%s/applications/app-1", testParent),
				}.Build(),
				pb.Application_builder{
					Name: fmt.Sprintf("%s/applications/app-2", "foo"),
				}.Build(),
			},
			parent: testParent,
			want: []*pb.Application{
				pb.Application_builder{
					Name: fmt.Sprintf("%s/applications/app-1", testParent),
				}.Build(),
			},
		},
	}

	for _, test := range tests {
		t.Run(test.name, func(t *testing.T) {
			manager := mustCreateManager(test.applications, []*pb.Instance{})
			got, err := manager.ListApplications(test.parent)
			if err != nil {
				t.Fatalf("want nil, got error %v", err)
			}
			if len(got) != len(test.want) {
				t.Errorf("want %d applications, got %d applications", len(test.want), len(got))
			}
			for id := range test.want {
				if diff := cmp.Diff(got[id], test.want[id], protocmp.Transform()); diff != "" {
					t.Errorf("want %s, got %s, (-want +got)\n %s", test.want[id], got[id], diff)
				}
			}
		})
	}
}

func TestListInstances(t *testing.T) {
	tests := []struct {
		name      string
		instances []*pb.Instance
		parent    string
		want      []*pb.Instance
	}{
		{
			name:      "successfully list empty instances",
			instances: []*pb.Instance{},
			parent:    testParent,
			want:      []*pb.Instance{},
		},
		{
			name: "successfully list instances",
			instances: []*pb.Instance{
				pb.Instance_builder{
					Name: fmt.Sprintf("%s/Instances/ins-1", testApp),
				}.Build(),
				pb.Instance_builder{
					Name: fmt.Sprintf("%s/Instances/ins-2", testApp),
				}.Build(),
			},
			parent: testParent,
			want: []*pb.Instance{
				pb.Instance_builder{
					Name: fmt.Sprintf("%s/Instances/ins-1", testApp),
				}.Build(),
				pb.Instance_builder{
					Name: fmt.Sprintf("%s/Instances/ins-2", testApp),
				}.Build(),
			},
		},
		{
			name: "successfully partially list applications",
			instances: []*pb.Instance{
				pb.Instance_builder{
					Name: fmt.Sprintf("%s/Instances/ins-1", testApp),
				}.Build(),
				pb.Instance_builder{
					Name: fmt.Sprintf("%s/Instances/ins-2", "foo"),
				}.Build(),
			},
			parent: testParent,
			want: []*pb.Instance{
				pb.Instance_builder{
					Name: fmt.Sprintf("%s/Instances/ins-1", testApp),
				}.Build(),
			},
		},
	}

	for _, test := range tests {
		t.Run(test.name, func(t *testing.T) {
			manager := mustCreateManager([]*pb.Application{}, test.instances)
			got, err := manager.ListInstances(test.parent)
			if err != nil {
				t.Fatalf("want nil, got error %v", err)
			}
			if len(got) != len(test.want) {
				t.Errorf("want %d applications, got %d applications", len(test.want), len(got))
			}
			for id := range test.want {
				if diff := cmp.Diff(got[id], test.want[id], protocmp.Transform()); diff != "" {
					t.Errorf("want %s, got %s, (-want +got)\n %s", test.want[id], got[id], diff)
				}
			}
		})
	}
}

func mustCreateManager(apps []*pb.Application, instances []*pb.Instance) Interface {
	listener, err := net.Listen("tcp", ":0")
	if err != nil {
		log.Fatal(err)
	}
	s := grpc.NewServer()

	server := &fakeAppPlatformService{
		applications: apps,
		instances:    instances,
	}
	rpcpb.RegisterAppPlatformServer(s, server)
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
