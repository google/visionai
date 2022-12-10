// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

// Package main provides he entry point for the vaictl command.
package main

import (
	"fmt"
	"os"

	"visionai/tools/vaictl/pkg"
)

func main() {
	if err := pkg.Execute(); err != nil {
		fmt.Fprintf(os.Stderr, "%v\n", err)
		os.Exit(1)
	}
}
