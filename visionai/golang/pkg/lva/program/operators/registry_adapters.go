// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package operators

import (
	"fmt"

	lvapb "google3/google/cloud/visionai/v1alpha1_lva_go_proto"
	oplistpb "google3/third_party/visionai/golang/pkg/lva/program/proto/operator_list_go_proto"
)

var _ lvapb.OperatorDefinition

// InsertOpsFrom reads an OperatorList, extracts each OperatorDefinition's
// information, and inserts them into the given OperatorRegistry.
func InsertOpsFrom(registry *OperatorRegistry, opList *oplistpb.OperatorList) error {
	if registry == nil {
		return fmt.Errorf("the given *OperatorRegistry is nil")
	}
	if opList == nil {
		return fmt.Errorf("the given *OperatorList is nil")
	}

	for _, opDef := range opList.GetOperators() {
		var opInfo OperatorInfo
		opInfo.Name = opDef.GetOperator()

		opInfo.InputArgs = []ArgumentInfo{}
		for _, argDef := range opDef.GetInputArgs() {
			var argInfo ArgumentInfo
			argInfo.Name = argDef.GetArgument()
			argInfo.Type = argDef.GetType()
			opInfo.InputArgs = append(opInfo.InputArgs, argInfo)
		}

		opInfo.OutputArgs = []ArgumentInfo{}
		for _, argDef := range opDef.GetOutputArgs() {
			var argInfo ArgumentInfo
			argInfo.Name = argDef.GetArgument()
			argInfo.Type = argDef.GetType()
			opInfo.OutputArgs = append(opInfo.OutputArgs, argInfo)
		}

		opInfo.Attributes = []AttributeInfo{}
		for _, attrDef := range opDef.GetAttributes() {
			var attrInfo AttributeInfo
			attrInfo.Name = attrDef.GetAttribute()
			attrInfo.Type = attrDef.GetType()

			// It turns out that the oneof field itself is nil
			// when the field is unset rather than having a non-nil
			// field whose value is nil.
			if attrDef.GetDefaultValue() == nil {
				attrInfo.DefaultValue = nil
				opInfo.Attributes = append(opInfo.Attributes, attrInfo)
				continue
			}

			attrInfo.DefaultValue = nil
			if attrDef.GetDefaultValue().HasValue() {
				switch attrDef.GetDefaultValue().WhichValue() {
				case lvapb.AttributeValue_I_case:
					attrInfo.DefaultValue = attrDef.GetDefaultValue().GetI()
				case lvapb.AttributeValue_F_case:
					attrInfo.DefaultValue = attrDef.GetDefaultValue().GetF()
				case lvapb.AttributeValue_B_case:
					attrInfo.DefaultValue = attrDef.GetDefaultValue().GetB()
				case lvapb.AttributeValue_S_case:
					attrInfo.DefaultValue = string(attrDef.GetDefaultValue().GetS())
				default:
					return fmt.Errorf("got an unexpected DefaultValue type (%T) for attribute %q of operator %q", attrDef.GetDefaultValue().WhichValue(), attrDef.GetAttribute(), opDef.GetOperator())
				}
			}
			opInfo.Attributes = append(opInfo.Attributes, attrInfo)
		}

		// Check and populate Operator resources.
		if opDef.GetResources() == nil {
			return fmt.Errorf("operator %q did not specify any resource requirements", opDef.GetOperator())
		}
		if opDef.GetResources().GetCpu() == "" {
			return fmt.Errorf("operator %q did not specify its required CPU resources", opDef.GetOperator())
		}
		if opDef.GetResources().GetMemory() == "" {
			return fmt.Errorf("operator %q did not specify its required memory resources", opDef.GetOperator())
		}
		opInfo.Resources = &ResourceInfo{
			Cpu:             opDef.GetResources().GetCpu(),
			Memory:          opDef.GetResources().GetMemory(),
			Gpus:            int(opDef.GetResources().GetGpus()),
			LatencyBudgetMs: int(opDef.GetResources().GetLatencyBudgetMs()),
		}

		if err := registry.Insert(opDef.GetOperator(), &opInfo); err != nil {
			return err
		}
	}
	return nil
}
