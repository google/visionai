// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
//
// -----------------------------------------------------------------------------
// packet_codec_base.h
// -----------------------------------------------------------------------------
//
// This header is the only public interface that users should include to gain
// access to packet codecs. For the most part, users should not even worry about
// this file and strictly deal with packet.h.
//
// For codec users, only two methods are relevant: Pack and Unpack.
//
// For codec developers, please see packet_codec_base.h for more documentation
// on how to add a new codec and how to extend the set of packet types.

#ifndef THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_PACKET_CODEC_BASE_H_
#define THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_PACKET_CODEC_BASE_H_

#include <string>
#include <utility>

#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "absl/time/clock.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {
namespace packet_codecs {
namespace internal {
// PacketCodecBase is the CRTP base class for deriving new packet codecs.
//
// Derived: The type of a specific packet codec.
// T: The C++ type that this codec encodes/decodes to/from a Packet.
//
// Derived classes must also supply implementations of certain methods. These
// methods are all listed under the first protected section. With the exception
// of those methods marked optional, you have to add an *Impl method for each
// function listed there. For example, since TypeClass() is listed there, you
// are required to supply TypeClassImpl().
//
// Please see protected section below for the methods that you must supply.
//
// Example:
//
// Say you want to convert a C++ type Foo into a Packet. You would write a Codec
// as follows:
// ```
// class CodecForFoo : public PacketCodecBase<CodecForFoo, Foo> {
//   public:
//     std::string TypeClassImpl() const { ... }
//     // ... and all the other required methods...
// };
//
// ```
//
// If your goal is to actually extend the set of packet types, then after
// writing your new codec, you also need to add a template specialization in
// codec_selector.h to map your C++ types of interest to your new codec.
template <typename Derived, typename T>
class PacketCodecBase {
 private:
  typedef google::cloud::visionai::v1::Packet Packet;
  typedef google::cloud::visionai::v1::PacketType PacketType;
  typedef google::cloud::visionai::v1::PacketType::TypeDescriptor
      TypeDescriptor;

 public:
  // Packs an object of type T into a Packet.
  absl::Status Pack(const T&, Packet*) const;
  absl::Status Pack(T&&, Packet*) const;

  // Unpacks a Packet into an object of type T.
  absl::Status Unpack(const Packet&, T*) const;
  absl::Status Unpack(Packet&&, T*) const;

  ~PacketCodecBase() = default;
  PacketCodecBase() = default;
  PacketCodecBase(const PacketCodecBase&) = delete;
  PacketCodecBase& operator=(const PacketCodecBase&) = delete;

 protected:
  // The type class of Packets handled by this codec.
  //
  // Derived class MUST implement `TypeClassImpl`.
  std::string TypeClass() const;

  // Method to pack a TypeDescriptor from an object of type T.
  //
  // Derived class MUST implement `PackTypeDescriptorImpl`.
  absl::Status PackTypeDescriptor(const T&, TypeDescriptor*) const;

  // Method to validate a TypeDescriptor before unpacking the payload.
  //
  // Derived class MUST implement `ValidateTypeDescriptorImpl`.
  absl::Status ValidateTypeDescriptor(const TypeDescriptor&) const;

  // Methods to pack the payload string from an object of type T.
  //
  // Derived classes MUST implement `PackPayloadImpl` that takes a const lvalue
  // reference, and MAY implement the rvalue reference overload if any
  // optimizations apply in that case.
  absl::Status PackPayload(const T&, std::string*) const;
  absl::Status PackPayload(T&&, std::string*) const;

  // Methods to unpack the payload string to an object of type T.
  //
  // Derived classes MUST implement `UnpackPayloadImpl` that takes a const
  // lvalue reference, and MAY implement the rvalue reference overload if any
  // optimizations apply in that case.
  absl::Status UnpackPayload(const TypeDescriptor&, const std::string&,
                             T*) const;
  absl::Status UnpackPayload(const TypeDescriptor&, std::string&&, T*) const;

 private:
  absl::Status PackPacketType(const T&, PacketType*) const;
  absl::Status ValidatePacketType(const PacketType&) const;
  const Derived* derived() const { return static_cast<const Derived*>(this); }
};

template <typename Derived, typename T>
std::string PacketCodecBase<Derived, T>::TypeClass() const {
  return derived()->TypeClassImpl();
}

template <typename Derived, typename T>
absl::Status PacketCodecBase<Derived, T>::Pack(const T& t, Packet* p) const {
  if (p == nullptr) {
    return absl::InvalidArgumentError("Given a nullptr to a Packet");
  }
  VAI_RETURN_IF_ERROR(PackPacketType(t, p->mutable_header()->mutable_type()));
  VAI_RETURN_IF_ERROR(PackPayload(t, p->mutable_payload()));
  return absl::OkStatus();
}

template <typename Derived, typename T>
absl::Status PacketCodecBase<Derived, T>::Pack(T&& t, Packet* p) const {
  if (p == nullptr) {
    return absl::InvalidArgumentError("Given a nullptr to a Packet");
  }
  VAI_RETURN_IF_ERROR(PackPacketType(t, p->mutable_header()->mutable_type()));
  VAI_RETURN_IF_ERROR(PackPayload(std::move(t), p->mutable_payload()));
  return absl::OkStatus();
}

template <typename Derived, typename T>
absl::Status PacketCodecBase<Derived, T>::PackPacketType(const T& t,
                                                         PacketType* pt) const {
  *pt->mutable_type_class() = TypeClass();
  VAI_RETURN_IF_ERROR(PackTypeDescriptor(t, pt->mutable_type_descriptor()));
  return absl::OkStatus();
}

template <typename Derived, typename T>
absl::Status PacketCodecBase<Derived, T>::PackTypeDescriptor(
    const T& t, TypeDescriptor* td) const {
  return derived()->PackTypeDescriptorImpl(t, td);
}

template <typename Derived, typename T>
absl::Status PacketCodecBase<Derived, T>::PackPayload(const T& t,
                                                      std::string* s) const {
  return derived()->PackPayloadImpl(t, s);
}

template <typename Derived, typename T>
absl::Status PacketCodecBase<Derived, T>::PackPayload(T&& t,
                                                      std::string* s) const {
  return derived()->PackPayloadImpl(std::move(t), s);
}

template <typename Derived, typename T>
absl::Status PacketCodecBase<Derived, T>::Unpack(const Packet& p, T* t) const {
  if (t == nullptr) {
    return absl::InvalidArgumentError(
        "Given a nullptr to the destination object.");
  }
  VAI_RETURN_IF_ERROR(ValidatePacketType(p.header().type()));
  VAI_RETURN_IF_ERROR(
      UnpackPayload(p.header().type().type_descriptor(), p.payload(), t));
  return absl::OkStatus();
}

template <typename Derived, typename T>
absl::Status PacketCodecBase<Derived, T>::Unpack(Packet&& p, T* t) const {
  if (t == nullptr) {
    return absl::InvalidArgumentError(
        "Given a nullptr to the destination object.");
  }
  VAI_RETURN_IF_ERROR(ValidatePacketType(p.header().type()));
  VAI_RETURN_IF_ERROR(UnpackPayload(p.header().type().type_descriptor(),
                                std::move(*p.mutable_payload()), t));
  return absl::OkStatus();
}

template <typename Derived, typename T>
absl::Status PacketCodecBase<Derived, T>::ValidatePacketType(
    const PacketType& pt) const {
  if (pt.type_class() != TypeClass()) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "Got a Packet with type class \"%s\" but expected \"%s\".",
        pt.type_class(), this->TypeClass()));
  }
  return ValidateTypeDescriptor(pt.type_descriptor());
}

template <typename Derived, typename T>
absl::Status PacketCodecBase<Derived, T>::ValidateTypeDescriptor(
    const TypeDescriptor& td) const {
  return derived()->ValidateTypeDescriptorImpl(td);
}

template <typename Derived, typename T>
absl::Status PacketCodecBase<Derived, T>::UnpackPayload(
    const TypeDescriptor& td, const std::string& s, T* t) const {
  return derived()->UnpackPayloadImpl(td, s, t);
}

template <typename Derived, typename T>
absl::Status PacketCodecBase<Derived, T>::UnpackPayload(
    const TypeDescriptor& td, std::string&& s, T* t) const {
  return derived()->UnpackPayloadImpl(td, std::move(s), t);
}

}  // namespace internal
}  // namespace packet_codecs

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_CODECS_PACKET_CODEC_BASE_H_
