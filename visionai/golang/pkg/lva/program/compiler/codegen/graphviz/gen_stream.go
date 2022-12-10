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

func genStream(info *asg.StreamInfo, buf *bytes.Buffer) error {
	if info == nil {
		return fmt.Errorf("internal error: got a nil StreamInfo")
	}
	fmt.Fprintf(buf, "%q [fillcolor=%s, style=filled]\n", info.Name, "deepskyblue")
	return nil
}
