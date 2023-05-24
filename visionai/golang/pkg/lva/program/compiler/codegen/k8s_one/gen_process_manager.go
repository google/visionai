// Copyright 2023 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package k8s

import (
	"bytes"
	"fmt"
	"text/template"

	dpb "google3/google/protobuf/duration_go_proto"
	cpb "google3/third_party/visionai/proto/lva/process_manager_config_go_proto"

	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
)

type processManagerDeploymentTmplInfo struct {
	EnableDebug                    bool
	ProcessManagerDeploymentName   string
	ProcessManagerNamespaceName    string
	ProcessName                    string
	DockerRegistry                 string
	DockerImageName                string
	DockerImageTag                 string
	ProcessManagerConfigTextFormat string
}

// Process manager deployment template.
var processManagerDeploymentTemplate = `
apiVersion: apps/v1
kind: Deployment
metadata:
  name: '{{.ProcessManagerDeploymentName}}'
  namespace: "{{.ProcessManagerNamespaceName}}"
  labels:
    process: "{{.ProcessName}}"
  annotations:
    prometheus.io/scrape: "true"
    prometheus.io/port: "9090"
spec:
  selector:
    matchLabels:
      process: "{{.ProcessName}}"
  template:
    metadata:
      labels:
        process: "{{.ProcessName}}"
      annotations:
        prometheus.io/scrape: "true"
        prometheus.io/port: "9090"
    spec:
      enableServiceLinks: false
      containers:
        - name: process-manager
          image: "{{.DockerRegistry}}/{{.DockerImageName}}:{{.DockerImageTag}}"
          imagePullPolicy: Always
          command: [ "/main", "--alsologtostderr" ]
          {{- if .EnableDebug}}
          securityContext:
            capabilities:
              add:
              - SYS_PTRACE
          {{- end}}
          args:
          - --process_manager_config={{.ProcessManagerConfigTextFormat}}
`

// Analyzer is the struct contains analyzer and operator name.
type Analyzer struct {
	AnalyzerName string
	OperatorName string
}

func genProcessManagerDeployment(graph *asg.Graph, featureOptions FeatureOptions) (string, error) {
	// Get the list of all analyzer names.
	analyzers := []Analyzer{}
	for _, n := range graph.Nodes() {
		analyzerElement, ok := n.Element().(*asg.AnalyzerElement)
		if !ok {
			continue
		}
		analyzers = append(analyzers, Analyzer{
			AnalyzerName: analyzerElement.Info.Name,
			OperatorName: analyzerElement.Info.Operator.Name,
		})
	}

	config, err := genProcessManagerConfig(analyzers, &featureOptions)
	if err != nil {
		return "", err
	}
	tmpl, err := template.New("deployment").Parse(processManagerDeploymentTemplate)
	if err != nil {
		return "", err
	}

	tmplInfo := &processManagerDeploymentTmplInfo{
		EnableDebug:                    featureOptions.EnableDebug,
		ProcessManagerDeploymentName:   processNameTemplateString + "-manager",
		ProcessManagerNamespaceName:    namespaceTemplateString,
		DockerRegistry:                 dockerRegistryTemplateString,
		DockerImageName:                processManagerImageName,
		DockerImageTag:                 dockerImageTagTemplateString,
		ProcessName:                    processNameTemplateString,
		ProcessManagerConfigTextFormat: fmt.Sprintf("%v", config),
	}

	var renderedTmpl bytes.Buffer
	err = tmpl.Execute(&renderedTmpl, tmplInfo)
	if err != nil {
		return "", err
	}
	deploymentString := renderedTmpl.String()
	return deploymentString, nil
}

// genProcessManager is the top level function to generate process manager yamls.
func genProcessManager(ctx *Context) error {
	deploymentString, err := genProcessManagerDeployment(ctx.Asg, ctx.FeatureOptions)
	if err != nil {
		return err
	}
	if deploymentString != "" {
		ctx.yamls = append(ctx.yamls, deploymentString)
	}
	return nil
}

// genProcessManagerConfig generates the process manager config.
func genProcessManagerConfig(analyzers []Analyzer, featureOptions *FeatureOptions) (*cpb.ProcessManagerConfig, error) {
	config := &cpb.ProcessManagerConfig{
		ReportMode: &cpb.ProcessManagerConfig_K8SReportMode_{
			K8SReportMode: &cpb.ProcessManagerConfig_K8SReportMode{
				ProcessNamespace: namespaceTemplateString,
			},
		},
	}
	runMode := cpb.ProcessManagerConfig_UNSPECIFIED
	switch featureOptions.RunMode {
	case "live":
		runMode = cpb.ProcessManagerConfig_LIVE
	case "submission":
		runMode = cpb.ProcessManagerConfig_SUBMISSION
	default:
		return nil, fmt.Errorf("got an unsupported run mode %q", featureOptions.RunMode)
	}

	config.ProcessMetadata = &cpb.ProcessManagerConfig_ProcessMetadata{
		RunMode:      runMode,
		ClusterName:  fmt.Sprintf("projects/%s/locations/%s/clusters/%s", projectIDTemplateString, locationIDTemplateString, clusterIDTemplateString),
		AnalysisName: analysisNameTemplateString,
		ProcessName:  processNameTemplateString,
		BillingMode:  billingModeTemplateString,
		EventName:    eventIDTemplateString,
	}

	svcs := []*cpb.ProcessManagerConfig_ServiceConfig{}
	for _, az := range analyzers {
		svcs = append(svcs, &cpb.ProcessManagerConfig_ServiceConfig{
			Name:                az.AnalyzerName,
			HealthCheckTimeout:  dpb.New(defaultHealthCheckTimeout),
			HealthCheckInterval: dpb.New(defaultHealthCheckInterval),
			MaxAttempts:         defaultHealthCheckMaxAttempts,
			Endpoint:            fmt.Sprintf("%s:%d", analyzerNameTemplate(az.AnalyzerName), defaultStateServerPort),
			Operator:            az.OperatorName,
		})
	}
	config.Services = svcs
	return config, nil
}
