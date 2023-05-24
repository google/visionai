from google.protobuf import struct_pb2 as _struct_pb2
from google.protobuf import timestamp_pb2 as _timestamp_pb2
from google.protobuf.internal import containers as _containers
from google.protobuf.internal import enum_type_wrapper as _enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Iterable as _Iterable, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor
STREAM_ANNOTATION_TYPE_ACTIVE_ZONE: StreamAnnotationType
STREAM_ANNOTATION_TYPE_CROSSING_LINE: StreamAnnotationType
STREAM_ANNOTATION_TYPE_UNSPECIFIED: StreamAnnotationType

class AppPlatformCloudFunctionRequest(_message.Message):
    __slots__ = ["annotations", "app_platform_metadata"]
    class StructedInputAnnotation(_message.Message):
        __slots__ = ["annotation", "ingestion_time_micros"]
        ANNOTATION_FIELD_NUMBER: _ClassVar[int]
        INGESTION_TIME_MICROS_FIELD_NUMBER: _ClassVar[int]
        annotation: _struct_pb2.Struct
        ingestion_time_micros: int
        def __init__(self, ingestion_time_micros: _Optional[int] = ..., annotation: _Optional[_Union[_struct_pb2.Struct, _Mapping]] = ...) -> None: ...
    ANNOTATIONS_FIELD_NUMBER: _ClassVar[int]
    APP_PLATFORM_METADATA_FIELD_NUMBER: _ClassVar[int]
    annotations: _containers.RepeatedCompositeFieldContainer[AppPlatformCloudFunctionRequest.StructedInputAnnotation]
    app_platform_metadata: AppPlatformMetadata
    def __init__(self, app_platform_metadata: _Optional[_Union[AppPlatformMetadata, _Mapping]] = ..., annotations: _Optional[_Iterable[_Union[AppPlatformCloudFunctionRequest.StructedInputAnnotation, _Mapping]]] = ...) -> None: ...

class AppPlatformCloudFunctionResponse(_message.Message):
    __slots__ = ["annotation_passthrough", "annotations", "events"]
    class StructedOutputAnnotation(_message.Message):
        __slots__ = ["annotation"]
        ANNOTATION_FIELD_NUMBER: _ClassVar[int]
        annotation: _struct_pb2.Struct
        def __init__(self, annotation: _Optional[_Union[_struct_pb2.Struct, _Mapping]] = ...) -> None: ...
    ANNOTATIONS_FIELD_NUMBER: _ClassVar[int]
    ANNOTATION_PASSTHROUGH_FIELD_NUMBER: _ClassVar[int]
    EVENTS_FIELD_NUMBER: _ClassVar[int]
    annotation_passthrough: bool
    annotations: _containers.RepeatedCompositeFieldContainer[AppPlatformCloudFunctionResponse.StructedOutputAnnotation]
    events: _containers.RepeatedCompositeFieldContainer[AppPlatformEventBody]
    def __init__(self, annotations: _Optional[_Iterable[_Union[AppPlatformCloudFunctionResponse.StructedOutputAnnotation, _Mapping]]] = ..., annotation_passthrough: bool = ..., events: _Optional[_Iterable[_Union[AppPlatformEventBody, _Mapping]]] = ...) -> None: ...

class AppPlatformEventBody(_message.Message):
    __slots__ = ["event_id", "event_message", "payload"]
    EVENT_ID_FIELD_NUMBER: _ClassVar[int]
    EVENT_MESSAGE_FIELD_NUMBER: _ClassVar[int]
    PAYLOAD_FIELD_NUMBER: _ClassVar[int]
    event_id: str
    event_message: str
    payload: _struct_pb2.Struct
    def __init__(self, event_message: _Optional[str] = ..., payload: _Optional[_Union[_struct_pb2.Struct, _Mapping]] = ..., event_id: _Optional[str] = ...) -> None: ...

class AppPlatformMetadata(_message.Message):
    __slots__ = ["application", "instance_id", "node", "processor"]
    APPLICATION_FIELD_NUMBER: _ClassVar[int]
    INSTANCE_ID_FIELD_NUMBER: _ClassVar[int]
    NODE_FIELD_NUMBER: _ClassVar[int]
    PROCESSOR_FIELD_NUMBER: _ClassVar[int]
    application: str
    instance_id: str
    node: str
    processor: str
    def __init__(self, application: _Optional[str] = ..., instance_id: _Optional[str] = ..., node: _Optional[str] = ..., processor: _Optional[str] = ...) -> None: ...

class ClassificationPredictionResult(_message.Message):
    __slots__ = ["confidences", "display_names", "ids"]
    CONFIDENCES_FIELD_NUMBER: _ClassVar[int]
    DISPLAY_NAMES_FIELD_NUMBER: _ClassVar[int]
    IDS_FIELD_NUMBER: _ClassVar[int]
    confidences: _containers.RepeatedScalarFieldContainer[float]
    display_names: _containers.RepeatedScalarFieldContainer[str]
    ids: _containers.RepeatedScalarFieldContainer[int]
    def __init__(self, ids: _Optional[_Iterable[int]] = ..., display_names: _Optional[_Iterable[str]] = ..., confidences: _Optional[_Iterable[float]] = ...) -> None: ...

class ImageObjectDetectionPredictionResult(_message.Message):
    __slots__ = ["bboxes", "confidences", "display_names", "ids"]
    BBOXES_FIELD_NUMBER: _ClassVar[int]
    CONFIDENCES_FIELD_NUMBER: _ClassVar[int]
    DISPLAY_NAMES_FIELD_NUMBER: _ClassVar[int]
    IDS_FIELD_NUMBER: _ClassVar[int]
    bboxes: _containers.RepeatedCompositeFieldContainer[_struct_pb2.ListValue]
    confidences: _containers.RepeatedScalarFieldContainer[float]
    display_names: _containers.RepeatedScalarFieldContainer[str]
    ids: _containers.RepeatedScalarFieldContainer[int]
    def __init__(self, ids: _Optional[_Iterable[int]] = ..., display_names: _Optional[_Iterable[str]] = ..., confidences: _Optional[_Iterable[float]] = ..., bboxes: _Optional[_Iterable[_Union[_struct_pb2.ListValue, _Mapping]]] = ...) -> None: ...

class ImageSegmentationPredictionResult(_message.Message):
    __slots__ = ["category_mask", "confidence_mask"]
    CATEGORY_MASK_FIELD_NUMBER: _ClassVar[int]
    CONFIDENCE_MASK_FIELD_NUMBER: _ClassVar[int]
    category_mask: str
    confidence_mask: str
    def __init__(self, category_mask: _Optional[str] = ..., confidence_mask: _Optional[str] = ...) -> None: ...

class NormalizedPolygon(_message.Message):
    __slots__ = ["normalized_vertices"]
    NORMALIZED_VERTICES_FIELD_NUMBER: _ClassVar[int]
    normalized_vertices: _containers.RepeatedCompositeFieldContainer[NormalizedVertex]
    def __init__(self, normalized_vertices: _Optional[_Iterable[_Union[NormalizedVertex, _Mapping]]] = ...) -> None: ...

class NormalizedPolyline(_message.Message):
    __slots__ = ["normalized_vertices"]
    NORMALIZED_VERTICES_FIELD_NUMBER: _ClassVar[int]
    normalized_vertices: _containers.RepeatedCompositeFieldContainer[NormalizedVertex]
    def __init__(self, normalized_vertices: _Optional[_Iterable[_Union[NormalizedVertex, _Mapping]]] = ...) -> None: ...

class NormalizedVertex(_message.Message):
    __slots__ = ["x", "y"]
    X_FIELD_NUMBER: _ClassVar[int]
    Y_FIELD_NUMBER: _ClassVar[int]
    x: float
    y: float
    def __init__(self, x: _Optional[float] = ..., y: _Optional[float] = ...) -> None: ...

class ObjectDetectionPredictionResult(_message.Message):
    __slots__ = ["current_time", "identified_boxes"]
    class Entity(_message.Message):
        __slots__ = ["label_id", "label_string"]
        LABEL_ID_FIELD_NUMBER: _ClassVar[int]
        LABEL_STRING_FIELD_NUMBER: _ClassVar[int]
        label_id: int
        label_string: str
        def __init__(self, label_id: _Optional[int] = ..., label_string: _Optional[str] = ...) -> None: ...
    class IdentifiedBox(_message.Message):
        __slots__ = ["box_id", "confidence_score", "entity", "normalized_bounding_box"]
        class NormalizedBoundingBox(_message.Message):
            __slots__ = ["height", "width", "xmin", "ymin"]
            HEIGHT_FIELD_NUMBER: _ClassVar[int]
            WIDTH_FIELD_NUMBER: _ClassVar[int]
            XMIN_FIELD_NUMBER: _ClassVar[int]
            YMIN_FIELD_NUMBER: _ClassVar[int]
            height: float
            width: float
            xmin: float
            ymin: float
            def __init__(self, xmin: _Optional[float] = ..., ymin: _Optional[float] = ..., width: _Optional[float] = ..., height: _Optional[float] = ...) -> None: ...
        BOX_ID_FIELD_NUMBER: _ClassVar[int]
        CONFIDENCE_SCORE_FIELD_NUMBER: _ClassVar[int]
        ENTITY_FIELD_NUMBER: _ClassVar[int]
        NORMALIZED_BOUNDING_BOX_FIELD_NUMBER: _ClassVar[int]
        box_id: int
        confidence_score: float
        entity: ObjectDetectionPredictionResult.Entity
        normalized_bounding_box: ObjectDetectionPredictionResult.IdentifiedBox.NormalizedBoundingBox
        def __init__(self, box_id: _Optional[int] = ..., normalized_bounding_box: _Optional[_Union[ObjectDetectionPredictionResult.IdentifiedBox.NormalizedBoundingBox, _Mapping]] = ..., confidence_score: _Optional[float] = ..., entity: _Optional[_Union[ObjectDetectionPredictionResult.Entity, _Mapping]] = ...) -> None: ...
    CURRENT_TIME_FIELD_NUMBER: _ClassVar[int]
    IDENTIFIED_BOXES_FIELD_NUMBER: _ClassVar[int]
    current_time: _timestamp_pb2.Timestamp
    identified_boxes: _containers.RepeatedCompositeFieldContainer[ObjectDetectionPredictionResult.IdentifiedBox]
    def __init__(self, current_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., identified_boxes: _Optional[_Iterable[_Union[ObjectDetectionPredictionResult.IdentifiedBox, _Mapping]]] = ...) -> None: ...

class OccupancyCountingPredictionResult(_message.Message):
    __slots__ = ["current_time", "dwell_time_info", "identified_boxes", "stats", "track_info"]
    class DwellTimeInfo(_message.Message):
        __slots__ = ["dwell_end_time", "dwell_start_time", "track_id", "zone_id"]
        DWELL_END_TIME_FIELD_NUMBER: _ClassVar[int]
        DWELL_START_TIME_FIELD_NUMBER: _ClassVar[int]
        TRACK_ID_FIELD_NUMBER: _ClassVar[int]
        ZONE_ID_FIELD_NUMBER: _ClassVar[int]
        dwell_end_time: _timestamp_pb2.Timestamp
        dwell_start_time: _timestamp_pb2.Timestamp
        track_id: str
        zone_id: str
        def __init__(self, track_id: _Optional[str] = ..., zone_id: _Optional[str] = ..., dwell_start_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., dwell_end_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...
    class Entity(_message.Message):
        __slots__ = ["label_id", "label_string"]
        LABEL_ID_FIELD_NUMBER: _ClassVar[int]
        LABEL_STRING_FIELD_NUMBER: _ClassVar[int]
        label_id: int
        label_string: str
        def __init__(self, label_id: _Optional[int] = ..., label_string: _Optional[str] = ...) -> None: ...
    class IdentifiedBox(_message.Message):
        __slots__ = ["box_id", "entity", "normalized_bounding_box", "score", "track_id"]
        class NormalizedBoundingBox(_message.Message):
            __slots__ = ["height", "width", "xmin", "ymin"]
            HEIGHT_FIELD_NUMBER: _ClassVar[int]
            WIDTH_FIELD_NUMBER: _ClassVar[int]
            XMIN_FIELD_NUMBER: _ClassVar[int]
            YMIN_FIELD_NUMBER: _ClassVar[int]
            height: float
            width: float
            xmin: float
            ymin: float
            def __init__(self, xmin: _Optional[float] = ..., ymin: _Optional[float] = ..., width: _Optional[float] = ..., height: _Optional[float] = ...) -> None: ...
        BOX_ID_FIELD_NUMBER: _ClassVar[int]
        ENTITY_FIELD_NUMBER: _ClassVar[int]
        NORMALIZED_BOUNDING_BOX_FIELD_NUMBER: _ClassVar[int]
        SCORE_FIELD_NUMBER: _ClassVar[int]
        TRACK_ID_FIELD_NUMBER: _ClassVar[int]
        box_id: int
        entity: OccupancyCountingPredictionResult.Entity
        normalized_bounding_box: OccupancyCountingPredictionResult.IdentifiedBox.NormalizedBoundingBox
        score: float
        track_id: int
        def __init__(self, box_id: _Optional[int] = ..., normalized_bounding_box: _Optional[_Union[OccupancyCountingPredictionResult.IdentifiedBox.NormalizedBoundingBox, _Mapping]] = ..., score: _Optional[float] = ..., entity: _Optional[_Union[OccupancyCountingPredictionResult.Entity, _Mapping]] = ..., track_id: _Optional[int] = ...) -> None: ...
    class Stats(_message.Message):
        __slots__ = ["active_zone_counts", "crossing_line_counts", "full_frame_count"]
        class AccumulatedObjectCount(_message.Message):
            __slots__ = ["object_count", "start_time"]
            OBJECT_COUNT_FIELD_NUMBER: _ClassVar[int]
            START_TIME_FIELD_NUMBER: _ClassVar[int]
            object_count: OccupancyCountingPredictionResult.Stats.ObjectCount
            start_time: _timestamp_pb2.Timestamp
            def __init__(self, start_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., object_count: _Optional[_Union[OccupancyCountingPredictionResult.Stats.ObjectCount, _Mapping]] = ...) -> None: ...
        class ActiveZoneCount(_message.Message):
            __slots__ = ["annotation", "counts"]
            ANNOTATION_FIELD_NUMBER: _ClassVar[int]
            COUNTS_FIELD_NUMBER: _ClassVar[int]
            annotation: StreamAnnotation
            counts: _containers.RepeatedCompositeFieldContainer[OccupancyCountingPredictionResult.Stats.ObjectCount]
            def __init__(self, annotation: _Optional[_Union[StreamAnnotation, _Mapping]] = ..., counts: _Optional[_Iterable[_Union[OccupancyCountingPredictionResult.Stats.ObjectCount, _Mapping]]] = ...) -> None: ...
        class CrossingLineCount(_message.Message):
            __slots__ = ["accumulated_negative_direction_counts", "accumulated_positive_direction_counts", "annotation", "negative_direction_counts", "positive_direction_counts"]
            ACCUMULATED_NEGATIVE_DIRECTION_COUNTS_FIELD_NUMBER: _ClassVar[int]
            ACCUMULATED_POSITIVE_DIRECTION_COUNTS_FIELD_NUMBER: _ClassVar[int]
            ANNOTATION_FIELD_NUMBER: _ClassVar[int]
            NEGATIVE_DIRECTION_COUNTS_FIELD_NUMBER: _ClassVar[int]
            POSITIVE_DIRECTION_COUNTS_FIELD_NUMBER: _ClassVar[int]
            accumulated_negative_direction_counts: _containers.RepeatedCompositeFieldContainer[OccupancyCountingPredictionResult.Stats.AccumulatedObjectCount]
            accumulated_positive_direction_counts: _containers.RepeatedCompositeFieldContainer[OccupancyCountingPredictionResult.Stats.AccumulatedObjectCount]
            annotation: StreamAnnotation
            negative_direction_counts: _containers.RepeatedCompositeFieldContainer[OccupancyCountingPredictionResult.Stats.ObjectCount]
            positive_direction_counts: _containers.RepeatedCompositeFieldContainer[OccupancyCountingPredictionResult.Stats.ObjectCount]
            def __init__(self, annotation: _Optional[_Union[StreamAnnotation, _Mapping]] = ..., positive_direction_counts: _Optional[_Iterable[_Union[OccupancyCountingPredictionResult.Stats.ObjectCount, _Mapping]]] = ..., negative_direction_counts: _Optional[_Iterable[_Union[OccupancyCountingPredictionResult.Stats.ObjectCount, _Mapping]]] = ..., accumulated_positive_direction_counts: _Optional[_Iterable[_Union[OccupancyCountingPredictionResult.Stats.AccumulatedObjectCount, _Mapping]]] = ..., accumulated_negative_direction_counts: _Optional[_Iterable[_Union[OccupancyCountingPredictionResult.Stats.AccumulatedObjectCount, _Mapping]]] = ...) -> None: ...
        class ObjectCount(_message.Message):
            __slots__ = ["count", "entity"]
            COUNT_FIELD_NUMBER: _ClassVar[int]
            ENTITY_FIELD_NUMBER: _ClassVar[int]
            count: int
            entity: OccupancyCountingPredictionResult.Entity
            def __init__(self, entity: _Optional[_Union[OccupancyCountingPredictionResult.Entity, _Mapping]] = ..., count: _Optional[int] = ...) -> None: ...
        ACTIVE_ZONE_COUNTS_FIELD_NUMBER: _ClassVar[int]
        CROSSING_LINE_COUNTS_FIELD_NUMBER: _ClassVar[int]
        FULL_FRAME_COUNT_FIELD_NUMBER: _ClassVar[int]
        active_zone_counts: _containers.RepeatedCompositeFieldContainer[OccupancyCountingPredictionResult.Stats.ActiveZoneCount]
        crossing_line_counts: _containers.RepeatedCompositeFieldContainer[OccupancyCountingPredictionResult.Stats.CrossingLineCount]
        full_frame_count: _containers.RepeatedCompositeFieldContainer[OccupancyCountingPredictionResult.Stats.ObjectCount]
        def __init__(self, full_frame_count: _Optional[_Iterable[_Union[OccupancyCountingPredictionResult.Stats.ObjectCount, _Mapping]]] = ..., crossing_line_counts: _Optional[_Iterable[_Union[OccupancyCountingPredictionResult.Stats.CrossingLineCount, _Mapping]]] = ..., active_zone_counts: _Optional[_Iterable[_Union[OccupancyCountingPredictionResult.Stats.ActiveZoneCount, _Mapping]]] = ...) -> None: ...
    class TrackInfo(_message.Message):
        __slots__ = ["start_time", "track_id"]
        START_TIME_FIELD_NUMBER: _ClassVar[int]
        TRACK_ID_FIELD_NUMBER: _ClassVar[int]
        start_time: _timestamp_pb2.Timestamp
        track_id: str
        def __init__(self, track_id: _Optional[str] = ..., start_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...
    CURRENT_TIME_FIELD_NUMBER: _ClassVar[int]
    DWELL_TIME_INFO_FIELD_NUMBER: _ClassVar[int]
    IDENTIFIED_BOXES_FIELD_NUMBER: _ClassVar[int]
    STATS_FIELD_NUMBER: _ClassVar[int]
    TRACK_INFO_FIELD_NUMBER: _ClassVar[int]
    current_time: _timestamp_pb2.Timestamp
    dwell_time_info: _containers.RepeatedCompositeFieldContainer[OccupancyCountingPredictionResult.DwellTimeInfo]
    identified_boxes: _containers.RepeatedCompositeFieldContainer[OccupancyCountingPredictionResult.IdentifiedBox]
    stats: OccupancyCountingPredictionResult.Stats
    track_info: _containers.RepeatedCompositeFieldContainer[OccupancyCountingPredictionResult.TrackInfo]
    def __init__(self, current_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., identified_boxes: _Optional[_Iterable[_Union[OccupancyCountingPredictionResult.IdentifiedBox, _Mapping]]] = ..., stats: _Optional[_Union[OccupancyCountingPredictionResult.Stats, _Mapping]] = ..., track_info: _Optional[_Iterable[_Union[OccupancyCountingPredictionResult.TrackInfo, _Mapping]]] = ..., dwell_time_info: _Optional[_Iterable[_Union[OccupancyCountingPredictionResult.DwellTimeInfo, _Mapping]]] = ...) -> None: ...

class PersonalProtectiveEquipmentDetectionOutput(_message.Message):
    __slots__ = ["current_time", "detected_persons"]
    class DetectedPerson(_message.Message):
        __slots__ = ["body_coverage_score", "detected_person_identified_box", "detected_ppe_identified_boxes", "eyes_coverage_score", "face_coverage_score", "feet_coverage_score", "hands_coverage_score", "head_coverage_score", "person_id"]
        BODY_COVERAGE_SCORE_FIELD_NUMBER: _ClassVar[int]
        DETECTED_PERSON_IDENTIFIED_BOX_FIELD_NUMBER: _ClassVar[int]
        DETECTED_PPE_IDENTIFIED_BOXES_FIELD_NUMBER: _ClassVar[int]
        EYES_COVERAGE_SCORE_FIELD_NUMBER: _ClassVar[int]
        FACE_COVERAGE_SCORE_FIELD_NUMBER: _ClassVar[int]
        FEET_COVERAGE_SCORE_FIELD_NUMBER: _ClassVar[int]
        HANDS_COVERAGE_SCORE_FIELD_NUMBER: _ClassVar[int]
        HEAD_COVERAGE_SCORE_FIELD_NUMBER: _ClassVar[int]
        PERSON_ID_FIELD_NUMBER: _ClassVar[int]
        body_coverage_score: float
        detected_person_identified_box: PersonalProtectiveEquipmentDetectionOutput.PersonIdentifiedBox
        detected_ppe_identified_boxes: _containers.RepeatedCompositeFieldContainer[PersonalProtectiveEquipmentDetectionOutput.PPEIdentifiedBox]
        eyes_coverage_score: float
        face_coverage_score: float
        feet_coverage_score: float
        hands_coverage_score: float
        head_coverage_score: float
        person_id: int
        def __init__(self, person_id: _Optional[int] = ..., detected_person_identified_box: _Optional[_Union[PersonalProtectiveEquipmentDetectionOutput.PersonIdentifiedBox, _Mapping]] = ..., detected_ppe_identified_boxes: _Optional[_Iterable[_Union[PersonalProtectiveEquipmentDetectionOutput.PPEIdentifiedBox, _Mapping]]] = ..., face_coverage_score: _Optional[float] = ..., eyes_coverage_score: _Optional[float] = ..., head_coverage_score: _Optional[float] = ..., hands_coverage_score: _Optional[float] = ..., body_coverage_score: _Optional[float] = ..., feet_coverage_score: _Optional[float] = ...) -> None: ...
    class NormalizedBoundingBox(_message.Message):
        __slots__ = ["height", "width", "xmin", "ymin"]
        HEIGHT_FIELD_NUMBER: _ClassVar[int]
        WIDTH_FIELD_NUMBER: _ClassVar[int]
        XMIN_FIELD_NUMBER: _ClassVar[int]
        YMIN_FIELD_NUMBER: _ClassVar[int]
        height: float
        width: float
        xmin: float
        ymin: float
        def __init__(self, xmin: _Optional[float] = ..., ymin: _Optional[float] = ..., width: _Optional[float] = ..., height: _Optional[float] = ...) -> None: ...
    class PPEEntity(_message.Message):
        __slots__ = ["ppe_entity_id", "ppe_label_id", "ppe_label_string", "ppe_supercategory_label_string"]
        PPE_ENTITY_ID_FIELD_NUMBER: _ClassVar[int]
        PPE_LABEL_ID_FIELD_NUMBER: _ClassVar[int]
        PPE_LABEL_STRING_FIELD_NUMBER: _ClassVar[int]
        PPE_SUPERCATEGORY_LABEL_STRING_FIELD_NUMBER: _ClassVar[int]
        ppe_entity_id: int
        ppe_label_id: int
        ppe_label_string: str
        ppe_supercategory_label_string: str
        def __init__(self, ppe_label_id: _Optional[int] = ..., ppe_label_string: _Optional[str] = ..., ppe_supercategory_label_string: _Optional[str] = ..., ppe_entity_id: _Optional[int] = ...) -> None: ...
    class PPEIdentifiedBox(_message.Message):
        __slots__ = ["box_id", "confidence_score", "normalized_bounding_box", "ppe_entity"]
        BOX_ID_FIELD_NUMBER: _ClassVar[int]
        CONFIDENCE_SCORE_FIELD_NUMBER: _ClassVar[int]
        NORMALIZED_BOUNDING_BOX_FIELD_NUMBER: _ClassVar[int]
        PPE_ENTITY_FIELD_NUMBER: _ClassVar[int]
        box_id: int
        confidence_score: float
        normalized_bounding_box: PersonalProtectiveEquipmentDetectionOutput.NormalizedBoundingBox
        ppe_entity: PersonalProtectiveEquipmentDetectionOutput.PPEEntity
        def __init__(self, box_id: _Optional[int] = ..., normalized_bounding_box: _Optional[_Union[PersonalProtectiveEquipmentDetectionOutput.NormalizedBoundingBox, _Mapping]] = ..., confidence_score: _Optional[float] = ..., ppe_entity: _Optional[_Union[PersonalProtectiveEquipmentDetectionOutput.PPEEntity, _Mapping]] = ...) -> None: ...
    class PersonEntity(_message.Message):
        __slots__ = ["person_entity_id"]
        PERSON_ENTITY_ID_FIELD_NUMBER: _ClassVar[int]
        person_entity_id: int
        def __init__(self, person_entity_id: _Optional[int] = ...) -> None: ...
    class PersonIdentifiedBox(_message.Message):
        __slots__ = ["box_id", "confidence_score", "normalized_bounding_box", "person_entity"]
        BOX_ID_FIELD_NUMBER: _ClassVar[int]
        CONFIDENCE_SCORE_FIELD_NUMBER: _ClassVar[int]
        NORMALIZED_BOUNDING_BOX_FIELD_NUMBER: _ClassVar[int]
        PERSON_ENTITY_FIELD_NUMBER: _ClassVar[int]
        box_id: int
        confidence_score: float
        normalized_bounding_box: PersonalProtectiveEquipmentDetectionOutput.NormalizedBoundingBox
        person_entity: PersonalProtectiveEquipmentDetectionOutput.PersonEntity
        def __init__(self, box_id: _Optional[int] = ..., normalized_bounding_box: _Optional[_Union[PersonalProtectiveEquipmentDetectionOutput.NormalizedBoundingBox, _Mapping]] = ..., confidence_score: _Optional[float] = ..., person_entity: _Optional[_Union[PersonalProtectiveEquipmentDetectionOutput.PersonEntity, _Mapping]] = ...) -> None: ...
    CURRENT_TIME_FIELD_NUMBER: _ClassVar[int]
    DETECTED_PERSONS_FIELD_NUMBER: _ClassVar[int]
    current_time: _timestamp_pb2.Timestamp
    detected_persons: _containers.RepeatedCompositeFieldContainer[PersonalProtectiveEquipmentDetectionOutput.DetectedPerson]
    def __init__(self, current_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., detected_persons: _Optional[_Iterable[_Union[PersonalProtectiveEquipmentDetectionOutput.DetectedPerson, _Mapping]]] = ...) -> None: ...

class StreamAnnotation(_message.Message):
    __slots__ = ["active_zone", "crossing_line", "display_name", "id", "source_stream", "type"]
    ACTIVE_ZONE_FIELD_NUMBER: _ClassVar[int]
    CROSSING_LINE_FIELD_NUMBER: _ClassVar[int]
    DISPLAY_NAME_FIELD_NUMBER: _ClassVar[int]
    ID_FIELD_NUMBER: _ClassVar[int]
    SOURCE_STREAM_FIELD_NUMBER: _ClassVar[int]
    TYPE_FIELD_NUMBER: _ClassVar[int]
    active_zone: NormalizedPolygon
    crossing_line: NormalizedPolyline
    display_name: str
    id: str
    source_stream: str
    type: StreamAnnotationType
    def __init__(self, active_zone: _Optional[_Union[NormalizedPolygon, _Mapping]] = ..., crossing_line: _Optional[_Union[NormalizedPolyline, _Mapping]] = ..., id: _Optional[str] = ..., display_name: _Optional[str] = ..., source_stream: _Optional[str] = ..., type: _Optional[_Union[StreamAnnotationType, str]] = ...) -> None: ...

class StreamAnnotations(_message.Message):
    __slots__ = ["stream_annotations"]
    STREAM_ANNOTATIONS_FIELD_NUMBER: _ClassVar[int]
    stream_annotations: _containers.RepeatedCompositeFieldContainer[StreamAnnotation]
    def __init__(self, stream_annotations: _Optional[_Iterable[_Union[StreamAnnotation, _Mapping]]] = ...) -> None: ...

class VideoActionRecognitionPredictionResult(_message.Message):
    __slots__ = ["actions", "segment_end_time", "segment_start_time"]
    class IdentifiedAction(_message.Message):
        __slots__ = ["confidence", "display_name", "id"]
        CONFIDENCE_FIELD_NUMBER: _ClassVar[int]
        DISPLAY_NAME_FIELD_NUMBER: _ClassVar[int]
        ID_FIELD_NUMBER: _ClassVar[int]
        confidence: float
        display_name: str
        id: str
        def __init__(self, id: _Optional[str] = ..., display_name: _Optional[str] = ..., confidence: _Optional[float] = ...) -> None: ...
    ACTIONS_FIELD_NUMBER: _ClassVar[int]
    SEGMENT_END_TIME_FIELD_NUMBER: _ClassVar[int]
    SEGMENT_START_TIME_FIELD_NUMBER: _ClassVar[int]
    actions: _containers.RepeatedCompositeFieldContainer[VideoActionRecognitionPredictionResult.IdentifiedAction]
    segment_end_time: _timestamp_pb2.Timestamp
    segment_start_time: _timestamp_pb2.Timestamp
    def __init__(self, segment_start_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., segment_end_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., actions: _Optional[_Iterable[_Union[VideoActionRecognitionPredictionResult.IdentifiedAction, _Mapping]]] = ...) -> None: ...

class VideoClassificationPredictionResult(_message.Message):
    __slots__ = ["classifications", "segment_end_time", "segment_start_time"]
    class IdentifiedClassification(_message.Message):
        __slots__ = ["confidence", "display_name", "id"]
        CONFIDENCE_FIELD_NUMBER: _ClassVar[int]
        DISPLAY_NAME_FIELD_NUMBER: _ClassVar[int]
        ID_FIELD_NUMBER: _ClassVar[int]
        confidence: float
        display_name: str
        id: str
        def __init__(self, id: _Optional[str] = ..., display_name: _Optional[str] = ..., confidence: _Optional[float] = ...) -> None: ...
    CLASSIFICATIONS_FIELD_NUMBER: _ClassVar[int]
    SEGMENT_END_TIME_FIELD_NUMBER: _ClassVar[int]
    SEGMENT_START_TIME_FIELD_NUMBER: _ClassVar[int]
    classifications: _containers.RepeatedCompositeFieldContainer[VideoClassificationPredictionResult.IdentifiedClassification]
    segment_end_time: _timestamp_pb2.Timestamp
    segment_start_time: _timestamp_pb2.Timestamp
    def __init__(self, segment_start_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., segment_end_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., classifications: _Optional[_Iterable[_Union[VideoClassificationPredictionResult.IdentifiedClassification, _Mapping]]] = ...) -> None: ...

class VideoObjectTrackingPredictionResult(_message.Message):
    __slots__ = ["objects", "segment_end_time", "segment_start_time"]
    class BoundingBox(_message.Message):
        __slots__ = ["x_max", "x_min", "y_max", "y_min"]
        X_MAX_FIELD_NUMBER: _ClassVar[int]
        X_MIN_FIELD_NUMBER: _ClassVar[int]
        Y_MAX_FIELD_NUMBER: _ClassVar[int]
        Y_MIN_FIELD_NUMBER: _ClassVar[int]
        x_max: float
        x_min: float
        y_max: float
        y_min: float
        def __init__(self, x_min: _Optional[float] = ..., x_max: _Optional[float] = ..., y_min: _Optional[float] = ..., y_max: _Optional[float] = ...) -> None: ...
    class DetectedObject(_message.Message):
        __slots__ = ["bounding_box", "confidence", "display_name", "id", "track_id"]
        BOUNDING_BOX_FIELD_NUMBER: _ClassVar[int]
        CONFIDENCE_FIELD_NUMBER: _ClassVar[int]
        DISPLAY_NAME_FIELD_NUMBER: _ClassVar[int]
        ID_FIELD_NUMBER: _ClassVar[int]
        TRACK_ID_FIELD_NUMBER: _ClassVar[int]
        bounding_box: VideoObjectTrackingPredictionResult.BoundingBox
        confidence: float
        display_name: str
        id: str
        track_id: int
        def __init__(self, id: _Optional[str] = ..., display_name: _Optional[str] = ..., bounding_box: _Optional[_Union[VideoObjectTrackingPredictionResult.BoundingBox, _Mapping]] = ..., confidence: _Optional[float] = ..., track_id: _Optional[int] = ...) -> None: ...
    OBJECTS_FIELD_NUMBER: _ClassVar[int]
    SEGMENT_END_TIME_FIELD_NUMBER: _ClassVar[int]
    SEGMENT_START_TIME_FIELD_NUMBER: _ClassVar[int]
    objects: _containers.RepeatedCompositeFieldContainer[VideoObjectTrackingPredictionResult.DetectedObject]
    segment_end_time: _timestamp_pb2.Timestamp
    segment_start_time: _timestamp_pb2.Timestamp
    def __init__(self, segment_start_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., segment_end_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., objects: _Optional[_Iterable[_Union[VideoObjectTrackingPredictionResult.DetectedObject, _Mapping]]] = ...) -> None: ...

class StreamAnnotationType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []
