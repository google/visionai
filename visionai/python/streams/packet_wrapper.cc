// Copyright 2023 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "third_party/pybind11/include/pybind11/pybind11.h"
#include "third_party/pybind11_abseil/status_casters.h"
#include "third_party/pybind11_protobuf/native_proto_caster.h"
#include "visionai/streams/packet/packet.h"

PYBIND11_MODULE(_pywrap_packet, m) {
  pybind11::google::ImportStatusModule();
  pybind11_protobuf::ImportNativeProtoCasters();

  m.def("make_packet", &visionai::MakePacket<std::string>, pybind11::arg("s"));
}
