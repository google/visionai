# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: visionai/python/protos/googleapis/v1/streams_service.proto
"""Generated protocol buffer code."""
from google.protobuf.internal import builder as _builder
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


from google.api import annotations_pb2 as google_dot_api_dot_annotations__pb2
from google.api import client_pb2 as google_dot_api_dot_client__pb2
from google.api import field_behavior_pb2 as google_dot_api_dot_field__behavior__pb2
from google.api import resource_pb2 as google_dot_api_dot_resource__pb2
from visionai.python.protos.googleapis.v1 import common_pb2 as visionai_dot_python_dot_protos_dot_googleapis_dot_v1_dot_common__pb2
from visionai.python.protos.googleapis.v1 import streams_resources_pb2 as visionai_dot_python_dot_protos_dot_googleapis_dot_v1_dot_streams__resources__pb2
from google.longrunning import operations_pb2 as google_dot_longrunning_dot_operations__pb2
from google.protobuf import empty_pb2 as google_dot_protobuf_dot_empty__pb2
from google.protobuf import field_mask_pb2 as google_dot_protobuf_dot_field__mask__pb2
from google.protobuf import timestamp_pb2 as google_dot_protobuf_dot_timestamp__pb2


DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n:visionai/python/protos/googleapis/v1/streams_service.proto\x12\x18google.cloud.visionai.v1\x1a\x1cgoogle/api/annotations.proto\x1a\x17google/api/client.proto\x1a\x1fgoogle/api/field_behavior.proto\x1a\x19google/api/resource.proto\x1a\x31visionai/python/protos/googleapis/v1/common.proto\x1a<visionai/python/protos/googleapis/v1/streams_resources.proto\x1a#google/longrunning/operations.proto\x1a\x1bgoogle/protobuf/empty.proto\x1a google/protobuf/field_mask.proto\x1a\x1fgoogle/protobuf/timestamp.proto\"\x99\x01\n\x13ListClustersRequest\x12\x39\n\x06parent\x18\x01 \x01(\tB)\xe0\x41\x02\xfa\x41#\n!locations.googleapis.com/Location\x12\x11\n\tpage_size\x18\x02 \x01(\x05\x12\x12\n\npage_token\x18\x03 \x01(\t\x12\x0e\n\x06\x66ilter\x18\x04 \x01(\t\x12\x10\n\x08order_by\x18\x05 \x01(\t\"y\n\x14ListClustersResponse\x12\x33\n\x08\x63lusters\x18\x01 \x03(\x0b\x32!.google.cloud.visionai.v1.Cluster\x12\x17\n\x0fnext_page_token\x18\x02 \x01(\t\x12\x13\n\x0bunreachable\x18\x03 \x03(\t\"J\n\x11GetClusterRequest\x12\x35\n\x04name\x18\x01 \x01(\tB\'\xe0\x41\x02\xfa\x41!\n\x1fvisionai.googleapis.com/Cluster\"\xba\x01\n\x14\x43reateClusterRequest\x12\x37\n\x06parent\x18\x01 \x01(\tB\'\xe0\x41\x02\xfa\x41!\x12\x1fvisionai.googleapis.com/Cluster\x12\x17\n\ncluster_id\x18\x02 \x01(\tB\x03\xe0\x41\x02\x12\x37\n\x07\x63luster\x18\x03 \x01(\x0b\x32!.google.cloud.visionai.v1.ClusterB\x03\xe0\x41\x02\x12\x17\n\nrequest_id\x18\x04 \x01(\tB\x03\xe0\x41\x01\"\x9e\x01\n\x14UpdateClusterRequest\x12\x34\n\x0bupdate_mask\x18\x01 \x01(\x0b\x32\x1a.google.protobuf.FieldMaskB\x03\xe0\x41\x02\x12\x37\n\x07\x63luster\x18\x02 \x01(\x0b\x32!.google.cloud.visionai.v1.ClusterB\x03\xe0\x41\x02\x12\x17\n\nrequest_id\x18\x03 \x01(\tB\x03\xe0\x41\x01\"f\n\x14\x44\x65leteClusterRequest\x12\x35\n\x04name\x18\x01 \x01(\tB\'\xe0\x41\x02\xfa\x41!\n\x1fvisionai.googleapis.com/Cluster\x12\x17\n\nrequest_id\x18\x02 \x01(\tB\x03\xe0\x41\x01\"\x96\x01\n\x12ListStreamsRequest\x12\x37\n\x06parent\x18\x01 \x01(\tB\'\xe0\x41\x02\xfa\x41!\n\x1fvisionai.googleapis.com/Cluster\x12\x11\n\tpage_size\x18\x02 \x01(\x05\x12\x12\n\npage_token\x18\x03 \x01(\t\x12\x0e\n\x06\x66ilter\x18\x04 \x01(\t\x12\x10\n\x08order_by\x18\x05 \x01(\t\"v\n\x13ListStreamsResponse\x12\x31\n\x07streams\x18\x01 \x03(\x0b\x32 .google.cloud.visionai.v1.Stream\x12\x17\n\x0fnext_page_token\x18\x02 \x01(\t\x12\x13\n\x0bunreachable\x18\x03 \x03(\t\"H\n\x10GetStreamRequest\x12\x34\n\x04name\x18\x01 \x01(\tB&\xe0\x41\x02\xfa\x41 \n\x1evisionai.googleapis.com/Stream\"\xb6\x01\n\x13\x43reateStreamRequest\x12\x37\n\x06parent\x18\x01 \x01(\tB\'\xe0\x41\x02\xfa\x41!\n\x1fvisionai.googleapis.com/Cluster\x12\x16\n\tstream_id\x18\x02 \x01(\tB\x03\xe0\x41\x02\x12\x35\n\x06stream\x18\x03 \x01(\x0b\x32 .google.cloud.visionai.v1.StreamB\x03\xe0\x41\x02\x12\x17\n\nrequest_id\x18\x04 \x01(\tB\x03\xe0\x41\x01\"\x9b\x01\n\x13UpdateStreamRequest\x12\x34\n\x0bupdate_mask\x18\x01 \x01(\x0b\x32\x1a.google.protobuf.FieldMaskB\x03\xe0\x41\x02\x12\x35\n\x06stream\x18\x02 \x01(\x0b\x32 .google.cloud.visionai.v1.StreamB\x03\xe0\x41\x02\x12\x17\n\nrequest_id\x18\x03 \x01(\tB\x03\xe0\x41\x01\"d\n\x13\x44\x65leteStreamRequest\x12\x34\n\x04name\x18\x01 \x01(\tB&\xe0\x41\x02\xfa\x41 \n\x1evisionai.googleapis.com/Stream\x12\x17\n\nrequest_id\x18\x02 \x01(\tB\x03\xe0\x41\x01\"{\n\x19GetStreamThumbnailRequest\x12\x13\n\x06stream\x18\x01 \x01(\tB\x03\xe0\x41\x02\x12\x1c\n\x0fgcs_object_name\x18\x02 \x01(\tB\x03\xe0\x41\x02\x12\x12\n\x05\x65vent\x18\x03 \x01(\tB\x03\xe0\x41\x01\x12\x17\n\nrequest_id\x18\x04 \x01(\tB\x03\xe0\x41\x01\"\x1c\n\x1aGetStreamThumbnailResponse\"4\n\x1dGenerateStreamHlsTokenRequest\x12\x13\n\x06stream\x18\x01 \x01(\tB\x03\xe0\x41\x02\"d\n\x1eGenerateStreamHlsTokenResponse\x12\r\n\x05token\x18\x01 \x01(\t\x12\x33\n\x0f\x65xpiration_time\x18\x02 \x01(\x0b\x32\x1a.google.protobuf.Timestamp\"\x95\x01\n\x11ListEventsRequest\x12\x37\n\x06parent\x18\x01 \x01(\tB\'\xe0\x41\x02\xfa\x41!\n\x1fvisionai.googleapis.com/Cluster\x12\x11\n\tpage_size\x18\x02 \x01(\x05\x12\x12\n\npage_token\x18\x03 \x01(\t\x12\x0e\n\x06\x66ilter\x18\x04 \x01(\t\x12\x10\n\x08order_by\x18\x05 \x01(\t\"s\n\x12ListEventsResponse\x12/\n\x06\x65vents\x18\x01 \x03(\x0b\x32\x1f.google.cloud.visionai.v1.Event\x12\x17\n\x0fnext_page_token\x18\x02 \x01(\t\x12\x13\n\x0bunreachable\x18\x03 \x03(\t\"F\n\x0fGetEventRequest\x12\x33\n\x04name\x18\x01 \x01(\tB%\xe0\x41\x02\xfa\x41\x1f\n\x1dvisionai.googleapis.com/Event\"\xb2\x01\n\x12\x43reateEventRequest\x12\x37\n\x06parent\x18\x01 \x01(\tB\'\xe0\x41\x02\xfa\x41!\n\x1fvisionai.googleapis.com/Cluster\x12\x15\n\x08\x65vent_id\x18\x02 \x01(\tB\x03\xe0\x41\x02\x12\x33\n\x05\x65vent\x18\x03 \x01(\x0b\x32\x1f.google.cloud.visionai.v1.EventB\x03\xe0\x41\x02\x12\x17\n\nrequest_id\x18\x04 \x01(\tB\x03\xe0\x41\x01\"\x98\x01\n\x12UpdateEventRequest\x12\x34\n\x0bupdate_mask\x18\x01 \x01(\x0b\x32\x1a.google.protobuf.FieldMaskB\x03\xe0\x41\x02\x12\x33\n\x05\x65vent\x18\x02 \x01(\x0b\x32\x1f.google.cloud.visionai.v1.EventB\x03\xe0\x41\x02\x12\x17\n\nrequest_id\x18\x03 \x01(\tB\x03\xe0\x41\x01\"b\n\x12\x44\x65leteEventRequest\x12\x33\n\x04name\x18\x01 \x01(\tB%\xe0\x41\x02\xfa\x41\x1f\n\x1dvisionai.googleapis.com/Event\x12\x17\n\nrequest_id\x18\x02 \x01(\tB\x03\xe0\x41\x01\"\x95\x01\n\x11ListSeriesRequest\x12\x37\n\x06parent\x18\x01 \x01(\tB\'\xe0\x41\x02\xfa\x41!\n\x1fvisionai.googleapis.com/Cluster\x12\x11\n\tpage_size\x18\x02 \x01(\x05\x12\x12\n\npage_token\x18\x03 \x01(\t\x12\x0e\n\x06\x66ilter\x18\x04 \x01(\t\x12\x10\n\x08order_by\x18\x05 \x01(\t\"t\n\x12ListSeriesResponse\x12\x30\n\x06series\x18\x01 \x03(\x0b\x32 .google.cloud.visionai.v1.Series\x12\x17\n\x0fnext_page_token\x18\x02 \x01(\t\x12\x13\n\x0bunreachable\x18\x03 \x03(\t\"H\n\x10GetSeriesRequest\x12\x34\n\x04name\x18\x01 \x01(\tB&\xe0\x41\x02\xfa\x41 \n\x1evisionai.googleapis.com/Series\"\xb6\x01\n\x13\x43reateSeriesRequest\x12\x37\n\x06parent\x18\x01 \x01(\tB\'\xe0\x41\x02\xfa\x41!\n\x1fvisionai.googleapis.com/Cluster\x12\x16\n\tseries_id\x18\x02 \x01(\tB\x03\xe0\x41\x02\x12\x35\n\x06series\x18\x03 \x01(\x0b\x32 .google.cloud.visionai.v1.SeriesB\x03\xe0\x41\x02\x12\x17\n\nrequest_id\x18\x04 \x01(\tB\x03\xe0\x41\x01\"\x9b\x01\n\x13UpdateSeriesRequest\x12\x34\n\x0bupdate_mask\x18\x01 \x01(\x0b\x32\x1a.google.protobuf.FieldMaskB\x03\xe0\x41\x02\x12\x35\n\x06series\x18\x02 \x01(\x0b\x32 .google.cloud.visionai.v1.SeriesB\x03\xe0\x41\x02\x12\x17\n\nrequest_id\x18\x03 \x01(\tB\x03\xe0\x41\x01\"d\n\x13\x44\x65leteSeriesRequest\x12\x34\n\x04name\x18\x01 \x01(\tB&\xe0\x41\x02\xfa\x41 \n\x1evisionai.googleapis.com/Series\x12\x17\n\nrequest_id\x18\x02 \x01(\tB\x03\xe0\x41\x01\"\xbf\x01\n\x19MaterializeChannelRequest\x12\x37\n\x06parent\x18\x01 \x01(\tB\'\xe0\x41\x02\xfa\x41!\n\x1fvisionai.googleapis.com/Cluster\x12\x17\n\nchannel_id\x18\x02 \x01(\tB\x03\xe0\x41\x02\x12\x37\n\x07\x63hannel\x18\x03 \x01(\x0b\x32!.google.cloud.visionai.v1.ChannelB\x03\xe0\x41\x02\x12\x17\n\nrequest_id\x18\x04 \x01(\tB\x03\xe0\x41\x01\x32\xbc%\n\x0eStreamsService\x12\xac\x01\n\x0cListClusters\x12-.google.cloud.visionai.v1.ListClustersRequest\x1a..google.cloud.visionai.v1.ListClustersResponse\"=\x82\xd3\xe4\x93\x02.\x12,/v1/{parent=projects/*/locations/*}/clusters\xda\x41\x06parent\x12\x99\x01\n\nGetCluster\x12+.google.cloud.visionai.v1.GetClusterRequest\x1a!.google.cloud.visionai.v1.Cluster\";\x82\xd3\xe4\x93\x02.\x12,/v1/{name=projects/*/locations/*/clusters/*}\xda\x41\x04name\x12\xd8\x01\n\rCreateCluster\x12..google.cloud.visionai.v1.CreateClusterRequest\x1a\x1d.google.longrunning.Operation\"x\x82\xd3\xe4\x93\x02\x37\",/v1/{parent=projects/*/locations/*}/clusters:\x07\x63luster\xda\x41\x19parent,cluster,cluster_id\xca\x41\x1c\n\x07\x43luster\x12\x11OperationMetadata\x12\xda\x01\n\rUpdateCluster\x12..google.cloud.visionai.v1.UpdateClusterRequest\x1a\x1d.google.longrunning.Operation\"z\x82\xd3\xe4\x93\x02?24/v1/{cluster.name=projects/*/locations/*/clusters/*}:\x07\x63luster\xda\x41\x13\x63luster,update_mask\xca\x41\x1c\n\x07\x43luster\x12\x11OperationMetadata\x12\xc8\x01\n\rDeleteCluster\x12..google.cloud.visionai.v1.DeleteClusterRequest\x1a\x1d.google.longrunning.Operation\"h\x82\xd3\xe4\x93\x02.*,/v1/{name=projects/*/locations/*/clusters/*}\xda\x41\x04name\xca\x41*\n\x15google.protobuf.Empty\x12\x11OperationMetadata\x12\xb3\x01\n\x0bListStreams\x12,.google.cloud.visionai.v1.ListStreamsRequest\x1a-.google.cloud.visionai.v1.ListStreamsResponse\"G\x82\xd3\xe4\x93\x02\x38\x12\x36/v1/{parent=projects/*/locations/*/clusters/*}/streams\xda\x41\x06parent\x12\xa0\x01\n\tGetStream\x12*.google.cloud.visionai.v1.GetStreamRequest\x1a .google.cloud.visionai.v1.Stream\"E\x82\xd3\xe4\x93\x02\x38\x12\x36/v1/{name=projects/*/locations/*/clusters/*/streams/*}\xda\x41\x04name\x12\xdc\x01\n\x0c\x43reateStream\x12-.google.cloud.visionai.v1.CreateStreamRequest\x1a\x1d.google.longrunning.Operation\"~\x82\xd3\xe4\x93\x02@\"6/v1/{parent=projects/*/locations/*/clusters/*}/streams:\x06stream\xda\x41\x17parent,stream,stream_id\xca\x41\x1b\n\x06Stream\x12\x11OperationMetadata\x12\xdf\x01\n\x0cUpdateStream\x12-.google.cloud.visionai.v1.UpdateStreamRequest\x1a\x1d.google.longrunning.Operation\"\x80\x01\x82\xd3\xe4\x93\x02G2=/v1/{stream.name=projects/*/locations/*/clusters/*/streams/*}:\x06stream\xda\x41\x12stream,update_mask\xca\x41\x1b\n\x06Stream\x12\x11OperationMetadata\x12\xd0\x01\n\x0c\x44\x65leteStream\x12-.google.cloud.visionai.v1.DeleteStreamRequest\x1a\x1d.google.longrunning.Operation\"r\x82\xd3\xe4\x93\x02\x38*6/v1/{name=projects/*/locations/*/clusters/*/streams/*}\xda\x41\x04name\xca\x41*\n\x15google.protobuf.Empty\x12\x11OperationMetadata\x12\x86\x02\n\x12GetStreamThumbnail\x12\x33.google.cloud.visionai.v1.GetStreamThumbnailRequest\x1a\x1d.google.longrunning.Operation\"\x9b\x01\x82\xd3\xe4\x93\x02J\"E/v1/{stream=projects/*/locations/*/clusters/*/streams/*}:getThumbnail:\x01*\xda\x41\x16stream,gcs_object_name\xca\x41/\n\x1aGetStreamThumbnailResponse\x12\x11OperationMetadata\x12\xf0\x01\n\x16GenerateStreamHlsToken\x12\x37.google.cloud.visionai.v1.GenerateStreamHlsTokenRequest\x1a\x38.google.cloud.visionai.v1.GenerateStreamHlsTokenResponse\"c\x82\xd3\xe4\x93\x02T\"O/v1/{stream=projects/*/locations/*/clusters/*/streams/*}:generateStreamHlsToken:\x01*\xda\x41\x06stream\x12\xaf\x01\n\nListEvents\x12+.google.cloud.visionai.v1.ListEventsRequest\x1a,.google.cloud.visionai.v1.ListEventsResponse\"F\x82\xd3\xe4\x93\x02\x37\x12\x35/v1/{parent=projects/*/locations/*/clusters/*}/events\xda\x41\x06parent\x12\x9c\x01\n\x08GetEvent\x12).google.cloud.visionai.v1.GetEventRequest\x1a\x1f.google.cloud.visionai.v1.Event\"D\x82\xd3\xe4\x93\x02\x37\x12\x35/v1/{name=projects/*/locations/*/clusters/*/events/*}\xda\x41\x04name\x12\xd5\x01\n\x0b\x43reateEvent\x12,.google.cloud.visionai.v1.CreateEventRequest\x1a\x1d.google.longrunning.Operation\"y\x82\xd3\xe4\x93\x02>\"5/v1/{parent=projects/*/locations/*/clusters/*}/events:\x05\x65vent\xda\x41\x15parent,event,event_id\xca\x41\x1a\n\x05\x45vent\x12\x11OperationMetadata\x12\xd7\x01\n\x0bUpdateEvent\x12,.google.cloud.visionai.v1.UpdateEventRequest\x1a\x1d.google.longrunning.Operation\"{\x82\xd3\xe4\x93\x02\x44\x32;/v1/{event.name=projects/*/locations/*/clusters/*/events/*}:\x05\x65vent\xda\x41\x11\x65vent,update_mask\xca\x41\x1a\n\x05\x45vent\x12\x11OperationMetadata\x12\xcd\x01\n\x0b\x44\x65leteEvent\x12,.google.cloud.visionai.v1.DeleteEventRequest\x1a\x1d.google.longrunning.Operation\"q\x82\xd3\xe4\x93\x02\x37*5/v1/{name=projects/*/locations/*/clusters/*/events/*}\xda\x41\x04name\xca\x41*\n\x15google.protobuf.Empty\x12\x11OperationMetadata\x12\xaf\x01\n\nListSeries\x12+.google.cloud.visionai.v1.ListSeriesRequest\x1a,.google.cloud.visionai.v1.ListSeriesResponse\"F\x82\xd3\xe4\x93\x02\x37\x12\x35/v1/{parent=projects/*/locations/*/clusters/*}/series\xda\x41\x06parent\x12\x9f\x01\n\tGetSeries\x12*.google.cloud.visionai.v1.GetSeriesRequest\x1a .google.cloud.visionai.v1.Series\"D\x82\xd3\xe4\x93\x02\x37\x12\x35/v1/{name=projects/*/locations/*/clusters/*/series/*}\xda\x41\x04name\x12\xdb\x01\n\x0c\x43reateSeries\x12-.google.cloud.visionai.v1.CreateSeriesRequest\x1a\x1d.google.longrunning.Operation\"}\x82\xd3\xe4\x93\x02?\"5/v1/{parent=projects/*/locations/*/clusters/*}/series:\x06series\xda\x41\x17parent,series,series_id\xca\x41\x1b\n\x06Series\x12\x11OperationMetadata\x12\xdd\x01\n\x0cUpdateSeries\x12-.google.cloud.visionai.v1.UpdateSeriesRequest\x1a\x1d.google.longrunning.Operation\"\x7f\x82\xd3\xe4\x93\x02\x46\x32</v1/{series.name=projects/*/locations/*/clusters/*/series/*}:\x06series\xda\x41\x12series,update_mask\xca\x41\x1b\n\x06Series\x12\x11OperationMetadata\x12\xcf\x01\n\x0c\x44\x65leteSeries\x12-.google.cloud.visionai.v1.DeleteSeriesRequest\x1a\x1d.google.longrunning.Operation\"q\x82\xd3\xe4\x93\x02\x37*5/v1/{name=projects/*/locations/*/clusters/*/series/*}\xda\x41\x04name\xca\x41*\n\x15google.protobuf.Empty\x12\x11OperationMetadata\x12\xee\x01\n\x12MaterializeChannel\x12\x33.google.cloud.visionai.v1.MaterializeChannelRequest\x1a\x1d.google.longrunning.Operation\"\x83\x01\x82\xd3\xe4\x93\x02\x42\"7/v1/{parent=projects/*/locations/*/clusters/*}/channels:\x07\x63hannel\xda\x41\x19parent,channel,channel_id\xca\x41\x1c\n\x07\x43hannel\x12\x11OperationMetadata\x1aK\xca\x41\x17visionai.googleapis.com\xd2\x41.https://www.googleapis.com/auth/cloud-platformB\xc3\x01\n\x1c\x63om.google.cloud.visionai.v1B\x13StreamsServiceProtoP\x01Z8cloud.google.com/go/visionai/apiv1/visionaipb;visionaipb\xaa\x02\x18Google.Cloud.VisionAI.V1\xca\x02\x18Google\\Cloud\\VisionAI\\V1\xea\x02\x1bGoogle::Cloud::VisionAI::V1b\x06proto3')

_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, globals())
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'visionai.python.protos.googleapis.v1.streams_service_pb2', globals())
if _descriptor._USE_C_DESCRIPTORS == False:

  DESCRIPTOR._options = None
  DESCRIPTOR._serialized_options = b'\n\034com.google.cloud.visionai.v1B\023StreamsServiceProtoP\001Z8cloud.google.com/go/visionai/apiv1/visionaipb;visionaipb\252\002\030Google.Cloud.VisionAI.V1\312\002\030Google\\Cloud\\VisionAI\\V1\352\002\033Google::Cloud::VisionAI::V1'
  _LISTCLUSTERSREQUEST.fields_by_name['parent']._options = None
  _LISTCLUSTERSREQUEST.fields_by_name['parent']._serialized_options = b'\340A\002\372A#\n!locations.googleapis.com/Location'
  _GETCLUSTERREQUEST.fields_by_name['name']._options = None
  _GETCLUSTERREQUEST.fields_by_name['name']._serialized_options = b'\340A\002\372A!\n\037visionai.googleapis.com/Cluster'
  _CREATECLUSTERREQUEST.fields_by_name['parent']._options = None
  _CREATECLUSTERREQUEST.fields_by_name['parent']._serialized_options = b'\340A\002\372A!\022\037visionai.googleapis.com/Cluster'
  _CREATECLUSTERREQUEST.fields_by_name['cluster_id']._options = None
  _CREATECLUSTERREQUEST.fields_by_name['cluster_id']._serialized_options = b'\340A\002'
  _CREATECLUSTERREQUEST.fields_by_name['cluster']._options = None
  _CREATECLUSTERREQUEST.fields_by_name['cluster']._serialized_options = b'\340A\002'
  _CREATECLUSTERREQUEST.fields_by_name['request_id']._options = None
  _CREATECLUSTERREQUEST.fields_by_name['request_id']._serialized_options = b'\340A\001'
  _UPDATECLUSTERREQUEST.fields_by_name['update_mask']._options = None
  _UPDATECLUSTERREQUEST.fields_by_name['update_mask']._serialized_options = b'\340A\002'
  _UPDATECLUSTERREQUEST.fields_by_name['cluster']._options = None
  _UPDATECLUSTERREQUEST.fields_by_name['cluster']._serialized_options = b'\340A\002'
  _UPDATECLUSTERREQUEST.fields_by_name['request_id']._options = None
  _UPDATECLUSTERREQUEST.fields_by_name['request_id']._serialized_options = b'\340A\001'
  _DELETECLUSTERREQUEST.fields_by_name['name']._options = None
  _DELETECLUSTERREQUEST.fields_by_name['name']._serialized_options = b'\340A\002\372A!\n\037visionai.googleapis.com/Cluster'
  _DELETECLUSTERREQUEST.fields_by_name['request_id']._options = None
  _DELETECLUSTERREQUEST.fields_by_name['request_id']._serialized_options = b'\340A\001'
  _LISTSTREAMSREQUEST.fields_by_name['parent']._options = None
  _LISTSTREAMSREQUEST.fields_by_name['parent']._serialized_options = b'\340A\002\372A!\n\037visionai.googleapis.com/Cluster'
  _GETSTREAMREQUEST.fields_by_name['name']._options = None
  _GETSTREAMREQUEST.fields_by_name['name']._serialized_options = b'\340A\002\372A \n\036visionai.googleapis.com/Stream'
  _CREATESTREAMREQUEST.fields_by_name['parent']._options = None
  _CREATESTREAMREQUEST.fields_by_name['parent']._serialized_options = b'\340A\002\372A!\n\037visionai.googleapis.com/Cluster'
  _CREATESTREAMREQUEST.fields_by_name['stream_id']._options = None
  _CREATESTREAMREQUEST.fields_by_name['stream_id']._serialized_options = b'\340A\002'
  _CREATESTREAMREQUEST.fields_by_name['stream']._options = None
  _CREATESTREAMREQUEST.fields_by_name['stream']._serialized_options = b'\340A\002'
  _CREATESTREAMREQUEST.fields_by_name['request_id']._options = None
  _CREATESTREAMREQUEST.fields_by_name['request_id']._serialized_options = b'\340A\001'
  _UPDATESTREAMREQUEST.fields_by_name['update_mask']._options = None
  _UPDATESTREAMREQUEST.fields_by_name['update_mask']._serialized_options = b'\340A\002'
  _UPDATESTREAMREQUEST.fields_by_name['stream']._options = None
  _UPDATESTREAMREQUEST.fields_by_name['stream']._serialized_options = b'\340A\002'
  _UPDATESTREAMREQUEST.fields_by_name['request_id']._options = None
  _UPDATESTREAMREQUEST.fields_by_name['request_id']._serialized_options = b'\340A\001'
  _DELETESTREAMREQUEST.fields_by_name['name']._options = None
  _DELETESTREAMREQUEST.fields_by_name['name']._serialized_options = b'\340A\002\372A \n\036visionai.googleapis.com/Stream'
  _DELETESTREAMREQUEST.fields_by_name['request_id']._options = None
  _DELETESTREAMREQUEST.fields_by_name['request_id']._serialized_options = b'\340A\001'
  _GETSTREAMTHUMBNAILREQUEST.fields_by_name['stream']._options = None
  _GETSTREAMTHUMBNAILREQUEST.fields_by_name['stream']._serialized_options = b'\340A\002'
  _GETSTREAMTHUMBNAILREQUEST.fields_by_name['gcs_object_name']._options = None
  _GETSTREAMTHUMBNAILREQUEST.fields_by_name['gcs_object_name']._serialized_options = b'\340A\002'
  _GETSTREAMTHUMBNAILREQUEST.fields_by_name['event']._options = None
  _GETSTREAMTHUMBNAILREQUEST.fields_by_name['event']._serialized_options = b'\340A\001'
  _GETSTREAMTHUMBNAILREQUEST.fields_by_name['request_id']._options = None
  _GETSTREAMTHUMBNAILREQUEST.fields_by_name['request_id']._serialized_options = b'\340A\001'
  _GENERATESTREAMHLSTOKENREQUEST.fields_by_name['stream']._options = None
  _GENERATESTREAMHLSTOKENREQUEST.fields_by_name['stream']._serialized_options = b'\340A\002'
  _LISTEVENTSREQUEST.fields_by_name['parent']._options = None
  _LISTEVENTSREQUEST.fields_by_name['parent']._serialized_options = b'\340A\002\372A!\n\037visionai.googleapis.com/Cluster'
  _GETEVENTREQUEST.fields_by_name['name']._options = None
  _GETEVENTREQUEST.fields_by_name['name']._serialized_options = b'\340A\002\372A\037\n\035visionai.googleapis.com/Event'
  _CREATEEVENTREQUEST.fields_by_name['parent']._options = None
  _CREATEEVENTREQUEST.fields_by_name['parent']._serialized_options = b'\340A\002\372A!\n\037visionai.googleapis.com/Cluster'
  _CREATEEVENTREQUEST.fields_by_name['event_id']._options = None
  _CREATEEVENTREQUEST.fields_by_name['event_id']._serialized_options = b'\340A\002'
  _CREATEEVENTREQUEST.fields_by_name['event']._options = None
  _CREATEEVENTREQUEST.fields_by_name['event']._serialized_options = b'\340A\002'
  _CREATEEVENTREQUEST.fields_by_name['request_id']._options = None
  _CREATEEVENTREQUEST.fields_by_name['request_id']._serialized_options = b'\340A\001'
  _UPDATEEVENTREQUEST.fields_by_name['update_mask']._options = None
  _UPDATEEVENTREQUEST.fields_by_name['update_mask']._serialized_options = b'\340A\002'
  _UPDATEEVENTREQUEST.fields_by_name['event']._options = None
  _UPDATEEVENTREQUEST.fields_by_name['event']._serialized_options = b'\340A\002'
  _UPDATEEVENTREQUEST.fields_by_name['request_id']._options = None
  _UPDATEEVENTREQUEST.fields_by_name['request_id']._serialized_options = b'\340A\001'
  _DELETEEVENTREQUEST.fields_by_name['name']._options = None
  _DELETEEVENTREQUEST.fields_by_name['name']._serialized_options = b'\340A\002\372A\037\n\035visionai.googleapis.com/Event'
  _DELETEEVENTREQUEST.fields_by_name['request_id']._options = None
  _DELETEEVENTREQUEST.fields_by_name['request_id']._serialized_options = b'\340A\001'
  _LISTSERIESREQUEST.fields_by_name['parent']._options = None
  _LISTSERIESREQUEST.fields_by_name['parent']._serialized_options = b'\340A\002\372A!\n\037visionai.googleapis.com/Cluster'
  _GETSERIESREQUEST.fields_by_name['name']._options = None
  _GETSERIESREQUEST.fields_by_name['name']._serialized_options = b'\340A\002\372A \n\036visionai.googleapis.com/Series'
  _CREATESERIESREQUEST.fields_by_name['parent']._options = None
  _CREATESERIESREQUEST.fields_by_name['parent']._serialized_options = b'\340A\002\372A!\n\037visionai.googleapis.com/Cluster'
  _CREATESERIESREQUEST.fields_by_name['series_id']._options = None
  _CREATESERIESREQUEST.fields_by_name['series_id']._serialized_options = b'\340A\002'
  _CREATESERIESREQUEST.fields_by_name['series']._options = None
  _CREATESERIESREQUEST.fields_by_name['series']._serialized_options = b'\340A\002'
  _CREATESERIESREQUEST.fields_by_name['request_id']._options = None
  _CREATESERIESREQUEST.fields_by_name['request_id']._serialized_options = b'\340A\001'
  _UPDATESERIESREQUEST.fields_by_name['update_mask']._options = None
  _UPDATESERIESREQUEST.fields_by_name['update_mask']._serialized_options = b'\340A\002'
  _UPDATESERIESREQUEST.fields_by_name['series']._options = None
  _UPDATESERIESREQUEST.fields_by_name['series']._serialized_options = b'\340A\002'
  _UPDATESERIESREQUEST.fields_by_name['request_id']._options = None
  _UPDATESERIESREQUEST.fields_by_name['request_id']._serialized_options = b'\340A\001'
  _DELETESERIESREQUEST.fields_by_name['name']._options = None
  _DELETESERIESREQUEST.fields_by_name['name']._serialized_options = b'\340A\002\372A \n\036visionai.googleapis.com/Series'
  _DELETESERIESREQUEST.fields_by_name['request_id']._options = None
  _DELETESERIESREQUEST.fields_by_name['request_id']._serialized_options = b'\340A\001'
  _MATERIALIZECHANNELREQUEST.fields_by_name['parent']._options = None
  _MATERIALIZECHANNELREQUEST.fields_by_name['parent']._serialized_options = b'\340A\002\372A!\n\037visionai.googleapis.com/Cluster'
  _MATERIALIZECHANNELREQUEST.fields_by_name['channel_id']._options = None
  _MATERIALIZECHANNELREQUEST.fields_by_name['channel_id']._serialized_options = b'\340A\002'
  _MATERIALIZECHANNELREQUEST.fields_by_name['channel']._options = None
  _MATERIALIZECHANNELREQUEST.fields_by_name['channel']._serialized_options = b'\340A\002'
  _MATERIALIZECHANNELREQUEST.fields_by_name['request_id']._options = None
  _MATERIALIZECHANNELREQUEST.fields_by_name['request_id']._serialized_options = b'\340A\001'
  _STREAMSSERVICE._options = None
  _STREAMSSERVICE._serialized_options = b'\312A\027visionai.googleapis.com\322A.https://www.googleapis.com/auth/cloud-platform'
  _STREAMSSERVICE.methods_by_name['ListClusters']._options = None
  _STREAMSSERVICE.methods_by_name['ListClusters']._serialized_options = b'\202\323\344\223\002.\022,/v1/{parent=projects/*/locations/*}/clusters\332A\006parent'
  _STREAMSSERVICE.methods_by_name['GetCluster']._options = None
  _STREAMSSERVICE.methods_by_name['GetCluster']._serialized_options = b'\202\323\344\223\002.\022,/v1/{name=projects/*/locations/*/clusters/*}\332A\004name'
  _STREAMSSERVICE.methods_by_name['CreateCluster']._options = None
  _STREAMSSERVICE.methods_by_name['CreateCluster']._serialized_options = b'\202\323\344\223\0027\",/v1/{parent=projects/*/locations/*}/clusters:\007cluster\332A\031parent,cluster,cluster_id\312A\034\n\007Cluster\022\021OperationMetadata'
  _STREAMSSERVICE.methods_by_name['UpdateCluster']._options = None
  _STREAMSSERVICE.methods_by_name['UpdateCluster']._serialized_options = b'\202\323\344\223\002?24/v1/{cluster.name=projects/*/locations/*/clusters/*}:\007cluster\332A\023cluster,update_mask\312A\034\n\007Cluster\022\021OperationMetadata'
  _STREAMSSERVICE.methods_by_name['DeleteCluster']._options = None
  _STREAMSSERVICE.methods_by_name['DeleteCluster']._serialized_options = b'\202\323\344\223\002.*,/v1/{name=projects/*/locations/*/clusters/*}\332A\004name\312A*\n\025google.protobuf.Empty\022\021OperationMetadata'
  _STREAMSSERVICE.methods_by_name['ListStreams']._options = None
  _STREAMSSERVICE.methods_by_name['ListStreams']._serialized_options = b'\202\323\344\223\0028\0226/v1/{parent=projects/*/locations/*/clusters/*}/streams\332A\006parent'
  _STREAMSSERVICE.methods_by_name['GetStream']._options = None
  _STREAMSSERVICE.methods_by_name['GetStream']._serialized_options = b'\202\323\344\223\0028\0226/v1/{name=projects/*/locations/*/clusters/*/streams/*}\332A\004name'
  _STREAMSSERVICE.methods_by_name['CreateStream']._options = None
  _STREAMSSERVICE.methods_by_name['CreateStream']._serialized_options = b'\202\323\344\223\002@\"6/v1/{parent=projects/*/locations/*/clusters/*}/streams:\006stream\332A\027parent,stream,stream_id\312A\033\n\006Stream\022\021OperationMetadata'
  _STREAMSSERVICE.methods_by_name['UpdateStream']._options = None
  _STREAMSSERVICE.methods_by_name['UpdateStream']._serialized_options = b'\202\323\344\223\002G2=/v1/{stream.name=projects/*/locations/*/clusters/*/streams/*}:\006stream\332A\022stream,update_mask\312A\033\n\006Stream\022\021OperationMetadata'
  _STREAMSSERVICE.methods_by_name['DeleteStream']._options = None
  _STREAMSSERVICE.methods_by_name['DeleteStream']._serialized_options = b'\202\323\344\223\0028*6/v1/{name=projects/*/locations/*/clusters/*/streams/*}\332A\004name\312A*\n\025google.protobuf.Empty\022\021OperationMetadata'
  _STREAMSSERVICE.methods_by_name['GetStreamThumbnail']._options = None
  _STREAMSSERVICE.methods_by_name['GetStreamThumbnail']._serialized_options = b'\202\323\344\223\002J\"E/v1/{stream=projects/*/locations/*/clusters/*/streams/*}:getThumbnail:\001*\332A\026stream,gcs_object_name\312A/\n\032GetStreamThumbnailResponse\022\021OperationMetadata'
  _STREAMSSERVICE.methods_by_name['GenerateStreamHlsToken']._options = None
  _STREAMSSERVICE.methods_by_name['GenerateStreamHlsToken']._serialized_options = b'\202\323\344\223\002T\"O/v1/{stream=projects/*/locations/*/clusters/*/streams/*}:generateStreamHlsToken:\001*\332A\006stream'
  _STREAMSSERVICE.methods_by_name['ListEvents']._options = None
  _STREAMSSERVICE.methods_by_name['ListEvents']._serialized_options = b'\202\323\344\223\0027\0225/v1/{parent=projects/*/locations/*/clusters/*}/events\332A\006parent'
  _STREAMSSERVICE.methods_by_name['GetEvent']._options = None
  _STREAMSSERVICE.methods_by_name['GetEvent']._serialized_options = b'\202\323\344\223\0027\0225/v1/{name=projects/*/locations/*/clusters/*/events/*}\332A\004name'
  _STREAMSSERVICE.methods_by_name['CreateEvent']._options = None
  _STREAMSSERVICE.methods_by_name['CreateEvent']._serialized_options = b'\202\323\344\223\002>\"5/v1/{parent=projects/*/locations/*/clusters/*}/events:\005event\332A\025parent,event,event_id\312A\032\n\005Event\022\021OperationMetadata'
  _STREAMSSERVICE.methods_by_name['UpdateEvent']._options = None
  _STREAMSSERVICE.methods_by_name['UpdateEvent']._serialized_options = b'\202\323\344\223\002D2;/v1/{event.name=projects/*/locations/*/clusters/*/events/*}:\005event\332A\021event,update_mask\312A\032\n\005Event\022\021OperationMetadata'
  _STREAMSSERVICE.methods_by_name['DeleteEvent']._options = None
  _STREAMSSERVICE.methods_by_name['DeleteEvent']._serialized_options = b'\202\323\344\223\0027*5/v1/{name=projects/*/locations/*/clusters/*/events/*}\332A\004name\312A*\n\025google.protobuf.Empty\022\021OperationMetadata'
  _STREAMSSERVICE.methods_by_name['ListSeries']._options = None
  _STREAMSSERVICE.methods_by_name['ListSeries']._serialized_options = b'\202\323\344\223\0027\0225/v1/{parent=projects/*/locations/*/clusters/*}/series\332A\006parent'
  _STREAMSSERVICE.methods_by_name['GetSeries']._options = None
  _STREAMSSERVICE.methods_by_name['GetSeries']._serialized_options = b'\202\323\344\223\0027\0225/v1/{name=projects/*/locations/*/clusters/*/series/*}\332A\004name'
  _STREAMSSERVICE.methods_by_name['CreateSeries']._options = None
  _STREAMSSERVICE.methods_by_name['CreateSeries']._serialized_options = b'\202\323\344\223\002?\"5/v1/{parent=projects/*/locations/*/clusters/*}/series:\006series\332A\027parent,series,series_id\312A\033\n\006Series\022\021OperationMetadata'
  _STREAMSSERVICE.methods_by_name['UpdateSeries']._options = None
  _STREAMSSERVICE.methods_by_name['UpdateSeries']._serialized_options = b'\202\323\344\223\002F2</v1/{series.name=projects/*/locations/*/clusters/*/series/*}:\006series\332A\022series,update_mask\312A\033\n\006Series\022\021OperationMetadata'
  _STREAMSSERVICE.methods_by_name['DeleteSeries']._options = None
  _STREAMSSERVICE.methods_by_name['DeleteSeries']._serialized_options = b'\202\323\344\223\0027*5/v1/{name=projects/*/locations/*/clusters/*/series/*}\332A\004name\312A*\n\025google.protobuf.Empty\022\021OperationMetadata'
  _STREAMSSERVICE.methods_by_name['MaterializeChannel']._options = None
  _STREAMSSERVICE.methods_by_name['MaterializeChannel']._serialized_options = b'\202\323\344\223\002B\"7/v1/{parent=projects/*/locations/*/clusters/*}/channels:\007channel\332A\031parent,channel,channel_id\312A\034\n\007Channel\022\021OperationMetadata'
  _LISTCLUSTERSREQUEST._serialized_start=450
  _LISTCLUSTERSREQUEST._serialized_end=603
  _LISTCLUSTERSRESPONSE._serialized_start=605
  _LISTCLUSTERSRESPONSE._serialized_end=726
  _GETCLUSTERREQUEST._serialized_start=728
  _GETCLUSTERREQUEST._serialized_end=802
  _CREATECLUSTERREQUEST._serialized_start=805
  _CREATECLUSTERREQUEST._serialized_end=991
  _UPDATECLUSTERREQUEST._serialized_start=994
  _UPDATECLUSTERREQUEST._serialized_end=1152
  _DELETECLUSTERREQUEST._serialized_start=1154
  _DELETECLUSTERREQUEST._serialized_end=1256
  _LISTSTREAMSREQUEST._serialized_start=1259
  _LISTSTREAMSREQUEST._serialized_end=1409
  _LISTSTREAMSRESPONSE._serialized_start=1411
  _LISTSTREAMSRESPONSE._serialized_end=1529
  _GETSTREAMREQUEST._serialized_start=1531
  _GETSTREAMREQUEST._serialized_end=1603
  _CREATESTREAMREQUEST._serialized_start=1606
  _CREATESTREAMREQUEST._serialized_end=1788
  _UPDATESTREAMREQUEST._serialized_start=1791
  _UPDATESTREAMREQUEST._serialized_end=1946
  _DELETESTREAMREQUEST._serialized_start=1948
  _DELETESTREAMREQUEST._serialized_end=2048
  _GETSTREAMTHUMBNAILREQUEST._serialized_start=2050
  _GETSTREAMTHUMBNAILREQUEST._serialized_end=2173
  _GETSTREAMTHUMBNAILRESPONSE._serialized_start=2175
  _GETSTREAMTHUMBNAILRESPONSE._serialized_end=2203
  _GENERATESTREAMHLSTOKENREQUEST._serialized_start=2205
  _GENERATESTREAMHLSTOKENREQUEST._serialized_end=2257
  _GENERATESTREAMHLSTOKENRESPONSE._serialized_start=2259
  _GENERATESTREAMHLSTOKENRESPONSE._serialized_end=2359
  _LISTEVENTSREQUEST._serialized_start=2362
  _LISTEVENTSREQUEST._serialized_end=2511
  _LISTEVENTSRESPONSE._serialized_start=2513
  _LISTEVENTSRESPONSE._serialized_end=2628
  _GETEVENTREQUEST._serialized_start=2630
  _GETEVENTREQUEST._serialized_end=2700
  _CREATEEVENTREQUEST._serialized_start=2703
  _CREATEEVENTREQUEST._serialized_end=2881
  _UPDATEEVENTREQUEST._serialized_start=2884
  _UPDATEEVENTREQUEST._serialized_end=3036
  _DELETEEVENTREQUEST._serialized_start=3038
  _DELETEEVENTREQUEST._serialized_end=3136
  _LISTSERIESREQUEST._serialized_start=3139
  _LISTSERIESREQUEST._serialized_end=3288
  _LISTSERIESRESPONSE._serialized_start=3290
  _LISTSERIESRESPONSE._serialized_end=3406
  _GETSERIESREQUEST._serialized_start=3408
  _GETSERIESREQUEST._serialized_end=3480
  _CREATESERIESREQUEST._serialized_start=3483
  _CREATESERIESREQUEST._serialized_end=3665
  _UPDATESERIESREQUEST._serialized_start=3668
  _UPDATESERIESREQUEST._serialized_end=3823
  _DELETESERIESREQUEST._serialized_start=3825
  _DELETESERIESREQUEST._serialized_end=3925
  _MATERIALIZECHANNELREQUEST._serialized_start=3928
  _MATERIALIZECHANNELREQUEST._serialized_end=4119
  _STREAMSSERVICE._serialized_start=4122
  _STREAMSSERVICE._serialized_end=8918
# @@protoc_insertion_point(module_scope)
