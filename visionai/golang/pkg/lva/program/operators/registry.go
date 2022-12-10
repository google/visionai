// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package operators

import (
	"fmt"
	"sync"

	"google3/third_party/golang/cpy/cpy"
)

// OperatorRegistry is a data structure that stores a mapping between
// operator names and the their semantic information (such as type).
//
// It is typically populated before any real work is done (e.g. during
// compiler initialization), so that the rest of the program may access
// the relevant information more easily.
type OperatorRegistry struct {
	sync.RWMutex
	mapping map[string]*OperatorInfo
}

// NewOperatorRegistry creates a new (empty) registry.
func NewOperatorRegistry() *OperatorRegistry {
	freshRegistry := &OperatorRegistry{
		mapping: make(map[string]*OperatorInfo),
	}
	return freshRegistry
}

// Lookup returns a deep copy of the OperatorInfo for the operator opName.
func (o *OperatorRegistry) Lookup(opName string) (*OperatorInfo, error) {
	o.RLock()
	defer o.RUnlock()
	v, ok := o.mapping[opName]
	if !ok {
		return nil, fmt.Errorf("could not find the operator %q among the those that are supported", opName)
	}

	// Make a deep copy.
	vCpy, ok := cpy.New(cpy.IgnoreAllUnexported()).Copy(v).(*OperatorInfo)
	if !ok {
		return nil, fmt.Errorf("internal error when deepcopying")
	}
	return vCpy, nil
}

// Insert adds the operator opName and associates it with the given
// OperatorInfo. The registry will hold a deepcopy of the given opInfo.
func (o *OperatorRegistry) Insert(opName string, opInfo *OperatorInfo) error {
	// Make a deep copy.
	v, ok := cpy.New(cpy.IgnoreAllUnexported()).Copy(opInfo).(*OperatorInfo)
	if !ok {
		return fmt.Errorf("internal error when deepcopying")
	}

	o.Lock()
	defer o.Unlock()
	if _, ok := o.mapping[opName]; ok {
		return fmt.Errorf("operator %q is already present in the registry", opName)
	}
	o.mapping[opName] = v
	return nil
}
