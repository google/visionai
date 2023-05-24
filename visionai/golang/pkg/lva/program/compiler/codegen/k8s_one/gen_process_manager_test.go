// Copyright 2023 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package k8s

import (
	"regexp"
	"testing"

	"google3/base/go/log"
	"google3/base/go/runfiles"
	"google3/third_party/golang/cmp/cmp"
	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
	"google3/third_party/visionai/golang/pkg/lva/program/compiler/parse/parse"
	"google3/third_party/visionai/golang/pkg/lva/program/operators/operators"
)

func testOperatorRegistry() (*operators.OperatorRegistry, error) {
	return operators.RegistryFromOpListFile(runfiles.Path("google3/third_party/visionai/golang/pkg/lva/program/data/test_oplist.pbtxt"))
}

func mustCreateASG(program string) *asg.Graph {
	registry, err := testOperatorRegistry()
	if err != nil {
		log.Fatalf("testOperatorRegistry() failed: %v", err)
	}
	pctx := &parse.Context{
		InputProgramText: program,
		OperatorRegistry: registry,
	}
	asg, err := parse.Parse(pctx)
	if err != nil {
		log.Fatalf("parse.Parse() failed: %v", err)
	}
	return asg
}

func TestGenProcessManagerDeployment(t *testing.T) {
	analysisText := `
analyzers: <
  analyzer: "placeholder"
  operator: "Placeholder"
>
analyzers: <
  analyzer: "stdout_sink"
  operator: "StdoutSink"
  inputs: <
    input: "placeholder:output"
  >
>
`
	tests := []struct {
		name     string
		mode     string
		wantErr  bool
		wantyaml string
	}{
		{
			name:    "live mode",
			mode:    "live",
			wantErr: false,
			wantyaml: `
apiVersion: apps/v1
kind: Deployment
metadata:
  name: '{{.ProcessName}}-manager'
  namespace: "{{.Namespace}}"
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
          image: "{{.DockerRegistry}}/process-manager:{{.DockerImageTag}}"
          imagePullPolicy: Always
          command: [ "/main", "--alsologtostderr" ]
          args:
          - --process_manager_config=services:{name:"placeholder"  endpoint:"placeholder-{{.AnalyzerSuffix}}:17000"  health_check_timeout:{seconds:60}  health_check_interval:{seconds:5}  max_attempts:3 operator:"Placeholder"}  services:{name:"stdout_sink"  endpoint:"stdout-sink-{{.AnalyzerSuffix}}:17000"  health_check_timeout:{seconds:60}  health_check_interval:{seconds:5} max_attempts:3 operator:"StdoutSink"}  k8s_report_mode:{process_namespace:"{{.Namespace}}"}  process_metadata:{run_mode:LIVE  cluster_name:"projects/{{.ProjectID}}/locations/{{.LocationID}}/clusters/{{.ClusterID}}" analysis_name:"{{.AnalysisName}}" process_name:"{{.ProcessName}}"  billing_mode:"{{.BillingMode}}" event_name:"{{.EventID}}"}
`,
		},
		{
			name:    "submission mode",
			mode:    "submission",
			wantErr: false,
			wantyaml: `
apiVersion: apps/v1
kind: Deployment
metadata:
  name: '{{.ProcessName}}-manager'
  namespace: "{{.Namespace}}"
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
          image: "{{.DockerRegistry}}/process-manager:{{.DockerImageTag}}"
          imagePullPolicy: Always
          command: [ "/main", "--alsologtostderr" ]
          args:
          - --process_manager_config=services:{name:"placeholder"  endpoint:"placeholder-{{.AnalyzerSuffix}}:17000"  health_check_timeout:{seconds:60}  health_check_interval:{seconds:5}  max_attempts:3 operator:"Placeholder"}  services:{name:"stdout_sink"  endpoint:"stdout-sink-{{.AnalyzerSuffix}}:17000"  health_check_timeout:{seconds:60}  health_check_interval:{seconds:5} max_attempts:3 operator:"StdoutSink"}  k8s_report_mode:{process_namespace:"{{.Namespace}}"}  process_metadata:{run_mode:SUBMISSION  cluster_name:"projects/{{.ProjectID}}/locations/{{.LocationID}}/clusters/{{.ClusterID}}" analysis_name:"{{.AnalysisName}}" process_name:"{{.ProcessName}}"  billing_mode:"{{.BillingMode}}" event_name:"{{.EventID}}"}
`,
		},
		{
			name:    "unknown mode",
			mode:    "unknown",
			wantErr: true,
		},
	}

	for _, tc := range tests {
		ctx := &Context{
			Asg: mustCreateASG(analysisText),
			FeatureOptions: FeatureOptions{
				RunMode: tc.mode,
			},
		}
		err := genProcessManager(ctx)
		if gotErr := err != nil; gotErr != tc.wantErr {
			t.Errorf("genProcessManager() got error %v, want error %v", err, tc.wantErr)
		}
		if tc.wantyaml != "" {
			if len(ctx.yamls) != 1 {
				t.Fatalf("expect 1 yaml generated and filled in yamls field, got %d yamls", len(ctx.yamls))
			}
			yaml := ctx.yamls[0]

			space := regexp.MustCompile(`\s+`)
			got := space.ReplaceAllString(yaml, " ")
			want := space.ReplaceAllString(tc.wantyaml, " ")
			if diff := cmp.Diff(got, want); diff != "" {
				t.Errorf("got diff for yaml in yamls fields of context (-want +got): %v", diff)
			}
		}
	}
}
