// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package sema

import (
	"fmt"
	"strings"

	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
)

// TODO: This probably shouldn't simply be ignored.
// It needs to check it doesn't connect to different types,
// even if that one type can be anything.
func isIgnorableType(t string) bool {
	return t == "special/any"
}

// It needs to allow the operator to accept the subtype input from the upstream operator.
// e.g.:
// protobuf/google.cloud.visionai.v1.VideoActionRecognitionPredictionResult -> protobuf
func isSubtype(t1 string, t2 string) bool {
	return strings.HasPrefix(t1, t2)
}

func areStreamTypesMatched(t1 string, t2 string) bool {
	if isIgnorableType(t1) || isIgnorableType(t2) || isSubtype(t1, t2) {
		return true
	}
	return t1 == t2
}

// computeAtAnalyzerElement does the following at the AnalyzerElement:
// + Does type matching on its inputs.
// + Sets the input StreamInfo slice.
// + Initializes output StreamInfo slice to the expected length,
//   but with nils. These will be filled by computeAtStreamElement.
func computeAtAnalyzerElement(n *asg.Node) error {
	analyzerElement, ok := n.Element().(*asg.AnalyzerElement)
	if !ok {
		return fmt.Errorf("internal error: %q is not an Analyzer (got %T)", n.Name(), n.Element())
	}
	analyzerInfo := analyzerElement.Info
	operator := analyzerInfo.Operator

	// Figure out which input stream is incident on which input op argument.
	inputIdxStreamInfoMap := make(map[int]*asg.StreamInfo)
	for _, e := range n.InEdges() {
		neighbor := e.Src()
		inputIdx := e.DinIdx()

		// Skip sentinel.
		if neighbor.IsSource() || neighbor.IsSink() {
			continue
		}

		// These should not happen unless there is a bug.
		if _, exists := inputIdxStreamInfoMap[inputIdx]; exists {
			return fmt.Errorf("internal error: %q has a duplicate input edge at index %d", n.Name(), inputIdx)
		}

		streamElement, ok := neighbor.Element().(*asg.StreamElement)
		if !ok {
			return fmt.Errorf("internal error: %q has a non-stream input (%T) at index %d", n.Name(), neighbor.Element(), inputIdx)
		}

		inputIdxStreamInfoMap[inputIdx] = streamElement.Info
	}

	// Check that every input argument of the analyzer's Operator
	// has a stream of the correct type matched.
	inputStreamInfo := []*asg.StreamInfo{}
	for i, argumentInfo := range operator.InputArgs {
		streamInfo, ok := inputIdxStreamInfoMap[i]
		if !ok {
			return fmt.Errorf("%q does not have an input stream for argument %q of operator %q", n.Name(), argumentInfo.Name, operator.Name)
		}
		if !areStreamTypesMatched(streamInfo.Type, argumentInfo.Type) {
			return fmt.Errorf("input argument %q of operator %q called from analyzer %q requires a stream of type %q but got stream %q which is of type %q", argumentInfo.Name, operator.Name, n.Name(), argumentInfo.Type, streamInfo.Name, streamInfo.Type)
		}
		inputStreamInfo = append(inputStreamInfo, streamInfo)
		delete(inputIdxStreamInfoMap, i)
	}

	// Check that there were no extra input streams specified.
	if len(inputIdxStreamInfoMap) != 0 {
		var extraStreamNames []string
		for _, v := range inputIdxStreamInfoMap {
			extraStreamNames = append(extraStreamNames, v.Name)
		}
		return fmt.Errorf("%q given extra stream input arguments (%s)", n.Name(), strings.Join(extraStreamNames, ", "))
	}

	// Set the input StreamInfo.
	if len(analyzerInfo.InputStreams) != len(inputStreamInfo) {
		return fmt.Errorf("internal error: %q has allocated %d InputStreamInfo slots but got %d input streams", n.Name(), len(analyzerInfo.InputStreams), len(inputStreamInfo))
	}
	for i, streamInfo := range inputStreamInfo {
		analyzerInfo.InputStreams[i].Stream = streamInfo
	}

	// Check that outputs are in bounds.
	//
	// We do not do type matching as this is deferred to the
	// dependent element.
	for _, e := range n.OutEdges() {
		neighbor := e.Dst()
		outputIdx := e.SoutIdx()

		// Skip sentinel.
		if neighbor.IsSource() || neighbor.IsSink() {
			continue
		}

		if outputIdx >= len(operator.OutputArgs) {
			return fmt.Errorf("%q has an output index (%d) that is out of bounds", operator.Name, outputIdx)
		}
	}
	return nil
}

// computeAtStreamElement does type checking at a StreamElement.
// It does type matching on the inputs, and sets the incoming
// AnalyzerElement's output StreamInfo.
func computeAtStreamElement(n *asg.Node) error {
	streamElement, ok := n.Element().(*asg.StreamElement)
	if !ok {
		return fmt.Errorf("internal error: %q is not a StreamElement (got %T)", n.Name(), n.Element())
	}
	streamInfo := streamElement.Info

	// Check inputs.
	for _, e := range n.InEdges() {
		if e.Src().IsSource() {
			continue
		}

		// Input must be at the 0'th index.
		if e.DinIdx() != 0 {
			return fmt.Errorf("StreamElement must only have input at the 0'th index. %q has it at %d", n.Name(), e.DinIdx())
		}

		// Input must be from an analyzer.
		analyzerElement, ok := e.Src().Element().(*asg.AnalyzerElement)
		if !ok {
			return fmt.Errorf("%q got a non-analyzer input (%T)", n.Name(), e.Src().Element())
		}
		analyzerInfo := analyzerElement.Info
		operator := analyzerInfo.Operator
		operatorArgInfoList := operator.OutputArgs

		// Analyzer output type must match.
		analyzerOutputIdx := e.SoutIdx()
		if analyzerOutputIdx >= len(operatorArgInfoList) {
			return fmt.Errorf("%q got an input from a non-existent output argument index (%d) of operator %q from %q", n.Name(), analyzerOutputIdx, operator.Name, e.Src().Name())
		}
		analyzerOutputType := operatorArgInfoList[analyzerOutputIdx].Type
		if !areStreamTypesMatched(streamInfo.Type, analyzerOutputType) {
			return fmt.Errorf("%q expects an input of type %q but got type %q from analyzer %q", n.Name(), streamInfo.Type, e.Src().Name(), analyzerOutputType)

		}

		// Set the StreamInfo at the AnalyzerElement.
		outputStreamInfo := analyzerInfo.OutputStreams
		if analyzerOutputIdx >= len(outputStreamInfo) {
			return fmt.Errorf("internal error: out of bound access to AnalyzerElement %q's output stream info", analyzerInfo.Name)
		}
		outputStreamInfo[analyzerOutputIdx].Stream = streamInfo
	}

	// Check outputs are leaving index 0.
	//
	// We don't do argument type matching here as it is deferred
	// to dependents.
	for _, e := range n.OutEdges() {
		if e.Dst().IsSink() {
			continue
		}

		// Output must be from the 0'th index.
		if e.SoutIdx() != 0 {
			return fmt.Errorf("StreamElement must only have output at the 0'th index. %q has it at %d", n.Name(), e.SoutIdx())
		}
	}

	return nil
}

// Top level argument checker.
//
// It is expected to be run as a visitor with ReverseDFS.
func checkAndSetArgumentInformation(n *asg.Node) error {
	element := n.Element()
	switch element.(type) {
	case *asg.StreamElement:
		return computeAtStreamElement(n)
	case *asg.AnalyzerElement:
		return computeAtAnalyzerElement(n)
	case *asg.SentinelElement:
		return nil
	case nil:
		return fmt.Errorf("internal error: %q does not have a ASG element set", n.Name())
	default:
		return fmt.Errorf("internal error: unrecognized ASG element type %T", n.Name())
	}
}
