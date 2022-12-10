// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

 #include "visionai/util/net/grpc/status_util.h"

namespace visionai {

namespace {

absl::StatusCode MapStatusCode(grpc::StatusCode const& code) {
  switch (code) {
    case grpc::StatusCode::OK:
      return absl::StatusCode::kOk;
    case grpc::StatusCode::CANCELLED:
      return absl::StatusCode::kCancelled;
    case grpc::StatusCode::UNKNOWN:
      return absl::StatusCode::kUnknown;
    case grpc::StatusCode::INVALID_ARGUMENT:
      return absl::StatusCode::kInvalidArgument;
    case grpc::StatusCode::DEADLINE_EXCEEDED:
      return absl::StatusCode::kDeadlineExceeded;
    case grpc::StatusCode::NOT_FOUND:
      return absl::StatusCode::kNotFound;
    case grpc::StatusCode::ALREADY_EXISTS:
      return absl::StatusCode::kAlreadyExists;
    case grpc::StatusCode::PERMISSION_DENIED:
      return absl::StatusCode::kPermissionDenied;
    case grpc::StatusCode::UNAUTHENTICATED:
      return absl::StatusCode::kUnauthenticated;
    case grpc::StatusCode::RESOURCE_EXHAUSTED:
      return absl::StatusCode::kResourceExhausted;
    case grpc::StatusCode::FAILED_PRECONDITION:
      return absl::StatusCode::kFailedPrecondition;
    case grpc::StatusCode::ABORTED:
      return absl::StatusCode::kAborted;
    case grpc::StatusCode::OUT_OF_RANGE:
      return absl::StatusCode::kOutOfRange;
    case grpc::StatusCode::UNIMPLEMENTED:
      return absl::StatusCode::kUnimplemented;
    case grpc::StatusCode::INTERNAL:
      return absl::StatusCode::kInternal;
    case grpc::StatusCode::UNAVAILABLE:
      return absl::StatusCode::kUnavailable;
    case grpc::StatusCode::DATA_LOSS:
      return absl::StatusCode::kDataLoss;
    default:
      return absl::StatusCode::kUnknown;
  }
}

grpc::StatusCode MapStatusCode(absl::StatusCode const& code) {
  switch (code) {
    case absl::StatusCode::kOk:
      return grpc::StatusCode::OK;
    case absl::StatusCode::kCancelled:
      return grpc::StatusCode::CANCELLED;
    case absl::StatusCode::kUnknown:
      return grpc::StatusCode::UNKNOWN;
    case absl::StatusCode::kInvalidArgument:
      return grpc::StatusCode::INVALID_ARGUMENT;
    case absl::StatusCode::kDeadlineExceeded:
      return grpc::StatusCode::DEADLINE_EXCEEDED;
    case absl::StatusCode::kNotFound:
      return grpc::StatusCode::NOT_FOUND;
    case absl::StatusCode::kAlreadyExists:
      return grpc::StatusCode::ALREADY_EXISTS;
    case absl::StatusCode::kPermissionDenied:
      return grpc::StatusCode::PERMISSION_DENIED;
    case absl::StatusCode::kUnauthenticated:
      return grpc::StatusCode::UNAUTHENTICATED;
    case absl::StatusCode::kResourceExhausted:
      return grpc::StatusCode::RESOURCE_EXHAUSTED;
    case absl::StatusCode::kFailedPrecondition:
      return grpc::StatusCode::FAILED_PRECONDITION;
    case absl::StatusCode::kAborted:
      return grpc::StatusCode::ABORTED;
    case absl::StatusCode::kOutOfRange:
      return grpc::StatusCode::OUT_OF_RANGE;
    case absl::StatusCode::kUnimplemented:
      return grpc::StatusCode::UNIMPLEMENTED;
    case absl::StatusCode::kInternal:
      return grpc::StatusCode::INTERNAL;
    case absl::StatusCode::kUnavailable:
      return grpc::StatusCode::UNAVAILABLE;
    case absl::StatusCode::kDataLoss:
      return grpc::StatusCode::DATA_LOSS;
    default:
      return grpc::StatusCode::UNKNOWN;
  }
}

}  // namespace

absl::Status ToAbseilStatus(const grpc::Status& status) {
  return absl::Status(MapStatusCode(status.error_code()),
                      status.error_message());
}

grpc::Status ToGrpcStatus(const absl::Status& status) {
  return grpc::Status(MapStatusCode(status.code()),
                      std::string{status.message()});
}

}  // namespace visionai
