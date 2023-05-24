// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package k8s

import (
	"fmt"

	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
)

// genAnalyzer is the top level function that generates k8s code
// for the given analyzer.
func genAnalyzer(info *asg.AnalyzerInfo, ctx *Context) error {
	if info == nil {
		return fmt.Errorf("internal error: got a nil AnalyzerInfo")
	}

	switch ctx.FeatureOptions.RunMode {
	case "submission":
		// Generate the analyzer's pod.
		// Pod resources will also be filled.
		podString, err := genAnalyzerPod(info, ctx)
		if err != nil {
			return err
		}
		if podString != "" {
			ctx.yamls = append(ctx.yamls, podString)
		}
	default:
		// Generate the analyzer's deployment.
		// Deployment resources will also be filled.
		deploymentString, err := genAnalyzerDeployment(info, ctx)
		if err != nil {
			return err
		}
		if deploymentString != "" {
			ctx.yamls = append(ctx.yamls, deploymentString)
		}
	}

	// Generate the analyzer's services.
	serviceString, err := genAnalyzerService(info, ctx)
	if err != nil {
		return err
	}
	if serviceString != "" {
		ctx.yamls = append(ctx.yamls, serviceString)
	}

	return nil
}
