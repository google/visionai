// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package sema

import (
	"fmt"

	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
)

// checkAndSetAnalyzerAttributes checks that the attributes specified
// on the analyzer are defined in the operator and also have the expected
// types.
//
// On success, the analyzer will have all attributes expected in the operator
// filled with a usable value.
func checkAndSetAnalyzerAttributes(n *asg.Node) error {
	analyzerElement, ok := n.Element().(*asg.AnalyzerElement)
	if !ok {
		return fmt.Errorf("internal error: %q is not an Analyzer (got %T)", n.Name(), n.Element())
	}
	analyzerInfo := analyzerElement.Info
	analyzerAttrMap := analyzerInfo.Attributes

	// Form the operator's attribute map, filled with default values.
	operator := analyzerInfo.Operator
	operatorAttrMap := make(map[string]*asg.AttributeValueInfo)
	for _, attributeInfo := range operator.Attributes {
		operatorAttrMap[attributeInfo.Name] = &asg.AttributeValueInfo{
			Type:  attributeInfo.Type,
			Value: attributeInfo.DefaultValue,
		}
	}

	// Check that there are no extra attributes specified in the analyzer.
	for k := range analyzerAttrMap {
		_, ok := operatorAttrMap[k]
		if !ok {
			return fmt.Errorf("analyzer %q specified an unknown attribute %q for operator %q", n.Name(), k, operator.Name)
		}
	}

	// Fill analyzer's attribute map with default values if not already specified.
	for k, ov := range operatorAttrMap {
		av, ok := analyzerAttrMap[k]

		// Check the type if already specified.
		if ok {
			if av.Type != ov.Type {
				return fmt.Errorf("operator %q expects type %q for attribute %q but got type %q from analyzer %q", operator.Name, av.Type, k, ov.Type, n.Name())
			}
			continue
		}

		// Require a value if no default is available.
		if ov.Value == nil {
			return fmt.Errorf("operator %q requires a value to be specified for attribute %q, but none was given in analyzer %q", operator.Name, k, n.Name())
		}

		analyzerAttrMap[k] = ov
	}

	return nil
}

func checkAndSetAttributeInformation(n *asg.Node) error {
	element := n.Element()
	switch element.(type) {
	case *asg.StreamElement:
		return nil
	case *asg.AnalyzerElement:
		return checkAndSetAnalyzerAttributes(n)
	case *asg.SentinelElement:
		return nil
	case nil:
		return fmt.Errorf("internal error: %q does not have a ASG element set", n.Name())
	default:
		return fmt.Errorf("internal error: unrecognized ASG element type %T", n.Name())
	}
}
