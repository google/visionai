// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package k8s

import (
	"bytes"
	"text/template"

	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
)

type serviceTmplInfo struct {
	AnalyzerServiceName   string
	AnalyzerNamespaceName string
	InternalGrpcPort      int
	InternalStatePort     int
	MetricsPort           int
	MetricsDisabled       bool
	EventID               string
	AnalyzerName          string
	AnalysisName          string
	ProcessName           string
}

// TODO(b/271164856): Match the service and deployment/pod only via process label after live/submission merging.
var serviceTemplate = `apiVersion: v1
kind: Service
metadata:
  labels:
    process: "{{.ProcessName}}"
  name: "{{.AnalyzerServiceName}}"
  namespace: {{.AnalyzerNamespaceName}}
spec:
  type: ClusterIP
  selector:
    process: "{{.ProcessName}}"
    event: "{{.EventID}}"
    analyzer: {{.AnalyzerName}}
    analysis: "{{.AnalysisName}}"
  ports:
    - name: tcp
      port: {{.InternalGrpcPort}}
      targetPort: {{.InternalGrpcPort}}
      protocol: TCP
    - name: grpc-state-port
      port: {{.InternalStatePort}}
      targetPort: {{.InternalStatePort}}
      protocol: TCP
    {{- if not .MetricsDisabled}}
    - name: http-prometheus
      port: {{.MetricsPort}}
      targetPort: {{.MetricsPort}}
      protocol: TCP
    {{- end}}
`

// genAnalyzerService generates a service based on the given
// analyzer info.
// Generate services for all analyzers, including the sink operators.
func genAnalyzerService(info *asg.AnalyzerInfo, ctx *Context) (string, error) {
	tmpl, err := template.New("service").Parse(serviceTemplate)
	if err != nil {
		return "", err
	}
	tmplInfo := &serviceTmplInfo{
		AnalyzerNamespaceName: namespaceTemplateString,
		AnalyzerServiceName:   analyzerNameTemplate(info.Name),
		InternalGrpcPort:      internalGrpcOutputPort,
		InternalStatePort:     defaultStateServerPort,
		MetricsPort:           analyzerMetricsPort(info.MonitoringInfo),
		MetricsDisabled:       analyzerMetricsDisabled(info.MonitoringInfo),
		EventID:               eventIDTemplateString,
		AnalyzerName:          info.Name,
		AnalysisName:          analysisNameTemplateString,
		ProcessName:           processNameTemplateString,
	}
	var renderedTmpl bytes.Buffer
	err = tmpl.Execute(&renderedTmpl, tmplInfo)
	if err != nil {
		return "", err
	}
	serviceString := renderedTmpl.String()

	return serviceString, nil
}
