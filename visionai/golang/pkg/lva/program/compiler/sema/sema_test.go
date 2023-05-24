// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package sema

import (
	"fmt"
	"reflect"
	"strings"
	"testing"

	"google3/base/go/runfiles"
	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
	"google3/third_party/visionai/golang/pkg/lva/program/compiler/parse/parse"
	"google3/third_party/visionai/golang/pkg/lva/program/operators/operators"
)

func testOperatorRegistry() (*operators.OperatorRegistry, error) {
	return operators.RegistryFromOpListFile(runfiles.Path("google3/third_party/visionai/golang/pkg/lva/program/data/test_oplist.pbtxt"))
}

type testArguments struct {
	inputText          string
	attributeOverrides []string
}

func runSemaOnText(args testArguments) (*asg.Graph, error) {
	registry, err := testOperatorRegistry()
	if err != nil {
		return nil, err
	}

	pctx := &parse.Context{
		InputProgramText: args.inputText,
		OperatorRegistry: registry,
	}
	asg, err := parse.Parse(pctx)
	if err != nil {
		return nil, err
	}

	sctx := &Context{
		Asg:                asg,
		AttributeOverrides: args.attributeOverrides,
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
	aGraph, err := runSemaOnText(testArguments{
		inputText: analysisText,
	})
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
	_, err := runSemaOnText(testArguments{
		inputText: analysisText,
	})
	checkNil(t, err)

	analysisText = `
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
  analyzer: "protobuf_sink"
  operator: "ProtobufSink"
  inputs: <
    input: "detector:bbox"
  >
>
`
	_, err = runSemaOnText(testArguments{
		inputText: analysisText,
	})
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
	_, err = runSemaOnText(testArguments{
		inputText: analysisText,
	})
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
	_, err = runSemaOnText(testArguments{
		inputText: analysisText,
	})
	checkNotNil(t, err)

	analysisText = `
analyzers: <
  analyzer: "fake"
  operator: "Fake"
>
`
	_, err = runSemaOnText(testArguments{
		inputText: analysisText,
	})
	checkNotNil(t, err)
}

func TestSema_AttributeTest(t *testing.T) {
	analysisText := `
analyzers: <
  analyzer: "attr_only"
  operator: "AttributesOnly"
>
`
	_, err := runSemaOnText(testArguments{
		inputText: analysisText,
	})
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
	aGraph, err := runSemaOnText(testArguments{
		inputText: analysisText,
	})
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
	_, err = runSemaOnText(testArguments{
		inputText: analysisText,
	})
	checkNotNil(t, err)
}

func TestSema_AttributeOverrideTest(t *testing.T) {
	analysisText := `
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

	// Test for successful attribute overrides.
	overrideStringValue := "new!"
	var overrideIntValue int64 = 42
	overrideFloatValue := 2.0
	overrideBoolValue := true
	aGraph, err := runSemaOnText(testArguments{
		inputText: analysisText,
		attributeOverrides: []string{
			fmt.Sprintf("attr_only:required=%v", overrideStringValue),
			fmt.Sprintf("attr_only:not_required=%v", overrideStringValue),
			fmt.Sprintf("attr_only:not_required_int=%v", overrideIntValue),
			fmt.Sprintf("attr_only:not_required_float=%v", overrideFloatValue),
			fmt.Sprintf("attr_only:not_required_bool=%v", overrideBoolValue),
		},
	})
	checkNil(t, err)

	analyzerInfoMap, err := getAnalyzerInfoMap(aGraph)
	checkNil(t, err)

	aInfo, ok := analyzerInfoMap["attr_only"]
	checkTrue(t, ok)

	attrValueInfo, ok := aInfo.Attributes["required"]
	checkTrue(t, ok)
	actualStringValue, ok := attrValueInfo.Value.(string)
	checkTrue(t, ok)
	checkEqual(t, actualStringValue, overrideStringValue)

	attrValueInfo, ok = aInfo.Attributes["not_required"]
	checkTrue(t, ok)
	actualStringValue, ok = attrValueInfo.Value.(string)
	checkTrue(t, ok)
	checkEqual(t, actualStringValue, overrideStringValue)

	attrValueInfo, ok = aInfo.Attributes["not_required_int"]
	checkTrue(t, ok)
	actualIntValue, ok := attrValueInfo.Value.(int64)
	checkTrue(t, ok)
	checkEqual(t, actualIntValue, overrideIntValue)

	attrValueInfo, ok = aInfo.Attributes["not_required_float"]
	checkTrue(t, ok)
	actualFloatValue, ok := attrValueInfo.Value.(float64)
	checkTrue(t, ok)
	checkEqual(t, actualFloatValue, overrideFloatValue)

	attrValueInfo, ok = aInfo.Attributes["not_required_bool"]
	checkTrue(t, ok)
	actualBoolValue, ok := attrValueInfo.Value.(bool)
	checkTrue(t, ok)
	checkEqual(t, actualBoolValue, overrideBoolValue)

	// Test for unknown analyzer.
	_, err = runSemaOnText(testArguments{
		inputText: analysisText,
		attributeOverrides: []string{
			"unknown_analyzer:attr1=val1",
		},
	})
	checkNotNil(t, err)
	wantErrorSubstr := "encountered attribute overrides for an unknown analyzer"
	if !strings.Contains(err.Error(), wantErrorSubstr) {
		t.Fatalf("expected error string to contain %q. Actual message: %q", wantErrorSubstr, err.Error())
	}

	// Test for unknown attribute.
	_, err = runSemaOnText(testArguments{
		inputText: analysisText,
		attributeOverrides: []string{
			"attr_only:attr1=val1",
		},
	})
	checkNotNil(t, err)
	wantErrorSubstr = "which is not defined in its operator"
	if !strings.Contains(err.Error(), wantErrorSubstr) {
		t.Fatalf("expected error string to contain %q. Actual message: %q", wantErrorSubstr, err.Error())
	}

	// Test for bad int override value.
	badOverrideIntValue := "clearly_a_string"
	_, err = runSemaOnText(testArguments{
		inputText: analysisText,
		attributeOverrides: []string{
			fmt.Sprintf("attr_only:not_required_int=%v", badOverrideIntValue),
		},
	})
	checkNotNil(t, err)
	wantErrorSubstr = fmt.Sprintf("but got an override value string of %q", badOverrideIntValue)
	if !strings.Contains(err.Error(), wantErrorSubstr) {
		t.Fatalf("expected error string to contain %q. Actual message: %q", wantErrorSubstr, err.Error())
	}
}

func TestSema_NodeInfoUpdateTest(t *testing.T) {
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
	aGraph, err := runSemaOnText(testArguments{
		inputText: analysisText,
	})
	checkNil(t, err)

	streamInfoMap, err := getStreamInfoMap(aGraph)
	checkNil(t, err)
	streamInfo, ok := streamInfoMap["video_src:output"]
	checkTrue(t, ok)
	checkEqual(t, 1, streamInfo.DownstreamAnalyzers)

	streamInfo, ok = streamInfoMap["audio_src:output"]
	checkTrue(t, ok)
	checkEqual(t, 1, streamInfo.DownstreamAnalyzers)

	streamInfo, ok = streamInfoMap["fake:action"]
	checkTrue(t, ok)
	checkEqual(t, 0, streamInfo.DownstreamAnalyzers)

	streamInfo, ok = streamInfoMap["fake:identity"]
	checkTrue(t, ok)
	checkEqual(t, 0, streamInfo.DownstreamAnalyzers)

	analysisText = `
analyzers: <
  analyzer: "placeholder"
  operator: "Placeholder"
>
analyzers: <
  analyzer: "detector0"
  operator: "FakeDetector"
  inputs: <
    input: "placeholder:output"
  >
>
analyzers: <
  analyzer: "detector1"
  operator: "FakeDetector"
  inputs: <
    input: "placeholder:output"
  >
>`
	aGraph, err = runSemaOnText(testArguments{
		inputText: analysisText,
	})
	checkNil(t, err)

	streamInfoMap, err = getStreamInfoMap(aGraph)
	checkNil(t, err)
	streamInfo, ok = streamInfoMap["placeholder:output"]
	checkTrue(t, ok)
	checkEqual(t, 2, streamInfo.DownstreamAnalyzers)
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
