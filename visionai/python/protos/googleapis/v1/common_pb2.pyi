from google.api import field_behavior_pb2 as _field_behavior_pb2
from google.api import resource_pb2 as _resource_pb2
from google.protobuf import timestamp_pb2 as _timestamp_pb2
from google.protobuf.internal import containers as _containers
from google.protobuf.internal import enum_type_wrapper as _enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Iterable as _Iterable, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class Cluster(_message.Message):
    __slots__ = ["annotations", "create_time", "dataplane_service_endpoint", "labels", "name", "psc_target", "state", "update_time"]
    class State(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = []
    class AnnotationsEntry(_message.Message):
        __slots__ = ["key", "value"]
        KEY_FIELD_NUMBER: _ClassVar[int]
        VALUE_FIELD_NUMBER: _ClassVar[int]
        key: str
        value: str
        def __init__(self, key: _Optional[str] = ..., value: _Optional[str] = ...) -> None: ...
    class LabelsEntry(_message.Message):
        __slots__ = ["key", "value"]
        KEY_FIELD_NUMBER: _ClassVar[int]
        VALUE_FIELD_NUMBER: _ClassVar[int]
        key: str
        value: str
        def __init__(self, key: _Optional[str] = ..., value: _Optional[str] = ...) -> None: ...
    ANNOTATIONS_FIELD_NUMBER: _ClassVar[int]
    CREATE_TIME_FIELD_NUMBER: _ClassVar[int]
    DATAPLANE_SERVICE_ENDPOINT_FIELD_NUMBER: _ClassVar[int]
    ERROR: Cluster.State
    LABELS_FIELD_NUMBER: _ClassVar[int]
    NAME_FIELD_NUMBER: _ClassVar[int]
    PROVISIONING: Cluster.State
    PSC_TARGET_FIELD_NUMBER: _ClassVar[int]
    RUNNING: Cluster.State
    STATE_FIELD_NUMBER: _ClassVar[int]
    STATE_UNSPECIFIED: Cluster.State
    STOPPING: Cluster.State
    UPDATE_TIME_FIELD_NUMBER: _ClassVar[int]
    annotations: _containers.ScalarMap[str, str]
    create_time: _timestamp_pb2.Timestamp
    dataplane_service_endpoint: str
    labels: _containers.ScalarMap[str, str]
    name: str
    psc_target: str
    state: Cluster.State
    update_time: _timestamp_pb2.Timestamp
    def __init__(self, name: _Optional[str] = ..., create_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., update_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., labels: _Optional[_Mapping[str, str]] = ..., annotations: _Optional[_Mapping[str, str]] = ..., dataplane_service_endpoint: _Optional[str] = ..., state: _Optional[_Union[Cluster.State, str]] = ..., psc_target: _Optional[str] = ...) -> None: ...

class GcsSource(_message.Message):
    __slots__ = ["uris"]
    URIS_FIELD_NUMBER: _ClassVar[int]
    uris: _containers.RepeatedScalarFieldContainer[str]
    def __init__(self, uris: _Optional[_Iterable[str]] = ...) -> None: ...

class OperationMetadata(_message.Message):
    __slots__ = ["api_version", "create_time", "end_time", "requested_cancellation", "status_message", "target", "verb"]
    API_VERSION_FIELD_NUMBER: _ClassVar[int]
    CREATE_TIME_FIELD_NUMBER: _ClassVar[int]
    END_TIME_FIELD_NUMBER: _ClassVar[int]
    REQUESTED_CANCELLATION_FIELD_NUMBER: _ClassVar[int]
    STATUS_MESSAGE_FIELD_NUMBER: _ClassVar[int]
    TARGET_FIELD_NUMBER: _ClassVar[int]
    VERB_FIELD_NUMBER: _ClassVar[int]
    api_version: str
    create_time: _timestamp_pb2.Timestamp
    end_time: _timestamp_pb2.Timestamp
    requested_cancellation: bool
    status_message: str
    target: str
    verb: str
    def __init__(self, create_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., end_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., target: _Optional[str] = ..., verb: _Optional[str] = ..., status_message: _Optional[str] = ..., requested_cancellation: bool = ..., api_version: _Optional[str] = ...) -> None: ...
