from google.api import annotations_pb2 as _annotations_pb2
from google.api import client_pb2 as _client_pb2
from google.api import field_behavior_pb2 as _field_behavior_pb2
from google.api import resource_pb2 as _resource_pb2
from visionai.python.protos.googleapis.v1 import common_pb2 as _common_pb2
from visionai.python.protos.googleapis.v1 import lva_resources_pb2 as _lva_resources_pb2
from google.longrunning import operations_pb2 as _operations_pb2
from google.protobuf import empty_pb2 as _empty_pb2
from google.protobuf import field_mask_pb2 as _field_mask_pb2
from google.protobuf.internal import containers as _containers
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Iterable as _Iterable, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class BatchRunProcessRequest(_message.Message):
    __slots__ = ["batch_id", "options", "parent", "requests"]
    class BatchRunProcessOptions(_message.Message):
        __slots__ = ["batch_size", "retry_count"]
        BATCH_SIZE_FIELD_NUMBER: _ClassVar[int]
        RETRY_COUNT_FIELD_NUMBER: _ClassVar[int]
        batch_size: int
        retry_count: int
        def __init__(self, retry_count: _Optional[int] = ..., batch_size: _Optional[int] = ...) -> None: ...
    BATCH_ID_FIELD_NUMBER: _ClassVar[int]
    OPTIONS_FIELD_NUMBER: _ClassVar[int]
    PARENT_FIELD_NUMBER: _ClassVar[int]
    REQUESTS_FIELD_NUMBER: _ClassVar[int]
    batch_id: str
    options: BatchRunProcessRequest.BatchRunProcessOptions
    parent: str
    requests: _containers.RepeatedCompositeFieldContainer[CreateProcessRequest]
    def __init__(self, parent: _Optional[str] = ..., requests: _Optional[_Iterable[_Union[CreateProcessRequest, _Mapping]]] = ..., options: _Optional[_Union[BatchRunProcessRequest.BatchRunProcessOptions, _Mapping]] = ..., batch_id: _Optional[str] = ...) -> None: ...

class BatchRunProcessResponse(_message.Message):
    __slots__ = ["batch_id", "processes"]
    BATCH_ID_FIELD_NUMBER: _ClassVar[int]
    PROCESSES_FIELD_NUMBER: _ClassVar[int]
    batch_id: str
    processes: _containers.RepeatedCompositeFieldContainer[_lva_resources_pb2.Process]
    def __init__(self, batch_id: _Optional[str] = ..., processes: _Optional[_Iterable[_Union[_lva_resources_pb2.Process, _Mapping]]] = ...) -> None: ...

class CreateAnalysisRequest(_message.Message):
    __slots__ = ["analysis", "analysis_id", "parent", "request_id"]
    ANALYSIS_FIELD_NUMBER: _ClassVar[int]
    ANALYSIS_ID_FIELD_NUMBER: _ClassVar[int]
    PARENT_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    analysis: _lva_resources_pb2.Analysis
    analysis_id: str
    parent: str
    request_id: str
    def __init__(self, parent: _Optional[str] = ..., analysis_id: _Optional[str] = ..., analysis: _Optional[_Union[_lva_resources_pb2.Analysis, _Mapping]] = ..., request_id: _Optional[str] = ...) -> None: ...

class CreateProcessRequest(_message.Message):
    __slots__ = ["parent", "process", "process_id", "request_id"]
    PARENT_FIELD_NUMBER: _ClassVar[int]
    PROCESS_FIELD_NUMBER: _ClassVar[int]
    PROCESS_ID_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    parent: str
    process: _lva_resources_pb2.Process
    process_id: str
    request_id: str
    def __init__(self, parent: _Optional[str] = ..., process_id: _Optional[str] = ..., process: _Optional[_Union[_lva_resources_pb2.Process, _Mapping]] = ..., request_id: _Optional[str] = ...) -> None: ...

class DeleteAnalysisRequest(_message.Message):
    __slots__ = ["name", "request_id"]
    NAME_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    name: str
    request_id: str
    def __init__(self, name: _Optional[str] = ..., request_id: _Optional[str] = ...) -> None: ...

class DeleteProcessRequest(_message.Message):
    __slots__ = ["name", "request_id"]
    NAME_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    name: str
    request_id: str
    def __init__(self, name: _Optional[str] = ..., request_id: _Optional[str] = ...) -> None: ...

class GetAnalysisRequest(_message.Message):
    __slots__ = ["name"]
    NAME_FIELD_NUMBER: _ClassVar[int]
    name: str
    def __init__(self, name: _Optional[str] = ...) -> None: ...

class GetProcessRequest(_message.Message):
    __slots__ = ["name"]
    NAME_FIELD_NUMBER: _ClassVar[int]
    name: str
    def __init__(self, name: _Optional[str] = ...) -> None: ...

class ListAnalysesRequest(_message.Message):
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

class ListAnalysesResponse(_message.Message):
    __slots__ = ["analyses", "next_page_token", "unreachable"]
    ANALYSES_FIELD_NUMBER: _ClassVar[int]
    NEXT_PAGE_TOKEN_FIELD_NUMBER: _ClassVar[int]
    UNREACHABLE_FIELD_NUMBER: _ClassVar[int]
    analyses: _containers.RepeatedCompositeFieldContainer[_lva_resources_pb2.Analysis]
    next_page_token: str
    unreachable: _containers.RepeatedScalarFieldContainer[str]
    def __init__(self, analyses: _Optional[_Iterable[_Union[_lva_resources_pb2.Analysis, _Mapping]]] = ..., next_page_token: _Optional[str] = ..., unreachable: _Optional[_Iterable[str]] = ...) -> None: ...

class ListProcessesRequest(_message.Message):
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

class ListProcessesResponse(_message.Message):
    __slots__ = ["next_page_token", "processes", "unreachable"]
    NEXT_PAGE_TOKEN_FIELD_NUMBER: _ClassVar[int]
    PROCESSES_FIELD_NUMBER: _ClassVar[int]
    UNREACHABLE_FIELD_NUMBER: _ClassVar[int]
    next_page_token: str
    processes: _containers.RepeatedCompositeFieldContainer[_lva_resources_pb2.Process]
    unreachable: _containers.RepeatedScalarFieldContainer[str]
    def __init__(self, processes: _Optional[_Iterable[_Union[_lva_resources_pb2.Process, _Mapping]]] = ..., next_page_token: _Optional[str] = ..., unreachable: _Optional[_Iterable[str]] = ...) -> None: ...

class UpdateAnalysisRequest(_message.Message):
    __slots__ = ["analysis", "request_id", "update_mask"]
    ANALYSIS_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    UPDATE_MASK_FIELD_NUMBER: _ClassVar[int]
    analysis: _lva_resources_pb2.Analysis
    request_id: str
    update_mask: _field_mask_pb2.FieldMask
    def __init__(self, update_mask: _Optional[_Union[_field_mask_pb2.FieldMask, _Mapping]] = ..., analysis: _Optional[_Union[_lva_resources_pb2.Analysis, _Mapping]] = ..., request_id: _Optional[str] = ...) -> None: ...

class UpdateProcessRequest(_message.Message):
    __slots__ = ["process", "request_id", "update_mask"]
    PROCESS_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    UPDATE_MASK_FIELD_NUMBER: _ClassVar[int]
    process: _lva_resources_pb2.Process
    request_id: str
    update_mask: _field_mask_pb2.FieldMask
    def __init__(self, update_mask: _Optional[_Union[_field_mask_pb2.FieldMask, _Mapping]] = ..., process: _Optional[_Union[_lva_resources_pb2.Process, _Mapping]] = ..., request_id: _Optional[str] = ...) -> None: ...
