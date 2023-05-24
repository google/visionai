// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

// Package k8s provides the linker to render the k8s templates.
package k8s

import (
	"bytes"
	"fmt"

	template "google3/security/safetext/yamltemplate"

	kopb "google3/third_party/visionai/golang/pkg/lva/program/proto/k8s_one_go_proto"
)

// K8SOneSymbols contains all the missing information that must be completed
// in the yaml template of a compiled K8SOneProgram.
//
// To complete a K8SOneProgram's yaml template, populate this struct and use
// safetext/yamltemplate to execute the yaml template over this struct.
type K8SOneSymbols struct {
	// The endpoint to the series server.
	//
	// This can be a dns-name:port or an ip:port.
	SeriesServerEndpoint string

	// The namespace into which the completed resources will deployed into.
	Namespace string

	// The docker registry containing the operator binaries.
	DockerRegistry string

	// The docker image tag containing the tag of the GCR image
	DockerImageTag string

	// The mapping that assigns each input series placeholder name with
	// a specific series name.
	//
	// The set of input series placeholder names that must be applied are
	// specified in each compiled K8SOneProgram.
	InputSeries map[string]string

	// The mapping that assigns each output series placeholder name with
	// a specific series name.
	//
	// The set of output series placeholder names that must be applied are
	// specified in each compiled K8SOneProgram.
	OutputSeries map[string]string

	// The tenant project ID.
	TenantProjectID string

	// The consumer project ID.
	ProjectID string

	// The location ID.
	LocationID string

	// The cluster ID.
	ClusterID string

	// The event ID.
	EventID string

	// The mapping that assigns each input stream placeholder name with
	// a specific stream name.
	//
	// The set of input series placeholder names that must be applied are
	// specified in each compiled K8SOneProgram.
	InputStream map[string]string

	// The mapping that assigns each output stream placeholder name with
	// a specific stream name.
	//
	// The set of output series placeholder names that must be applied are
	// specified in each compiled K8SOneProgram.
	OutputStream map[string]string

	// The lessee name.
	Lessee string

	// The suffix of the deployments/services of each analyzer.
	// Hash value of the combination of the analysis name and the event ID.
	AnalyzerSuffix string

	// The name of analysis.
	AnalysisName string

	// The name of the process.
	ProcessName string

	// The billing mode of the Process.
	BillingMode string
}

// Link uses the given K8SOneSymbols to complete a K8SOneProgram.
//
// On success, it returns a string holding a yaml that can be directly applied
// onto a k8s cluster.
func Link(program *kopb.K8SOneProgram, symbols *K8SOneSymbols) (string, error) {
	templateData := &symbolsTemplateData{}

	if symbols.SeriesServerEndpoint == "" {
		return "", fmt.Errorf("the given K8SOneSymbols does not contain a SeriesServerEndpoint")
	}
	templateData.SeriesServerEndpoint = symbols.SeriesServerEndpoint

	if symbols.Namespace == "" {
		return "", fmt.Errorf("the given K8SOneSymbols does not contain a Namespace")
	}
	templateData.Namespace = symbols.Namespace

	if symbols.DockerRegistry == "" {
		return "", fmt.Errorf("the given K8SOneSymbols does not contain a DockerRegistry")
	}
	templateData.DockerRegistry = symbols.DockerRegistry

	// Use "latest" as the default value for DockerImageTag when the input DockerImageTag is empty
	if symbols.DockerImageTag == "" {
		templateData.DockerImageTag = "latest"
	} else {
		templateData.DockerImageTag = symbols.DockerImageTag
	}

	if symbols.EventID == "" {
		return "", fmt.Errorf("the given K8SOneSymbols does not contain an EventID")
	}
	templateData.EventID = symbols.EventID

	if symbols.TenantProjectID == "" {
		return "", fmt.Errorf("the given K8SOneSymbols does not contain a TenantProjectID")
	}
	templateData.TenantProjectID = symbols.TenantProjectID

	if symbols.ProjectID == "" {
		return "", fmt.Errorf("the given K8SOneSymbols does not contain a ProjectID")
	}
	templateData.ProjectID = symbols.ProjectID

	if symbols.LocationID == "" {
		return "", fmt.Errorf("the given K8SOneSymbols does not contain a LocationID")
	}
	templateData.LocationID = symbols.LocationID

	if symbols.ClusterID == "" {
		return "", fmt.Errorf("the given K8SOneSymbols does not contain a ClusterID")
	}
	templateData.ClusterID = symbols.ClusterID

	if symbols.Lessee == "" {
		return "", fmt.Errorf("the given K8SOneSymbols does not contain a Lessee")
	}
	templateData.Lessee = symbols.Lessee

	for _, k := range program.GetInputSeriesPlaceholderNames() {
		v, ok := symbols.InputSeries[k]
		if !ok {
			return "", fmt.Errorf("the given K8SOneSymbols does not map a value for the input series placeholder %q", k)
		}
		templateData.InputSeries = append(templateData.InputSeries, v)
		v, ok = symbols.InputStream[k]
		if !ok {
			return "", fmt.Errorf("the given K8SOneSymbols does not map a value for the input stream placeholder %q", k)
		}
		templateData.InputStream = append(templateData.InputStream, v)
	}

	for _, k := range program.GetOutputSeriesPlaceholderNames() {
		v, ok := symbols.OutputSeries[k]
		if !ok {
			return "", fmt.Errorf("the given K8SOneSymbols does not map a value for the output series placeholder %q", k)
		}
		templateData.OutputSeries = append(templateData.OutputSeries, v)
		v, ok = symbols.OutputStream[k]
		if !ok {
			return "", fmt.Errorf("the given K8SOneSymbols does not map a value for the output stream placeholder %q", k)
		}
		templateData.OutputStream = append(templateData.OutputStream, v)
	}

	if symbols.AnalyzerSuffix == "" {
		return "", fmt.Errorf("the given K8SOneSymbols does not contain a AnalyzerSuffix")
	}
	templateData.AnalyzerSuffix = symbols.AnalyzerSuffix

	if symbols.AnalysisName == "" {
		return "", fmt.Errorf("the given K8SOneSymbols does not contain a AnalysisName")
	}
	templateData.AnalysisName = symbols.AnalysisName

	if symbols.ProcessName == "" {
		return "", fmt.Errorf("the given K8SOneSymbols does not contain a ProcessName")
	}
	templateData.ProcessName = symbols.ProcessName

	if symbols.BillingMode == "" {
		return "", fmt.Errorf("the given K8SOneSymbols does not contain a BillingMode")
	}
	templateData.BillingMode = symbols.BillingMode

	linked, err := renderTemplate(program.GetYamlTemplate(), templateData)
	if err != nil {
		return "", fmt.Errorf("failed to render program template: %v", err)
	}
	return linked, nil
}

// symbolsTemplateData is the actual data structure applied to the template.
type symbolsTemplateData struct {
	SeriesServerEndpoint string
	Namespace            string
	DockerRegistry       string
	DockerImageTag       string
	InputSeries          []string
	OutputSeries         []string
	ProjectID            string
	TenantProjectID      string
	LocationID           string
	ClusterID            string
	EventID              string
	InputStream          []string
	OutputStream         []string
	Lessee               string
	AnalyzerSuffix       string
	AnalysisName         string
	ProcessName          string
	BillingMode          string
}

// renderTemplate is the method that actually applies values to the template.
func renderTemplate(programTemplate string, symbols *symbolsTemplateData) (string, error) {
	tmpl, err := template.New("yaml-template").Parse(programTemplate)
	if err != nil {
		return "", err
	}

	var renderedTmpl bytes.Buffer
	err = tmpl.Execute(&renderedTmpl, symbols)
	if err != nil {
		return "", err
	}

	return renderedTmpl.String(), nil
}
