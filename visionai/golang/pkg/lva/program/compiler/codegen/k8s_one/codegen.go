// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package k8s

import (
	"fmt"
	"strings"

	"google3/third_party/golang/protobuf/v2/encoding/prototext/prototext"
	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
	kopb "google3/third_party/visionai/golang/pkg/lva/program/proto/k8s_one_go_proto"
)

// Codegen is the main entry point into the k8s_one code generator.
func Codegen(ctx *Context) error {
	if err := ctx.verifyInitialization(); err != nil {
		return err
	}

	// Generate non element specific code.
	if err := genProcessManager(ctx); err != nil {
		return err
	}

	// Generate code for each element.
	leave := func(n *asg.Node) error {
		element := n.Element()
		switch element := element.(type) {
		case *asg.StreamElement:
			return nil
		case *asg.AnalyzerElement:
			return genAnalyzer(element.Info, ctx)
		case *asg.SentinelElement:
			return nil
		default:
			return fmt.Errorf(
				"internal error: unrecognized ASG Element type %T for %q",
				element, n.Name())
		}
	}
	if err := asg.ReverseDFS(ctx.Asg, nil, leave); err != nil {
		return err
	}

	outputString, err := toOutputString(ctx)
	if err != nil {
		return err
	}
	ctx.OutputString = outputString

	return nil
}

func toOutputString(ctx *Context) (string, error) {
	kop := &kopb.K8SOneProgram{}

	// Save the placeholder names.
	kop.SetInputSeriesPlaceholderNames(ctx.inputPlaceholderNames)
	kop.SetOutputSeriesPlaceholderNames(ctx.outputPlaceholderNames)

	// Save the yaml template.
	//
	// Merge the generated yamls into one.
	kop.SetYamlTemplate(strings.Join(ctx.yamls, "---\n"))
	if ctx.Verbose {
		fmt.Printf("\nInput Placeholder Names\n")
		fmt.Printf("========================\n")
		fmt.Printf("\n%v\n", kop.GetInputSeriesPlaceholderNames())
		fmt.Printf("\nOutput Placeholder Names\n")
		fmt.Printf("========================\n")
		fmt.Printf("\n%v\n", kop.GetOutputSeriesPlaceholderNames())
		fmt.Printf("\nYaml Template\n")
		fmt.Printf("=============\n")
		fmt.Printf("\n%v\n", kop.GetYamlTemplate())
	}

	// Serialize the output program into a string.
	outputString := prototext.Format(kop)

	return outputString, nil
}
