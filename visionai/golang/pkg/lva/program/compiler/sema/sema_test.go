// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package sema

import (
	"reflect"
	"testing"

	"google3/base/go/runfiles"
	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
	"google3/third_party/visionai/golang/pkg/lva/program/compiler/parse/parse"
	"google3/third_party/visionai/golang/pkg/lva/program/operators/operators"
)

func testOperatorRegistry() (*operators.OperatorRegistry, error) {
	return operators.RegistryFromOpListFile(runfiles.Path("google3/third_party/visionai/golang/pkg/lva/program/data/test_oplist.pbtxt"))
}

func runSemaOnText(inputText string) (*asg.Graph, error) {
	registry, err := testOperatorRegistry()
	if err != nil {
		return nil, err
	}

	pctx := &parse.Context{
		InputProgramText: inputText,
		OperatorRegistry: registry,
	}
	asg, err := parse.Parse(pctx)
	if err != nil {
		return nil, err
	}

	sctx := &Context{
		Asg: asg,
	}

	if err := Sema(sctx); err != nil {
		return nil, err
	}

	return sctx.Asg, nil
}

func getStreamInfoMap(aGraph *asg.Graph) (map[string]*asg.StreamInfo, error) {
	streamInfoMap := make(map[string]*asg.StreamInfo)
	for _, n := range aGraph.Nodes() {
		switch elem := n.Element().(type) {
		case *asg.StreamElement:
			streamInfoMap[n.Name()] = elem.Info
		}
	}
	return streamInfoMap, nil
}

func getAnalyzerInfoMap(aGraph *asg.Graph) (map[string]*asg.AnalyzerInfo, error) {
	analyzerInfoMap := make(map[string]*asg.AnalyzerInfo)
	for _, n := range aGraph.Nodes() {
		switch elem := n.Element().(type) {
		case *asg.AnalyzerElement:
			analyzerInfoMap[n.Name()] = elem.Info
		}
	}
	return analyzerInfoMap, nil
}

func TestSema_BasicPipeline(t *testing.T) {
	analysisText := `
analyzers: <
  analyzer: "placeholder"
  operator: "Placeholder"
>
analyzers: <
  analyzer: "detector"
  operator: "FakeDetector"
  inputs: <
    input: "placeholder:output"
  >
>
analyzers: <
  analyzer: "bbox_counter"
  operator: "BboxCounter"
  inputs: <
    input: "detector:bbox"
  >
>
analyzers: <
  analyzer: "stdout_sink"
  operator: "StdoutSink"
  inputs: <
    input: "bbox_counter:count"
  >
>
`
	aGraph, err := runSemaOnText(analysisText)
	checkNil(t, err)

	streamInfoMap, err := getStreamInfoMap(aGraph)
	checkNil(t, err)
	checkEqual(t, 5, len(streamInfoMap))

	analyzerInfoMap, err := getAnalyzerInfoMap(aGraph)
	checkNil(t, err)
	checkEqual(t, 4, len(analyzerInfoMap))

	for _, aInfo := range analyzerInfoMap {
		inputStreamInfo := aInfo.InputStreams
		for _, isInfo := range inputStreamInfo {
			sInfo := isInfo.Stream
			checkNotNil(t, sInfo)
			streamInfo, ok := streamInfoMap[sInfo.Name]
			checkTrue(t, ok)
			checkEqual(t, sInfo, streamInfo)
		}

		outputStreamInfo := aInfo.OutputStreams
		for _, osInfo := range outputStreamInfo {
			sInfo := osInfo.Stream
			checkNotNil(t, sInfo)
			streamInfo, ok := streamInfoMap[sInfo.Name]
			checkTrue(t, ok)
			checkEqual(t, sInfo, streamInfo)
		}

		operatorAttributes := aInfo.Operator.Attributes
		attributes := aInfo.Attributes
		checkEqual(t, len(attributes), len(operatorAttributes))
		for _, opAttributeInfo := range operatorAttributes {
			attributeValueInfo, ok := attributes[opAttributeInfo.Name]
			checkTrue(t, ok)
			checkEqual(t, opAttributeInfo.Type, attributeValueInfo.Type)
			_, ok = attributeValueInfo.Value.(float32)
			checkTrue(t, ok)
		}
	}
}

func TestSema_ArgumentTest(t *testing.T) {
	analysisText := `
analyzers: <
  analyzer: "video_src"
  operator: "VideoSource"
>
analyzers: <
  analyzer: "audio_src"
  operator: "AudioSource"
>
analyzers: <
  analyzer: "fake"
  operator: "Fake"
  inputs: <
    input: "video_src:output"
  >
  inputs: <
    input: "audio_src:output"
  >
>
`
	_, err := runSemaOnText(analysisText)
	checkNil(t, err)

	analysisText = `
analyzers: <
  analyzer: "video_src"
  operator: "VideoSource"
>
analyzers: <
  analyzer: "audio_src"
  operator: "AudioSource"
>
analyzers: <
  analyzer: "fake"
  operator: "Fake"
  inputs: <
    input: "audio_src:output"
  >
  inputs: <
    input: "video_src:output"
  >
>
`
	_, err = runSemaOnText(analysisText)
	checkNotNil(t, err)

	analysisText = `
analyzers: <
  analyzer: "video_src"
  operator: "VideoSource"
>
analyzers: <
  analyzer: "audio_src"
  operator: "AudioSource"
>
analyzers: <
  analyzer: "fake"
  operator: "Fake"
  inputs: <
    input: "video_src:output"
  >
>
`
	_, err = runSemaOnText(analysisText)
	checkNotNil(t, err)

	analysisText = `
analyzers: <
  analyzer: "fake"
  operator: "Fake"
>
`
	_, err = runSemaOnText(analysisText)
	checkNotNil(t, err)
}

func TestSema_AttributeTest(t *testing.T) {
	analysisText := `
analyzers: <
  analyzer: "attr_only"
  operator: "AttributesOnly"
>
`
	_, err := runSemaOnText(analysisText)
	checkNotNil(t, err)

	analysisText = `
analyzers: <
  analyzer: "attr_only"
  operator: "AttributesOnly"
  attrs: <
    key: "required"
    value: <
      s: "explicitly_supplied"
    >
  >
>
`
	aGraph, err := runSemaOnText(analysisText)
	checkNil(t, err)

	analyzerInfoMap, err := getAnalyzerInfoMap(aGraph)
	checkNil(t, err)

	aInfo, ok := analyzerInfoMap["attr_only"]
	checkTrue(t, ok)

	attrValueInfo, ok := aInfo.Attributes["not_required"]
	checkTrue(t, ok)
	defaultValue, ok := attrValueInfo.Value.(string)
	checkTrue(t, ok)
	checkEqual(t, "hello!", defaultValue)

	analysisText = `
analyzers: <
  analyzer: "attr_only"
  operator: "AttributesOnly"
  attrs: <
    key: "non_existent_key"
    value: <
      s: "something"
    >
  >
  attrs: <
    key: "required"
    value: <
      s: "explicitly_supplied"
    >
  >
>
`
	_, err = runSemaOnText(analysisText)
	checkNotNil(t, err)
}

func checkNil(t *testing.T, obj interface{}) {
	t.Helper()
	if obj != nil && !reflect.ValueOf(obj).IsNil() {
		t.Fatalf("want nil, got %v", obj)
	}
}

func checkNotNil(t *testing.T, obj interface{}) {
	t.Helper()
	if obj == nil || reflect.ValueOf(obj).IsNil() {
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
