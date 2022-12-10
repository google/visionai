// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package operators

import (
	"google3/third_party/golang/protobuf/v2/encoding/prototext/prototext"

	oplistpb "google3/third_party/visionai/golang/pkg/lva/program/proto/operator_list_go_proto"
)

// DefaultOperatorList reads the default embedded operator list.
func DefaultOperatorList() (*oplistpb.OperatorList, error) {
	opList := &oplistpb.OperatorList{}
	err := prototext.Unmarshal([]byte(MainEmbeddedOperatorListLiteral), opList)
	if err != nil {
		return nil, err
	}
	return opList, nil
}

// DefaultRegistry creates and populates an OperatorRegistry based
// on the values of the vars above (which are typically configured
// through the command line / config file).
func DefaultRegistry() (*OperatorRegistry, error) {
	opList, err := DefaultOperatorList()
	if err != nil {
		return nil, err
	}
	return RegistryFromOperatorList(opList)
}
