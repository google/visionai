// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package k8s

import (
	"fmt"
	"strconv"

	"google3/third_party/golang/kubeapi/core/v1/v1"
	"google3/third_party/golang/kubeapimachinery/pkg/api/resource/resource"

	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
	"google3/third_party/visionai/golang/pkg/lva/program/util/util"
)

// genAnalyzerCpuAndMemory fills cpu and memory resource requests of an analyzer
// into the given ResourceRequirements.
func genAnalyzerCpuAndMemory(reqs *v1.ResourceRequirements, info *asg.AnalyzerInfo) error {
	requests := reqs.Requests
	if info.Resources.Cpu != "" {
		resourceName := v1.ResourceName("cpu")
		_, ok := requests[resourceName]
		if ok {
			return fmt.Errorf("internal error: cpu requests already present")
		}
		q, err := resource.ParseQuantity(info.Resources.Cpu)
		if err != nil {
			return fmt.Errorf("cannot parse cpu value %q", info.Resources.Cpu)
		}
		requests[resourceName] = q
	}
	if info.Resources.Memory != "" {
		resourceName := v1.ResourceName("memory")
		_, ok := requests[resourceName]
		if ok {
			return fmt.Errorf("internal error: memory requests already present")
		}
		q, err := resource.ParseQuantity(info.Resources.Memory)
		if err != nil {
			return fmt.Errorf("cannot parse memory value %q", info.Resources.Memory)
		}
		requests[resourceName] = q
	}
	return nil
}

// fillGpuNodeSelector populates a node selector map to indicate the
// required GPU make.
//
// TODO: Clean this up and put the GPU make in the ASG info.
func fillGpuNodeSelector(info *asg.AnalyzerInfo) (*map[string]string, error) {
	if info.Resources.Gpus <= 0 {
		return nil, nil
	}
	nodeSelector := map[string]string{
		"cloud.google.com/gke-accelerator": "nvidia-tesla-t4",
	}
	return &nodeSelector, nil
}

// genAnalyzerGpus fills gpu resource information specified in an analyzer
// into the given ResourceRequirements.
func genAnalyzerGpus(reqs *v1.ResourceRequirements, info *asg.AnalyzerInfo) error {
	if info.Resources.Gpus <= 0 {
		return nil
	}
	resourceName := v1.ResourceName("nvidia.com/gpu")
	requests := reqs.Requests
	q, err := resource.ParseQuantity(strconv.Itoa(info.Resources.Gpus))
	if err != nil {
		return fmt.Errorf("cannot parse gpu value %q", strconv.Itoa(info.Resources.Gpus))
	}

	_, ok := requests[resourceName]
	if ok {
		return fmt.Errorf("internal error: gpu requests already present")
	}
	requests[resourceName] = q

	limits := reqs.Limits
	_, ok = limits[resourceName]
	if ok {
		return fmt.Errorf("internal error: gpu limits already present")
	}
	limits[resourceName] = q

	return nil
}

// genAnalyzerDeploymentResource populates resources associated with
// a deployment. This is mostly container resources like compute,
// gpus, etc.
func genAnalyzerDeploymentResource(inDeploymentString string, info *asg.AnalyzerInfo) (string, error) {
	// Parse a deployment out of the yaml.
	deployment, err := util.ParseK8sDeploymentFromYaml(inDeploymentString)
	if err != nil {
		return "", err
	}

	// We expect there to be a single container.
	if len(deployment.Spec.Template.Spec.Containers) != 1 {
		return "", fmt.Errorf("the deployment spec must contain exactly 1 container; found %d", len(deployment.Spec.Template.Spec.Containers))
	}

	// Initialize requests and limits (they can be nil).
	resourceRequirements := &deployment.Spec.Template.Spec.Containers[0].Resources
	if resourceRequirements.Requests == nil {
		resourceRequirements.Requests = v1.ResourceList{}
	}
	if resourceRequirements.Limits == nil {
		resourceRequirements.Limits = v1.ResourceList{}
	}

	// Fill cpu and memory resources.
	if err = genAnalyzerCpuAndMemory(resourceRequirements, info); err != nil {
		return "", err
	}

	// Fill information required to get GPUs.
	//
	// TODO: Clean this up.
	if err = genAnalyzerGpus(resourceRequirements, info); err != nil {
		return "", err
	}
	nodeSelector, _ := fillGpuNodeSelector(info)
	if nodeSelector != nil {
		deployment.Spec.Template.Spec.NodeSelector = *nodeSelector
	}

	// Serialize results back into a yaml.
	deploymentString, err := util.SerializeK8sObjectIntoYaml(deployment)
	if err != nil {
		return "", err
	}
	return deploymentString, nil
}
