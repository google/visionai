from google.api import field_behavior_pb2 as _field_behavior_pb2
from google.api import resource_pb2 as _resource_pb2
from google.protobuf import duration_pb2 as _duration_pb2
from google.protobuf import struct_pb2 as _struct_pb2
from google.protobuf import timestamp_pb2 as _timestamp_pb2
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class GstreamerBufferDescriptor(_message.Message):
    __slots__ = ["caps_string", "dts_time", "duration", "is_key_frame", "pts_time"]
    CAPS_STRING_FIELD_NUMBER: _ClassVar[int]
    DTS_TIME_FIELD_NUMBER: _ClassVar[int]
    DURATION_FIELD_NUMBER: _ClassVar[int]
    IS_KEY_FRAME_FIELD_NUMBER: _ClassVar[int]
    PTS_TIME_FIELD_NUMBER: _ClassVar[int]
    caps_string: str
    dts_time: _timestamp_pb2.Timestamp
    duration: _duration_pb2.Duration
    is_key_frame: bool
    pts_time: _timestamp_pb2.Timestamp
    def __init__(self, caps_string: _Optional[str] = ..., is_key_frame: bool = ..., pts_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., dts_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., duration: _Optional[_Union[_duration_pb2.Duration, _Mapping]] = ...) -> None: ...

class Packet(_message.Message):
    __slots__ = ["header", "payload"]
    HEADER_FIELD_NUMBER: _ClassVar[int]
    PAYLOAD_FIELD_NUMBER: _ClassVar[int]
    header: PacketHeader
    payload: bytes
    def __init__(self, header: _Optional[_Union[PacketHeader, _Mapping]] = ..., payload: _Optional[bytes] = ...) -> None: ...

class PacketHeader(_message.Message):
    __slots__ = ["capture_time", "flags", "metadata", "series_metadata", "server_metadata", "trace_context", "type"]
    CAPTURE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    METADATA_FIELD_NUMBER: _ClassVar[int]
    SERIES_METADATA_FIELD_NUMBER: _ClassVar[int]
    SERVER_METADATA_FIELD_NUMBER: _ClassVar[int]
    TRACE_CONTEXT_FIELD_NUMBER: _ClassVar[int]
    TYPE_FIELD_NUMBER: _ClassVar[int]
    capture_time: _timestamp_pb2.Timestamp
    flags: int
    metadata: _struct_pb2.Struct
    series_metadata: SeriesMetadata
    server_metadata: ServerMetadata
    trace_context: str
    type: PacketType
    def __init__(self, capture_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., type: _Optional[_Union[PacketType, _Mapping]] = ..., metadata: _Optional[_Union[_struct_pb2.Struct, _Mapping]] = ..., server_metadata: _Optional[_Union[ServerMetadata, _Mapping]] = ..., series_metadata: _Optional[_Union[SeriesMetadata, _Mapping]] = ..., flags: _Optional[int] = ..., trace_context: _Optional[str] = ...) -> None: ...

class PacketType(_message.Message):
    __slots__ = ["type_class", "type_descriptor"]
    class TypeDescriptor(_message.Message):
        __slots__ = ["gstreamer_buffer_descriptor", "raw_image_descriptor", "type"]
        GSTREAMER_BUFFER_DESCRIPTOR_FIELD_NUMBER: _ClassVar[int]
        RAW_IMAGE_DESCRIPTOR_FIELD_NUMBER: _ClassVar[int]
        TYPE_FIELD_NUMBER: _ClassVar[int]
        gstreamer_buffer_descriptor: GstreamerBufferDescriptor
        raw_image_descriptor: RawImageDescriptor
        type: str
        def __init__(self, gstreamer_buffer_descriptor: _Optional[_Union[GstreamerBufferDescriptor, _Mapping]] = ..., raw_image_descriptor: _Optional[_Union[RawImageDescriptor, _Mapping]] = ..., type: _Optional[str] = ...) -> None: ...
    TYPE_CLASS_FIELD_NUMBER: _ClassVar[int]
    TYPE_DESCRIPTOR_FIELD_NUMBER: _ClassVar[int]
    type_class: str
    type_descriptor: PacketType.TypeDescriptor
    def __init__(self, type_class: _Optional[str] = ..., type_descriptor: _Optional[_Union[PacketType.TypeDescriptor, _Mapping]] = ...) -> None: ...

class RawImageDescriptor(_message.Message):
    __slots__ = ["format", "height", "width"]
    FORMAT_FIELD_NUMBER: _ClassVar[int]
    HEIGHT_FIELD_NUMBER: _ClassVar[int]
    WIDTH_FIELD_NUMBER: _ClassVar[int]
    format: str
    height: int
    width: int
    def __init__(self, format: _Optional[str] = ..., height: _Optional[int] = ..., width: _Optional[int] = ...) -> None: ...

class SeriesMetadata(_message.Message):
    __slots__ = ["series"]
    SERIES_FIELD_NUMBER: _ClassVar[int]
    series: str
    def __init__(self, series: _Optional[str] = ...) -> None: ...

class ServerMetadata(_message.Message):
    __slots__ = ["ingest_time", "offset"]
    INGEST_TIME_FIELD_NUMBER: _ClassVar[int]
    OFFSET_FIELD_NUMBER: _ClassVar[int]
    ingest_time: _timestamp_pb2.Timestamp
    offset: int
    def __init__(self, offset: _Optional[int] = ..., ingest_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...
