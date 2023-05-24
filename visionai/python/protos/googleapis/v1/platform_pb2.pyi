from google.api import annotations_pb2 as _annotations_pb2
from google.api import client_pb2 as _client_pb2
from google.api import field_behavior_pb2 as _field_behavior_pb2
from google.api import resource_pb2 as _resource_pb2
from visionai.python.protos.googleapis.v1 import annotations_pb2 as _annotations_pb2_1
from visionai.python.protos.googleapis.v1 import common_pb2 as _common_pb2
from google.longrunning import operations_pb2 as _operations_pb2
from google.protobuf import duration_pb2 as _duration_pb2
from google.protobuf import field_mask_pb2 as _field_mask_pb2
from google.protobuf import timestamp_pb2 as _timestamp_pb2
from google.protobuf.internal import containers as _containers
from google.protobuf.internal import enum_type_wrapper as _enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Iterable as _Iterable, Mapping as _Mapping, Optional as _Optional, Union as _Union

ACCELERATOR_TYPE_UNSPECIFIED: AcceleratorType
DESCRIPTOR: _descriptor.FileDescriptor
IMAGE_CLASSIFICATION: ModelType
MODEL_TYPE_UNSPECIFIED: ModelType
NVIDIA_TESLA_A100: AcceleratorType
NVIDIA_TESLA_K80: AcceleratorType
NVIDIA_TESLA_P100: AcceleratorType
NVIDIA_TESLA_P4: AcceleratorType
NVIDIA_TESLA_T4: AcceleratorType
NVIDIA_TESLA_V100: AcceleratorType
OBJECT_DETECTION: ModelType
OCCUPANCY_COUNTING: ModelType
PERSON_BLUR: ModelType
TPU_V2: AcceleratorType
TPU_V3: AcceleratorType
VERTEX_CUSTOM: ModelType
VIDEO_ACTION_RECOGNITION: ModelType
VIDEO_CLASSIFICATION: ModelType
VIDEO_OBJECT_TRACKING: ModelType

class AIEnabledDevicesInputConfig(_message.Message):
    __slots__ = []
    def __init__(self) -> None: ...

class AddApplicationStreamInputRequest(_message.Message):
    __slots__ = ["application_stream_inputs", "name", "request_id"]
    APPLICATION_STREAM_INPUTS_FIELD_NUMBER: _ClassVar[int]
    NAME_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    application_stream_inputs: _containers.RepeatedCompositeFieldContainer[ApplicationStreamInput]
    name: str
    request_id: str
    def __init__(self, name: _Optional[str] = ..., application_stream_inputs: _Optional[_Iterable[_Union[ApplicationStreamInput, _Mapping]]] = ..., request_id: _Optional[str] = ...) -> None: ...

class AddApplicationStreamInputResponse(_message.Message):
    __slots__ = []
    def __init__(self) -> None: ...

class Application(_message.Message):
    __slots__ = ["application_configs", "create_time", "description", "display_name", "labels", "name", "runtime_info", "state", "update_time"]
    class State(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = []
    class ApplicationRuntimeInfo(_message.Message):
        __slots__ = ["deploy_time", "global_output_resources", "monitoring_config"]
        class GlobalOutputResource(_message.Message):
            __slots__ = ["key", "output_resource", "producer_node"]
            KEY_FIELD_NUMBER: _ClassVar[int]
            OUTPUT_RESOURCE_FIELD_NUMBER: _ClassVar[int]
            PRODUCER_NODE_FIELD_NUMBER: _ClassVar[int]
            key: str
            output_resource: str
            producer_node: str
            def __init__(self, output_resource: _Optional[str] = ..., producer_node: _Optional[str] = ..., key: _Optional[str] = ...) -> None: ...
        class MonitoringConfig(_message.Message):
            __slots__ = ["enabled"]
            ENABLED_FIELD_NUMBER: _ClassVar[int]
            enabled: bool
            def __init__(self, enabled: bool = ...) -> None: ...
        DEPLOY_TIME_FIELD_NUMBER: _ClassVar[int]
        GLOBAL_OUTPUT_RESOURCES_FIELD_NUMBER: _ClassVar[int]
        MONITORING_CONFIG_FIELD_NUMBER: _ClassVar[int]
        deploy_time: _timestamp_pb2.Timestamp
        global_output_resources: _containers.RepeatedCompositeFieldContainer[Application.ApplicationRuntimeInfo.GlobalOutputResource]
        monitoring_config: Application.ApplicationRuntimeInfo.MonitoringConfig
        def __init__(self, deploy_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., global_output_resources: _Optional[_Iterable[_Union[Application.ApplicationRuntimeInfo.GlobalOutputResource, _Mapping]]] = ..., monitoring_config: _Optional[_Union[Application.ApplicationRuntimeInfo.MonitoringConfig, _Mapping]] = ...) -> None: ...
    class LabelsEntry(_message.Message):
        __slots__ = ["key", "value"]
        KEY_FIELD_NUMBER: _ClassVar[int]
        VALUE_FIELD_NUMBER: _ClassVar[int]
        key: str
        value: str
        def __init__(self, key: _Optional[str] = ..., value: _Optional[str] = ...) -> None: ...
    APPLICATION_CONFIGS_FIELD_NUMBER: _ClassVar[int]
    CREATED: Application.State
    CREATE_TIME_FIELD_NUMBER: _ClassVar[int]
    CREATING: Application.State
    DELETED: Application.State
    DELETING: Application.State
    DEPLOYED: Application.State
    DEPLOYING: Application.State
    DESCRIPTION_FIELD_NUMBER: _ClassVar[int]
    DISPLAY_NAME_FIELD_NUMBER: _ClassVar[int]
    ERROR: Application.State
    FIXING: Application.State
    LABELS_FIELD_NUMBER: _ClassVar[int]
    NAME_FIELD_NUMBER: _ClassVar[int]
    RUNTIME_INFO_FIELD_NUMBER: _ClassVar[int]
    STATE_FIELD_NUMBER: _ClassVar[int]
    STATE_UNSPECIFIED: Application.State
    UNDEPLOYING: Application.State
    UPDATE_TIME_FIELD_NUMBER: _ClassVar[int]
    UPDATING: Application.State
    application_configs: ApplicationConfigs
    create_time: _timestamp_pb2.Timestamp
    description: str
    display_name: str
    labels: _containers.ScalarMap[str, str]
    name: str
    runtime_info: Application.ApplicationRuntimeInfo
    state: Application.State
    update_time: _timestamp_pb2.Timestamp
    def __init__(self, name: _Optional[str] = ..., create_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., update_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., labels: _Optional[_Mapping[str, str]] = ..., display_name: _Optional[str] = ..., description: _Optional[str] = ..., application_configs: _Optional[_Union[ApplicationConfigs, _Mapping]] = ..., runtime_info: _Optional[_Union[Application.ApplicationRuntimeInfo, _Mapping]] = ..., state: _Optional[_Union[Application.State, str]] = ...) -> None: ...

class ApplicationConfigs(_message.Message):
    __slots__ = ["event_delivery_config", "nodes"]
    class EventDeliveryConfig(_message.Message):
        __slots__ = ["channel", "minimal_delivery_interval"]
        CHANNEL_FIELD_NUMBER: _ClassVar[int]
        MINIMAL_DELIVERY_INTERVAL_FIELD_NUMBER: _ClassVar[int]
        channel: str
        minimal_delivery_interval: _duration_pb2.Duration
        def __init__(self, channel: _Optional[str] = ..., minimal_delivery_interval: _Optional[_Union[_duration_pb2.Duration, _Mapping]] = ...) -> None: ...
    EVENT_DELIVERY_CONFIG_FIELD_NUMBER: _ClassVar[int]
    NODES_FIELD_NUMBER: _ClassVar[int]
    event_delivery_config: ApplicationConfigs.EventDeliveryConfig
    nodes: _containers.RepeatedCompositeFieldContainer[Node]
    def __init__(self, nodes: _Optional[_Iterable[_Union[Node, _Mapping]]] = ..., event_delivery_config: _Optional[_Union[ApplicationConfigs.EventDeliveryConfig, _Mapping]] = ...) -> None: ...

class ApplicationInstance(_message.Message):
    __slots__ = ["instance", "instance_id"]
    INSTANCE_FIELD_NUMBER: _ClassVar[int]
    INSTANCE_ID_FIELD_NUMBER: _ClassVar[int]
    instance: Instance
    instance_id: str
    def __init__(self, instance_id: _Optional[str] = ..., instance: _Optional[_Union[Instance, _Mapping]] = ...) -> None: ...

class ApplicationNodeAnnotation(_message.Message):
    __slots__ = ["annotations", "node"]
    ANNOTATIONS_FIELD_NUMBER: _ClassVar[int]
    NODE_FIELD_NUMBER: _ClassVar[int]
    annotations: _containers.RepeatedCompositeFieldContainer[_annotations_pb2_1.StreamAnnotation]
    node: str
    def __init__(self, node: _Optional[str] = ..., annotations: _Optional[_Iterable[_Union[_annotations_pb2_1.StreamAnnotation, _Mapping]]] = ...) -> None: ...

class ApplicationStreamInput(_message.Message):
    __slots__ = ["stream_with_annotation"]
    STREAM_WITH_ANNOTATION_FIELD_NUMBER: _ClassVar[int]
    stream_with_annotation: StreamWithAnnotation
    def __init__(self, stream_with_annotation: _Optional[_Union[StreamWithAnnotation, _Mapping]] = ...) -> None: ...

class AutoscalingMetricSpec(_message.Message):
    __slots__ = ["metric_name", "target"]
    METRIC_NAME_FIELD_NUMBER: _ClassVar[int]
    TARGET_FIELD_NUMBER: _ClassVar[int]
    metric_name: str
    target: int
    def __init__(self, metric_name: _Optional[str] = ..., target: _Optional[int] = ...) -> None: ...

class BigQueryConfig(_message.Message):
    __slots__ = ["cloud_function_mapping", "create_default_table_if_not_exists", "table"]
    class CloudFunctionMappingEntry(_message.Message):
        __slots__ = ["key", "value"]
        KEY_FIELD_NUMBER: _ClassVar[int]
        VALUE_FIELD_NUMBER: _ClassVar[int]
        key: str
        value: str
        def __init__(self, key: _Optional[str] = ..., value: _Optional[str] = ...) -> None: ...
    CLOUD_FUNCTION_MAPPING_FIELD_NUMBER: _ClassVar[int]
    CREATE_DEFAULT_TABLE_IF_NOT_EXISTS_FIELD_NUMBER: _ClassVar[int]
    TABLE_FIELD_NUMBER: _ClassVar[int]
    cloud_function_mapping: _containers.ScalarMap[str, str]
    create_default_table_if_not_exists: bool
    table: str
    def __init__(self, table: _Optional[str] = ..., cloud_function_mapping: _Optional[_Mapping[str, str]] = ..., create_default_table_if_not_exists: bool = ...) -> None: ...

class CreateApplicationInstancesRequest(_message.Message):
    __slots__ = ["application_instances", "name", "request_id"]
    APPLICATION_INSTANCES_FIELD_NUMBER: _ClassVar[int]
    NAME_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    application_instances: _containers.RepeatedCompositeFieldContainer[ApplicationInstance]
    name: str
    request_id: str
    def __init__(self, name: _Optional[str] = ..., application_instances: _Optional[_Iterable[_Union[ApplicationInstance, _Mapping]]] = ..., request_id: _Optional[str] = ...) -> None: ...

class CreateApplicationInstancesResponse(_message.Message):
    __slots__ = []
    def __init__(self) -> None: ...

class CreateApplicationRequest(_message.Message):
    __slots__ = ["application", "application_id", "parent", "request_id"]
    APPLICATION_FIELD_NUMBER: _ClassVar[int]
    APPLICATION_ID_FIELD_NUMBER: _ClassVar[int]
    PARENT_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    application: Application
    application_id: str
    parent: str
    request_id: str
    def __init__(self, parent: _Optional[str] = ..., application_id: _Optional[str] = ..., application: _Optional[_Union[Application, _Mapping]] = ..., request_id: _Optional[str] = ...) -> None: ...

class CreateDraftRequest(_message.Message):
    __slots__ = ["draft", "draft_id", "parent", "request_id"]
    DRAFT_FIELD_NUMBER: _ClassVar[int]
    DRAFT_ID_FIELD_NUMBER: _ClassVar[int]
    PARENT_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    draft: Draft
    draft_id: str
    parent: str
    request_id: str
    def __init__(self, parent: _Optional[str] = ..., draft_id: _Optional[str] = ..., draft: _Optional[_Union[Draft, _Mapping]] = ..., request_id: _Optional[str] = ...) -> None: ...

class CreateProcessorRequest(_message.Message):
    __slots__ = ["parent", "processor", "processor_id", "request_id"]
    PARENT_FIELD_NUMBER: _ClassVar[int]
    PROCESSOR_FIELD_NUMBER: _ClassVar[int]
    PROCESSOR_ID_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    parent: str
    processor: Processor
    processor_id: str
    request_id: str
    def __init__(self, parent: _Optional[str] = ..., processor_id: _Optional[str] = ..., processor: _Optional[_Union[Processor, _Mapping]] = ..., request_id: _Optional[str] = ...) -> None: ...

class CustomProcessorSourceInfo(_message.Message):
    __slots__ = ["additional_info", "model_schema", "source_type", "vertex_model"]
    class SourceType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = []
    class AdditionalInfoEntry(_message.Message):
        __slots__ = ["key", "value"]
        KEY_FIELD_NUMBER: _ClassVar[int]
        VALUE_FIELD_NUMBER: _ClassVar[int]
        key: str
        value: str
        def __init__(self, key: _Optional[str] = ..., value: _Optional[str] = ...) -> None: ...
    class ModelSchema(_message.Message):
        __slots__ = ["instances_schema", "parameters_schema", "predictions_schema"]
        INSTANCES_SCHEMA_FIELD_NUMBER: _ClassVar[int]
        PARAMETERS_SCHEMA_FIELD_NUMBER: _ClassVar[int]
        PREDICTIONS_SCHEMA_FIELD_NUMBER: _ClassVar[int]
        instances_schema: _common_pb2.GcsSource
        parameters_schema: _common_pb2.GcsSource
        predictions_schema: _common_pb2.GcsSource
        def __init__(self, instances_schema: _Optional[_Union[_common_pb2.GcsSource, _Mapping]] = ..., parameters_schema: _Optional[_Union[_common_pb2.GcsSource, _Mapping]] = ..., predictions_schema: _Optional[_Union[_common_pb2.GcsSource, _Mapping]] = ...) -> None: ...
    ADDITIONAL_INFO_FIELD_NUMBER: _ClassVar[int]
    MODEL_SCHEMA_FIELD_NUMBER: _ClassVar[int]
    SOURCE_TYPE_FIELD_NUMBER: _ClassVar[int]
    SOURCE_TYPE_UNSPECIFIED: CustomProcessorSourceInfo.SourceType
    VERTEX_AUTOML: CustomProcessorSourceInfo.SourceType
    VERTEX_CUSTOM: CustomProcessorSourceInfo.SourceType
    VERTEX_MODEL_FIELD_NUMBER: _ClassVar[int]
    additional_info: _containers.ScalarMap[str, str]
    model_schema: CustomProcessorSourceInfo.ModelSchema
    source_type: CustomProcessorSourceInfo.SourceType
    vertex_model: str
    def __init__(self, vertex_model: _Optional[str] = ..., source_type: _Optional[_Union[CustomProcessorSourceInfo.SourceType, str]] = ..., additional_info: _Optional[_Mapping[str, str]] = ..., model_schema: _Optional[_Union[CustomProcessorSourceInfo.ModelSchema, _Mapping]] = ...) -> None: ...

class DedicatedResources(_message.Message):
    __slots__ = ["autoscaling_metric_specs", "machine_spec", "max_replica_count", "min_replica_count"]
    AUTOSCALING_METRIC_SPECS_FIELD_NUMBER: _ClassVar[int]
    MACHINE_SPEC_FIELD_NUMBER: _ClassVar[int]
    MAX_REPLICA_COUNT_FIELD_NUMBER: _ClassVar[int]
    MIN_REPLICA_COUNT_FIELD_NUMBER: _ClassVar[int]
    autoscaling_metric_specs: _containers.RepeatedCompositeFieldContainer[AutoscalingMetricSpec]
    machine_spec: MachineSpec
    max_replica_count: int
    min_replica_count: int
    def __init__(self, machine_spec: _Optional[_Union[MachineSpec, _Mapping]] = ..., min_replica_count: _Optional[int] = ..., max_replica_count: _Optional[int] = ..., autoscaling_metric_specs: _Optional[_Iterable[_Union[AutoscalingMetricSpec, _Mapping]]] = ...) -> None: ...

class DeleteApplicationInstancesRequest(_message.Message):
    __slots__ = ["instance_ids", "name", "request_id"]
    INSTANCE_IDS_FIELD_NUMBER: _ClassVar[int]
    NAME_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    instance_ids: _containers.RepeatedScalarFieldContainer[str]
    name: str
    request_id: str
    def __init__(self, name: _Optional[str] = ..., instance_ids: _Optional[_Iterable[str]] = ..., request_id: _Optional[str] = ...) -> None: ...

class DeleteApplicationInstancesResponse(_message.Message):
    __slots__ = []
    def __init__(self) -> None: ...

class DeleteApplicationRequest(_message.Message):
    __slots__ = ["force", "name", "request_id"]
    FORCE_FIELD_NUMBER: _ClassVar[int]
    NAME_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    force: bool
    name: str
    request_id: str
    def __init__(self, name: _Optional[str] = ..., request_id: _Optional[str] = ..., force: bool = ...) -> None: ...

class DeleteDraftRequest(_message.Message):
    __slots__ = ["name", "request_id"]
    NAME_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    name: str
    request_id: str
    def __init__(self, name: _Optional[str] = ..., request_id: _Optional[str] = ...) -> None: ...

class DeleteProcessorRequest(_message.Message):
    __slots__ = ["name", "request_id"]
    NAME_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    name: str
    request_id: str
    def __init__(self, name: _Optional[str] = ..., request_id: _Optional[str] = ...) -> None: ...

class DeployApplicationRequest(_message.Message):
    __slots__ = ["enable_monitoring", "name", "request_id", "validate_only"]
    ENABLE_MONITORING_FIELD_NUMBER: _ClassVar[int]
    NAME_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    VALIDATE_ONLY_FIELD_NUMBER: _ClassVar[int]
    enable_monitoring: bool
    name: str
    request_id: str
    validate_only: bool
    def __init__(self, name: _Optional[str] = ..., validate_only: bool = ..., request_id: _Optional[str] = ..., enable_monitoring: bool = ...) -> None: ...

class DeployApplicationResponse(_message.Message):
    __slots__ = []
    def __init__(self) -> None: ...

class Draft(_message.Message):
    __slots__ = ["create_time", "description", "display_name", "draft_application_configs", "labels", "name", "update_time"]
    class LabelsEntry(_message.Message):
        __slots__ = ["key", "value"]
        KEY_FIELD_NUMBER: _ClassVar[int]
        VALUE_FIELD_NUMBER: _ClassVar[int]
        key: str
        value: str
        def __init__(self, key: _Optional[str] = ..., value: _Optional[str] = ...) -> None: ...
    CREATE_TIME_FIELD_NUMBER: _ClassVar[int]
    DESCRIPTION_FIELD_NUMBER: _ClassVar[int]
    DISPLAY_NAME_FIELD_NUMBER: _ClassVar[int]
    DRAFT_APPLICATION_CONFIGS_FIELD_NUMBER: _ClassVar[int]
    LABELS_FIELD_NUMBER: _ClassVar[int]
    NAME_FIELD_NUMBER: _ClassVar[int]
    UPDATE_TIME_FIELD_NUMBER: _ClassVar[int]
    create_time: _timestamp_pb2.Timestamp
    description: str
    display_name: str
    draft_application_configs: ApplicationConfigs
    labels: _containers.ScalarMap[str, str]
    name: str
    update_time: _timestamp_pb2.Timestamp
    def __init__(self, name: _Optional[str] = ..., create_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., update_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., labels: _Optional[_Mapping[str, str]] = ..., display_name: _Optional[str] = ..., description: _Optional[str] = ..., draft_application_configs: _Optional[_Union[ApplicationConfigs, _Mapping]] = ...) -> None: ...

class GeneralObjectDetectionConfig(_message.Message):
    __slots__ = []
    def __init__(self) -> None: ...

class GetApplicationRequest(_message.Message):
    __slots__ = ["name"]
    NAME_FIELD_NUMBER: _ClassVar[int]
    name: str
    def __init__(self, name: _Optional[str] = ...) -> None: ...

class GetDraftRequest(_message.Message):
    __slots__ = ["name"]
    NAME_FIELD_NUMBER: _ClassVar[int]
    name: str
    def __init__(self, name: _Optional[str] = ...) -> None: ...

class GetInstanceRequest(_message.Message):
    __slots__ = ["name"]
    NAME_FIELD_NUMBER: _ClassVar[int]
    name: str
    def __init__(self, name: _Optional[str] = ...) -> None: ...

class GetProcessorRequest(_message.Message):
    __slots__ = ["name"]
    NAME_FIELD_NUMBER: _ClassVar[int]
    name: str
    def __init__(self, name: _Optional[str] = ...) -> None: ...

class Instance(_message.Message):
    __slots__ = ["create_time", "description", "display_name", "input_resources", "labels", "name", "output_resources", "state", "update_time"]
    class State(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = []
    class InputResource(_message.Message):
        __slots__ = ["annotated_stream", "annotations", "consumer_node", "input_resource", "input_resource_binding"]
        ANNOTATED_STREAM_FIELD_NUMBER: _ClassVar[int]
        ANNOTATIONS_FIELD_NUMBER: _ClassVar[int]
        CONSUMER_NODE_FIELD_NUMBER: _ClassVar[int]
        INPUT_RESOURCE_BINDING_FIELD_NUMBER: _ClassVar[int]
        INPUT_RESOURCE_FIELD_NUMBER: _ClassVar[int]
        annotated_stream: StreamWithAnnotation
        annotations: ResourceAnnotations
        consumer_node: str
        input_resource: str
        input_resource_binding: str
        def __init__(self, input_resource: _Optional[str] = ..., annotated_stream: _Optional[_Union[StreamWithAnnotation, _Mapping]] = ..., consumer_node: _Optional[str] = ..., input_resource_binding: _Optional[str] = ..., annotations: _Optional[_Union[ResourceAnnotations, _Mapping]] = ...) -> None: ...
    class LabelsEntry(_message.Message):
        __slots__ = ["key", "value"]
        KEY_FIELD_NUMBER: _ClassVar[int]
        VALUE_FIELD_NUMBER: _ClassVar[int]
        key: str
        value: str
        def __init__(self, key: _Optional[str] = ..., value: _Optional[str] = ...) -> None: ...
    class OutputResource(_message.Message):
        __slots__ = ["autogen", "is_temporary", "output_resource", "output_resource_binding", "producer_node"]
        AUTOGEN_FIELD_NUMBER: _ClassVar[int]
        IS_TEMPORARY_FIELD_NUMBER: _ClassVar[int]
        OUTPUT_RESOURCE_BINDING_FIELD_NUMBER: _ClassVar[int]
        OUTPUT_RESOURCE_FIELD_NUMBER: _ClassVar[int]
        PRODUCER_NODE_FIELD_NUMBER: _ClassVar[int]
        autogen: bool
        is_temporary: bool
        output_resource: str
        output_resource_binding: str
        producer_node: str
        def __init__(self, output_resource: _Optional[str] = ..., producer_node: _Optional[str] = ..., output_resource_binding: _Optional[str] = ..., is_temporary: bool = ..., autogen: bool = ...) -> None: ...
    CREATED: Instance.State
    CREATE_TIME_FIELD_NUMBER: _ClassVar[int]
    CREATING: Instance.State
    DELETED: Instance.State
    DELETING: Instance.State
    DEPLOYED: Instance.State
    DEPLOYING: Instance.State
    DESCRIPTION_FIELD_NUMBER: _ClassVar[int]
    DISPLAY_NAME_FIELD_NUMBER: _ClassVar[int]
    ERROR: Instance.State
    FIXING: Instance.State
    INPUT_RESOURCES_FIELD_NUMBER: _ClassVar[int]
    LABELS_FIELD_NUMBER: _ClassVar[int]
    NAME_FIELD_NUMBER: _ClassVar[int]
    OUTPUT_RESOURCES_FIELD_NUMBER: _ClassVar[int]
    STATE_FIELD_NUMBER: _ClassVar[int]
    STATE_UNSPECIFIED: Instance.State
    UNDEPLOYING: Instance.State
    UPDATE_TIME_FIELD_NUMBER: _ClassVar[int]
    UPDATING: Instance.State
    create_time: _timestamp_pb2.Timestamp
    description: str
    display_name: str
    input_resources: _containers.RepeatedCompositeFieldContainer[Instance.InputResource]
    labels: _containers.ScalarMap[str, str]
    name: str
    output_resources: _containers.RepeatedCompositeFieldContainer[Instance.OutputResource]
    state: Instance.State
    update_time: _timestamp_pb2.Timestamp
    def __init__(self, name: _Optional[str] = ..., create_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., update_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., labels: _Optional[_Mapping[str, str]] = ..., display_name: _Optional[str] = ..., description: _Optional[str] = ..., input_resources: _Optional[_Iterable[_Union[Instance.InputResource, _Mapping]]] = ..., output_resources: _Optional[_Iterable[_Union[Instance.OutputResource, _Mapping]]] = ..., state: _Optional[_Union[Instance.State, str]] = ...) -> None: ...

class ListApplicationsRequest(_message.Message):
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

class ListApplicationsResponse(_message.Message):
    __slots__ = ["applications", "next_page_token", "unreachable"]
    APPLICATIONS_FIELD_NUMBER: _ClassVar[int]
    NEXT_PAGE_TOKEN_FIELD_NUMBER: _ClassVar[int]
    UNREACHABLE_FIELD_NUMBER: _ClassVar[int]
    applications: _containers.RepeatedCompositeFieldContainer[Application]
    next_page_token: str
    unreachable: _containers.RepeatedScalarFieldContainer[str]
    def __init__(self, applications: _Optional[_Iterable[_Union[Application, _Mapping]]] = ..., next_page_token: _Optional[str] = ..., unreachable: _Optional[_Iterable[str]] = ...) -> None: ...

class ListDraftsRequest(_message.Message):
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

class ListDraftsResponse(_message.Message):
    __slots__ = ["drafts", "next_page_token", "unreachable"]
    DRAFTS_FIELD_NUMBER: _ClassVar[int]
    NEXT_PAGE_TOKEN_FIELD_NUMBER: _ClassVar[int]
    UNREACHABLE_FIELD_NUMBER: _ClassVar[int]
    drafts: _containers.RepeatedCompositeFieldContainer[Draft]
    next_page_token: str
    unreachable: _containers.RepeatedScalarFieldContainer[str]
    def __init__(self, drafts: _Optional[_Iterable[_Union[Draft, _Mapping]]] = ..., next_page_token: _Optional[str] = ..., unreachable: _Optional[_Iterable[str]] = ...) -> None: ...

class ListInstancesRequest(_message.Message):
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

class ListInstancesResponse(_message.Message):
    __slots__ = ["instances", "next_page_token", "unreachable"]
    INSTANCES_FIELD_NUMBER: _ClassVar[int]
    NEXT_PAGE_TOKEN_FIELD_NUMBER: _ClassVar[int]
    UNREACHABLE_FIELD_NUMBER: _ClassVar[int]
    instances: _containers.RepeatedCompositeFieldContainer[Instance]
    next_page_token: str
    unreachable: _containers.RepeatedScalarFieldContainer[str]
    def __init__(self, instances: _Optional[_Iterable[_Union[Instance, _Mapping]]] = ..., next_page_token: _Optional[str] = ..., unreachable: _Optional[_Iterable[str]] = ...) -> None: ...

class ListPrebuiltProcessorsRequest(_message.Message):
    __slots__ = ["parent"]
    PARENT_FIELD_NUMBER: _ClassVar[int]
    parent: str
    def __init__(self, parent: _Optional[str] = ...) -> None: ...

class ListPrebuiltProcessorsResponse(_message.Message):
    __slots__ = ["processors"]
    PROCESSORS_FIELD_NUMBER: _ClassVar[int]
    processors: _containers.RepeatedCompositeFieldContainer[Processor]
    def __init__(self, processors: _Optional[_Iterable[_Union[Processor, _Mapping]]] = ...) -> None: ...

class ListProcessorsRequest(_message.Message):
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

class ListProcessorsResponse(_message.Message):
    __slots__ = ["next_page_token", "processors", "unreachable"]
    NEXT_PAGE_TOKEN_FIELD_NUMBER: _ClassVar[int]
    PROCESSORS_FIELD_NUMBER: _ClassVar[int]
    UNREACHABLE_FIELD_NUMBER: _ClassVar[int]
    next_page_token: str
    processors: _containers.RepeatedCompositeFieldContainer[Processor]
    unreachable: _containers.RepeatedScalarFieldContainer[str]
    def __init__(self, processors: _Optional[_Iterable[_Union[Processor, _Mapping]]] = ..., next_page_token: _Optional[str] = ..., unreachable: _Optional[_Iterable[str]] = ...) -> None: ...

class MachineSpec(_message.Message):
    __slots__ = ["accelerator_count", "accelerator_type", "machine_type"]
    ACCELERATOR_COUNT_FIELD_NUMBER: _ClassVar[int]
    ACCELERATOR_TYPE_FIELD_NUMBER: _ClassVar[int]
    MACHINE_TYPE_FIELD_NUMBER: _ClassVar[int]
    accelerator_count: int
    accelerator_type: AcceleratorType
    machine_type: str
    def __init__(self, machine_type: _Optional[str] = ..., accelerator_type: _Optional[_Union[AcceleratorType, str]] = ..., accelerator_count: _Optional[int] = ...) -> None: ...

class MediaWarehouseConfig(_message.Message):
    __slots__ = ["corpus", "region", "ttl"]
    CORPUS_FIELD_NUMBER: _ClassVar[int]
    REGION_FIELD_NUMBER: _ClassVar[int]
    TTL_FIELD_NUMBER: _ClassVar[int]
    corpus: str
    region: str
    ttl: _duration_pb2.Duration
    def __init__(self, corpus: _Optional[str] = ..., region: _Optional[str] = ..., ttl: _Optional[_Union[_duration_pb2.Duration, _Mapping]] = ...) -> None: ...

class Node(_message.Message):
    __slots__ = ["display_name", "name", "node_config", "output_all_output_channels_to_stream", "parents", "processor"]
    class InputEdge(_message.Message):
        __slots__ = ["connected_input_channel", "parent_node", "parent_output_channel"]
        CONNECTED_INPUT_CHANNEL_FIELD_NUMBER: _ClassVar[int]
        PARENT_NODE_FIELD_NUMBER: _ClassVar[int]
        PARENT_OUTPUT_CHANNEL_FIELD_NUMBER: _ClassVar[int]
        connected_input_channel: str
        parent_node: str
        parent_output_channel: str
        def __init__(self, parent_node: _Optional[str] = ..., parent_output_channel: _Optional[str] = ..., connected_input_channel: _Optional[str] = ...) -> None: ...
    DISPLAY_NAME_FIELD_NUMBER: _ClassVar[int]
    NAME_FIELD_NUMBER: _ClassVar[int]
    NODE_CONFIG_FIELD_NUMBER: _ClassVar[int]
    OUTPUT_ALL_OUTPUT_CHANNELS_TO_STREAM_FIELD_NUMBER: _ClassVar[int]
    PARENTS_FIELD_NUMBER: _ClassVar[int]
    PROCESSOR_FIELD_NUMBER: _ClassVar[int]
    display_name: str
    name: str
    node_config: ProcessorConfig
    output_all_output_channels_to_stream: bool
    parents: _containers.RepeatedCompositeFieldContainer[Node.InputEdge]
    processor: str
    def __init__(self, output_all_output_channels_to_stream: bool = ..., name: _Optional[str] = ..., display_name: _Optional[str] = ..., node_config: _Optional[_Union[ProcessorConfig, _Mapping]] = ..., processor: _Optional[str] = ..., parents: _Optional[_Iterable[_Union[Node.InputEdge, _Mapping]]] = ...) -> None: ...

class OccupancyCountConfig(_message.Message):
    __slots__ = ["enable_dwelling_time_tracking", "enable_people_counting", "enable_vehicle_counting"]
    ENABLE_DWELLING_TIME_TRACKING_FIELD_NUMBER: _ClassVar[int]
    ENABLE_PEOPLE_COUNTING_FIELD_NUMBER: _ClassVar[int]
    ENABLE_VEHICLE_COUNTING_FIELD_NUMBER: _ClassVar[int]
    enable_dwelling_time_tracking: bool
    enable_people_counting: bool
    enable_vehicle_counting: bool
    def __init__(self, enable_people_counting: bool = ..., enable_vehicle_counting: bool = ..., enable_dwelling_time_tracking: bool = ...) -> None: ...

class PersonBlurConfig(_message.Message):
    __slots__ = ["faces_only", "person_blur_type"]
    class PersonBlurType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = []
    BLUR_FILTER: PersonBlurConfig.PersonBlurType
    FACES_ONLY_FIELD_NUMBER: _ClassVar[int]
    FULL_OCCULUSION: PersonBlurConfig.PersonBlurType
    PERSON_BLUR_TYPE_FIELD_NUMBER: _ClassVar[int]
    PERSON_BLUR_TYPE_UNSPECIFIED: PersonBlurConfig.PersonBlurType
    faces_only: bool
    person_blur_type: PersonBlurConfig.PersonBlurType
    def __init__(self, person_blur_type: _Optional[_Union[PersonBlurConfig.PersonBlurType, str]] = ..., faces_only: bool = ...) -> None: ...

class PersonVehicleDetectionConfig(_message.Message):
    __slots__ = ["enable_people_counting", "enable_vehicle_counting"]
    ENABLE_PEOPLE_COUNTING_FIELD_NUMBER: _ClassVar[int]
    ENABLE_VEHICLE_COUNTING_FIELD_NUMBER: _ClassVar[int]
    enable_people_counting: bool
    enable_vehicle_counting: bool
    def __init__(self, enable_people_counting: bool = ..., enable_vehicle_counting: bool = ...) -> None: ...

class PersonalProtectiveEquipmentDetectionConfig(_message.Message):
    __slots__ = ["enable_face_coverage_detection", "enable_hands_coverage_detection", "enable_head_coverage_detection"]
    ENABLE_FACE_COVERAGE_DETECTION_FIELD_NUMBER: _ClassVar[int]
    ENABLE_HANDS_COVERAGE_DETECTION_FIELD_NUMBER: _ClassVar[int]
    ENABLE_HEAD_COVERAGE_DETECTION_FIELD_NUMBER: _ClassVar[int]
    enable_face_coverage_detection: bool
    enable_hands_coverage_detection: bool
    enable_head_coverage_detection: bool
    def __init__(self, enable_face_coverage_detection: bool = ..., enable_head_coverage_detection: bool = ..., enable_hands_coverage_detection: bool = ...) -> None: ...

class Processor(_message.Message):
    __slots__ = ["configuration_typeurl", "create_time", "custom_processor_source_info", "description", "display_name", "labels", "model_type", "name", "processor_io_spec", "processor_type", "state", "supported_annotation_types", "supports_post_processing", "update_time"]
    class ProcessorState(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = []
    class ProcessorType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = []
    class LabelsEntry(_message.Message):
        __slots__ = ["key", "value"]
        KEY_FIELD_NUMBER: _ClassVar[int]
        VALUE_FIELD_NUMBER: _ClassVar[int]
        key: str
        value: str
        def __init__(self, key: _Optional[str] = ..., value: _Optional[str] = ...) -> None: ...
    ACTIVE: Processor.ProcessorState
    CONFIGURATION_TYPEURL_FIELD_NUMBER: _ClassVar[int]
    CONNECTOR: Processor.ProcessorType
    CREATE_TIME_FIELD_NUMBER: _ClassVar[int]
    CREATING: Processor.ProcessorState
    CUSTOM: Processor.ProcessorType
    CUSTOM_PROCESSOR_SOURCE_INFO_FIELD_NUMBER: _ClassVar[int]
    DELETING: Processor.ProcessorState
    DESCRIPTION_FIELD_NUMBER: _ClassVar[int]
    DISPLAY_NAME_FIELD_NUMBER: _ClassVar[int]
    FAILED: Processor.ProcessorState
    LABELS_FIELD_NUMBER: _ClassVar[int]
    MODEL_TYPE_FIELD_NUMBER: _ClassVar[int]
    NAME_FIELD_NUMBER: _ClassVar[int]
    PRETRAINED: Processor.ProcessorType
    PROCESSOR_IO_SPEC_FIELD_NUMBER: _ClassVar[int]
    PROCESSOR_STATE_UNSPECIFIED: Processor.ProcessorState
    PROCESSOR_TYPE_FIELD_NUMBER: _ClassVar[int]
    PROCESSOR_TYPE_UNSPECIFIED: Processor.ProcessorType
    STATE_FIELD_NUMBER: _ClassVar[int]
    SUPPORTED_ANNOTATION_TYPES_FIELD_NUMBER: _ClassVar[int]
    SUPPORTS_POST_PROCESSING_FIELD_NUMBER: _ClassVar[int]
    UPDATE_TIME_FIELD_NUMBER: _ClassVar[int]
    configuration_typeurl: str
    create_time: _timestamp_pb2.Timestamp
    custom_processor_source_info: CustomProcessorSourceInfo
    description: str
    display_name: str
    labels: _containers.ScalarMap[str, str]
    model_type: ModelType
    name: str
    processor_io_spec: ProcessorIOSpec
    processor_type: Processor.ProcessorType
    state: Processor.ProcessorState
    supported_annotation_types: _containers.RepeatedScalarFieldContainer[_annotations_pb2_1.StreamAnnotationType]
    supports_post_processing: bool
    update_time: _timestamp_pb2.Timestamp
    def __init__(self, name: _Optional[str] = ..., create_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., update_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., labels: _Optional[_Mapping[str, str]] = ..., display_name: _Optional[str] = ..., description: _Optional[str] = ..., processor_type: _Optional[_Union[Processor.ProcessorType, str]] = ..., model_type: _Optional[_Union[ModelType, str]] = ..., custom_processor_source_info: _Optional[_Union[CustomProcessorSourceInfo, _Mapping]] = ..., state: _Optional[_Union[Processor.ProcessorState, str]] = ..., processor_io_spec: _Optional[_Union[ProcessorIOSpec, _Mapping]] = ..., configuration_typeurl: _Optional[str] = ..., supported_annotation_types: _Optional[_Iterable[_Union[_annotations_pb2_1.StreamAnnotationType, str]]] = ..., supports_post_processing: bool = ...) -> None: ...

class ProcessorConfig(_message.Message):
    __slots__ = ["ai_enabled_devices_input_config", "big_query_config", "general_object_detection_config", "media_warehouse_config", "occupancy_count_config", "person_blur_config", "person_vehicle_detection_config", "personal_protective_equipment_detection_config", "vertex_automl_video_config", "vertex_automl_vision_config", "vertex_custom_config", "video_stream_input_config"]
    AI_ENABLED_DEVICES_INPUT_CONFIG_FIELD_NUMBER: _ClassVar[int]
    BIG_QUERY_CONFIG_FIELD_NUMBER: _ClassVar[int]
    GENERAL_OBJECT_DETECTION_CONFIG_FIELD_NUMBER: _ClassVar[int]
    MEDIA_WAREHOUSE_CONFIG_FIELD_NUMBER: _ClassVar[int]
    OCCUPANCY_COUNT_CONFIG_FIELD_NUMBER: _ClassVar[int]
    PERSONAL_PROTECTIVE_EQUIPMENT_DETECTION_CONFIG_FIELD_NUMBER: _ClassVar[int]
    PERSON_BLUR_CONFIG_FIELD_NUMBER: _ClassVar[int]
    PERSON_VEHICLE_DETECTION_CONFIG_FIELD_NUMBER: _ClassVar[int]
    VERTEX_AUTOML_VIDEO_CONFIG_FIELD_NUMBER: _ClassVar[int]
    VERTEX_AUTOML_VISION_CONFIG_FIELD_NUMBER: _ClassVar[int]
    VERTEX_CUSTOM_CONFIG_FIELD_NUMBER: _ClassVar[int]
    VIDEO_STREAM_INPUT_CONFIG_FIELD_NUMBER: _ClassVar[int]
    ai_enabled_devices_input_config: AIEnabledDevicesInputConfig
    big_query_config: BigQueryConfig
    general_object_detection_config: GeneralObjectDetectionConfig
    media_warehouse_config: MediaWarehouseConfig
    occupancy_count_config: OccupancyCountConfig
    person_blur_config: PersonBlurConfig
    person_vehicle_detection_config: PersonVehicleDetectionConfig
    personal_protective_equipment_detection_config: PersonalProtectiveEquipmentDetectionConfig
    vertex_automl_video_config: VertexAutoMLVideoConfig
    vertex_automl_vision_config: VertexAutoMLVisionConfig
    vertex_custom_config: VertexCustomConfig
    video_stream_input_config: VideoStreamInputConfig
    def __init__(self, video_stream_input_config: _Optional[_Union[VideoStreamInputConfig, _Mapping]] = ..., ai_enabled_devices_input_config: _Optional[_Union[AIEnabledDevicesInputConfig, _Mapping]] = ..., media_warehouse_config: _Optional[_Union[MediaWarehouseConfig, _Mapping]] = ..., person_blur_config: _Optional[_Union[PersonBlurConfig, _Mapping]] = ..., occupancy_count_config: _Optional[_Union[OccupancyCountConfig, _Mapping]] = ..., person_vehicle_detection_config: _Optional[_Union[PersonVehicleDetectionConfig, _Mapping]] = ..., vertex_automl_vision_config: _Optional[_Union[VertexAutoMLVisionConfig, _Mapping]] = ..., vertex_automl_video_config: _Optional[_Union[VertexAutoMLVideoConfig, _Mapping]] = ..., vertex_custom_config: _Optional[_Union[VertexCustomConfig, _Mapping]] = ..., general_object_detection_config: _Optional[_Union[GeneralObjectDetectionConfig, _Mapping]] = ..., big_query_config: _Optional[_Union[BigQueryConfig, _Mapping]] = ..., personal_protective_equipment_detection_config: _Optional[_Union[PersonalProtectiveEquipmentDetectionConfig, _Mapping]] = ...) -> None: ...

class ProcessorIOSpec(_message.Message):
    __slots__ = ["graph_input_channel_specs", "graph_output_channel_specs", "instance_resource_input_binding_specs", "instance_resource_output_binding_specs"]
    class DataType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = []
    class GraphInputChannelSpec(_message.Message):
        __slots__ = ["accepted_data_type_uris", "data_type", "max_connection_allowed", "name", "required"]
        ACCEPTED_DATA_TYPE_URIS_FIELD_NUMBER: _ClassVar[int]
        DATA_TYPE_FIELD_NUMBER: _ClassVar[int]
        MAX_CONNECTION_ALLOWED_FIELD_NUMBER: _ClassVar[int]
        NAME_FIELD_NUMBER: _ClassVar[int]
        REQUIRED_FIELD_NUMBER: _ClassVar[int]
        accepted_data_type_uris: _containers.RepeatedScalarFieldContainer[str]
        data_type: ProcessorIOSpec.DataType
        max_connection_allowed: int
        name: str
        required: bool
        def __init__(self, name: _Optional[str] = ..., data_type: _Optional[_Union[ProcessorIOSpec.DataType, str]] = ..., accepted_data_type_uris: _Optional[_Iterable[str]] = ..., required: bool = ..., max_connection_allowed: _Optional[int] = ...) -> None: ...
    class GraphOutputChannelSpec(_message.Message):
        __slots__ = ["data_type", "data_type_uri", "name"]
        DATA_TYPE_FIELD_NUMBER: _ClassVar[int]
        DATA_TYPE_URI_FIELD_NUMBER: _ClassVar[int]
        NAME_FIELD_NUMBER: _ClassVar[int]
        data_type: ProcessorIOSpec.DataType
        data_type_uri: str
        name: str
        def __init__(self, name: _Optional[str] = ..., data_type: _Optional[_Union[ProcessorIOSpec.DataType, str]] = ..., data_type_uri: _Optional[str] = ...) -> None: ...
    class InstanceResourceInputBindingSpec(_message.Message):
        __slots__ = ["config_type_uri", "name", "resource_type_uri"]
        CONFIG_TYPE_URI_FIELD_NUMBER: _ClassVar[int]
        NAME_FIELD_NUMBER: _ClassVar[int]
        RESOURCE_TYPE_URI_FIELD_NUMBER: _ClassVar[int]
        config_type_uri: str
        name: str
        resource_type_uri: str
        def __init__(self, config_type_uri: _Optional[str] = ..., resource_type_uri: _Optional[str] = ..., name: _Optional[str] = ...) -> None: ...
    class InstanceResourceOutputBindingSpec(_message.Message):
        __slots__ = ["explicit", "name", "resource_type_uri"]
        EXPLICIT_FIELD_NUMBER: _ClassVar[int]
        NAME_FIELD_NUMBER: _ClassVar[int]
        RESOURCE_TYPE_URI_FIELD_NUMBER: _ClassVar[int]
        explicit: bool
        name: str
        resource_type_uri: str
        def __init__(self, name: _Optional[str] = ..., resource_type_uri: _Optional[str] = ..., explicit: bool = ...) -> None: ...
    DATA_TYPE_UNSPECIFIED: ProcessorIOSpec.DataType
    GRAPH_INPUT_CHANNEL_SPECS_FIELD_NUMBER: _ClassVar[int]
    GRAPH_OUTPUT_CHANNEL_SPECS_FIELD_NUMBER: _ClassVar[int]
    INSTANCE_RESOURCE_INPUT_BINDING_SPECS_FIELD_NUMBER: _ClassVar[int]
    INSTANCE_RESOURCE_OUTPUT_BINDING_SPECS_FIELD_NUMBER: _ClassVar[int]
    PROTO: ProcessorIOSpec.DataType
    VIDEO: ProcessorIOSpec.DataType
    graph_input_channel_specs: _containers.RepeatedCompositeFieldContainer[ProcessorIOSpec.GraphInputChannelSpec]
    graph_output_channel_specs: _containers.RepeatedCompositeFieldContainer[ProcessorIOSpec.GraphOutputChannelSpec]
    instance_resource_input_binding_specs: _containers.RepeatedCompositeFieldContainer[ProcessorIOSpec.InstanceResourceInputBindingSpec]
    instance_resource_output_binding_specs: _containers.RepeatedCompositeFieldContainer[ProcessorIOSpec.InstanceResourceOutputBindingSpec]
    def __init__(self, graph_input_channel_specs: _Optional[_Iterable[_Union[ProcessorIOSpec.GraphInputChannelSpec, _Mapping]]] = ..., graph_output_channel_specs: _Optional[_Iterable[_Union[ProcessorIOSpec.GraphOutputChannelSpec, _Mapping]]] = ..., instance_resource_input_binding_specs: _Optional[_Iterable[_Union[ProcessorIOSpec.InstanceResourceInputBindingSpec, _Mapping]]] = ..., instance_resource_output_binding_specs: _Optional[_Iterable[_Union[ProcessorIOSpec.InstanceResourceOutputBindingSpec, _Mapping]]] = ...) -> None: ...

class RemoveApplicationStreamInputRequest(_message.Message):
    __slots__ = ["name", "request_id", "target_stream_inputs"]
    class TargetStreamInput(_message.Message):
        __slots__ = ["stream"]
        STREAM_FIELD_NUMBER: _ClassVar[int]
        stream: str
        def __init__(self, stream: _Optional[str] = ...) -> None: ...
    NAME_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    TARGET_STREAM_INPUTS_FIELD_NUMBER: _ClassVar[int]
    name: str
    request_id: str
    target_stream_inputs: _containers.RepeatedCompositeFieldContainer[RemoveApplicationStreamInputRequest.TargetStreamInput]
    def __init__(self, name: _Optional[str] = ..., target_stream_inputs: _Optional[_Iterable[_Union[RemoveApplicationStreamInputRequest.TargetStreamInput, _Mapping]]] = ..., request_id: _Optional[str] = ...) -> None: ...

class RemoveApplicationStreamInputResponse(_message.Message):
    __slots__ = []
    def __init__(self) -> None: ...

class ResourceAnnotations(_message.Message):
    __slots__ = ["application_annotations", "node_annotations"]
    APPLICATION_ANNOTATIONS_FIELD_NUMBER: _ClassVar[int]
    NODE_ANNOTATIONS_FIELD_NUMBER: _ClassVar[int]
    application_annotations: _containers.RepeatedCompositeFieldContainer[_annotations_pb2_1.StreamAnnotation]
    node_annotations: _containers.RepeatedCompositeFieldContainer[ApplicationNodeAnnotation]
    def __init__(self, application_annotations: _Optional[_Iterable[_Union[_annotations_pb2_1.StreamAnnotation, _Mapping]]] = ..., node_annotations: _Optional[_Iterable[_Union[ApplicationNodeAnnotation, _Mapping]]] = ...) -> None: ...

class StreamWithAnnotation(_message.Message):
    __slots__ = ["application_annotations", "node_annotations", "stream"]
    class NodeAnnotation(_message.Message):
        __slots__ = ["annotations", "node"]
        ANNOTATIONS_FIELD_NUMBER: _ClassVar[int]
        NODE_FIELD_NUMBER: _ClassVar[int]
        annotations: _containers.RepeatedCompositeFieldContainer[_annotations_pb2_1.StreamAnnotation]
        node: str
        def __init__(self, node: _Optional[str] = ..., annotations: _Optional[_Iterable[_Union[_annotations_pb2_1.StreamAnnotation, _Mapping]]] = ...) -> None: ...
    APPLICATION_ANNOTATIONS_FIELD_NUMBER: _ClassVar[int]
    NODE_ANNOTATIONS_FIELD_NUMBER: _ClassVar[int]
    STREAM_FIELD_NUMBER: _ClassVar[int]
    application_annotations: _containers.RepeatedCompositeFieldContainer[_annotations_pb2_1.StreamAnnotation]
    node_annotations: _containers.RepeatedCompositeFieldContainer[StreamWithAnnotation.NodeAnnotation]
    stream: str
    def __init__(self, stream: _Optional[str] = ..., application_annotations: _Optional[_Iterable[_Union[_annotations_pb2_1.StreamAnnotation, _Mapping]]] = ..., node_annotations: _Optional[_Iterable[_Union[StreamWithAnnotation.NodeAnnotation, _Mapping]]] = ...) -> None: ...

class UndeployApplicationRequest(_message.Message):
    __slots__ = ["name", "request_id"]
    NAME_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    name: str
    request_id: str
    def __init__(self, name: _Optional[str] = ..., request_id: _Optional[str] = ...) -> None: ...

class UndeployApplicationResponse(_message.Message):
    __slots__ = []
    def __init__(self) -> None: ...

class UpdateApplicationInstancesRequest(_message.Message):
    __slots__ = ["allow_missing", "application_instances", "name", "request_id"]
    class UpdateApplicationInstance(_message.Message):
        __slots__ = ["instance", "instance_id", "update_mask"]
        INSTANCE_FIELD_NUMBER: _ClassVar[int]
        INSTANCE_ID_FIELD_NUMBER: _ClassVar[int]
        UPDATE_MASK_FIELD_NUMBER: _ClassVar[int]
        instance: Instance
        instance_id: str
        update_mask: _field_mask_pb2.FieldMask
        def __init__(self, update_mask: _Optional[_Union[_field_mask_pb2.FieldMask, _Mapping]] = ..., instance: _Optional[_Union[Instance, _Mapping]] = ..., instance_id: _Optional[str] = ...) -> None: ...
    ALLOW_MISSING_FIELD_NUMBER: _ClassVar[int]
    APPLICATION_INSTANCES_FIELD_NUMBER: _ClassVar[int]
    NAME_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    allow_missing: bool
    application_instances: _containers.RepeatedCompositeFieldContainer[UpdateApplicationInstancesRequest.UpdateApplicationInstance]
    name: str
    request_id: str
    def __init__(self, name: _Optional[str] = ..., application_instances: _Optional[_Iterable[_Union[UpdateApplicationInstancesRequest.UpdateApplicationInstance, _Mapping]]] = ..., request_id: _Optional[str] = ..., allow_missing: bool = ...) -> None: ...

class UpdateApplicationInstancesResponse(_message.Message):
    __slots__ = []
    def __init__(self) -> None: ...

class UpdateApplicationRequest(_message.Message):
    __slots__ = ["application", "request_id", "update_mask"]
    APPLICATION_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    UPDATE_MASK_FIELD_NUMBER: _ClassVar[int]
    application: Application
    request_id: str
    update_mask: _field_mask_pb2.FieldMask
    def __init__(self, update_mask: _Optional[_Union[_field_mask_pb2.FieldMask, _Mapping]] = ..., application: _Optional[_Union[Application, _Mapping]] = ..., request_id: _Optional[str] = ...) -> None: ...

class UpdateApplicationStreamInputRequest(_message.Message):
    __slots__ = ["allow_missing", "application_stream_inputs", "name", "request_id"]
    ALLOW_MISSING_FIELD_NUMBER: _ClassVar[int]
    APPLICATION_STREAM_INPUTS_FIELD_NUMBER: _ClassVar[int]
    NAME_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    allow_missing: bool
    application_stream_inputs: _containers.RepeatedCompositeFieldContainer[ApplicationStreamInput]
    name: str
    request_id: str
    def __init__(self, name: _Optional[str] = ..., application_stream_inputs: _Optional[_Iterable[_Union[ApplicationStreamInput, _Mapping]]] = ..., request_id: _Optional[str] = ..., allow_missing: bool = ...) -> None: ...

class UpdateApplicationStreamInputResponse(_message.Message):
    __slots__ = []
    def __init__(self) -> None: ...

class UpdateDraftRequest(_message.Message):
    __slots__ = ["allow_missing", "draft", "request_id", "update_mask"]
    ALLOW_MISSING_FIELD_NUMBER: _ClassVar[int]
    DRAFT_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    UPDATE_MASK_FIELD_NUMBER: _ClassVar[int]
    allow_missing: bool
    draft: Draft
    request_id: str
    update_mask: _field_mask_pb2.FieldMask
    def __init__(self, update_mask: _Optional[_Union[_field_mask_pb2.FieldMask, _Mapping]] = ..., draft: _Optional[_Union[Draft, _Mapping]] = ..., request_id: _Optional[str] = ..., allow_missing: bool = ...) -> None: ...

class UpdateProcessorRequest(_message.Message):
    __slots__ = ["processor", "request_id", "update_mask"]
    PROCESSOR_FIELD_NUMBER: _ClassVar[int]
    REQUEST_ID_FIELD_NUMBER: _ClassVar[int]
    UPDATE_MASK_FIELD_NUMBER: _ClassVar[int]
    processor: Processor
    request_id: str
    update_mask: _field_mask_pb2.FieldMask
    def __init__(self, update_mask: _Optional[_Union[_field_mask_pb2.FieldMask, _Mapping]] = ..., processor: _Optional[_Union[Processor, _Mapping]] = ..., request_id: _Optional[str] = ...) -> None: ...

class VertexAutoMLVideoConfig(_message.Message):
    __slots__ = ["blocked_labels", "bounding_box_size_limit", "confidence_threshold", "max_predictions"]
    BLOCKED_LABELS_FIELD_NUMBER: _ClassVar[int]
    BOUNDING_BOX_SIZE_LIMIT_FIELD_NUMBER: _ClassVar[int]
    CONFIDENCE_THRESHOLD_FIELD_NUMBER: _ClassVar[int]
    MAX_PREDICTIONS_FIELD_NUMBER: _ClassVar[int]
    blocked_labels: _containers.RepeatedScalarFieldContainer[str]
    bounding_box_size_limit: float
    confidence_threshold: float
    max_predictions: int
    def __init__(self, confidence_threshold: _Optional[float] = ..., blocked_labels: _Optional[_Iterable[str]] = ..., max_predictions: _Optional[int] = ..., bounding_box_size_limit: _Optional[float] = ...) -> None: ...

class VertexAutoMLVisionConfig(_message.Message):
    __slots__ = ["confidence_threshold", "max_predictions"]
    CONFIDENCE_THRESHOLD_FIELD_NUMBER: _ClassVar[int]
    MAX_PREDICTIONS_FIELD_NUMBER: _ClassVar[int]
    confidence_threshold: float
    max_predictions: int
    def __init__(self, confidence_threshold: _Optional[float] = ..., max_predictions: _Optional[int] = ...) -> None: ...

class VertexCustomConfig(_message.Message):
    __slots__ = ["attach_application_metadata", "dedicated_resources", "max_prediction_fps", "post_processing_cloud_function"]
    ATTACH_APPLICATION_METADATA_FIELD_NUMBER: _ClassVar[int]
    DEDICATED_RESOURCES_FIELD_NUMBER: _ClassVar[int]
    MAX_PREDICTION_FPS_FIELD_NUMBER: _ClassVar[int]
    POST_PROCESSING_CLOUD_FUNCTION_FIELD_NUMBER: _ClassVar[int]
    attach_application_metadata: bool
    dedicated_resources: DedicatedResources
    max_prediction_fps: int
    post_processing_cloud_function: str
    def __init__(self, max_prediction_fps: _Optional[int] = ..., dedicated_resources: _Optional[_Union[DedicatedResources, _Mapping]] = ..., post_processing_cloud_function: _Optional[str] = ..., attach_application_metadata: bool = ...) -> None: ...

class VideoStreamInputConfig(_message.Message):
    __slots__ = ["streams", "streams_with_annotation"]
    STREAMS_FIELD_NUMBER: _ClassVar[int]
    STREAMS_WITH_ANNOTATION_FIELD_NUMBER: _ClassVar[int]
    streams: _containers.RepeatedScalarFieldContainer[str]
    streams_with_annotation: _containers.RepeatedCompositeFieldContainer[StreamWithAnnotation]
    def __init__(self, streams: _Optional[_Iterable[str]] = ..., streams_with_annotation: _Optional[_Iterable[_Union[StreamWithAnnotation, _Mapping]]] = ...) -> None: ...

class ModelType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []

class AcceleratorType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []
