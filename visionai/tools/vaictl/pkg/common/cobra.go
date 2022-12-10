// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package common

import (
	"fmt"

	"github.com/spf13/cobra"
)

var prefixCount = 0

// UniquifyViperKey generates a unique viper dictionary key for a
// given prefix and key name.
func UniquifyViperKey(prefix string, key string) string {
	result := fmt.Sprintf("%s%d.%s", prefix, prefixCount, key)
	prefixCount++
	return result
}

// CallParentPersistentPreRunE chains up calls to all the PersistentPreRunE's
// that are the parents of the given command.
func CallParentPersistentPreRunE(cmd *cobra.Command, args []string) error {
	if parent := cmd.Parent(); parent != nil {
		if parent.PersistentPreRunE != nil {
			return parent.PersistentPreRunE(parent, args)
		}
	}
	return nil
}
