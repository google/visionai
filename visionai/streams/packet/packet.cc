// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/packet/packet.h"

#include <cstdint>
#include <utility>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "visionai/util/time_util.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

std::string GetTypeClass(const Packet& p) {
  return p.header().type().type_class();
}

std::string GetType(const Packet& p) {
  return p.header().type().type_descriptor().type();
}

std::string GetTypeName(const Packet& p) {
  absl::string_view type_class = p.header().type().type_class();
  absl::string_view type = p.header().type().type_descriptor().type();
  if (type.empty()) {
    return std::string(type_class);
  }
  return absl::StrFormat("%s/%s", type_class, type);
}

absl::Time GetCaptureTime(const Packet& p) {
  return ToAbseilTimestamp(p.header().capture_time());
}

absl::Status SetCaptureTime(const absl::Time& t, Packet* p) {
  if (p == nullptr) {
    return absl::InvalidArgumentError("Given a nullptr to a Packet");
  }
  *p->mutable_header()->mutable_capture_time() = ToProtoTimestamp(t);
  return absl::OkStatus();
}

absl::Time GetIngestTime(const Packet& p) {
  return ToAbseilTimestamp(p.header().server_metadata().ingest_time());
}

absl::Status SetIngestTime(const absl::Time& t, Packet* p) {
  if (p == nullptr) {
    return absl::InvalidArgumentError("Given a nullptr to a Packet");
  }
  *p->mutable_header()->mutable_server_metadata()->mutable_ingest_time() =
      ToProtoTimestamp(t);
  return absl::OkStatus();
}

int64_t GetOffset(const Packet& p) {
  return p.header().server_metadata().offset();
}

absl::Status SetOffset(int64_t offset, Packet* p) {
  if (p == nullptr) {
    return absl::InvalidArgumentError("Given a nullptr to a Packet");
  }
  p->mutable_header()->mutable_server_metadata()->set_offset(offset);
  return absl::OkStatus();
}

absl::Status SetSeriesName(const std::string& name, Packet* p) {
  if (p == nullptr) {
    return absl::InvalidArgumentError("Given a nullptr to a Packet");
  }
  p->mutable_header()->mutable_series_metadata()->set_series(name);
  return absl::OkStatus();
}

std::string GetSeriesName(const Packet& p) {
  return p.header().series_metadata().series();
}

absl::Status SetMetadataField(const std::string& key,
                              const google::protobuf::Value& value, Packet* p) {
  if (p == nullptr) {
    return absl::InvalidArgumentError("Given a nullptr to a Packet");
  }
  p->mutable_header()->mutable_metadata()->mutable_fields()->operator[](key) =
      value;
  return absl::OkStatus();
}

absl::Status RemoveMetadataField(const std::string& key, Packet* p) {
  if (p == nullptr) {
    return absl::InvalidArgumentError("Given a nullptr to a Packet");
  }
  p->mutable_header()->mutable_metadata()->mutable_fields()->erase(key);
  return absl::OkStatus();
}

absl::StatusOr<google::protobuf::Value> GetMetadataField(const std::string& key,
                                                         const Packet& p) {
  if (!p.header().metadata().fields().contains(key)) {
    return absl::NotFoundError(
        absl::StrFormat("The field %s is unset in the packet metadata", key));
  }
  return p.header().metadata().fields().at(key);
}

bool IsProtobufPacket(const Packet& p) {
  return GetTypeClass(p) == std::string("protobuf");
}

bool IsSignalPacket(const Packet& p) {
  return GetTypeClass(p) == std::string("signal");
}

bool IsPhantomPacket(const Packet& p) {
  return GetTypeName(p) == std::string("signal/phantom");
}

bool IsEOSPacket(const Packet& p) {
  return GetTypeName(p) == std::string("signal/eos");
}

absl::StatusOr<Packet> MakePhantomPacket() {
  Signal s(Signal::SignalCode::kPhantom);
  return MakePacket(std::move(s));
}

absl::StatusOr<Packet> MakeEOSPacket() {
  Signal s(Signal::SignalCode::kEOS);
  return MakePacket(std::move(s));
}

}  // namespace visionai
