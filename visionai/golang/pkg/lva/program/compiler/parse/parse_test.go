// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package parse

import (
	"fmt"
	"reflect"
	"testing"

	"google3/base/go/runfiles"
	lvapb "google3/google/cloud/visionai/v1alpha1_lva_go_proto"
	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
	"google3/third_party/visionai/golang/pkg/lva/program/operators/operators"
)

func testOperatorRegistry() (*operators.OperatorRegistry, error) {
	return operators.RegistryFromOpListFile(runfiles.Path("google3/third_party/visionai/golang/pkg/lva/program/data/test_oplist.pbtxt"))
}

func findAnalyzerInfo(name string, aGraph *asg.Graph) (*asg.AnalyzerInfo, error) {
	for _, node := range aGraph.Nodes() {
		analyzerElem, ok := node.Element().(*asg.AnalyzerElement)
		if ok {
			if analyzerElem.Info != nil && analyzerElem.Info.Name == name {
				return analyzerElem.Info, nil
			}
		}
	}
	return nil, fmt.Errorf("could not find analyzer element with name %q", name)
}

func TestBuildAsgFromDipoleDef(t *testing.T) {
	registry, err := testOperatorRegistry()
	checkNil(t, err)

	analysisDef := lvapb.AnalysisDefinition_builder{
		Analyzers: []*lvapb.AnalyzerDefinition{
			lvapb.AnalyzerDefinition_builder{
				Analyzer: "in0_out1",
				Operator: "NoInOneOut",
			}.Build(),
			lvapb.AnalyzerDefinition_builder{
				Analyzer: "in1_out0",
				Operator: "OneInNoOut",
				Inputs: []*lvapb.AnalyzerDefinition_StreamInput{
					lvapb.AnalyzerDefinition_StreamInput_builder{
						Input: "in0_out1:output_0",
					}.Build(),
				},
			}.Build(),
		},
	}.Build()
	aGraph, err := buildAsgFrom(registry, analysisDef)
	checkNil(t, err)
	checkNotNil(t, aGraph)
	checkNodesAndEdges(t, aGraph, 5, 4)

	in1out0AnalyzerInfo, err := findAnalyzerInfo("in1_out0", aGraph)
	checkNil(t, err)
	checkNotNil(t, in1out0AnalyzerInfo)
	checkEqual(t, "in1_out0", in1out0AnalyzerInfo.Name)
	checkEqual(t, 1, len(in1out0AnalyzerInfo.InputStreams))
	checkNil(t, in1out0AnalyzerInfo.InputStreams[0].Stream)
	checkEqual(t, 0, len(in1out0AnalyzerInfo.OutputStreams))
}

func TestBuildAsgFromTwolegTadpoleDef(t *testing.T) {
	registry, err := testOperatorRegistry()
	checkNil(t, err)
	analysisDef := lvapb.AnalysisDefinition_builder{
		Analyzers: []*lvapb.AnalyzerDefinition{
			lvapb.AnalyzerDefinition_builder{
				Analyzer: "in0_out1",
				Operator: "NoInOneOut",
			}.Build(),
			lvapb.AnalyzerDefinition_builder{
				Analyzer: "in1_out2",
				Operator: "OneInTwoOut",
				Inputs: []*lvapb.AnalyzerDefinition_StreamInput{
					lvapb.AnalyzerDefinition_StreamInput_builder{
						Input: "in0_out1:output_0",
					}.Build(),
				},
			}.Build(),
			lvapb.AnalyzerDefinition_builder{
				Analyzer: "in2_out1",
				Operator: "TwoInOneOut",
				Inputs: []*lvapb.AnalyzerDefinition_StreamInput{
					lvapb.AnalyzerDefinition_StreamInput_builder{
						Input: "in1_out2:output_0",
					}.Build(),
					lvapb.AnalyzerDefinition_StreamInput_builder{
						Input: "in1_out2:output_1",
					}.Build(),
				},
			}.Build(),
			lvapb.AnalyzerDefinition_builder{
				Analyzer: "in1_out0",
				Operator: "OneInNoOut",
				Inputs: []*lvapb.AnalyzerDefinition_StreamInput{
					lvapb.AnalyzerDefinition_StreamInput_builder{
						Input: "in2_out1:output_0",
					}.Build(),
				},
			}.Build(),
		},
	}.Build()
	aGraph, err := buildAsgFrom(registry, analysisDef)
	checkNil(t, err)
	checkNotNil(t, aGraph)
	checkNodesAndEdges(t, aGraph, 10, 10)

	in0out1AnalyzerInfo, err := findAnalyzerInfo("in0_out1", aGraph)
	checkNil(t, err)
	checkNotNil(t, in0out1AnalyzerInfo)
	checkEqual(t, "in0_out1", in0out1AnalyzerInfo.Name)
	checkEqual(t, 0, len(in0out1AnalyzerInfo.InputStreams))
	checkEqual(t, 1, len(in0out1AnalyzerInfo.OutputStreams))
	checkNil(t, in0out1AnalyzerInfo.OutputStreams[0].Stream)

	in1out2AnalyzerInfo, err := findAnalyzerInfo("in1_out2", aGraph)
	checkNil(t, err)
	checkNotNil(t, in1out2AnalyzerInfo)
	checkEqual(t, "in1_out2", in1out2AnalyzerInfo.Name)
	checkEqual(t, 1, len(in1out2AnalyzerInfo.InputStreams))
	checkNil(t, in1out2AnalyzerInfo.InputStreams[0].Stream)
	checkEqual(t, 2, len(in1out2AnalyzerInfo.OutputStreams))
	checkNil(t, in1out2AnalyzerInfo.OutputStreams[0].Stream)
	checkNil(t, in1out2AnalyzerInfo.OutputStreams[1].Stream)

	in2out1AnalyzerInfo, err := findAnalyzerInfo("in2_out1", aGraph)
	checkNil(t, err)
	checkNotNil(t, in2out1AnalyzerInfo)
	checkEqual(t, "in2_out1", in2out1AnalyzerInfo.Name)
	checkEqual(t, 2, len(in2out1AnalyzerInfo.InputStreams))
	checkNil(t, in2out1AnalyzerInfo.InputStreams[0].Stream)
	checkNil(t, in2out1AnalyzerInfo.InputStreams[1].Stream)
	checkEqual(t, 1, len(in2out1AnalyzerInfo.OutputStreams))
	checkNil(t, in2out1AnalyzerInfo.OutputStreams[0].Stream)
}

func TestBuildAsgFromBadNames(t *testing.T) {
	registry, err := testOperatorRegistry()
	checkNil(t, err)
	badAnalyzerNameDef := lvapb.AnalysisDefinition_builder{
		Analyzers: []*lvapb.AnalyzerDefinition{
			lvapb.AnalyzerDefinition_builder{
				Analyzer: "badanalyzername!",
				Operator: "NoInOneOut",
				Inputs: []*lvapb.AnalyzerDefinition_StreamInput{
					lvapb.AnalyzerDefinition_StreamInput_builder{
						Input: "good_analyzer:output_0",
					}.Build(),
				},
			}.Build(),
		},
	}.Build()
	aGraph, err := buildAsgFrom(registry, badAnalyzerNameDef)
	checkNotNil(t, err)
	checkNil(t, aGraph)

	badStreamRefDef := lvapb.AnalysisDefinition_builder{
		Analyzers: []*lvapb.AnalyzerDefinition{
			lvapb.AnalyzerDefinition_builder{
				Analyzer: "good_name_1",
				Operator: "NoInOneOut",
			}.Build(),
			lvapb.AnalyzerDefinition_builder{
				Analyzer: "good_name_2",
				Operator: "NoInOneOut",
				Inputs: []*lvapb.AnalyzerDefinition_StreamInput{
					lvapb.AnalyzerDefinition_StreamInput_builder{
						Input: "good_name_1:badref",
					}.Build(),
				},
			}.Build(),
		},
	}.Build()
	aGraph, err = buildAsgFrom(registry, badStreamRefDef)
	checkNotNil(t, err)
	checkNil(t, aGraph)
}

func TestBuildAsgResources(t *testing.T) {
	registry, err := testOperatorRegistry()
	checkNil(t, err)
	{
		analysisDef := lvapb.AnalysisDefinition_builder{
			Analyzers: []*lvapb.AnalyzerDefinition{
				lvapb.AnalyzerDefinition_builder{
					Analyzer: "in0_out0",
					Operator: "NoInNoOut",
				}.Build(),
			},
		}.Build()
		aGraph, err := buildAsgFrom(registry, analysisDef)
		checkNil(t, err)
		checkNotNil(t, aGraph)
		nodes := aGraph.Nodes()
		checkEqual(t, 3, len(nodes))
		analyzerElem, ok := nodes[2].Element().(*asg.AnalyzerElement)
		checkTrue(t, ok)
		checkNotNil(t, analyzerElem.Info)
		resources := analyzerElem.Info.Resources
		checkNotNil(t, resources)
		checkEqual(t, "100m", resources.Cpu)
		checkEqual(t, "100Mi", resources.Memory)
		checkEqual(t, 0, resources.Gpus)
		checkEqual(t, 1000, resources.LatencyBudgetMs)
	}

	{
		analysisDef := lvapb.AnalysisDefinition_builder{
			Analyzers: []*lvapb.AnalyzerDefinition{
				lvapb.AnalyzerDefinition_builder{
					Analyzer: "in0_out0",
					Operator: "NoInNoOut1",
				}.Build(),
			},
		}.Build()
		aGraph, err := buildAsgFrom(registry, analysisDef)
		checkNil(t, err)
		checkNotNil(t, aGraph)
		nodes := aGraph.Nodes()
		checkEqual(t, 3, len(nodes))
		analyzerElem, ok := nodes[2].Element().(*asg.AnalyzerElement)
		checkTrue(t, ok)
		checkNotNil(t, analyzerElem.Info)
		resources := analyzerElem.Info.Resources
		checkNotNil(t, resources)
		checkEqual(t, "100m", resources.Cpu)
		checkEqual(t, "100Mi", resources.Memory)
		checkEqual(t, 1, resources.Gpus)
		checkEqual(t, 0, resources.LatencyBudgetMs)
	}
}

func TestBuildAsgEnvvars(t *testing.T) {
	registry, err := testOperatorRegistry()
	checkNil(t, err)
	analysisDef := lvapb.AnalysisDefinition_builder{
		Analyzers: []*lvapb.AnalyzerDefinition{
			lvapb.AnalyzerDefinition_builder{
				Analyzer: "in0_out0",
				Operator: "NoInNoOut",
				DebugOptions: lvapb.AnalyzerDefinition_DebugOptions_builder{
					EnvironmentVariables: map[string]string{
						"key": "value",
					},
				}.Build(),
			}.Build(),
		},
	}.Build()
	aGraph, err := buildAsgFrom(registry, analysisDef)
	checkNil(t, err)
	checkNotNil(t, aGraph)
	nodes := aGraph.Nodes()
	checkEqual(t, 3, len(nodes))
	analyzerElem, ok := nodes[2].Element().(*asg.AnalyzerElement)
	checkTrue(t, ok)
	checkNotNil(t, analyzerElem.Info)
	resources := analyzerElem.Info.Resources
	checkNotNil(t, resources)
	v, found := resources.Envvars["key"]
	checkTrue(t, found)
	checkEqual(t, v, "value")
}

func checkNodesAndEdges(t *testing.T, g *asg.Graph, nodes, edges int) {
	t.Helper()
	if nodes != g.NumNodes() || edges != g.NumEdges() {
		t.Helper()
		t.Fatalf("want %d nodes and %d edge, got %d nodes and %d edges", nodes, edges, g.NumNodes(), g.NumEdges())
	}
}

func isNil(obj interface{}) bool {
	return obj == nil || reflect.ValueOf(obj).IsNil()
}

func checkNil(t *testing.T, obj interface{}) {
	t.Helper()
	if !isNil(obj) {
		t.Fatalf("want nil, got %v", obj)
	}
}

func checkNotNil(t *testing.T, obj interface{}) {
	t.Helper()
	if isNil(obj) {
		t.Fatal("want non-nil, got nil")
	}
}

func checkTrue(t *testing.T, got bool) {
	t.Helper()
	if !got {
		t.Fatalf("want true, got %t", got)
	}
}

func checkFalse(t *testing.T, got bool) {
	t.Helper()
	if got {
		t.Fatalf("want false, got %t", got)
	}
}

func checkEqual(t *testing.T, want, got interface{}) {
	t.Helper()
	if want != got {
		t.Errorf("want %v, got %v", want, got)
	}
}
