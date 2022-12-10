// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package operators

import (
	"fmt"
	"io/ioutil"

	"google3/third_party/golang/protobuf/v2/encoding/prototext/prototext"

	lvapb "google3/google/cloud/visionai/v1alpha1_lva_go_proto"
	oplistpb "google3/third_party/visionai/golang/pkg/lva/program/proto/operator_list_go_proto"
)

// OperatorDefinitionFromFile reads a file containing a serialized OperatorDefinition,
// deserializes it, and returns the OperatorDefinition object.
func OperatorDefinitionFromFile(filePath string) (*lvapb.OperatorDefinition, error) {
	contents, err := ioutil.ReadFile(filePath)
	if err != nil {
		return nil, fmt.Errorf("failed to read the file %q: %v", filePath, err)
	}
	opDef := &lvapb.OperatorDefinition{}
	if err := prototext.Unmarshal(contents, opDef); err != nil {
		return nil, fmt.Errorf("failed to unmarshall input contents of %q as a textformatted OperatorDefinition protobuf: %v", filePath, err)
	}
	return opDef, nil
}

// OperatorListFromFile reads a file containing a serialized OperatorList, deserializes it,
// and returns the actual OperatorList object.
func OperatorListFromFile(filePath string) (*oplistpb.OperatorList, error) {
	contents, err := ioutil.ReadFile(filePath)
	if err != nil {
		return nil, fmt.Errorf("failed to read the file %q: %v", filePath, err)
	}
	opList := &oplistpb.OperatorList{}
	if err := prototext.Unmarshal(contents, opList); err != nil {
		return nil, fmt.Errorf("failed to unmarshall input contents of %q as a textformatted OperatorList protobuf: %v", filePath, err)
	}
	return opList, nil
}

// RegistryFromOperatorList creates and populates an OperatorRegistry
// from an OperatorList.
func RegistryFromOperatorList(opList *oplistpb.OperatorList) (*OperatorRegistry, error) {
	registry := NewOperatorRegistry()
	if err := InsertOpsFrom(registry, opList); err != nil {
		return nil, fmt.Errorf("failed to insert operators from an OperatorList: %v", err)
	}
	return registry, nil
}

// RegistryFromOpListFile creates and populates an OperatorRegistry
// from a path to an OperatorList protobuf textformat file.
func RegistryFromOpListFile(filePath string) (*OperatorRegistry, error) {
	opList, err := OperatorListFromFile(filePath)
	if err != nil {
		return nil, fmt.Errorf("failed to get an OperatorList from file %v: %v", filePath, err)
	}
	return RegistryFromOperatorList(opList)
}
