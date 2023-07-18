// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package parse

import (
	"fmt"

	lvapb "google3/google/cloud/visionai/v1alpha1_lva_go_proto"
	"google3/third_party/golang/protobuf/v2/encoding/prototext/prototext"
	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
	"google3/third_party/visionai/golang/pkg/lva/program/operators/operators"
)

func buildAsg(ctx *Context) error {
	analysisDef, err := readAnalysisDefinition(ctx)
	if err != nil {
		return err
	}

	aGraph, err := buildAsgFrom(ctx.OperatorRegistry, analysisDef, ctx.OperatorsInfo)
	if err != nil {
		return fmt.Errorf("failed to build the ASG the given AnalysisDefinition: %v", err)
	}
	ctx.aGraph = aGraph
	return nil
}

func readAnalysisDefinition(ctx *Context) (*lvapb.AnalysisDefinition, error) {
	// TODO: Currently only support protobuf textformat. Extend to binary
	// and/or json as needed.
	analysisDefinition := &lvapb.AnalysisDefinition{}
	if err := prototext.Unmarshal([]byte(ctx.InputProgramText), analysisDefinition); err != nil {
		return nil, fmt.Errorf("failed to unmarshall the input as a textformatted analysis definition protobuf: %v", err)
	}
	return analysisDefinition, nil
}

func toAttributeValueInfo(attrVal *lvapb.AttributeValue) (*asg.AttributeValueInfo, error) {
	attrValueInfo := &asg.AttributeValueInfo{}
	if !attrVal.HasValue() {
		return nil, fmt.Errorf("AttributeValue.Value is not set")
	}
	switch attrVal.WhichValue() {
	case lvapb.AttributeValue_I_case:
		attrValueInfo.Type = "int"
		attrValueInfo.Value = attrVal.GetI()
	case lvapb.AttributeValue_F_case:
		attrValueInfo.Type = "float"
		attrValueInfo.Value = attrVal.GetF()
	case lvapb.AttributeValue_B_case:
		attrValueInfo.Type = "bool"
		attrValueInfo.Value = attrVal.GetB()
	case lvapb.AttributeValue_S_case:
		attrValueInfo.Type = "string"
		attrValueInfo.Value = string(attrVal.GetS())
	default:
		return nil, fmt.Errorf("AttributeValue.Value has unexpected type %T", attrVal.WhichValue())
	}
	return attrValueInfo, nil
}

func fillSentinelInfo(sentinelNode *asg.Node) error {
	sentinelElement, ok := sentinelNode.Element().(*asg.SentinelElement)
	if !ok {
		return fmt.Errorf("given a Node (%q) that is not a sentinel",
			sentinelElement.Name())
	}
	sentenelInfo := sentinelElement.Info

	inputNodeNames := []string{}
	for _, edge := range sentinelNode.InEdges() {
		inputNodeNames = append(inputNodeNames, edge.Src().Element().Name())
	}
	sentenelInfo.InputNodeNames = inputNodeNames

	outputNodeNames := []string{}
	for _, edge := range sentinelNode.OutEdges() {
		outputNodeNames = append(outputNodeNames, edge.Dst().Element().Name())
	}
	sentenelInfo.OutputNodeNames = outputNodeNames

	return nil
}

func buildAsgFrom(opRegistry *operators.OperatorRegistry, analysisDef *lvapb.AnalysisDefinition, opInfo map[string]*operators.OperatorInfo) (*asg.Graph, error) {
	aGraph, err := asg.NewGraph()
	if err != nil {
		return nil, fmt.Errorf("failed to create an empty graph: %v", err)
	}

	// Use a symbol table for the following purposes:
	// + Detect duplicate definitions.
	// + Detect references to undefined names.
	// + Easy access to corresponding asg.Node.
	symtab := make(map[string]*asg.Node)

	// Create the analyzer nodes.
	for _, analyzerDef := range analysisDef.GetAnalyzers() {
		analyzerName := analyzerDef.GetAnalyzer()
		if !IsValidAnalyzerName(analyzerName) {
			return nil, fmt.Errorf("given an invalid analyzer name %q. It must match %q", analyzerName, analyzerNamePattern)
		}

		if _, exists := symtab[analyzerName]; exists {
			return nil, fmt.Errorf("analyzer names must be unique: %q is used multiple times", analyzerName)
		}

		var analyzerOpInfo *operators.OperatorInfo
		if opInfo == nil || len(opInfo) == 0 {
			// TODO(b/283698767): Remove this branch after migration to new operator registry is done.
			info, err := opRegistry.Lookup(analyzerDef.GetOperator())
			if err != nil {
				return nil, fmt.Errorf("analyzer %q requested to run an unknown operator %q", analyzerName, analyzerDef.GetOperator())
			}
			analyzerOpInfo = info
		} else {
			info, ok := opInfo[analyzerDef.GetAnalyzer()]
			if !ok {
				return nil, fmt.Errorf("analyzer %q requested to run an unknown operator %q", analyzerName, analyzerDef.GetOperator())
			}
			analyzerOpInfo = info
		}

		analyzerAttributes := make(map[string]*asg.AttributeValueInfo)
		for attrName, attrVal := range analyzerDef.GetAttrs() {
			// TODO: This should be checked during operator registration.
			if !IsValidOperatorAttributeName(attrName) {
				return nil, fmt.Errorf("given an invalid attribute name %q. It must match %q", attrName, operatorAttributeNamePattern)
			}
			if _, exists := analyzerAttributes[attrName]; exists {
				return nil, fmt.Errorf("an analyzer attribute must not be specified more than once: please check attribute %q in analyzer %q", attrName, analyzerName)
			}
			attrValueInfo, err := toAttributeValueInfo(attrVal)
			if err != nil {
				return nil, fmt.Errorf("the value of attribute %q for analyzer %q was not specified correctly: %v", attrName, analyzerName, err)
			}
			analyzerAttributes[attrName] = attrValueInfo
		}

		analyzerResources := &asg.ResourceInfo{
			Cpu:             analyzerOpInfo.Resources.Cpu,
			CpuLimits:       analyzerOpInfo.Resources.CpuLimits,
			Memory:          analyzerOpInfo.Resources.Memory,
			MemoryLimits:    analyzerOpInfo.Resources.MemoryLimits,
			Gpus:            analyzerOpInfo.Resources.Gpus,
			Envvars:         map[string]string{},
			LatencyBudgetMs: analyzerOpInfo.Resources.LatencyBudgetMs,
		}
		if analyzerDef.GetDebugOptions() != nil {
			for k, v := range analyzerDef.GetDebugOptions().GetEnvironmentVariables() {
				analyzerResources.Envvars[k] = v
			}
		}

		analyzerNode, err := aGraph.AddNode()
		if err != nil {
			return nil, fmt.Errorf("internal error: %v", err)
		}
		analyzerNode.SetElement(
			&asg.AnalyzerElement{
				Info: &asg.AnalyzerInfo{
					Name:       analyzerName,
					Operator:   analyzerOpInfo,
					Attributes: analyzerAttributes,
					Resources:  analyzerResources,
				},
			},
		)
		symtab[analyzerName] = analyzerNode
	}

	// Create the stream nodes and analyzer output edges that
	// produces data for the stream.
	for _, analyzerDef := range analysisDef.GetAnalyzers() {
		analyzerName := analyzerDef.GetAnalyzer()
		analyzerNode, ok := symtab[analyzerName]
		if !ok {
			return nil, fmt.Errorf("internal error: %v", err)
		}
		analyzerInfo := analyzerNode.Element().(*asg.AnalyzerElement).Info

		outputStreamInfoList := []*asg.OutputStreamInfo{}
		for outIdx, outArgsInfo := range analyzerInfo.Operator.OutputArgs {

			streamName, err := JoinAnalyzerOutputName(analyzerName, outArgsInfo.Name)
			if err != nil {
				return nil, fmt.Errorf("could not form a valid analyzer output name: %v", err)
			}
			if _, exists := symtab[streamName]; exists {
				return nil, fmt.Errorf("the name %q has already been defined", streamName)
			}

			streamNode, err := aGraph.AddNode()
			if err != nil {
				return nil, fmt.Errorf("internal error: %v", err)
			}
			streamInfo := &asg.StreamInfo{
				Name: streamName,
				Type: outArgsInfo.Type,
			}
			streamNode.SetElement(
				&asg.StreamElement{
					Info: streamInfo,
				},
			)
			symtab[streamName] = streamNode

			_, err = aGraph.AddEdge(analyzerNode, outIdx, streamNode, 0)
			if err != nil {
				return nil, fmt.Errorf("internal error: %v", err)
			}

			// TODO: Fix the asymmetry between this and the input stream info.
			// This kind of caching action seems to be a post parse thing.
			outputStreamInfoList = append(outputStreamInfoList, &asg.OutputStreamInfo{})
		}
		analyzerInfo.OutputStreams = outputStreamInfoList
	}

	// Add analyzer input edges.
	for _, analyzerDef := range analysisDef.GetAnalyzers() {
		analyzerNode, ok := symtab[analyzerDef.GetAnalyzer()]
		if !ok {
			return nil, fmt.Errorf("internal error: %v", err)
		}

		inputStreamInfoList := []*asg.InputStreamInfo{}
		for inIdx, streamInput := range analyzerDef.GetInputs() {
			inputName := streamInput.GetInput()
			streamNode, ok := symtab[inputName]
			if !ok {
				return nil, fmt.Errorf("analyzer %q references an undefined input %q", analyzerDef.GetAnalyzer(), inputName)
			}

			_, err = aGraph.AddEdge(streamNode, 0, analyzerNode, inIdx)
			if err != nil {
				return nil, fmt.Errorf("internal error: %v", err)
			}

			// Note: Filling contents of the input stream info is deferred to
			// sema.go. Here we just make sure an object is available.
			//
			// TODO: Remove this asymmetry.
			inputStreamInfoList = append(inputStreamInfoList, &asg.InputStreamInfo{})
		}
		analyzerInfo := analyzerNode.Element().(*asg.AnalyzerElement).Info
		analyzerInfo.InputStreams = inputStreamInfoList
	}

	// Connect Source/Sink sentinels.
	err = asg.FixupSourceAndSinkEdges(aGraph)
	if err != nil {
		return nil, fmt.Errorf("internal error: %v", err)
	}
	// Fill sentinel info.
	//
	// TODO: This might also be better factored elsewhere.
	//       Fix this when we consolidate the input/output filling.
	err = fillSentinelInfo(aGraph.SourceNode())
	if err != nil {
		return nil, fmt.Errorf("internal error: %v", err)
	}
	err = fillSentinelInfo(aGraph.SinkNode())
	if err != nil {
		return nil, fmt.Errorf("internal error: %v", err)
	}
	return aGraph, nil
}
