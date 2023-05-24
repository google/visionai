from google.protobuf.internal import containers as _containers
from google.protobuf.internal import enum_type_wrapper as _enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Iterable as _Iterable, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor
LIVE: RunMode
RUN_MODE_UNSPECIFIED: RunMode
SUBMISSION: RunMode

class AnalysisDefinition(_message.Message):
    __slots__ = ["analyzers"]
    ANALYZERS_FIELD_NUMBER: _ClassVar[int]
    analyzers: _containers.RepeatedCompositeFieldContainer[AnalyzerDefinition]
    def __init__(self, analyzers: _Optional[_Iterable[_Union[AnalyzerDefinition, _Mapping]]] = ...) -> None: ...

class AnalyzerDefinition(_message.Message):
    __slots__ = ["analyzer", "attrs", "debug_options", "inputs", "operator"]
    class AttrsEntry(_message.Message):
        __slots__ = ["key", "value"]
        KEY_FIELD_NUMBER: _ClassVar[int]
        VALUE_FIELD_NUMBER: _ClassVar[int]
        key: str
        value: AttributeValue
        def __init__(self, key: _Optional[str] = ..., value: _Optional[_Union[AttributeValue, _Mapping]] = ...) -> None: ...
    class DebugOptions(_message.Message):
        __slots__ = ["environment_variables"]
        class EnvironmentVariablesEntry(_message.Message):
            __slots__ = ["key", "value"]
            KEY_FIELD_NUMBER: _ClassVar[int]
            VALUE_FIELD_NUMBER: _ClassVar[int]
            key: str
            value: str
            def __init__(self, key: _Optional[str] = ..., value: _Optional[str] = ...) -> None: ...
        ENVIRONMENT_VARIABLES_FIELD_NUMBER: _ClassVar[int]
        environment_variables: _containers.ScalarMap[str, str]
        def __init__(self, environment_variables: _Optional[_Mapping[str, str]] = ...) -> None: ...
    class StreamInput(_message.Message):
        __slots__ = ["input"]
        INPUT_FIELD_NUMBER: _ClassVar[int]
        input: str
        def __init__(self, input: _Optional[str] = ...) -> None: ...
    ANALYZER_FIELD_NUMBER: _ClassVar[int]
    ATTRS_FIELD_NUMBER: _ClassVar[int]
    DEBUG_OPTIONS_FIELD_NUMBER: _ClassVar[int]
    INPUTS_FIELD_NUMBER: _ClassVar[int]
    OPERATOR_FIELD_NUMBER: _ClassVar[int]
    analyzer: str
    attrs: _containers.MessageMap[str, AttributeValue]
    debug_options: AnalyzerDefinition.DebugOptions
    inputs: _containers.RepeatedCompositeFieldContainer[AnalyzerDefinition.StreamInput]
    operator: str
    def __init__(self, analyzer: _Optional[str] = ..., operator: _Optional[str] = ..., inputs: _Optional[_Iterable[_Union[AnalyzerDefinition.StreamInput, _Mapping]]] = ..., attrs: _Optional[_Mapping[str, AttributeValue]] = ..., debug_options: _Optional[_Union[AnalyzerDefinition.DebugOptions, _Mapping]] = ...) -> None: ...

class AttributeValue(_message.Message):
    __slots__ = ["b", "f", "i", "s"]
    B_FIELD_NUMBER: _ClassVar[int]
    F_FIELD_NUMBER: _ClassVar[int]
    I_FIELD_NUMBER: _ClassVar[int]
    S_FIELD_NUMBER: _ClassVar[int]
    b: bool
    f: float
    i: int
    s: bytes
    def __init__(self, i: _Optional[int] = ..., f: _Optional[float] = ..., b: bool = ..., s: _Optional[bytes] = ...) -> None: ...

class RunStatus(_message.Message):
    __slots__ = ["reason", "state"]
    class State(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = []
    COMPLETED: RunStatus.State
    FAILED: RunStatus.State
    INITIALIZING: RunStatus.State
    PENDING: RunStatus.State
    REASON_FIELD_NUMBER: _ClassVar[int]
    RUNNING: RunStatus.State
    STATE_FIELD_NUMBER: _ClassVar[int]
    STATE_UNSPECIFIED: RunStatus.State
    reason: str
    state: RunStatus.State
    def __init__(self, state: _Optional[_Union[RunStatus.State, str]] = ..., reason: _Optional[str] = ...) -> None: ...

class RunMode(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []
