// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package operators

import (
	"testing"

	lvapb "google3/google/cloud/visionai/v1alpha1_lva_go_proto"
	oplistpb "google3/third_party/visionai/golang/pkg/lva/program/proto/operator_list_go_proto"
)

func TestOperatorRegistryEmptyRegistry(t *testing.T) {
	registry := NewOperatorRegistry()
	if registry == nil {
		t.Error("want non-nil, got nil")
	}
	opInfo, err := registry.Lookup("MyOp")
	if opInfo != nil {
		t.Errorf("want nil, got non-nil %v", opInfo)
	}
	if err == nil {
		t.Error("want non-nil error, got nil")
	}
}

func TestOperatorRegistryInsert(t *testing.T) {
	registry := NewOperatorRegistry()

	var err error
	if len(registry.mapping) != 0 {
		t.Errorf("want empty registry mapping, got %d registry mappings", len(registry.mapping))
	}

	err = registry.Insert("MyOp",
		&OperatorInfo{
			Name: "MyOp",
		},
	)
	if err != nil {
		t.Errorf("want nil error, got %v", err)
	}
	if len(registry.mapping) != 1 {
		t.Errorf("want 1 registry mapping, got %d registry mapping", len(registry.mapping))
	}

	err = registry.Insert("YourOp",
		&OperatorInfo{
			Name: "YourOp",
		},
	)
	if err != nil {
		t.Errorf("want nil error, got %v", err)
	}
	if len(registry.mapping) != 2 {
		t.Errorf("want 2 registry mappings, got %d registry mapping", len(registry.mapping))
	}

	err = registry.Insert("YourOp",
		&OperatorInfo{
			Name: "YourOp",
		},
	)
	if err == nil {
		t.Error("want non-nil error, got nil")
	}
	if len(registry.mapping) != 2 {
		t.Errorf("want 2 registry mappings, got %d registry mapping", len(registry.mapping))
	}

	opInfo, err := registry.Lookup("MyOp")
	if err != nil {
		t.Errorf("want nil error, got %v", err)
	}
	if opInfo == nil {
		t.Fatal("want non-nil operator info, got nil")
	}
	if opInfo.Name != "MyOp" {
		t.Errorf("want %v, got %v", "MyOp", opInfo.Name)
	}
}

func TestOpRegistryInsertFromOperatorList(t *testing.T) {
	registry := NewOperatorRegistry()

	var err error
	var threshold float32 = 0.5

	oplist := oplistpb.OperatorList_builder{
		Operators: []*lvapb.OperatorDefinition{
			lvapb.OperatorDefinition_builder{
				Operator: "Placeholder",
				OutputArgs: []*lvapb.OperatorDefinition_ArgumentDefinition{
					lvapb.OperatorDefinition_ArgumentDefinition_builder{
						Argument: "output",
						Type:     "ANY",
					}.Build(),
				},
				Resources: lvapb.ResourceSpecification_builder{
					Cpu:    "100m",
					Memory: "100Mi",
				}.Build(),
			}.Build(),
			lvapb.OperatorDefinition_builder{
				Operator: "PersonDetection",
				InputArgs: []*lvapb.OperatorDefinition_ArgumentDefinition{
					lvapb.OperatorDefinition_ArgumentDefinition_builder{
						Argument: "video",
						Type:     "ais.video",
					}.Build(),
				},
				OutputArgs: []*lvapb.OperatorDefinition_ArgumentDefinition{
					lvapb.OperatorDefinition_ArgumentDefinition_builder{
						Argument: "overlay-rgb",
						Type:     "ais.raw-image",
					}.Build(),
					lvapb.OperatorDefinition_ArgumentDefinition_builder{
						Argument: "bbox-text",
						Type:     "ais.string",
					}.Build(),
				},
				Attributes: []*lvapb.OperatorDefinition_AttributeDefinition{
					lvapb.OperatorDefinition_AttributeDefinition_builder{
						Attribute:    "threshold",
						Type:         "float",
						DefaultValue: lvapb.AttributeValue_builder{F: &threshold}.Build(),
					}.Build(),
				},
				Resources: lvapb.ResourceSpecification_builder{
					Cpu:    "100m",
					Memory: "100Mi",
				}.Build(),
			}.Build(),
		},
	}.Build()

	err = InsertOpsFrom(registry, oplist)
	if err != nil {
		t.Errorf("want nil err, got %v", err)
	}
	if len(registry.mapping) != 2 {
		t.Errorf("want 2, got %d", len(registry.mapping))
	}

	opInfo, err := registry.Lookup("PersonDetection")
	if err != nil {
		t.Errorf("want nil err, got %v", err)
	}
	if opInfo == nil {
		t.Fatal("want non-nil operator info, get nil")
	}
	if opInfo.Name != "PersonDetection" {
		t.Errorf("want PersonDetection, got %q", opInfo.Name)
	}
	if len(opInfo.InputArgs) != 1 {
		t.Fatalf("want 1 input argument, got %d", len(opInfo.InputArgs))
	}
	if opInfo.InputArgs[0].Name != "video" {
		t.Errorf("want video, got %q", opInfo.InputArgs[0].Name)
	}
	if opInfo.InputArgs[0].Type != "ais.video" {
		t.Errorf("want ais.video, got %q", opInfo.InputArgs[0].Type)
	}
	if len(opInfo.OutputArgs) != 2 {
		t.Fatalf("want 2 output arguments, got %d", len(opInfo.OutputArgs))
	}
	if opInfo.OutputArgs[0].Name != "overlay-rgb" {
		t.Errorf("want overlay-rgb, got %q", opInfo.OutputArgs[0].Name)
	}
	if opInfo.OutputArgs[0].Type != "ais.raw-image" {
		t.Errorf("want ais.raw-image, got %q", opInfo.OutputArgs[0].Type)
	}

	if opInfo.OutputArgs[1].Name != "bbox-text" {
		t.Errorf("want bbox-text, got %q", opInfo.OutputArgs[1].Name)
	}
	if opInfo.OutputArgs[1].Type != "ais.string" {
		t.Errorf("want ais.string, got %q", opInfo.OutputArgs[1].Type)
	}
	if len(opInfo.Attributes) != 1 {
		t.Fatalf("want 1 attribute, got %d attributes", len(opInfo.Attributes))
	}
	if opInfo.Attributes[0].Name != "threshold" {
		t.Errorf("want threshold, got %q", opInfo.Attributes[0].Name)
	}
	if opInfo.Attributes[0].Type != "float" {
		t.Errorf("want float, got %q", opInfo.Attributes[0].Type)
	}
	v, ok := opInfo.Attributes[0].DefaultValue.(float32)
	if !ok {
		t.Fatalf("want float32, got %T", opInfo.Attributes[0].DefaultValue)
	}
	if v != float32(0.5) {
		t.Errorf("want threshold 0.5, got %f", v)
	}
}
