// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package util

import (
	"bytes"
	"fmt"
	"strings"

	appsv1 "google3/third_party/golang/kubeapi/apps/v1/v1"
	"google3/third_party/golang/kubeapimachinery/pkg/runtime/runtime"
	k8sjson "google3/third_party/golang/kubeapimachinery/pkg/runtime/serializer/json/json"
	"google3/third_party/golang/kubeclient/kubernetes/scheme/scheme"
	"google3/third_party/golang/yaml/yaml"
)

// ParseYamlIntoK8sObject parses yaml string to a list of objects and returns error if the input is invalid.
func ParseYamlIntoK8sObject(input string) ([]runtime.Object, error) {
	objs := strings.Split(input, "---")
	resource := make([]runtime.Object, 0, len(objs))
	for _, v := range objs {
		v = strings.TrimSpace(v)
		if v == "" {
			continue
		}

		// try k8s codecs
		obj, _, err := scheme.Codecs.UniversalDeserializer().Decode([]byte(v), nil, nil)
		if err != nil {
			return nil, fmt.Errorf("template parse error: %v", err)
		}
		resource = append(resource, obj)
	}
	return resource, nil
}

// ParseK8sDeploymentFromYaml parses one deployment from the given yaml.
func ParseK8sDeploymentFromYaml(input string) (*appsv1.Deployment, error) {
	objects, err := ParseYamlIntoK8sObject(input)
	if err != nil {
		return nil, fmt.Errorf("failed to parse the input string as a k8s yaml: %v", err)
	}
	if len(objects) != 1 {
		return nil, fmt.Errorf("got more than one (actual: %d) k8s object from the input yaml string", len(objects))
	}
	deployment, ok := objects[0].(*appsv1.Deployment)
	if !ok {
		return nil, fmt.Errorf("the given yaml was not a deployment (got %T)", objects[0])
	}
	return deployment, nil
}

// SerializeK8sObjectIntoYaml serializes a kubernetes object into a yaml string.
func SerializeK8sObjectIntoYaml(object runtime.Object) (string, error) {
	// We explicitly serialize to JSON, and use a newer go-yaml to convert
	// that into a yaml. This is because older go-yaml versions unecessarily
	// wraps long strings which breaks the text/template required in the
	// generated program. The apimachinery serializes uses this older version.
	//
	// We simply do the final yaml marshal step with the newer go-yaml.
	var jsonBuf bytes.Buffer
	serializer := k8sjson.NewSerializerWithOptions(
		k8sjson.DefaultMetaFactory, nil, nil,
		k8sjson.SerializerOptions{
			Yaml: false,
		},
	)
	if err := serializer.Encode(object, &jsonBuf); err != nil {
		return "", fmt.Errorf("failed to serialize object into a json: %v", err)
	}

	var jsonObj interface{}
	if err := yaml.Unmarshal(jsonBuf.Bytes(), &jsonObj); err != nil {
		return "", fmt.Errorf("failed to unmarshal json byte string: %v", err)
	}
	yamlBytes, err := yaml.Marshal(jsonObj)
	if err != nil {
		return "", fmt.Errorf("failed to marshal json into a yaml: %v", err)
	}

	return string(yamlBytes), nil
}
