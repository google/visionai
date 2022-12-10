// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package util

import (
	"context"
	"fmt"
	"time"

	lropb "google3/google/longrunning/operations_go_proto"
	lro "google3/google/longrunning/operations_grpc_go"
	"google3/util/task/go/status"
)

// WaitForOperation waits for the longrunning operator.
func WaitForOperation(ctx context.Context, c lro.OperationsClient, op *lropb.Operation) error {
	ticker := time.NewTicker(1 * time.Second)
	defer ticker.Stop()
	for {
		select {
		case <-ticker.C:
			// Poll the operation
			newOp, err := c.GetOperation(ctx, &lropb.GetOperationRequest{Name: op.Name})
			if err != nil {
				return fmt.Errorf("error calling GetOperation() on %s: %v", op.Name, err)
			}
			if newOp.Done {
				if opStatus := newOp.GetError(); opStatus != nil && status.Code(opStatus.Code) != status.OK {
					return fmt.Errorf("operation %s completed with error: %v", op.Name, opStatus)
				}
				return nil
			}
		case <-ctx.Done():
			return ctx.Err()
		}
	}
}
