// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package asg

// Element identifies the kind of entity that an ASG node represents.
//
// Data types satisfying this interface should contain references to
// element specific information (such as semantics) to be useful.
//
// When walking the ASG with a visitor, one may type assert/switch on
// specific element types.
type Element interface {
	AsgElement()

	// Name returns the name of this element.
	Name() string
}

// StreamElement represents a Stream in the ASG.
type StreamElement struct {
	Info *StreamInfo
}

// AsgElement represents a stream element in the ASG.
func (s *StreamElement) AsgElement() {}

// Name returns the name of this element.
func (s *StreamElement) Name() string {
	return s.Info.Name
}

// AnalyzerElement represents an Analyzer in the ASG.
type AnalyzerElement struct {
	Info *AnalyzerInfo
}

// AsgElement represents an analyzer element in the ASG.
func (a *AnalyzerElement) AsgElement() {}

// Name returns the name of this element.
func (a *AnalyzerElement) Name() string {
	return a.Info.Name
}

// SentinelElement represents a sentinel in the ASG.
type SentinelElement struct {
	Info *SentinelInfo
}

// AsgElement represents a sentinel element in the ASG.
func (s *SentinelElement) AsgElement() {}

// Name returns the name of this element.
func (s *SentinelElement) Name() string {
	return s.Info.Name
}
