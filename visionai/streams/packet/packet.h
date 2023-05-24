// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
//
// -----------------------------------------------------------------------------
// packet.h
// -----------------------------------------------------------------------------
//
// This header defines how google.cloud.visionai.v1alpha1.Packets are used
// throughout C++ code. Moreover, C++ is the source of truth that defines
// how correctly formed Packets are to be read/written. While it is possible to
// directly compose google.cloud.visionai.v1alpha1.Packets, outside of those
// routines defined here, they are strongly discouraged and not officially
// supported. To conform, you must either use methods defined here, or
// indirectly through any number of higher level programming APIs that are
// supported.
//
// Packets are typed and correspond exactly to one C++ type. However, the
// converse is not true; i.e. not all C++ types are convertible to Packets.
// Which types are able to be converted is decided by the set of codecs that are
// defined in the subdirectory packet_codecs, especially codec_selector.h.
// However, you do not need to worry too much about this mapping, as you will
// get compile errors if you try to convert an unsupported C++ type.
//
// This library defines how you convert an object of a specific C++ type to/from
// a Packet, as well as how you may access/mutate properties of a Packet.
// There are only a few classes of patterns that you need to become familiar
// with. They are the following:
// 1. Packet Marshaling: Methods to convert C++ objects into Packets.
// 2. Packet Unmarshaling: Methods to convert Packets into C++ objects.
// 3. Packet Accessors and Mutators.
//
// This header is organized into these respective sections, and you may consult
// them for more detailed use. The example below shows a fairly typical
// workflow.
//
// Example:
//
// At the sender
// ```
// // You have some protobuf message.
// SomeProtobufMessage msg;
//
// // Create a packet out of it.
// auto packet = MakePacket(std::move(msg));
//
// // Now you can send it to your streams.
// VAI_RETURN_IF_ERROR(Send(packet));
// ```
//
// At the receiver
// ```
// // You get a packet from the sender.
// Packet p;
// VAI_RETURN_IF_ERROR(Receive(&p));
//
// // Get the protobuf message out of it.
// // You may think of packetas as statusor.
// auto packetas = PacketAs<SomeProtobufMessage>(std::move(p));
// if (!packetas.ok()) {
//   // ... handle error...
// }
// SomeProtobufMessage msg = std::move(*packetas);
// ```

#ifndef THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_H_
#define THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_H_

#include <cstdint>
#include <utility>

#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/time/clock.h"
#include "visionai/streams/packet/packet_codecs/packet_codecs.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

// ----------------------------------------------------------------------------
// Type Aliases
// ----------------------------------------------------------------------------
using Packet = google::cloud::visionai::v1::Packet;
using PacketType = google::cloud::visionai::v1::PacketType;
using PacketHeader = google::cloud::visionai::v1::PacketHeader;

// ----------------------------------------------------------------------------
// Packet Marshaling
// ----------------------------------------------------------------------------
//
// The methods in this section create Packets from C++ types.
//
// We remind you that not all C++ types are able to be marshaled into Packets.
// The set of packet codecs decide which can, specifically in codec_selector.h.
// If you need a new type supported, then you may contact us and consider
// extending the set of packet codecs.

// Create a Packet from an object of type T.
template <typename T>
absl::StatusOr<Packet> MakePacket(T&& t);

// Pack the payload from an object of type T to an existing packet.
// In this case, the header type and the payload will be changed to the
// corresponding value in the type T. All the other fields in the header,
// including the capture time and the metadata, will remain unchanged.
template <typename T>
absl::Status Pack(T&& t, Packet* p);

// ----------------------------------------------------------------------------
// Packet Unmarshaling
// ----------------------------------------------------------------------------
//
// The methods and classes in this section extract C++ types from Packets.
//
// A PacketAs object is an adapter used to acquire/access the data in a given
// Packet. It provides methods so that the user can do so safely and
// conveniently. It is intended to be used similariy to StatusOr.
//
// Example:
//
// Suppose you have a Packet that you know converts to a C++ of type T. Then you
// may extract a C++ object of type T from it like so:
// ```
// Packet p;
// VAI_RETURN_IF_ERROR(Receive(&p));
// auto packetas = PacketAs<T>(p);
// if (!packetas.ok()) {
//   // Handle the error. You will end up here if the packet you received
//   // cannot actually get adapted into an object of type T.
// }
// auto t = std::move(*packetas);
// ```
//
// You might ask how you would know, a piori, what type T a Packet might convert
// to. If you are using Vision AI Streams, then you will be able to read this
// off of the Stream resource; i.e. the Stream knows what Packet type it
// holds at any given time, and from the type name, you'll be able to deduce the
// C++ type that corresponds to it.
template <typename T>
class PacketAs {
 public:
  // The value type that this PacketAs tries to convert into.
  typedef T value_type;

  // Constructs an instance by copying a source packet.
  explicit PacketAs(const Packet&);

  // Constructs an instance by moving in the source packet.
  explicit PacketAs(Packet&&);

  // Returns true if the value of the source packet is successfully adapted and
  // ready for access as a value of type T.
  bool ok();

  // Returns the status of the packet adaptation.
  absl::Status status() const;

  // Returns the header of the packet.
  const PacketHeader& header() const;

  // Returns a reference to a successfully converted object of type T if
  // `this->ok()`. Otherwise, it will CHECK-fail.
  //
  // You should prefer to use operator*, similar to what you would do with
  // absl::StatusOr.
  const T& value() const&;
  T& value() &;
  const T&& value() const&&;
  T&& value() &&;

  // Returns a reference to a successfully converted object of type T if
  // `this->ok()`. Otherwise, it will CHECK-fail.
  const T& operator*() const&;
  T& operator*() &;
  const T&& operator*() const&&;
  T&& operator*() &&;

  // Returns a pointer to a successfully converted object of type T if
  // `this->ok()`. Otherwise, it will CHECK-fail.
  const T* operator->() const;
  T* operator->();

  // Constructs an instance with a default constructed source Packet.
  // ok() will be false.
  PacketAs();

  // PacketAs is copy constructable/assignable if T is.
  PacketAs(const PacketAs<T>&) = default;
  PacketAs<T>& operator=(const PacketAs<T>&) = default;

  // PacketAs is move constructable/assignable if T is.
  PacketAs(PacketAs<T>&&) = default;
  PacketAs<T>& operator=(PacketAs<T>&&) = default;

 private:
  void Adapt();
  void EnsureOk();

  // Status indicating whether the adaptation was successful and why if not.
  absl::Status status_;

  // This is a Packet that holds the same metadata as the source packet.
  // However, its payload may differ and is likely empty.
  Packet packet_;

  // If ok() is true, this holds the source packet value adapted as a value of
  // type T. Otherwise, accessing this value is undefined behavior.
  T value_;
};

// ----------------------------------------------------------------------------
// Packet Accessors and Mutators
// ----------------------------------------------------------------------------
//
// The methods below are a set of functions that access or mutate Packets.
// Please use these methods instead of directly doing so through the Packet
// message, especially for mutators.

// Get the type class of the packet.
//
// This is the type class of the packet. Use `GetTypeName` for the full type of
// the packet.
std::string GetTypeClass(const Packet& p);

// Get the type value of the packet.
//
// This is the type value of the packet. Use `GetTypeName` for the full type of
// the packet.
std::string GetType(const Packet& p);

// Get the type name of the packet.
//
// This is the fully qualified name, prefixed by the type class.
std::string GetTypeName(const Packet& p);

// Get the capture time of the Packet.
absl::Time GetCaptureTime(const Packet&);

// Set the capture time of the Packet to a specific time.
absl::Status SetCaptureTime(const absl::Time&, Packet*);

// Get the ingest time of the Packet.
absl::Time GetIngestTime(const Packet&);

// Set the ingest time of the Packet to a specific time.
absl::Status SetIngestTime(const absl::Time&, Packet*);

// Get the offset of the Packet.
int64_t GetOffset(const Packet&);

// Set the offset of the Packet.
absl::Status SetOffset(int64_t offset, Packet*);

// Set the series name the Packet belongs to.
absl::Status SetSeriesName(const std::string&, Packet*);

// Get the series name of the Packet.
std::string GetSeriesName(const Packet&);

// Set a metadata field.
absl::Status SetMetadataField(const std::string&,
                              const google::protobuf::Value&, Packet*);

// Remove a metadata field.
absl::Status RemoveMetadataField(const std::string&, Packet*);

// Get a metadata field.
absl::StatusOr<google::protobuf::Value> GetMetadataField(const std::string&,
                                                         const Packet&);

// Decide whether the given packet is a Protobuf packet.
bool IsProtobufPacket(const Packet& p);

// ----------------------------------------------------------------------------
// Methods for working with signal Packets
// ----------------------------------------------------------------------------
//
// The methods below are wrapper functions to deal specifically with signal
// packets.
//
// Although signal packets themselves are just another packet type,
// they are often used in contexts where some data plane signaling is needed,
// rather than actually conveying any "real" data. For this reason, their common
// case usage pattern needs a few extra steps compared to the usual packet
// types. These methods standardizes and cleans up the call site for the most
// common uses of signals.

// Decide whether the given packet is a Signal packet.
bool IsSignalPacket(const Packet&);

// Decide whether the given packet is a phantom packet.
bool IsPhantomPacket(const Packet&);

// Decide whether the given packet is an eos packet.
bool IsEOSPacket(const Packet&);

// Create a phantom packet.
absl::StatusOr<Packet> MakePhantomPacket();

// Create an eos packet.
absl::StatusOr<Packet> MakeEOSPacket();

// ----------------------------------------------------------------------------
// Implementation

template <typename T>
absl::StatusOr<Packet> MakePacket(T&& t) {
  Packet p;
  VAI_RETURN_IF_ERROR(packet_codecs::Pack(t, &p));
  VAI_RETURN_IF_ERROR(SetCaptureTime(absl::Now(), &p));
  return p;
}

template <typename T>
absl::Status Pack(T&& t, Packet* p) {
  VAI_RETURN_IF_ERROR(packet_codecs::Pack(t, p));
  return absl::OkStatus();
}

template <typename T>
PacketAs<T>::PacketAs() {
  status_ = absl::UnknownError("This is a default constructed PacketAs");
}

template <typename T>
PacketAs<T>::PacketAs(const Packet& packet) : packet_(packet) {
  Adapt();
}

template <typename T>
PacketAs<T>::PacketAs(Packet&& packet) : packet_(std::move(packet)) {
  Adapt();
}

// Only call this during construction.
template <typename T>
void PacketAs<T>::Adapt() {
  Packet hollow_packet;
  *hollow_packet.mutable_header() = packet_.header();
  status_ = packet_codecs::Unpack(std::move(packet_), &value_);
  packet_ = hollow_packet;
}

template <typename T>
bool PacketAs<T>::ok() {
  return status_.ok();
}

template <typename T>
absl::Status PacketAs<T>::status() const {
  return status_;
}

template <typename T>
const PacketHeader& PacketAs<T>::header() const {
  return packet_.header();
}

template <typename T>
void PacketAs<T>::EnsureOk() {
  if (!ok()) {
    LOG(FATAL) << "The PacketAs was not successfully adapted: " << status_;
  }
}

template <typename T>
const T& PacketAs<T>::value() const& {
  EnsureOk();
  return value_;
}

template <typename T>
T& PacketAs<T>::value() & {
  EnsureOk();
  return value_;
}

template <typename T>
const T&& PacketAs<T>::value() const&& {
  EnsureOk();
  return std::move(value_);
}

template <typename T>
T&& PacketAs<T>::value() && {
  EnsureOk();
  return std::move(value_);
}

template <typename T>
const T& PacketAs<T>::operator*() const& {
  EnsureOk();
  return value_;
}

template <typename T>
T& PacketAs<T>::operator*() & {
  EnsureOk();
  return value_;
}

template <typename T>
const T&& PacketAs<T>::operator*() const&& {
  EnsureOk();
  return std::move(value_);
}

template <typename T>
T&& PacketAs<T>::operator*() && {
  EnsureOk();
  return std::move(value_);
}

template <typename T>
const T* PacketAs<T>::operator->() const {
  EnsureOk();
  return &value_;
}

template <typename T>
T* PacketAs<T>::operator->() {
  EnsureOk();
  return &value_;
}

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_PACKET_PACKET_H_
