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

func TestGenAnalyzerDeployment(t *testing.T) {
	_, err := genAnalyzerDeployment(&asg.AnalyzerInfo{
		Name: "name",
		Operator: &operators.OperatorInfo{
			Name: "de_id_0",
		},
		Resources: &asg.ResourceInfo{
			LatencyBudgetMs: 10,
		},
		InputStreams: []*asg.InputStreamInfo{
			&asg.InputStreamInfo{
				Stream: &asg.StreamInfo{
					Name: "analyzer:16000",
				},
			},
		},
	}, &Context{
		RuntimeInfo: &asg.RuntimeInfo{
			IncludeEnv:   true,
			AnalysisName: "analysis",
		},
	})

	if err != nil {
		t.Errorf("error generating deployment: %v", err)
	}
}
