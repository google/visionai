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
	MetricsPort           int
	MetricsDisabled       bool
	EventID               string
	AnalyzerName          string
	AnalysisName          string
}

var serviceTemplate = `apiVersion: v1
kind: Service
metadata:
  name: "{{.AnalyzerServiceName}}"
  namespace: {{.AnalyzerNamespaceName}}
spec:
  type: ClusterIP
  selector:
    event: "{{.EventID}}"
    analyzer: {{.AnalyzerName}}
    analysis: "{{.AnalysisName}}"
  ports:
    - name: tcp
      port: {{.InternalGrpcPort}}
      targetPort: {{.InternalGrpcPort}}
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
func genAnalyzerService(info *asg.AnalyzerInfo, ctx *Context) (string, error) {
	// No need for a service if there are no outputs.
	//
	// Recall that, for now, only grpc streams appear in OutputStreams.
	// AIS outputs are taken care by the StreamSink special case.
	if len(info.OutputStreams) == 0 {
		return "", nil
	}
	tmpl, err := template.New("service").Parse(serviceTemplate)
	if err != nil {
		return "", err
	}
	tmplInfo := &serviceTmplInfo{
		AnalyzerNamespaceName: namespaceTemplateString,
		AnalyzerServiceName:   analyzerNameTemplate(info.Name),
		InternalGrpcPort:      internalGrpcOutputPort,
		MetricsPort:           analyzerMetricsPort(info.MonitoringInfo),
		MetricsDisabled:       analyzerMetricsDisabled(info.MonitoringInfo),
		EventID:               eventIDTemplateString,
		AnalyzerName:          info.Name,
		AnalysisName:          analysisNameTemplateString,
	}
	var renderedTmpl bytes.Buffer
	err = tmpl.Execute(&renderedTmpl, tmplInfo)
	if err != nil {
		return "", err
	}
	serviceString := renderedTmpl.String()

	return serviceString, nil
}
