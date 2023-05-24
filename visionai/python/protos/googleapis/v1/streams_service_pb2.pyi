from google.api import annotations_pb2 as _annotations_pb2
from google.api import client_pb2 as _client_pb2
from google.api import field_behavior_pb2 as _field_behavior_pb2
from google.api import resource_pb2 as _resource_pb2
from visionai.python.protos.googleapis.v1 import common_pb2 as _common_pb2
from visionai.python.protos.googleapis.v1 import streams_resources_pb2 as _streams_resources_pb2
from google.longrunning import operations_pb2 as _operations_pb2
from google.protobuf import empty_pb2 as _empty_pb2
from google.protobuf import field_mask_pb2 as _field_mask_pb2
from google.protobuf import timestamp_pb2 as _timestamp_pb2
from google.protobuf.internal import containers as _containers
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Iterable as _Iterable, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class CreateClusterRequest(_message.Message):
    __slots__ = ["cluster", "cluster_id", "parent", "request_id"]
    CLUSTER_FIELD_NUMBER: _ClassVar[int]
    CLUSTER_ID_FIELD_NUMBER: _ClassVar[int]
    PARENT_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    cluster: _common_pb2.Cluster
    cluster_id: str
    parent: str
    request_id: str
    def __init__(self, parent: _Optional[str] = ..., cluster_id: _Optional[str] = ..., cluster: _Optional[_Union[_common_pb2.Cluster, _Mapping]] = ..., request_id: _Optional[str] = ...) -> None: ...

class CreateEventRequest(_message.Message):
    __slots__ = ["event", "event_id", "parent", "request_id"]
    EVENT_FIELD_NUMBER: _ClassVar[int]
    EVENT_ID_FIELD_NUMBER: _ClassVar[int]
    PARENT_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    event: _streams_resources_pb2.Event
    event_id: str
    parent: str
    request_id: str
    def __init__(self, parent: _Optional[str] = ..., event_id: _Optional[str] = ..., event: _Optional[_Union[_streams_resources_pb2.Event, _Mapping]] = ..., request_id: _Optional[str] = ...) -> None: ...

class CreateSeriesRequest(_message.Message):
    __slots__ = ["parent", "request_id", "series", "series_id"]
    PARENT_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    SERIES_FIELD_NUMBER: _ClassVar[int]
    SERIES_ID_FIELD_NUMBER: _ClassVar[int]
    parent: str
    request_id: str
    series: _streams_resources_pb2.Series
    series_id: str
    def __init__(self, parent: _Optional[str] = ..., series_id: _Optional[str] = ..., series: _Optional[_Union[_streams_resources_pb2.Series, _Mapping]] = ..., request_id: _Optional[str] = ...) -> None: ...

class CreateStreamRequest(_message.Message):
    __slots__ = ["parent", "request_id", "stream", "stream_id"]
    PARENT_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    STREAM_FIELD_NUMBER: _ClassVar[int]
    STREAM_ID_FIELD_NUMBER: _ClassVar[int]
    parent: str
    request_id: str
    stream: _streams_resources_pb2.Stream
    stream_id: str
    def __init__(self, parent: _Optional[str] = ..., stream_id: _Optional[str] = ..., stream: _Optional[_Union[_streams_resources_pb2.Stream, _Mapping]] = ..., request_id: _Optional[str] = ...) -> None: ...

class DeleteClusterRequest(_message.Message):
    __slots__ = ["name", "request_id"]
    NAME_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    name: str
    request_id: str
    def __init__(self, name: _Optional[str] = ..., request_id: _Optional[str] = ...) -> None: ...

class DeleteEventRequest(_message.Message):
    __slots__ = ["name", "request_id"]
    NAME_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    name: str
    request_id: str
    def __init__(self, name: _Optional[str] = ..., request_id: _Optional[str] = ...) -> None: ...

class DeleteSeriesRequest(_message.Message):
    __slots__ = ["name", "request_id"]
    NAME_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    name: str
    request_id: str
    def __init__(self, name: _Optional[str] = ..., request_id: _Optional[str] = ...) -> None: ...

class DeleteStreamRequest(_message.Message):
    __slots__ = ["name", "request_id"]
    NAME_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    name: str
    request_id: str
    def __init__(self, name: _Optional[str] = ..., request_id: _Optional[str] = ...) -> None: ...

class GenerateStreamHlsTokenRequest(_message.Message):
    __slots__ = ["stream"]
    STREAM_FIELD_NUMBER: _ClassVar[int]
    stream: str
    def __init__(self, stream: _Optional[str] = ...) -> None: ...

class GenerateStreamHlsTokenResponse(_message.Message):
    __slots__ = ["expiration_time", "token"]
    EXPIRATION_TIME_FIELD_NUMBER: _ClassVar[int]
    TOKEN_FIELD_NUMBER: _ClassVar[int]
    expiration_time: _timestamp_pb2.Timestamp
    token: str
    def __init__(self, token: _Optional[str] = ..., expiration_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class GetClusterRequest(_message.Message):
    __slots__ = ["name"]
    NAME_FIELD_NUMBER: _ClassVar[int]
    name: str
    def __init__(self, name: _Optional[str] = ...) -> None: ...

class GetEventRequest(_message.Message):
    __slots__ = ["name"]
    NAME_FIELD_NUMBER: _ClassVar[int]
    name: str
    def __init__(self, name: _Optional[str] = ...) -> None: ...

class GetSeriesRequest(_message.Message):
    __slots__ = ["name"]
    NAME_FIELD_NUMBER: _ClassVar[int]
    name: str
    def __init__(self, name: _Optional[str] = ...) -> None: ...

class GetStreamRequest(_message.Message):
    __slots__ = ["name"]
    NAME_FIELD_NUMBER: _ClassVar[int]
    name: str
    def __init__(self, name: _Optional[str] = ...) -> None: ...

class GetStreamThumbnailRequest(_message.Message):
    __slots__ = ["event", "gcs_object_name", "request_id", "stream"]
    EVENT_FIELD_NUMBER: _ClassVar[int]
    GCS_OBJECT_NAME_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    STREAM_FIELD_NUMBER: _ClassVar[int]
    event: str
    gcs_object_name: str
    request_id: str
    stream: str
    def __init__(self, stream: _Optional[str] = ..., gcs_object_name: _Optional[str] = ..., event: _Optional[str] = ..., request_id: _Optional[str] = ...) -> None: ...

class GetStreamThumbnailResponse(_message.Message):
    __slots__ = []
    def __init__(self) -> None: ...

class ListClustersRequest(_message.Message):
    __slots__ = ["filter", "order_by", "page_size", "page_token", "parent"]
    FILTER_FIELD_NUMBER: _ClassVar[int]
    ORDER_BY_FIELD_NUMBER: _ClassVar[int]
    PAGE_SIZE_FIELD_NUMBER: _ClassVar[int]
    PAGE_TOKEN_FIELD_NUMBER: _ClassVar[int]
    PARENT_FIELD_NUMBER: _ClassVar[int]
    filter: str
    order_by: str
    page_size: int
    page_token: str
    parent: str
    def __init__(self, parent: _Optional[str] = ..., page_size: _Optional[int] = ..., page_token: _Optional[str] = ..., filter: _Optional[str] = ..., order_by: _Optional[str] = ...) -> None: ...

class ListClustersResponse(_message.Message):
    __slots__ = ["clusters", "next_page_token", "unreachable"]
    CLUSTERS_FIELD_NUMBER: _ClassVar[int]
    NEXT_PAGE_TOKEN_FIELD_NUMBER: _ClassVar[int]
    UNREACHABLE_FIELD_NUMBER: _ClassVar[int]
    clusters: _containers.RepeatedCompositeFieldContainer[_common_pb2.Cluster]
    next_page_token: str
    unreachable: _containers.RepeatedScalarFieldContainer[str]
    def __init__(self, clusters: _Optional[_Iterable[_Union[_common_pb2.Cluster, _Mapping]]] = ..., next_page_token: _Optional[str] = ..., unreachable: _Optional[_Iterable[str]] = ...) -> None: ...

class ListEventsRequest(_message.Message):
    __slots__ = ["filter", "order_by", "page_size", "page_token", "parent"]
    FILTER_FIELD_NUMBER: _ClassVar[int]
    ORDER_BY_FIELD_NUMBER: _ClassVar[int]
    PAGE_SIZE_FIELD_NUMBER: _ClassVar[int]
    PAGE_TOKEN_FIELD_NUMBER: _ClassVar[int]
    PARENT_FIELD_NUMBER: _ClassVar[int]
    filter: str
    order_by: str
    page_size: int
    page_token: str
    parent: str
    def __init__(self, parent: _Optional[str] = ..., page_size: _Optional[int] = ..., page_token: _Optional[str] = ..., filter: _Optional[str] = ..., order_by: _Optional[str] = ...) -> None: ...

class ListEventsResponse(_message.Message):
    __slots__ = ["events", "next_page_token", "unreachable"]
    EVENTS_FIELD_NUMBER: _ClassVar[int]
    NEXT_PAGE_TOKEN_FIELD_NUMBER: _ClassVar[int]
    UNREACHABLE_FIELD_NUMBER: _ClassVar[int]
    events: _containers.RepeatedCompositeFieldContainer[_streams_resources_pb2.Event]
    next_page_token: str
    unreachable: _containers.RepeatedScalarFieldContainer[str]
    def __init__(self, events: _Optional[_Iterable[_Union[_streams_resources_pb2.Event, _Mapping]]] = ..., next_page_token: _Optional[str] = ..., unreachable: _Optional[_Iterable[str]] = ...) -> None: ...

class ListSeriesRequest(_message.Message):
    __slots__ = ["filter", "order_by", "page_size", "page_token", "parent"]
    FILTER_FIELD_NUMBER: _ClassVar[int]
    ORDER_BY_FIELD_NUMBER: _ClassVar[int]
    PAGE_SIZE_FIELD_NUMBER: _ClassVar[int]
    PAGE_TOKEN_FIELD_NUMBER: _ClassVar[int]
    PARENT_FIELD_NUMBER: _ClassVar[int]
    filter: str
    order_by: str
    page_size: int
    page_token: str
    parent: str
    def __init__(self, parent: _Optional[str] = ..., page_size: _Optional[int] = ..., page_token: _Optional[str] = ..., filter: _Optional[str] = ..., order_by: _Optional[str] = ...) -> None: ...

class ListSeriesResponse(_message.Message):
    __slots__ = ["next_page_token", "series", "unreachable"]
    NEXT_PAGE_TOKEN_FIELD_NUMBER: _ClassVar[int]
    SERIES_FIELD_NUMBER: _ClassVar[int]
    UNREACHABLE_FIELD_NUMBER: _ClassVar[int]
    next_page_token: str
    series: _containers.RepeatedCompositeFieldContainer[_streams_resources_pb2.Series]
    unreachable: _containers.RepeatedScalarFieldContainer[str]
    def __init__(self, series: _Optional[_Iterable[_Union[_streams_resources_pb2.Series, _Mapping]]] = ..., next_page_token: _Optional[str] = ..., unreachable: _Optional[_Iterable[str]] = ...) -> None: ...

class ListStreamsRequest(_message.Message):
    __slots__ = ["filter", "order_by", "page_size", "page_token", "parent"]
    FILTER_FIELD_NUMBER: _ClassVar[int]
    ORDER_BY_FIELD_NUMBER: _ClassVar[int]
    PAGE_SIZE_FIELD_NUMBER: _ClassVar[int]
    PAGE_TOKEN_FIELD_NUMBER: _ClassVar[int]
    PARENT_FIELD_NUMBER: _ClassVar[int]
    filter: str
    order_by: str
    page_size: int
    page_token: str
    parent: str
    def __init__(self, parent: _Optional[str] = ..., page_size: _Optional[int] = ..., page_token: _Optional[str] = ..., filter: _Optional[str] = ..., order_by: _Optional[str] = ...) -> None: ...

class ListStreamsResponse(_message.Message):
    __slots__ = ["next_page_token", "streams", "unreachable"]
    NEXT_PAGE_TOKEN_FIELD_NUMBER: _ClassVar[int]
    STREAMS_FIELD_NUMBER: _ClassVar[int]
    UNREACHABLE_FIELD_NUMBER: _ClassVar[int]
    next_page_token: str
    streams: _containers.RepeatedCompositeFieldContainer[_streams_resources_pb2.Stream]
    unreachable: _containers.RepeatedScalarFieldContainer[str]
    def __init__(self, streams: _Optional[_Iterable[_Union[_streams_resources_pb2.Stream, _Mapping]]] = ..., next_page_token: _Optional[str] = ..., unreachable: _Optional[_Iterable[str]] = ...) -> None: ...

class MaterializeChannelRequest(_message.Message):
    __slots__ = ["channel", "channel_id", "parent", "request_id"]
    CHANNEL_FIELD_NUMBER: _ClassVar[int]
    CHANNEL_ID_FIELD_NUMBER: _ClassVar[int]
    PARENT_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    channel: _streams_resources_pb2.Channel
    channel_id: str
    parent: str
    request_id: str
    def __init__(self, parent: _Optional[str] = ..., channel_id: _Optional[str] = ..., channel: _Optional[_Union[_streams_resources_pb2.Channel, _Mapping]] = ..., request_id: _Optional[str] = ...) -> None: ...

class UpdateClusterRequest(_message.Message):
    __slots__ = ["cluster", "request_id", "update_mask"]
    CLUSTER_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    UPDATE_MASK_FIELD_NUMBER: _ClassVar[int]
    cluster: _common_pb2.Cluster
    request_id: str
    update_mask: _field_mask_pb2.FieldMask
    def __init__(self, update_mask: _Optional[_Union[_field_mask_pb2.FieldMask, _Mapping]] = ..., cluster: _Optional[_Union[_common_pb2.Cluster, _Mapping]] = ..., request_id: _Optional[str] = ...) -> None: ...

class UpdateEventRequest(_message.Message):
    __slots__ = ["event", "request_id", "update_mask"]
    EVENT_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    UPDATE_MASK_FIELD_NUMBER: _ClassVar[int]
    event: _streams_resources_pb2.Event
    request_id: str
    update_mask: _field_mask_pb2.FieldMask
    def __init__(self, update_mask: _Optional[_Union[_field_mask_pb2.FieldMask, _Mapping]] = ..., event: _Optional[_Union[_streams_resources_pb2.Event, _Mapping]] = ..., request_id: _Optional[str] = ...) -> None: ...

class UpdateSeriesRequest(_message.Message):
    __slots__ = ["request_id", "series", "update_mask"]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    SERIES_FIELD_NUMBER: _ClassVar[int]
    UPDATE_MASK_FIELD_NUMBER: _ClassVar[int]
    request_id: str
    series: _streams_resources_pb2.Series
    update_mask: _field_mask_pb2.FieldMask
    def __init__(self, update_mask: _Optional[_Union[_field_mask_pb2.FieldMask, _Mapping]] = ..., series: _Optional[_Union[_streams_resources_pb2.Series, _Mapping]] = ..., request_id: _Optional[str] = ...) -> None: ...

class UpdateStreamRequest(_message.Message):
    __slots__ = ["request_id", "stream", "update_mask"]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    STREAM_FIELD_NUMBER: _ClassVar[int]
    UPDATE_MASK_FIELD_NUMBER: _ClassVar[int]
    request_id: str
    stream: _streams_resources_pb2.Stream
    update_mask: _field_mask_pb2.FieldMask
    def __init__(self, update_mask: _Optional[_Union[_field_mask_pb2.FieldMask, _Mapping]] = ..., stream: _Optional[_Union[_streams_resources_pb2.Stream, _Mapping]] = ..., request_id: _Optional[str] = ...) -> None: ...
