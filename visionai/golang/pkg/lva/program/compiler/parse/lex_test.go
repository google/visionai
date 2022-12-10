// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package parse

import (
	"testing"
)

func TestIsValidAnalyzerName(t *testing.T) {
	var tests = []struct {
		input string
		want  bool
	}{
		{"", false},
		{"1", false},
		{"A", false},
		{"a", true},
		{"_", false},
		{"_a", false},
		{"_1", false},
		{"1_", false},
		{"a_", false},
		{"a_1", true},
		{"a_a", true},
		{"a-a", false},
		{"a a", false},
		{"a_2", true},
		{"1_2", false},
		{"a__3", false},
		{"_a_123", false},
		{"abc_123", true},
		{"abc_df2", true},
		{"abc_A3f2", false},
		{"abc:123", false},
		{"abc_a3f2_", false},
		{"abc_a3f2_123", true},
		{"abc_a3f2__123", false},
	}
	for _, test := range tests {
		got := IsValidAnalyzerName(test.input)
		if test.want != got {
			t.Errorf("IsValidAnalyzerName(%q) want %t, got %t", test.input, test.want, got)
		}
	}
}

func TestIsValidOperatorArgumentName(t *testing.T) {
	var tests = []struct {
		input string
		want  bool
	}{
		{"", false},
		{"1", false},
		{"A", false},
		{"a", true},
		{"_", false},
		{"_a", false},
		{"_1", false},
		{"1_", false},
		{"a_", false},
		{"a_1", true},
		{"a_a", true},
		{"a-a", false},
		{"a a", false},
		{"a_2", true},
		{"1_2", false},
		{"a__3", false},
		{"_a_123", false},
		{"abc_123", true},
		{"abc_df2", true},
		{"abc_A3f2", false},
		{"abc:123", false},
		{"abc_a3f2_", false},
		{"abc_a3f2_123", true},
		{"abc_a3f2__123", false},
	}
	for _, test := range tests {
		got := IsValidOperatorArgumentName(test.input)
		if test.want != got {
			t.Errorf("IsValidOperatorArgumentName(%q) want %t, got %t", test.input, test.want, got)
		}
	}
}

func TestAnalyzerOutputName(t *testing.T) {
	var tests = []struct {
		input                             string
		expectedIsValidAnalyzerOutputName bool
		expectedSplitAnalyzerOutputName   []string
	}{
		{"", false, nil},
		{"1", false, nil},
		{"A", false, nil},
		{"a", false, nil},
		{"_", false, nil},
		{"_a", false, nil},
		{"_1", false, nil},
		{"1_", false, nil},
		{"a_", false, nil},
		{"a_1", false, nil},
		{"a_a", false, nil},
		{"a-a", false, nil},
		{"a a", false, nil},
		{"a_2", false, nil},
		{"1_2", false, nil},
		{"a__3", false, nil},
		{"_a_123", false, nil},
		{"abc_123", false, nil},
		{"abc_df2", false, nil},
		{"abc_A3f2", false, nil},
		{"abc_a3f2_", false, nil},
		{"abc_a3f2_123", false, nil},
		{"abc_a3f2__123", false, nil},
		{":", false, nil},
		{"a", false, nil},
		{"a:", false, nil},
		{":a", false, nil},
		{"a:a", true, []string{"a", "a"}},
		{"a12:34", false, nil},
		{"a12_34:b34_12", true, []string{"a12_34", "b34_12"}},
	}
	for _, test := range tests {
		got := IsValidAnalyzerOutputName(test.input)
		if got != test.expectedIsValidAnalyzerOutputName {
			t.Errorf("IsValidAnalyzerOutputName(%q) got %t, want %t", test.input, got, test.expectedIsValidAnalyzerOutputName)
		}
		actualSplitAnalyzerOutputName := SplitAnalyzerOutputName(test.input)
		if test.expectedSplitAnalyzerOutputName == nil {
			if actualSplitAnalyzerOutputName != nil {
				t.Errorf("want nil, got non-nil %q", actualSplitAnalyzerOutputName)
			}
		} else {
			if len(test.expectedSplitAnalyzerOutputName) != len(actualSplitAnalyzerOutputName) {
				t.Errorf("want %d, got %d", len(test.expectedSplitAnalyzerOutputName), len(actualSplitAnalyzerOutputName))
			}
			for i, val := range test.expectedSplitAnalyzerOutputName {
				if val != actualSplitAnalyzerOutputName[i] {
					t.Errorf("want %q, got %q", val, actualSplitAnalyzerOutputName[i])
				}
			}
		}
	}
}

func TestJoinAnalyzerOutputName(t *testing.T) {
	var tests = []struct {
		inputAnaName string
		inputArgName string
		wantString   string
		wantNilError bool
	}{
		{"", "", "", false},
		{"", "abc", "", false},
		{"abc", "", "", false},
		{"abc", "abc", "abc:abc", true},
		{"abc1", "a1bc", "abc1:a1bc", true},
		{"1abc", "abc", "", false},
		{"abc", "_abc", "", false},
	}
	for _, test := range tests {
		actualString, err := JoinAnalyzerOutputName(test.inputAnaName, test.inputArgName)
		if test.wantString != actualString {
			t.Errorf("IsValidAnalyzerOutputName(%q, %q) got %q, want %q", test.inputAnaName, test.inputArgName, actualString, test.wantString)
		}
		if test.wantNilError {
			if err != nil {
				t.Errorf("want nil, got non-nil %v", err)
			}
		} else {
			if err == nil {
				t.Error("want non-nil, got nil")
			}
		}
	}
}
