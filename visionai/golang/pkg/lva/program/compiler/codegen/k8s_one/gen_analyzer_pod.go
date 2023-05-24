// Copyright 2023 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package k8s

import (
	"google3/third_party/golang/k8s_io/api/v/v0_23/core/v1/v1"
	metav1 "google3/third_party/golang/k8s_io/apimachinery/v/v0_23/pkg/apis/meta/v1/v1"
	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
	"google3/third_party/visionai/golang/pkg/lva/program/util/util"
)

// genAnalyzerPod generates a pod definition based on the given analyzer info. The pod will never restart.
// This is used for submission mode.
func genAnalyzerPod(info *asg.AnalyzerInfo, ctx *Context) (string, error) {
	str, err := genAnalyzerDeployment(info, ctx)
	if err != nil {
		return "", err
	}
	// Parse a deployment out of the yaml.
	deployment, err := util.ParseK8sDeploymentFromYaml(str)
	if err != nil {
		return "", err
	}

	metadata := deployment.Spec.Template.ObjectMeta
	// Use the same namespace and name as deployment.
	metadata.SetName(deployment.Name)
	metadata.SetNamespace(deployment.Namespace)

	spec := deployment.Spec.Template.Spec
	// Never restart the pod.
	spec.RestartPolicy = v1.RestartPolicyNever

	pod := &v1.Pod{
		TypeMeta: metav1.TypeMeta{
			Kind:       "Pod",
			APIVersion: "v1",
		},
		ObjectMeta: metadata,
		Spec:       spec,
	}

	// Serialize results back into a yaml.
	podString, err := util.SerializeK8sObjectIntoYaml(pod)
	if err != nil {
		return "", err
	}
	return podString, nil
}
