// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package util

import (
	"reflect"
	"strings"
	"testing"

	appsv1 "google3/third_party/golang/k8s_io/api/v/v0_23/apps/v1/v1"
	corev1 "google3/third_party/golang/k8s_io/api/v/v0_23/core/v1/v1"
	"google3/third_party/golang/k8s_io/apimachinery/v/v0_23/pkg/apis/meta/v1/v1"
	"google3/third_party/golang/k8s_io/apimachinery/v/v0_23/pkg/runtime/runtime"
	"google3/third_party/golang/k8s_io/apimachinery/v/v0_23/pkg/util/intstr/intstr"
)

func TestParse(t *testing.T) {
	testCases := []struct {
		desc         string
		input        string
		wantResource []runtime.Object
		wantError    string
	}{
		{
			desc: "parse deployment and service",
			input: `
apiVersion: v1
kind: Service
metadata:
 name: foo
spec:
 selector:
   app: foo
 ports:
   - protocol: TCP
     port: 80
     targetPort: 6000
 clusterIP: 1.1.1.1
 type: ClusterIP
---
---
apiVersion: apps/v1
kind: Deployment
metadata:
 name: bar
spec:
 selector:
   matchLabels:
     app: bar
`,
			wantResource: []runtime.Object{
				&corev1.Service{
					TypeMeta: v1.TypeMeta{
						Kind:       "Service",
						APIVersion: "v1",
					},
					ObjectMeta: v1.ObjectMeta{
						Name: "foo",
					},
					Spec: corev1.ServiceSpec{
						Ports: []corev1.ServicePort{
							{
								Protocol:   corev1.ProtocolTCP,
								Port:       80,
								TargetPort: intstr.FromInt(6000),
							},
						},
						ClusterIP: "1.1.1.1",
						Type:      "ClusterIP",
						Selector:  map[string]string{"app": "foo"},
					},
				},
				&appsv1.Deployment{
					TypeMeta: v1.TypeMeta{
						Kind:       "Deployment",
						APIVersion: "apps/v1",
					},
					ObjectMeta: v1.ObjectMeta{
						Name: "bar",
					},
					Spec: appsv1.DeploymentSpec{
						Selector: &v1.LabelSelector{
							MatchLabels: map[string]string{"app": "bar"},
						},
					},
				},
			},
		},
		{
			desc: "unknown kind",
			input: `
apiVersion: v1
kind: Unknown
`,
			wantError: "template parse error",
		},
	}

	for _, tc := range testCases {
		t.Run(tc.desc, func(t *testing.T) {
			r, e := ParseYamlIntoK8sObject(tc.input)
			if e != nil {
				if !strings.Contains(e.Error(), tc.wantError) {
					t.Errorf("Got %v, want %v", e.Error(), tc.wantError)
				}
				return
			}

			if r != nil {
				if !reflect.DeepEqual(r, tc.wantResource) {
					t.Errorf("Get %v, want %v", r, tc.wantResource)
				}
			}
		})
	}
}

func TestDeploymentParse(t *testing.T) {
	testCases := []struct {
		desc         string
		input        string
		wantResource *appsv1.Deployment
		wantError    string
	}{
		{
			desc: "parse a single deployment",
			input: `
apiVersion: apps/v1
kind: Deployment
metadata:
 name: bar
spec:
 selector:
   matchLabels:
     app: bar
`,
			wantResource: &appsv1.Deployment{
				TypeMeta: v1.TypeMeta{
					Kind:       "Deployment",
					APIVersion: "apps/v1",
				},
				ObjectMeta: v1.ObjectMeta{
					Name: "bar",
				},
				Spec: appsv1.DeploymentSpec{
					Selector: &v1.LabelSelector{
						MatchLabels: map[string]string{"app": "bar"},
					},
				},
			},
		},
		{
			desc: "fail to parse non-deployment",
			input: `
apiVersion: v1
kind: Service
metadata:
 name: foo
spec:
 selector:
   app: foo
 ports:
   - protocol: TCP
     port: 80
     targetPort: 6000
 clusterIP: 1.1.1.1
 type: ClusterIP
`,
			wantError: "the given yaml was not a deployment",
		},
	}

	for _, tc := range testCases {
		t.Run(tc.desc, func(t *testing.T) {
			r, e := ParseK8sDeploymentFromYaml(tc.input)
			if e != nil {
				if !strings.Contains(e.Error(), tc.wantError) {
					t.Errorf("Got %v, want %v", e.Error(), tc.wantError)
				}
				return
			}

			if r != nil {
				if !reflect.DeepEqual(r, tc.wantResource) {
					t.Errorf("Get %v, want %v", r, tc.wantResource)
				}
			}
		})
	}
}

func TestSerializesK8sObjectIntoYaml(t *testing.T) {
	testCases := []struct {
		desc      string
		input     runtime.Object
		wantError string
	}{
		{
			desc: "serialize a deployment",
			input: &appsv1.Deployment{
				TypeMeta: v1.TypeMeta{
					Kind:       "Deployment",
					APIVersion: "apps/v1",
				},
				ObjectMeta: v1.ObjectMeta{
					Name: "bar",
				},
				Spec: appsv1.DeploymentSpec{
					Selector: &v1.LabelSelector{
						MatchLabels: map[string]string{"app": "bar"},
					},
				},
			},
		},
		{
			desc: "serialize a service",
			input: &corev1.Service{
				TypeMeta: v1.TypeMeta{
					Kind:       "Service",
					APIVersion: "v1",
				},
				ObjectMeta: v1.ObjectMeta{
					Name: "foo",
				},
				Spec: corev1.ServiceSpec{
					Ports: []corev1.ServicePort{
						{
							Protocol:   corev1.ProtocolTCP,
							Port:       80,
							TargetPort: intstr.FromInt(6000),
						},
					},
					ClusterIP: "1.1.1.1",
					Type:      "ClusterIP",
					Selector:  map[string]string{"app": "foo"},
				},
			},
		},
	}

	for _, tc := range testCases {
		t.Run(tc.desc, func(t *testing.T) {
			yamlString, e := SerializeK8sObjectIntoYaml(tc.input)
			if e != nil {
				if !strings.Contains(e.Error(), tc.wantError) {
					t.Errorf("Got %v, want %v", e.Error(), tc.wantError)
				}
				return
			}

			r, e := ParseYamlIntoK8sObject(yamlString)
			if e != nil {
				t.Errorf("ParseYamlIntoK8sObject failed")
				return
			}

			if r != nil {
				if !reflect.DeepEqual(r[0], tc.input) {
					t.Errorf("Get %v, want %v", r[0], tc.input)
				}
			}
		})
	}
}
