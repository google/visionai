// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package k8s

import (
	"bytes"
	"fmt"
	"sort"
	"strings"
	"text/template"

	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
)

type deploymentTmplInfo struct {
	EnableDebug              bool
	AnalyzerDeploymentName   string
	AnalyzerNamespaceName    string
	ContainerName            string
	DockerRegistry           string
	DockerImageName          string
	DockerImageTag           string
	Workdir                  string
	MainBinaryName           string
	OpDeclFname              string
	AnalyzerConfigTextFormat string
	AttributesString         string

	MetricsPort     int
	MetricsDisabled bool

	IncludeRuntimeEnv bool
	AnalysisNameEnv   string
	AnalyzerNameEnv   string
	ProjectIDEnv      string

	EventID      string
	AnalysisName string
	OperatorName string
	ProcessName  string

	DockerImageLink   string
	EnableNewRegistry bool
}

var deploymentTemplate = `
apiVersion: apps/v1
kind: Deployment
metadata:
  name: '{{.AnalyzerDeploymentName}}'
  namespace: "{{.AnalyzerNamespaceName}}"
  labels:
    process: "{{.ProcessName}}"
    event: "{{.EventID}}"
    analyzer: {{.AnalyzerNameEnv}}
    analysis: "{{.AnalysisName}}"
    operator: {{.OperatorName}}
{{if not .MetricsDisabled}}
  annotations:
    prometheus.io/scrape: "true"
    prometheus.io/port: "{{.MetricsPort}}"
{{end}}
spec:
  minReadySeconds: 3
  strategy:
    type: Recreate
  selector:
    matchLabels:
      process: "{{.ProcessName}}"
      event: "{{.EventID}}"
      analyzer: {{.AnalyzerNameEnv}}
      analysis: "{{.AnalysisName}}"
      operator: {{.OperatorName}}
  template:
    metadata:
      labels:
        process: "{{.ProcessName}}"
        event: "{{.EventID}}"
        analyzer: {{.AnalyzerNameEnv}}
        analysis: "{{.AnalysisName}}"
        operator: {{.OperatorName}}
    {{- if not .MetricsDisabled}}
      annotations:
        prometheus.io/scrape: "true"
        prometheus.io/port: "{{.MetricsPort}}"
    {{- end}}
    spec:
      enableServiceLinks: false
      containers:
        - name: {{.ContainerName}}
    {{- if .EnableNewRegistry}}
		      image: "{{.DockerImageLink}}"
    {{- else}}
          image: "{{.DockerRegistry}}/operators/{{.DockerImageName}}:{{.DockerImageTag}}"
		{{- end}}
          ports:
    {{- if not .MetricsDisabled}}
            - name: http-prometheus
              containerPort: {{.MetricsPort}}
              protocol: TCP
    {{- end}}
          imagePullPolicy: Always
          command: [ "{{.Workdir}}/{{.MainBinaryName}}" ]
          {{- if .EnableDebug}}
          securityContext:
            capabilities:
              add:
              - SYS_PTRACE
          {{- end}}
          args:
          - --operator_declaration_file={{.Workdir}}/{{.OpDeclFname}}
          - --analyzer_config_contents={{.AnalyzerConfigTextFormat}}
	  {{- if .AttributesString}}
          - --attributes={{.AttributesString}}
	  {{- end}}
          env:
            - name: GLOG_alsologtostderr
              value: "1"
            - name: PROJECT_ID
              value: "{{.ProjectIDEnv}}"
    {{- if .IncludeRuntimeEnv}}
            - name: ANALYSIS_NAME
              value: "{{.AnalysisNameEnv}}"
            - name: ANALYZER_NAME
              value: "{{.AnalyzerNameEnv}}"
    {{- end}}
`

// genAnalyzerDeployment generates a deployment based on the given
// analyzer info.
func genAnalyzerDeployment(info *asg.AnalyzerInfo, ctx *Context) (string, error) {
	// Generate the analyzer config.
	analyzerConfig, err := genAnalyzerConfig(info, ctx)
	if err != nil {
		return "", err
	}
	analyzerConfigTextFormat := fmt.Sprintf("%v", analyzerConfig)

	attributes := []string{}
	for k, v := range info.Attributes {
		s, err := toString(v)
		if err != nil {
			return "", err
		}
		attributes = append(attributes, fmt.Sprintf("%v=%v", k, s))
	}
	sort.Strings(attributes)
	attributesString := strings.Join(attributes, ",")

	// Render and save the result.
	tmpl, err := template.New("deployment").Parse(deploymentTemplate)
	if err != nil {
		return "", err
	}
	tmplInfo := &deploymentTmplInfo{
		EnableDebug:              ctx.FeatureOptions.EnableDebug,
		AnalyzerDeploymentName:   analyzerNameTemplate(info.Name),
		AnalyzerNamespaceName:    namespaceTemplateString,
		ContainerName:            mainContainerName,
		DockerRegistry:           dockerRegistryTemplateString,
		DockerImageName:          dockerImageName(info.Operator.Name),
		DockerImageTag:           dockerImageTagTemplateString,
		Workdir:                  containerWorkdir,
		MainBinaryName:           mainBinaryName,
		OpDeclFname:              opDeclFileName,
		AnalyzerConfigTextFormat: analyzerConfigTextFormat,
		AttributesString:         attributesString,
		MetricsPort:              analyzerMetricsPort(info.MonitoringInfo),
		MetricsDisabled:          analyzerMetricsDisabled(info.MonitoringInfo),

		IncludeRuntimeEnv: ctx.FeatureOptions.RuntimeInfo.IncludeEnv,
		AnalysisNameEnv:   ctx.FeatureOptions.RuntimeInfo.AnalysisName,
		AnalyzerNameEnv:   info.Name,
		ProjectIDEnv:      tenantProjectIDTemplateString,

		AnalysisName:      analysisNameTemplateString,
		EventID:           eventIDTemplateString,
		OperatorName:      info.Operator.Name,
		ProcessName:       processNameTemplateString,
		EnableNewRegistry: info.Operator.DockerImage != "",
		DockerImageLink:   info.Operator.DockerImage,
	}
	var renderedTmpl bytes.Buffer
	err = tmpl.Execute(&renderedTmpl, tmplInfo)
	if err != nil {
		return "", err
	}
	deploymentString := renderedTmpl.String()

	// Generate resource information.
	deploymentString, err = genAnalyzerDeploymentResource(deploymentString, info)
	if err != nil {
		return "", err
	}

	return deploymentString, nil
}
