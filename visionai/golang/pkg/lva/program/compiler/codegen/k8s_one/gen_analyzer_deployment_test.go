// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package k8s

import (
	"testing"

	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
	"google3/third_party/visionai/golang/pkg/lva/program/operators/operators"
)

func TestGenStreamSourceAnalyzerDeployment(t *testing.T) {
	_, err := genAnalyzerDeployment(&asg.AnalyzerInfo{
		Name: "name",
		Operator: &operators.OperatorInfo{
			Name: "StreamSource",
		},
		Resources: &asg.ResourceInfo{
			LatencyBudgetMs: 10,
			Cpu:             "1",
			CpuLimits:       "2",
			Memory:          "1Gi",
			MemoryLimits:    "2Gi",
		},
	}, &Context{
		FeatureOptions: FeatureOptions{
			RunMode: "live",
			RuntimeInfo: &asg.RuntimeInfo{
				IncludeEnv:   true,
				AnalysisName: "analysis",
			},
		},
	})

	if err != nil {
		t.Errorf("error generating deployment: %v", err)
	}
}

func TestGenGeneralSourceAnalyzerDeployment(t *testing.T) {
	_, err := genAnalyzerDeployment(&asg.AnalyzerInfo{
		Name: "name",
		Operator: &operators.OperatorInfo{
			Name: "de_id_0",
		},
		Resources: &asg.ResourceInfo{
			LatencyBudgetMs: 10,
			Cpu:             "1",
			CpuLimits:       "2",
			Memory:          "1Gi",
			MemoryLimits:    "2Gi",
		},
		InputStreams: []*asg.InputStreamInfo{
			&asg.InputStreamInfo{
				Stream: &asg.StreamInfo{
					Name: "analyzer:16000",
				},
			},
		},
	}, &Context{
		FeatureOptions: FeatureOptions{
			RunMode: "live",
			RuntimeInfo: &asg.RuntimeInfo{
				IncludeEnv:   true,
				AnalysisName: "analysis",
			},
		},
	})

	if err != nil {
		t.Errorf("error generating deployment: %v", err)
	}
}

func TestDeIDAnalyzerDeployment(t *testing.T) {
	analyzerAttrMap := make(map[string]*asg.AttributeValueInfo)
	var frameRate int64 = 6
	analyzerAttrMap["frame_rate"] = &asg.AttributeValueInfo{
		Type:  "int",
		Value: frameRate,
	}
	_, err := genAnalyzerDeployment(&asg.AnalyzerInfo{
		Name: "name",
		Operator: &operators.OperatorInfo{
			Name: "de_id_0",
		},
		Attributes: analyzerAttrMap,
		Resources: &asg.ResourceInfo{
			LatencyBudgetMs: 10,
			Cpu:             "1",
			CpuLimits:       "2",
			Memory:          "1Gi",
			MemoryLimits:    "2Gi",
		},
		InputStreams: []*asg.InputStreamInfo{
			&asg.InputStreamInfo{
				Stream: &asg.StreamInfo{
					Name: "analyzer:16000",
				},
			},
		},
	}, &Context{
		FeatureOptions: FeatureOptions{
			RunMode: "live",
			RuntimeInfo: &asg.RuntimeInfo{
				IncludeEnv:   true,
				AnalysisName: "analysis",
			},
		},
	})

	if err != nil {
		t.Errorf("error generating deployment: %v", err)
	}
}

func TestGenGcsVideoSourceAnalyzerDeployment(t *testing.T) {
	analyzerAttrMap := make(map[string]*asg.AttributeValueInfo)
	analyzerAttrMap["input_video_gcs_path"] = &asg.AttributeValueInfo{
		Type:  "string",
		Value: "gs://test-bucket/test-video.mp4",
	}
	_, err := genAnalyzerDeployment(&asg.AnalyzerInfo{
		Name: "name",
		Operator: &operators.OperatorInfo{
			Name: "GcsVideoSource",
		},
		Resources: &asg.ResourceInfo{
			LatencyBudgetMs: 10,
			Cpu:             "1",
			CpuLimits:       "2",
			Memory:          "1Gi",
			MemoryLimits:    "2Gi",
		},
		Attributes: analyzerAttrMap,
	}, &Context{
		FeatureOptions: FeatureOptions{
			RunMode: "submission",
			RuntimeInfo: &asg.RuntimeInfo{
				IncludeEnv:   true,
				AnalysisName: "analysis",
			},
		},
	})

	if err != nil {
		t.Errorf("error generating deployment: %v", err)
	}
}

func TestGenWarehouseVideoSourceAnalyzerDeployment(t *testing.T) {
	analyzerAttrMap := make(map[string]*asg.AttributeValueInfo)
	analyzerAttrMap["warehouse_endpoint"] = &asg.AttributeValueInfo{
		Type:  "string",
		Value: "http://localhost:0",
	}
	analyzerAttrMap["asset_name"] = &asg.AttributeValueInfo{
		Type:  "string",
		Value: "projects/123345/locations/us-west1/corpora/342533/assets/3432523",
	}
	analyzerAttrMap["fast_mode"] = &asg.AttributeValueInfo{
		Type:  "bool",
		Value: false,
	}
	_, err := genAnalyzerDeployment(&asg.AnalyzerInfo{
		Name: "name",
		Operator: &operators.OperatorInfo{
			Name: "WarehouseVideoSource",
		},
		Resources: &asg.ResourceInfo{
			LatencyBudgetMs: 10,
			Cpu:             "1",
			CpuLimits:       "2",
			Memory:          "1Gi",
			MemoryLimits:    "2Gi",
		},
		Attributes: analyzerAttrMap,
	}, &Context{
		FeatureOptions: FeatureOptions{
			RunMode: "submission",
			RuntimeInfo: &asg.RuntimeInfo{
				IncludeEnv:   true,
				AnalysisName: "analysis",
			},
		},
	})

	if err != nil {
		t.Errorf("error generating deployment: %v", err)
	}
}
