// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package k8s

import (
	"fmt"
	"regexp"
	"strconv"
	"strings"

	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
)

var matchFirstCap = regexp.MustCompile("(.)([A-Z][a-z]+)")
var matchAllCap = regexp.MustCompile("([a-z0-9])([A-Z])")

// dockerImageName takes an operator name and converts it to its docker image name.
//
// The input is expected to be in pascal case.
// The output is in kebab case.
//
// TODO: This should probably be enforced when the users compose operators.
func dockerImageName(opName string) string {
	kebab := matchFirstCap.ReplaceAllString(opName, "${1}-${2}")
	kebab = matchAllCap.ReplaceAllString(kebab, "${1}-${2}")
	return strings.ToLower(kebab)
}

// normalizeAnalyzerName normalizes an analyzer name into a form
// that is accetable by k8s.
func normalizeAnalyzerName(analyzerName string) string {
	return strings.ReplaceAll(analyzerName, "_", "-")
}

// analyzerNameTemplate standardizes the way analyzer deployments/services are named.
func analyzerNameTemplate(analyzerName string) string {
	return normalizeAnalyzerName(analyzerName) + "-" + analyzerSuffixTemplateString
}

// analyzerMetricsDisabled deduces whether the analyzer should not expose metrics.
func analyzerMetricsDisabled(monitoringInfo *asg.MonitoringInfo) bool {
	// If unspecified, default to exposing metrics
	if monitoringInfo == nil {
		return false
	}

	// Only disable metrics if absolutely no monitoring is allowed
	return monitoringInfo.Level == asg.NoMonitoring
}

// analyzerMetricsPort deduces the port on which metrics should be exposed in the analyzer.
func analyzerMetricsPort(monitoringInfo *asg.MonitoringInfo) int {
	// If unspecified, default to port 9090
	if monitoringInfo == nil {
		return 9090
	}

	return monitoringInfo.MetricsPort
}

// toString takes an attrValueInfo and returns the string representation of its value.
func toString(attrValueInfo *asg.AttributeValueInfo) (string, error) {
	switch t := attrValueInfo.Type; t {
	case "int":
		v, ok := attrValueInfo.Value.(int64)
		if !ok {
			return "", fmt.Errorf("internal error: expected %q but got %T", t, attrValueInfo.Value)
		}
		return strconv.FormatInt(v, 10), nil
	case "float":
		v, ok := attrValueInfo.Value.(float32)
		if !ok {
			return "", fmt.Errorf("internal error: expected %q but got %T", t, attrValueInfo.Value)
		}
		return strconv.FormatFloat(float64(v), 'G', -1, 32), nil
	case "bool":
		v, ok := attrValueInfo.Value.(bool)
		if !ok {
			return "", fmt.Errorf("internal error: expected %q but got %T", t, attrValueInfo.Value)
		}
		return strconv.FormatBool(v), nil
	case "string":
		v, ok := attrValueInfo.Value.(string)
		if !ok {
			return "", fmt.Errorf("internal error: expected %q but got %T", t, attrValueInfo.Value)
		}
		return string(v), nil
	default:
		return "", fmt.Errorf("internal error: unrecognized attribute type %q", t)
	}
}
