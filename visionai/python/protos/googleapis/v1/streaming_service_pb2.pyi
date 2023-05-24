from google.api import annotations_pb2 as _annotations_pb2
from google.api import client_pb2 as _client_pb2
from visionai.python.protos.googleapis.v1 import streaming_resources_pb2 as _streaming_resources_pb2
from google.protobuf import duration_pb2 as _duration_pb2
from google.protobuf import timestamp_pb2 as _timestamp_pb2
from google.protobuf.internal import enum_type_wrapper as _enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor
LEASE_TYPE_READER: LeaseType
LEASE_TYPE_UNSPECIFIED: LeaseType
LEASE_TYPE_WRITER: LeaseType

class AcquireLeaseRequest(_message.Message):
    __slots__ = ["lease_type", "owner", "series", "term"]
    LEASE_TYPE_FIELD_NUMBER: _ClassVar[int]
    OWNER_FIELD_NUMBER: _ClassVar[int]
    SERIES_FIELD_NUMBER: _ClassVar[int]
    TERM_FIELD_NUMBER: _ClassVar[int]
    lease_type: LeaseType
    owner: str
    series: str
    term: _duration_pb2.Duration
    def __init__(self, series: _Optional[str] = ..., owner: _Optional[str] = ..., term: _Optional[_Union[_duration_pb2.Duration, _Mapping]] = ..., lease_type: _Optional[_Union[LeaseType, str]] = ...) -> None: ...

class CommitRequest(_message.Message):
    __slots__ = ["offset"]
    OFFSET_FIELD_NUMBER: _ClassVar[int]
    offset: int
    def __init__(self, offset: _Optional[int] = ...) -> None: ...

class ControlledMode(_message.Message):
    __slots__ = ["fallback_starting_offset", "starting_logical_offset"]
    FALLBACK_STARTING_OFFSET_FIELD_NUMBER: _ClassVar[int]
    STARTING_LOGICAL_OFFSET_FIELD_NUMBER: _ClassVar[int]
    fallback_starting_offset: str
    starting_logical_offset: str
    def __init__(self, starting_logical_offset: _Optional[str] = ..., fallback_starting_offset: _Optional[str] = ...) -> None: ...

class EagerMode(_message.Message):
    __slots__ = []
    def __init__(self) -> None: ...

class EventUpdate(_message.Message):
    __slots__ = ["event", "offset", "series", "stream", "update_time"]
    EVENT_FIELD_NUMBER: _ClassVar[int]
    OFFSET_FIELD_NUMBER: _ClassVar[int]
    SERIES_FIELD_NUMBER: _ClassVar[int]
    STREAM_FIELD_NUMBER: _ClassVar[int]
    UPDATE_TIME_FIELD_NUMBER: _ClassVar[int]
    event: str
    offset: int
    series: str
    stream: str
    update_time: _timestamp_pb2.Timestamp
    def __init__(self, stream: _Optional[str] = ..., event: _Optional[str] = ..., series: _Optional[str] = ..., update_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., offset: _Optional[int] = ...) -> None: ...

class Lease(_message.Message):
    __slots__ = ["expire_time", "id", "lease_type", "owner", "series"]
    EXPIRE_TIME_FIELD_NUMBER: _ClassVar[int]
    ID_FIELD_NUMBER: _ClassVar[int]
    LEASE_TYPE_FIELD_NUMBER: _ClassVar[int]
    OWNER_FIELD_NUMBER: _ClassVar[int]
    SERIES_FIELD_NUMBER: _ClassVar[int]
    expire_time: _timestamp_pb2.Timestamp
    id: str
    lease_type: LeaseType
    owner: str
    series: str
    def __init__(self, id: _Optional[str] = ..., series: _Optional[str] = ..., owner: _Optional[str] = ..., expire_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., lease_type: _Optional[_Union[LeaseType, str]] = ...) -> None: ...

class ReceiveEventsControlResponse(_message.Message):
    __slots__ = ["heartbeat", "writes_done_request"]
    HEARTBEAT_FIELD_NUMBER: _ClassVar[int]
    WRITES_DONE_REQUEST_FIELD_NUMBER: _ClassVar[int]
    heartbeat: bool
    writes_done_request: bool
    def __init__(self, heartbeat: bool = ..., writes_done_request: bool = ...) -> None: ...

class ReceiveEventsRequest(_message.Message):
    __slots__ = ["commit_request", "setup_request"]
    class SetupRequest(_message.Message):
        __slots__ = ["cluster", "controlled_mode", "heartbeat_interval", "receiver", "stream", "writes_done_grace_period"]
        CLUSTER_FIELD_NUMBER: _ClassVar[int]
        CONTROLLED_MODE_FIELD_NUMBER: _ClassVar[int]
        HEARTBEAT_INTERVAL_FIELD_NUMBER: _ClassVar[int]
        RECEIVER_FIELD_NUMBER: _ClassVar[int]
        STREAM_FIELD_NUMBER: _ClassVar[int]
        WRITES_DONE_GRACE_PERIOD_FIELD_NUMBER: _ClassVar[int]
        cluster: str
        controlled_mode: ControlledMode
        heartbeat_interval: _duration_pb2.Duration
        receiver: str
        stream: str
        writes_done_grace_period: _duration_pb2.Duration
        def __init__(self, cluster: _Optional[str] = ..., stream: _Optional[str] = ..., receiver: _Optional[str] = ..., controlled_mode: _Optional[_Union[ControlledMode, _Mapping]] = ..., heartbeat_interval: _Optional[_Union[_duration_pb2.Duration, _Mapping]] = ..., writes_done_grace_period: _Optional[_Union[_duration_pb2.Duration, _Mapping]] = ...) -> None: ...
    COMMIT_REQUEST_FIELD_NUMBER: _ClassVar[int]
    SETUP_REQUEST_FIELD_NUMBER: _ClassVar[int]
    commit_request: CommitRequest
    setup_request: ReceiveEventsRequest.SetupRequest
    def __init__(self, setup_request: _Optional[_Union[ReceiveEventsRequest.SetupRequest, _Mapping]] = ..., commit_request: _Optional[_Union[CommitRequest, _Mapping]] = ...) -> None: ...

class ReceiveEventsResponse(_message.Message):
    __slots__ = ["control", "event_update"]
    CONTROL_FIELD_NUMBER: _ClassVar[int]
    EVENT_UPDATE_FIELD_NUMBER: _ClassVar[int]
    control: ReceiveEventsControlResponse
    event_update: EventUpdate
    def __init__(self, event_update: _Optional[_Union[EventUpdate, _Mapping]] = ..., control: _Optional[_Union[ReceiveEventsControlResponse, _Mapping]] = ...) -> None: ...

class ReceivePacketsControlResponse(_message.Message):
    __slots__ = ["heartbeat", "writes_done_request"]
    HEARTBEAT_FIELD_NUMBER: _ClassVar[int]
    WRITES_DONE_REQUEST_FIELD_NUMBER: _ClassVar[int]
    heartbeat: bool
    writes_done_request: bool
    def __init__(self, heartbeat: bool = ..., writes_done_request: bool = ...) -> None: ...

class ReceivePacketsRequest(_message.Message):
    __slots__ = ["commit_request", "setup_request"]
    class SetupRequest(_message.Message):
        __slots__ = ["controlled_receive_mode", "eager_receive_mode", "heartbeat_interval", "metadata", "receiver", "writes_done_grace_period"]
        CONTROLLED_RECEIVE_MODE_FIELD_NUMBER: _ClassVar[int]
        EAGER_RECEIVE_MODE_FIELD_NUMBER: _ClassVar[int]
        HEARTBEAT_INTERVAL_FIELD_NUMBER: _ClassVar[int]
        METADATA_FIELD_NUMBER: _ClassVar[int]
        RECEIVER_FIELD_NUMBER: _ClassVar[int]
        WRITES_DONE_GRACE_PERIOD_FIELD_NUMBER: _ClassVar[int]
        controlled_receive_mode: ControlledMode
        eager_receive_mode: EagerMode
        heartbeat_interval: _duration_pb2.Duration
        metadata: RequestMetadata
        receiver: str
        writes_done_grace_period: _duration_pb2.Duration
        def __init__(self, eager_receive_mode: _Optional[_Union[EagerMode, _Mapping]] = ..., controlled_receive_mode: _Optional[_Union[ControlledMode, _Mapping]] = ..., metadata: _Optional[_Union[RequestMetadata, _Mapping]] = ..., receiver: _Optional[str] = ..., heartbeat_interval: _Optional[_Union[_duration_pb2.Duration, _Mapping]] = ..., writes_done_grace_period: _Optional[_Union[_duration_pb2.Duration, _Mapping]] = ...) -> None: ...
    COMMIT_REQUEST_FIELD_NUMBER: _ClassVar[int]
    SETUP_REQUEST_FIELD_NUMBER: _ClassVar[int]
    commit_request: CommitRequest
    setup_request: ReceivePacketsRequest.SetupRequest
    def __init__(self, setup_request: _Optional[_Union[ReceivePacketsRequest.SetupRequest, _Mapping]] = ..., commit_request: _Optional[_Union[CommitRequest, _Mapping]] = ...) -> None: ...

class ReceivePacketsResponse(_message.Message):
    __slots__ = ["control", "packet"]
    CONTROL_FIELD_NUMBER: _ClassVar[int]
    PACKET_FIELD_NUMBER: _ClassVar[int]
    control: ReceivePacketsControlResponse
    packet: _streaming_resources_pb2.Packet
    def __init__(self, packet: _Optional[_Union[_streaming_resources_pb2.Packet, _Mapping]] = ..., control: _Optional[_Union[ReceivePacketsControlResponse, _Mapping]] = ...) -> None: ...

class ReleaseLeaseRequest(_message.Message):
    __slots__ = ["id", "owner", "series"]
    ID_FIELD_NUMBER: _ClassVar[int]
    OWNER_FIELD_NUMBER: _ClassVar[int]
    SERIES_FIELD_NUMBER: _ClassVar[int]
    id: str
    owner: str
    series: str
    def __init__(self, id: _Optional[str] = ..., series: _Optional[str] = ..., owner: _Optional[str] = ...) -> None: ...

class ReleaseLeaseResponse(_message.Message):
    __slots__ = []
    def __init__(self) -> None: ...

class RenewLeaseRequest(_message.Message):
    __slots__ = ["id", "owner", "series", "term"]
    ID_FIELD_NUMBER: _ClassVar[int]
    OWNER_FIELD_NUMBER: _ClassVar[int]
    SERIES_FIELD_NUMBER: _ClassVar[int]
    TERM_FIELD_NUMBER: _ClassVar[int]
    id: str
    owner: str
    series: str
    term: _duration_pb2.Duration
    def __init__(self, id: _Optional[str] = ..., series: _Optional[str] = ..., owner: _Optional[str] = ..., term: _Optional[_Union[_duration_pb2.Duration, _Mapping]] = ...) -> None: ...

class RequestMetadata(_message.Message):
    __slots__ = ["event", "lease_id", "lease_term", "owner", "series", "stream"]
    EVENT_FIELD_NUMBER: _ClassVar[int]
    LEASE_ID_FIELD_NUMBER: _ClassVar[int]
    LEASE_TERM_FIELD_NUMBER: _ClassVar[int]
    OWNER_FIELD_NUMBER: _ClassVar[int]
    SERIES_FIELD_NUMBER: _ClassVar[int]
    STREAM_FIELD_NUMBER: _ClassVar[int]
    event: str
    lease_id: str
    lease_term: _duration_pb2.Duration
    owner: str
    series: str
    stream: str
    def __init__(self, stream: _Optional[str] = ..., event: _Optional[str] = ..., series: _Optional[str] = ..., lease_id: _Optional[str] = ..., owner: _Optional[str] = ..., lease_term: _Optional[_Union[_duration_pb2.Duration, _Mapping]] = ...) -> None: ...

class SendPacketsRequest(_message.Message):
    __slots__ = ["metadata", "packet"]
    METADATA_FIELD_NUMBER: _ClassVar[int]
    PACKET_FIELD_NUMBER: _ClassVar[int]
    metadata: RequestMetadata
    packet: _streaming_resources_pb2.Packet
    def __init__(self, packet: _Optional[_Union[_streaming_resources_pb2.Packet, _Mapping]] = ..., metadata: _Optional[_Union[RequestMetadata, _Mapping]] = ...) -> None: ...

class SendPacketsResponse(_message.Message):
    __slots__ = []
    def __init__(self) -> None: ...

class LeaseType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []
