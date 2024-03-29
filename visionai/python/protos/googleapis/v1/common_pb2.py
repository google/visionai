# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: visionai/python/protos/googleapis/v1/common.proto
"""Generated protocol buffer code."""
from google.protobuf.internal import builder as _builder
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


from google.api import field_behavior_pb2 as google_dot_api_dot_field__behavior__pb2
from google.api import resource_pb2 as google_dot_api_dot_resource__pb2
from google.protobuf import timestamp_pb2 as google_dot_protobuf_dot_timestamp__pb2


DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n1visionai/python/protos/googleapis/v1/common.proto\x12\x18google.cloud.visionai.v1\x1a\x1fgoogle/api/field_behavior.proto\x1a\x19google/api/resource.proto\x1a\x1fgoogle/protobuf/timestamp.proto\"\xac\x05\n\x07\x43luster\x12\x11\n\x04name\x18\x01 \x01(\tB\x03\xe0\x41\x03\x12\x34\n\x0b\x63reate_time\x18\x02 \x01(\x0b\x32\x1a.google.protobuf.TimestampB\x03\xe0\x41\x03\x12\x34\n\x0bupdate_time\x18\x03 \x01(\x0b\x32\x1a.google.protobuf.TimestampB\x03\xe0\x41\x03\x12=\n\x06labels\x18\x04 \x03(\x0b\x32-.google.cloud.visionai.v1.Cluster.LabelsEntry\x12G\n\x0b\x61nnotations\x18\x05 \x03(\x0b\x32\x32.google.cloud.visionai.v1.Cluster.AnnotationsEntry\x12\'\n\x1a\x64\x61taplane_service_endpoint\x18\x06 \x01(\tB\x03\xe0\x41\x03\x12;\n\x05state\x18\x07 \x01(\x0e\x32\'.google.cloud.visionai.v1.Cluster.StateB\x03\xe0\x41\x03\x12\x17\n\npsc_target\x18\x08 \x01(\tB\x03\xe0\x41\x03\x1a-\n\x0bLabelsEntry\x12\x0b\n\x03key\x18\x01 \x01(\t\x12\r\n\x05value\x18\x02 \x01(\t:\x02\x38\x01\x1a\x32\n\x10\x41nnotationsEntry\x12\x0b\n\x03key\x18\x01 \x01(\t\x12\r\n\x05value\x18\x02 \x01(\t:\x02\x38\x01\"V\n\x05State\x12\x15\n\x11STATE_UNSPECIFIED\x10\x00\x12\x10\n\x0cPROVISIONING\x10\x01\x12\x0b\n\x07RUNNING\x10\x02\x12\x0c\n\x08STOPPING\x10\x03\x12\t\n\x05\x45RROR\x10\x04:`\xea\x41]\n\x1fvisionai.googleapis.com/Cluster\x12:projects/{project}/locations/{location}/clusters/{cluster}\"\x80\x02\n\x11OperationMetadata\x12\x34\n\x0b\x63reate_time\x18\x01 \x01(\x0b\x32\x1a.google.protobuf.TimestampB\x03\xe0\x41\x03\x12\x31\n\x08\x65nd_time\x18\x02 \x01(\x0b\x32\x1a.google.protobuf.TimestampB\x03\xe0\x41\x03\x12\x13\n\x06target\x18\x03 \x01(\tB\x03\xe0\x41\x03\x12\x11\n\x04verb\x18\x04 \x01(\tB\x03\xe0\x41\x03\x12\x1b\n\x0estatus_message\x18\x05 \x01(\tB\x03\xe0\x41\x03\x12#\n\x16requested_cancellation\x18\x06 \x01(\x08\x42\x03\xe0\x41\x03\x12\x18\n\x0b\x61pi_version\x18\x07 \x01(\tB\x03\xe0\x41\x03\"\x1e\n\tGcsSource\x12\x11\n\x04uris\x18\x01 \x03(\tB\x03\xe0\x41\x02\x42\xbb\x01\n\x1c\x63om.google.cloud.visionai.v1B\x0b\x43ommonProtoP\x01Z8cloud.google.com/go/visionai/apiv1/visionaipb;visionaipb\xaa\x02\x18Google.Cloud.VisionAI.V1\xca\x02\x18Google\\Cloud\\VisionAI\\V1\xea\x02\x1bGoogle::Cloud::VisionAI::V1b\x06proto3')

_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, globals())
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'visionai.python.protos.googleapis.v1.common_pb2', globals())
if _descriptor._USE_C_DESCRIPTORS == False:

  DESCRIPTOR._options = None
  DESCRIPTOR._serialized_options = b'\n\034com.google.cloud.visionai.v1B\013CommonProtoP\001Z8cloud.google.com/go/visionai/apiv1/visionaipb;visionaipb\252\002\030Google.Cloud.VisionAI.V1\312\002\030Google\\Cloud\\VisionAI\\V1\352\002\033Google::Cloud::VisionAI::V1'
  _CLUSTER_LABELSENTRY._options = None
  _CLUSTER_LABELSENTRY._serialized_options = b'8\001'
  _CLUSTER_ANNOTATIONSENTRY._options = None
  _CLUSTER_ANNOTATIONSENTRY._serialized_options = b'8\001'
  _CLUSTER.fields_by_name['name']._options = None
  _CLUSTER.fields_by_name['name']._serialized_options = b'\340A\003'
  _CLUSTER.fields_by_name['create_time']._options = None
  _CLUSTER.fields_by_name['create_time']._serialized_options = b'\340A\003'
  _CLUSTER.fields_by_name['update_time']._options = None
  _CLUSTER.fields_by_name['update_time']._serialized_options = b'\340A\003'
  _CLUSTER.fields_by_name['dataplane_service_endpoint']._options = None
  _CLUSTER.fields_by_name['dataplane_service_endpoint']._serialized_options = b'\340A\003'
  _CLUSTER.fields_by_name['state']._options = None
  _CLUSTER.fields_by_name['state']._serialized_options = b'\340A\003'
  _CLUSTER.fields_by_name['psc_target']._options = None
  _CLUSTER.fields_by_name['psc_target']._serialized_options = b'\340A\003'
  _CLUSTER._options = None
  _CLUSTER._serialized_options = b'\352A]\n\037visionai.googleapis.com/Cluster\022:projects/{project}/locations/{location}/clusters/{cluster}'
  _OPERATIONMETADATA.fields_by_name['create_time']._options = None
  _OPERATIONMETADATA.fields_by_name['create_time']._serialized_options = b'\340A\003'
  _OPERATIONMETADATA.fields_by_name['end_time']._options = None
  _OPERATIONMETADATA.fields_by_name['end_time']._serialized_options = b'\340A\003'
  _OPERATIONMETADATA.fields_by_name['target']._options = None
  _OPERATIONMETADATA.fields_by_name['target']._serialized_options = b'\340A\003'
  _OPERATIONMETADATA.fields_by_name['verb']._options = None
  _OPERATIONMETADATA.fields_by_name['verb']._serialized_options = b'\340A\003'
  _OPERATIONMETADATA.fields_by_name['status_message']._options = None
  _OPERATIONMETADATA.fields_by_name['status_message']._serialized_options = b'\340A\003'
  _OPERATIONMETADATA.fields_by_name['requested_cancellation']._options = None
  _OPERATIONMETADATA.fields_by_name['requested_cancellation']._serialized_options = b'\340A\003'
  _OPERATIONMETADATA.fields_by_name['api_version']._options = None
  _OPERATIONMETADATA.fields_by_name['api_version']._serialized_options = b'\340A\003'
  _GCSSOURCE.fields_by_name['uris']._options = None
  _GCSSOURCE.fields_by_name['uris']._serialized_options = b'\340A\002'
  _CLUSTER._serialized_start=173
  _CLUSTER._serialized_end=857
  _CLUSTER_LABELSENTRY._serialized_start=574
  _CLUSTER_LABELSENTRY._serialized_end=619
  _CLUSTER_ANNOTATIONSENTRY._serialized_start=621
  _CLUSTER_ANNOTATIONSENTRY._serialized_end=671
  _CLUSTER_STATE._serialized_start=673
  _CLUSTER_STATE._serialized_end=759
  _OPERATIONMETADATA._serialized_start=860
  _OPERATIONMETADATA._serialized_end=1116
  _GCSSOURCE._serialized_start=1118
  _GCSSOURCE._serialized_end=1148
# @@protoc_insertion_point(module_scope)
