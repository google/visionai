// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package parse

import (
	"fmt"
	"regexp"
)

var (
	analyzerNamePattern          string
	operatorArgumentNamePattern  string
	operatorAttributeNamePattern string
	analyzerOutputNamePattern    string
	analyzerNameRE               *regexp.Regexp
	operatorArgumentNameRE       *regexp.Regexp
	operatorAttributeNameRE      *regexp.Regexp
	analyzerOutputNameRE         *regexp.Regexp
)

func init() {
	analyzerNamePattern = `[a-z][a-z0-9]*(_[a-z0-9]+)*`

	// TODO: These should probably go somewhere with operators
	// and should be checked when an op is being written/registered.
	operatorArgumentNamePattern = `[a-z][a-z0-9]*(_[a-z0-9]+)*`
	operatorAttributeNamePattern = `[a-z][a-z0-9]*(_[a-z0-9]+)*`

	analyzerOutputNamePattern = fmt.Sprintf("(%s):(%s)", analyzerNamePattern, operatorArgumentNamePattern)

	analyzerNameRE = regexp.MustCompile(fmt.Sprintf("^%s$", analyzerNamePattern))
	operatorArgumentNameRE = regexp.MustCompile(fmt.Sprintf("^%s$", operatorArgumentNamePattern))
	operatorAttributeNameRE = regexp.MustCompile(fmt.Sprintf("^%s$", operatorAttributeNamePattern))
	analyzerOutputNameRE = regexp.MustCompile(fmt.Sprintf("^%s$", analyzerOutputNamePattern))
}

// IsValidAnalyzerName returns true if the given string conforms
// to the expected pattern of a valid analyzer name.
func IsValidAnalyzerName(name string) bool {
	return analyzerNameRE.MatchString(name)
}

// IsValidOperatorArgumentName returns true if the given string conforms
// to the expected pattern of a valid operator argument name.
func IsValidOperatorArgumentName(name string) bool {
	return operatorArgumentNameRE.MatchString(name)
}

// IsValidOperatorAttributeName returns true if the given string conforms
// to the expected pattern of a valid operator argument name.
func IsValidOperatorAttributeName(name string) bool {
	return operatorAttributeNameRE.MatchString(name)
}

// IsValidAnalyzerOutputName returns true if the given string conforms
// to the expected pattern of a valid analyzer's output name.
func IsValidAnalyzerOutputName(name string) bool {
	return analyzerOutputNameRE.MatchString(name)
}

// SplitAnalyzerOutputName returns nil if the given name is not a
// valid analyzer output name. Otherwise, it will return a string
// slice of length 2, the 0th element being the analyzer name
// and the 1st element being the argument name.
func SplitAnalyzerOutputName(name string) []string {
	submatch := analyzerOutputNameRE.FindStringSubmatch(name)
	if submatch == nil {
		return nil
	}
	if len(submatch) < 4 {
		return nil
	}
	return []string{submatch[1], submatch[3]}
}

// JoinAnalyzerOutputName verifies that the given anaName is a valid
// analyzer name and the given argName is a valid argument name,
// and returns a correctly joined analyzer output name and nil error.
// Otherwise, returns the empty string and a non-nil error.
func JoinAnalyzerOutputName(anaName string, argName string) (string, error) {
	if !IsValidAnalyzerName(anaName) {
		return "", fmt.Errorf("%q is not a valid analyzer name (must match %q)", anaName, analyzerOutputNamePattern)
	}
	if !IsValidOperatorArgumentName(argName) {
		return "", fmt.Errorf("%q is not a valid operator argument name (must match %q)", argName, operatorArgumentNamePattern)
	}
	return anaName + ":" + argName, nil
}
