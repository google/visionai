// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package operators

// ArgumentInfo contains information about an operator's argument.
//
// Recall that arguments refer to streaming input/outputs that actually
// get processed.
type ArgumentInfo struct {
	// Name of the argument.
	Name string

	// Type of the argument.
	Type string
}

// AttributeInfo contains information about an operator's attribute.
//
// Recall that attributes are operator configuration parameters.
type AttributeInfo struct {
	// Name of the attribute.
	Name string

	// Type of the attribute.
	Type string

	// DefaultValue used for the attribute if the user does not explicitly supply a value.
	DefaultValue interface{}
}

// ResourceInfo contains information about an operator's resources.
//
// This defines the amount of resources, everything ranging from compute,
// time, memory, etc., that are required to run the operator.
type ResourceInfo struct {
	// The amount of CPU resources required.
	Cpu string

	// The amount of memory resources required.
	Memory string

	// The number of GPUs required.
	Gpus int

	// The latency budget that the operator has.
	LatencyBudgetMs int

	// Environment variables required.
	Envvars map[string]string
}

// OperatorInfo contains information about an operator's interface.
//
// This is the operator declaration and defines its type.
type OperatorInfo struct {
	// Name of the operator.
	Name string

	// InputArgs contains the expected input argument sequence.
	InputArgs []ArgumentInfo

	// OutputArgs contains the expected output argument sequence.
	OutputArgs []ArgumentInfo

	// Attributes contains the expected sequence of attributes.
	Attributes []AttributeInfo

	// Resources contains the resource requirements.
	Resources *ResourceInfo
}
