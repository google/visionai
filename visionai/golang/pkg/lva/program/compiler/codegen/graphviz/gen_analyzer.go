// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package graphviz

import (
	"bytes"
	"fmt"

	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
)

func genAnalyzer(info *asg.AnalyzerInfo, buf *bytes.Buffer) error {
	if info == nil {
		return fmt.Errorf("internal error: got a nil AnalyzerInfo")
	}
	fmt.Fprintf(buf, "%q [fillcolor=%s, style=filled]\n", info.Name, "chartreuse")
	for _, inputStreamInfo := range info.InputStreams {
		fmt.Fprintf(buf, "%q -> %q\n", inputStreamInfo.Stream.Name, info.Name)
	}
	for _, outputStreamInfo := range info.OutputStreams {
		fmt.Fprintf(buf, "%q -> %q\n", info.Name, outputStreamInfo.Stream.Name)
	}
	return nil
}
