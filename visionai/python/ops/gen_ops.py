# Copyright 2023 Google LLC
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bs

# Python wrappers around operators
# Generated - do not directly modify.

from visionai.python.lva import graph


def automl_video_action_recognition(
    video,
    model_id="",
    model_uri="",
    graph_path="/google/inference_graph_action_recognition_streaming_edge.pbtxt",
    confidence_threshold=0,
    max_predictions=0,
    name="",
    g=None,
):
  """TODO Fill short description for "AutomlVideoActionRecognition".

  TODO Fill long description for "AutomlVideoActionRecognition".

  Args:
    video: A `Port` that produce packets of type "gst/video".
    model_id: An attribute of type `string`. Defaults to ''.
    model_uri: An attribute of type `string`. Defaults to ''.
    graph_path: An attribute of type `string`. Defaults to
      '/google/inference_graph_action_recognition_streaming_edge.pbtxt'.
    confidence_threshold: An attribute of type `float`. Defaults to 0.
    max_predictions: An attribute of type `int`. Defaults to 0.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type
    "protobuf/google.cloud.visionai.v1.VideoActionRecognitionPredictionResult".
  """
  n = graph.Node(
      name=name,
      operator="AutomlVideoActionRecognition",
      input_ports=["video"],
      output_ports=["prediction_result"],
      attributes={
          "model_id": model_id,
          "model_uri": model_uri,
          "graph_path": graph_path,
          "confidence_threshold": confidence_threshold,
          "max_predictions": max_predictions,
      },
  )
  g.add_node(n)
  g.add_edge(video, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def automl_video_classification(
    video,
    model_id="",
    model_uri="",
    graph_path="/google/inference_graph_classification_streaming_edge.pbtxt",
    confidence_threshold=0,
    max_predictions=0,
    name="",
    g=None,
):
  """TODO Fill short description for "AutomlVideoClassification".

  TODO Fill long description for "AutomlVideoClassification".

  Args:
    video: A `Port` that produce packets of type "gst/video".
    model_id: An attribute of type `string`. Defaults to ''.
    model_uri: An attribute of type `string`. Defaults to ''.
    graph_path: An attribute of type `string`. Defaults to
      '/google/inference_graph_classification_streaming_edge.pbtxt'.
    confidence_threshold: An attribute of type `float`. Defaults to 0.
    max_predictions: An attribute of type `int`. Defaults to 0.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type
    "protobuf/google.cloud.visionai.v1.VideoClassificationPredictionResult".
  """
  n = graph.Node(
      name=name,
      operator="AutomlVideoClassification",
      input_ports=["video"],
      output_ports=["prediction_result"],
      attributes={
          "model_id": model_id,
          "model_uri": model_uri,
          "graph_path": graph_path,
          "confidence_threshold": confidence_threshold,
          "max_predictions": max_predictions,
      },
  )
  g.add_node(n)
  g.add_edge(video, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def automl_video_object_tracking(
    video,
    model_id="",
    model_uri="",
    graph_path="/google/inference_graph_object_tracking_streaming_edge.pbtxt",
    confidence_threshold=0,
    max_predictions=0,
    min_bounding_box_size=0,
    name="",
    g=None,
):
  """TODO Fill short description for "AutomlVideoObjectTracking".

  TODO Fill long description for "AutomlVideoObjectTracking".

  Args:
    video: A `Port` that produce packets of type "gst/video".
    model_id: An attribute of type `string`. Defaults to ''.
    model_uri: An attribute of type `string`. Defaults to ''.
    graph_path: An attribute of type `string`. Defaults to
      '/google/inference_graph_object_tracking_streaming_edge.pbtxt'.
    confidence_threshold: An attribute of type `float`. Defaults to 0.
    max_predictions: An attribute of type `int`. Defaults to 0.
    min_bounding_box_size: An attribute of type `float`. Defaults to 0.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type
    "protobuf/google.cloud.visionai.v1.VideoObjectTrackingPredictionResult".
  """
  n = graph.Node(
      name=name,
      operator="AutomlVideoObjectTracking",
      input_ports=["video"],
      output_ports=["prediction_result"],
      attributes={
          "model_id": model_id,
          "model_uri": model_uri,
          "graph_path": graph_path,
          "confidence_threshold": confidence_threshold,
          "max_predictions": max_predictions,
          "min_bounding_box_size": min_bounding_box_size,
      },
  )
  g.add_node(n)
  g.add_edge(video, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def automl_vision_classification(
    video,
    model_gcs_path="",
    confidence_threshold=0,
    max_predictions=0,
    model_id="",
    name="",
    g=None,
):
  """TODO Fill short description for "AutomlVisionClassification".

  TODO Fill long description for "AutomlVisionClassification".

  Args:
    video: A `Port` that produce packets of type "gst/video".
    model_gcs_path: An attribute of type `string`. Defaults to ''.
    confidence_threshold: An attribute of type `float`. Defaults to 0.
    max_predictions: An attribute of type `int`. Defaults to 0.
    model_id: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type
    "protobuf/google.cloud.visionai.v1.ClassificationPredictionResult".
  """
  n = graph.Node(
      name=name,
      operator="AutomlVisionClassification",
      input_ports=["video"],
      output_ports=["prediction_result"],
      attributes={
          "model_gcs_path": model_gcs_path,
          "confidence_threshold": confidence_threshold,
          "max_predictions": max_predictions,
          "model_id": model_id,
      },
  )
  g.add_node(n)
  g.add_edge(video, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def automl_vision_object_detection(
    video,
    model_gcs_path="",
    confidence_threshold=0,
    max_predictions=0,
    model_id="",
    name="",
    g=None,
):
  """TODO Fill short description for "AutomlVisionObjectDetection".

  TODO Fill long description for "AutomlVisionObjectDetection".

  Args:
    video: A `Port` that produce packets of type "gst/video".
    model_gcs_path: An attribute of type `string`. Defaults to ''.
    confidence_threshold: An attribute of type `float`. Defaults to 0.
    max_predictions: An attribute of type `int`. Defaults to 0.
    model_id: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type
    "protobuf/google.cloud.visionai.v1.ImageObjectDetectionPredictionResult".
  """
  n = graph.Node(
      name=name,
      operator="AutomlVisionObjectDetection",
      input_ports=["video"],
      output_ports=["prediction_result"],
      attributes={
          "model_gcs_path": model_gcs_path,
          "confidence_threshold": confidence_threshold,
          "max_predictions": max_predictions,
          "model_id": model_id,
      },
  )
  g.add_node(n)
  g.add_edge(video, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def vertex_ai_custom(
    video,
    vertex_endpoint_name="",
    input_name="",
    vertex_online_prediction_service_endpoint="",
    app_platform_metadata="",
    max_prediction_fps=0,
    name="",
    g=None,
):
  """TODO Fill short description for "VertexAiCustom".

  TODO Fill long description for "VertexAiCustom".

  Args:
    video: A `Port` that produce packets of type "gst/video".
    vertex_endpoint_name: An attribute of type `string`. Defaults to ''.
    input_name: An attribute of type `string`. Defaults to ''.
    vertex_online_prediction_service_endpoint: An attribute of type `string`.
      Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    max_prediction_fps: An attribute of type `int`. Defaults to 0.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type
    "protobuf/google.cloud.aiplatform.v1.PredictResponse".
  """
  n = graph.Node(
      name=name,
      operator="VertexAiCustom",
      input_ports=["video"],
      output_ports=["prediction_result"],
      attributes={
          "vertex_endpoint_name": vertex_endpoint_name,
          "input_name": input_name,
          "vertex_online_prediction_service_endpoint": (
              vertex_online_prediction_service_endpoint
          ),
          "app_platform_metadata": app_platform_metadata,
          "max_prediction_fps": max_prediction_fps,
      },
  )
  g.add_node(n)
  g.add_edge(video, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def concat(string_0, string_1, lowercase=False, name="", g=None):
  """TODO Fill short description for "Concat".

  TODO Fill long description for "Concat".

  Args:
    string_0: A `Port` that produce packets of type "string".
    string_1: A `Port` that produce packets of type "string".
    lowercase: An attribute of type `bool`. Defaults to False.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type "string".
  """
  n = graph.Node(
      name=name,
      operator="Concat",
      input_ports=["string_0", "string_1"],
      output_ports=["concat_string"],
      attributes={"lowercase": lowercase},
  )
  g.add_node(n)
  g.add_edge(string_0, n.inputs()[0])
  g.add_edge(string_1, n.inputs()[1])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def gcs_video_source(input_video_gcs_path="", name="", g=None):
  """TODO Fill short description for "GcsVideoSource".

  TODO Fill long description for "GcsVideoSource".

  Args:
    input_video_gcs_path: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type "gst/video".
  """
  n = graph.Node(
      name=name,
      operator="GcsVideoSource",
      input_ports=[],
      output_ports=["output_stream"],
      attributes={"input_video_gcs_path": input_video_gcs_path},
  )
  g.add_node(n)

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def gcs_video_sink(input_stream, output_video_gcs_path="", name="", g=None):
  """TODO Fill short description for "GcsVideoSink".

  TODO Fill long description for "GcsVideoSink".

  Args:
    input_stream: A `Port` that produce packets of type "gst/video".
    output_video_gcs_path: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="GcsVideoSink",
      input_ports=["input_stream"],
      output_ports=[],
      attributes={"output_video_gcs_path": output_video_gcs_path},
  )
  g.add_node(n)
  g.add_edge(input_stream, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def gcs_proto_sink(annotation, output_file_gcs_path="", name="", g=None):
  """TODO Fill short description for "GcsProtoSink".

  TODO Fill long description for "GcsProtoSink".

  Args:
    annotation: A `Port` that produce packets of type "protobuf".
    output_file_gcs_path: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="GcsProtoSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={"output_file_gcs_path": output_file_gcs_path},
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def de_id(
    input_stream,
    distort_faces=True,
    distortion_mode="BLURRING",
    frame_rate=6,
    name="",
    g=None,
):
  """TODO Fill short description for "DeID".

  TODO Fill long description for "DeID".

  Args:
    input_stream: A `Port` that produce packets of type "gst/video".
    distort_faces: An attribute of type `bool`. Defaults to True.
    distortion_mode: An attribute of type `string`. Defaults to 'BLURRING'.
    frame_rate: An attribute of type `int`. Defaults to 6.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type "gst/video".
  """
  n = graph.Node(
      name=name,
      operator="DeID",
      input_ports=["input_stream"],
      output_ports=["output_stream"],
      attributes={
          "distort_faces": distort_faces,
          "distortion_mode": distortion_mode,
          "frame_rate": frame_rate,
      },
  )
  g.add_node(n)
  g.add_edge(input_stream, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def stream_sink(input, name="", g=None):
  """TODO Fill short description for "StreamSink".

  TODO Fill long description for "StreamSink".

  Args:
    input: A `Port` that produce packets of type "special/any".
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="StreamSink",
      input_ports=["input"],
      output_ports=[],
      attributes={},
  )
  g.add_node(n)
  g.add_edge(input, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def stream_source(name="", g=None):
  """TODO Fill short description for "StreamSource".

  TODO Fill long description for "StreamSource".

  Args:
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type "special/any".
  """
  n = graph.Node(
      name=name,
      operator="StreamSource",
      input_ports=[],
      output_ports=["output"],
      attributes={},
  )
  g.add_node(n)

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def occupancy_counting(
    input_stream,
    detect_person=True,
    detect_vehicle=True,
    detect_dwelling=True,
    lines="",
    zones="",
    name="",
    g=None,
):
  """TODO Fill short description for "OccupancyCounting".

  TODO Fill long description for "OccupancyCounting".

  Args:
    input_stream: A `Port` that produce packets of type "gst/video".
    detect_person: An attribute of type `bool`. Defaults to True.
    detect_vehicle: An attribute of type `bool`. Defaults to True.
    detect_dwelling: An attribute of type `bool`. Defaults to True.
    lines: An attribute of type `string`. Defaults to ''.
    zones: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type
    "protobuf/google.cloud.visionai.v1.OccupancyCountingPredictionResult".
  """
  n = graph.Node(
      name=name,
      operator="OccupancyCounting",
      input_ports=["input_stream"],
      output_ports=["output_stream"],
      attributes={
          "detect_person": detect_person,
          "detect_vehicle": detect_vehicle,
          "detect_dwelling": detect_dwelling,
          "lines": lines,
          "zones": zones,
      },
  )
  g.add_node(n)
  g.add_edge(input_stream, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def string_split(input_string, delimiter="/", name="", g=None):
  """Operator for splitting the string.

  The operator takes an input string and splits the input string into two with
  specified delimiter (default: "/").

  Args:
    input_string: A `Port` that produce packets of type "string".
    delimiter: An attribute of type `string`. Defaults to '/'.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A tuple of `Port` objects (string_0, string_1).

    string_0: A `Port` that produce packets of type "string".
    string_1: A `Port` that produce packets of type "string".
  """
  n = graph.Node(
      name=name,
      operator="StringSplit",
      input_ports=["input_string"],
      output_ports=["string_0", "string_1"],
      attributes={"delimiter": delimiter},
  )
  g.add_node(n)
  g.add_edge(input_string, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def clip_classification(string_0, string_1, lowercase=False, name="", g=None):
  """TODO Fill short description for "ClipClassification".

  TODO Fill long description for "ClipClassification".

  Args:
    string_0: A `Port` that produce packets of type "string".
    string_1: A `Port` that produce packets of type "string".
    lowercase: An attribute of type `bool`. Defaults to False.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type "string".
  """
  n = graph.Node(
      name=name,
      operator="ClipClassification",
      input_ports=["string_0", "string_1"],
      output_ports=["concat_string"],
      attributes={"lowercase": lowercase},
  )
  g.add_node(n)
  g.add_edge(string_0, n.inputs()[0])
  g.add_edge(string_1, n.inputs()[1])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def image_classification_warehouse_sink(
    annotation,
    use_insecure_channel=False,
    warehouse_endpoint="",
    asset_name="",
    image_classification_data_schema_id="",
    name="",
    g=None,
):
  """TODO Fill short description for "ImageClassificationWarehouseSink".

  TODO Fill long description for "ImageClassificationWarehouseSink".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.ClassificationPredictionResult".
    use_insecure_channel: An attribute of type `bool`. Defaults to False.
    warehouse_endpoint: An attribute of type `string`. Defaults to ''.
    asset_name: An attribute of type `string`. Defaults to ''.
    image_classification_data_schema_id: An attribute of type `string`. Defaults
      to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="ImageClassificationWarehouseSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={
          "use_insecure_channel": use_insecure_channel,
          "warehouse_endpoint": warehouse_endpoint,
          "asset_name": asset_name,
          "image_classification_data_schema_id": (
              image_classification_data_schema_id
          ),
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def image_object_detection_warehouse_sink(
    annotation,
    use_insecure_channel=False,
    warehouse_endpoint="",
    asset_name="",
    image_object_detection_data_schema_id="",
    name="",
    g=None,
):
  """TODO Fill short description for "ImageObjectDetectionWarehouseSink".

  TODO Fill long description for "ImageObjectDetectionWarehouseSink".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.ImageObjectDetectionPredictionResult".
    use_insecure_channel: An attribute of type `bool`. Defaults to False.
    warehouse_endpoint: An attribute of type `string`. Defaults to ''.
    asset_name: An attribute of type `string`. Defaults to ''.
    image_object_detection_data_schema_id: An attribute of type `string`.
      Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="ImageObjectDetectionWarehouseSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={
          "use_insecure_channel": use_insecure_channel,
          "warehouse_endpoint": warehouse_endpoint,
          "asset_name": asset_name,
          "image_object_detection_data_schema_id": (
              image_object_detection_data_schema_id
          ),
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def occupancy_counting_warehouse_sink(
    annotation,
    use_insecure_channel=False,
    warehouse_endpoint="",
    asset_name="",
    person_occupancy_data_schema_id="",
    vehicle_occupancy_data_schema_id="",
    name="",
    g=None,
):
  """TODO Fill short description for "OccupancyCountingWarehouseSink".

  TODO Fill long description for "OccupancyCountingWarehouseSink".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.OccupancyCountingPredictionResult".
    use_insecure_channel: An attribute of type `bool`. Defaults to False.
    warehouse_endpoint: An attribute of type `string`. Defaults to ''.
    asset_name: An attribute of type `string`. Defaults to ''.
    person_occupancy_data_schema_id: An attribute of type `string`. Defaults to
      ''.
    vehicle_occupancy_data_schema_id: An attribute of type `string`. Defaults to
      ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="OccupancyCountingWarehouseSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={
          "use_insecure_channel": use_insecure_channel,
          "warehouse_endpoint": warehouse_endpoint,
          "asset_name": asset_name,
          "person_occupancy_data_schema_id": person_occupancy_data_schema_id,
          "vehicle_occupancy_data_schema_id": vehicle_occupancy_data_schema_id,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def video_action_recognition_warehouse_sink(
    annotation,
    use_insecure_channel=False,
    warehouse_endpoint="",
    asset_name="",
    video_action_recognition_data_schema_id="",
    name="",
    g=None,
):
  """TODO Fill short description for "VideoActionRecognitionWarehouseSink".

  TODO Fill long description for "VideoActionRecognitionWarehouseSink".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.VideoActionRecognitionPredictionResult".
    use_insecure_channel: An attribute of type `bool`. Defaults to False.
    warehouse_endpoint: An attribute of type `string`. Defaults to ''.
    asset_name: An attribute of type `string`. Defaults to ''.
    video_action_recognition_data_schema_id: An attribute of type `string`.
      Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="VideoActionRecognitionWarehouseSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={
          "use_insecure_channel": use_insecure_channel,
          "warehouse_endpoint": warehouse_endpoint,
          "asset_name": asset_name,
          "video_action_recognition_data_schema_id": (
              video_action_recognition_data_schema_id
          ),
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def video_classification_warehouse_sink(
    annotation,
    use_insecure_channel=False,
    warehouse_endpoint="",
    asset_name="",
    video_classification_data_schema_id="",
    name="",
    g=None,
):
  """TODO Fill short description for "VideoClassificationWarehouseSink".

  TODO Fill long description for "VideoClassificationWarehouseSink".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.VideoClassificationPredictionResult".
    use_insecure_channel: An attribute of type `bool`. Defaults to False.
    warehouse_endpoint: An attribute of type `string`. Defaults to ''.
    asset_name: An attribute of type `string`. Defaults to ''.
    video_classification_data_schema_id: An attribute of type `string`. Defaults
      to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="VideoClassificationWarehouseSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={
          "use_insecure_channel": use_insecure_channel,
          "warehouse_endpoint": warehouse_endpoint,
          "asset_name": asset_name,
          "video_classification_data_schema_id": (
              video_classification_data_schema_id
          ),
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def video_object_tracking_warehouse_sink(
    annotation,
    use_insecure_channel=False,
    warehouse_endpoint="",
    asset_name="",
    video_object_tracking_data_schema_id="",
    name="",
    g=None,
):
  """TODO Fill short description for "VideoObjectTrackingWarehouseSink".

  TODO Fill long description for "VideoObjectTrackingWarehouseSink".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.VideoObjectTrackingPredictionResult".
    use_insecure_channel: An attribute of type `bool`. Defaults to False.
    warehouse_endpoint: An attribute of type `string`. Defaults to ''.
    asset_name: An attribute of type `string`. Defaults to ''.
    video_object_tracking_data_schema_id: An attribute of type `string`.
      Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="VideoObjectTrackingWarehouseSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={
          "use_insecure_channel": use_insecure_channel,
          "warehouse_endpoint": warehouse_endpoint,
          "asset_name": asset_name,
          "video_object_tracking_data_schema_id": (
              video_object_tracking_data_schema_id
          ),
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def vertex_ai_custom_warehouse_sink(
    annotation,
    use_insecure_channel=False,
    warehouse_endpoint="",
    asset_name="",
    vertex_ai_custom_data_schema_id="",
    name="",
    g=None,
):
  """TODO Fill short description for "VertexAiCustomWarehouseSink".

  TODO Fill long description for "VertexAiCustomWarehouseSink".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.aiplatform.v1.PredictResponse".
    use_insecure_channel: An attribute of type `bool`. Defaults to False.
    warehouse_endpoint: An attribute of type `string`. Defaults to ''.
    asset_name: An attribute of type `string`. Defaults to ''.
    vertex_ai_custom_data_schema_id: An attribute of type `string`. Defaults to
      ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="VertexAiCustomWarehouseSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={
          "use_insecure_channel": use_insecure_channel,
          "warehouse_endpoint": warehouse_endpoint,
          "asset_name": asset_name,
          "vertex_ai_custom_data_schema_id": vertex_ai_custom_data_schema_id,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def generic_object_detector_warehouse_sink(
    annotation,
    use_insecure_channel=False,
    warehouse_endpoint="",
    asset_name="",
    general_object_detection_data_schema_id="",
    name="",
    g=None,
):
  """TODO Fill short description for "GenericObjectDetectorWarehouseSink".

  TODO Fill long description for "GenericObjectDetectorWarehouseSink".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.ObjectDetectionPredictionResult".
    use_insecure_channel: An attribute of type `bool`. Defaults to False.
    warehouse_endpoint: An attribute of type `string`. Defaults to ''.
    asset_name: An attribute of type `string`. Defaults to ''.
    general_object_detection_data_schema_id: An attribute of type `string`.
      Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="GenericObjectDetectorWarehouseSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={
          "use_insecure_channel": use_insecure_channel,
          "warehouse_endpoint": warehouse_endpoint,
          "asset_name": asset_name,
          "general_object_detection_data_schema_id": (
              general_object_detection_data_schema_id
          ),
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def personal_protective_equipment_detector_warehouse_sink(
    annotation,
    use_insecure_channel=False,
    warehouse_endpoint="",
    asset_name="",
    personal_protective_equipment_detection_data_schema_id="",
    name="",
    g=None,
):
  """TODO Fill short description for "PersonalProtectiveEquipmentDetectorWarehouseSink".

  TODO Fill long description for
  "PersonalProtectiveEquipmentDetectorWarehouseSink".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.PersonalProtectiveEquipmentDetectionOutput".
    use_insecure_channel: An attribute of type `bool`. Defaults to False.
    warehouse_endpoint: An attribute of type `string`. Defaults to ''.
    asset_name: An attribute of type `string`. Defaults to ''.
    personal_protective_equipment_detection_data_schema_id: An attribute of type
      `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="PersonalProtectiveEquipmentDetectorWarehouseSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={
          "use_insecure_channel": use_insecure_channel,
          "warehouse_endpoint": warehouse_endpoint,
          "asset_name": asset_name,
          "personal_protective_equipment_detection_data_schema_id": (
              personal_protective_equipment_detection_data_schema_id
          ),
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def big_query_custom_model_sink(
    annotation,
    big_query_table="",
    big_query_service_address="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "BigQueryCustomModelSink".

  TODO Fill long description for "BigQueryCustomModelSink".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.aiplatform.v1.PredictResponse".
    big_query_table: An attribute of type `string`. Defaults to ''.
    big_query_service_address: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="BigQueryCustomModelSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={
          "big_query_table": big_query_table,
          "big_query_service_address": big_query_service_address,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def big_query_generic_object_detector_sink(
    annotation,
    big_query_table="",
    big_query_service_address="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "BigQueryGenericObjectDetectorSink".

  TODO Fill long description for "BigQueryGenericObjectDetectorSink".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.ObjectDetectionPredictionResult".
    big_query_table: An attribute of type `string`. Defaults to ''.
    big_query_service_address: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="BigQueryGenericObjectDetectorSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={
          "big_query_table": big_query_table,
          "big_query_service_address": big_query_service_address,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def big_query_personal_protective_equipment_detector_sink(
    annotation,
    big_query_table="",
    big_query_service_address="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "BigQueryPersonalProtectiveEquipmentDetectorSink".

  TODO Fill long description for
  "BigQueryPersonalProtectiveEquipmentDetectorSink".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.PersonalProtectiveEquipmentDetectionOutput".
    big_query_table: An attribute of type `string`. Defaults to ''.
    big_query_service_address: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="BigQueryPersonalProtectiveEquipmentDetectorSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={
          "big_query_table": big_query_table,
          "big_query_service_address": big_query_service_address,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def big_query_image_classification_sink(
    annotation,
    big_query_table="",
    big_query_service_address="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "BigQueryImageClassificationSink".

  TODO Fill long description for "BigQueryImageClassificationSink".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.ClassificationPredictionResult".
    big_query_table: An attribute of type `string`. Defaults to ''.
    big_query_service_address: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="BigQueryImageClassificationSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={
          "big_query_table": big_query_table,
          "big_query_service_address": big_query_service_address,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def big_query_image_object_detection_sink(
    annotation,
    big_query_table="",
    big_query_service_address="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "BigQueryImageObjectDetectionSink".

  TODO Fill long description for "BigQueryImageObjectDetectionSink".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.ImageObjectDetectionPredictionResult".
    big_query_table: An attribute of type `string`. Defaults to ''.
    big_query_service_address: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="BigQueryImageObjectDetectionSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={
          "big_query_table": big_query_table,
          "big_query_service_address": big_query_service_address,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def big_query_occupancy_count_sink(
    annotation,
    big_query_table="",
    big_query_service_address="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "BigQueryOccupancyCountSink".

  TODO Fill long description for "BigQueryOccupancyCountSink".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.OccupancyCountingPredictionResult".
    big_query_table: An attribute of type `string`. Defaults to ''.
    big_query_service_address: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="BigQueryOccupancyCountSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={
          "big_query_table": big_query_table,
          "big_query_service_address": big_query_service_address,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def big_query_passthrough_sink(
    annotation,
    big_query_table="",
    big_query_service_address="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "BigQueryPassthroughSink".

  TODO Fill long description for "BigQueryPassthroughSink".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest".
    big_query_table: An attribute of type `string`. Defaults to ''.
    big_query_service_address: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="BigQueryPassthroughSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={
          "big_query_table": big_query_table,
          "big_query_service_address": big_query_service_address,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def big_query_stream_metadata_sink(
    video,
    big_query_table="",
    big_query_service_address="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "BigQueryStreamMetadataSink".

  TODO Fill long description for "BigQueryStreamMetadataSink".

  Args:
    video: A `Port` that produce packets of type "gst/video".
    big_query_table: An attribute of type `string`. Defaults to ''.
    big_query_service_address: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="BigQueryStreamMetadataSink",
      input_ports=["video"],
      output_ports=[],
      attributes={
          "big_query_table": big_query_table,
          "big_query_service_address": big_query_service_address,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(video, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def big_query_video_action_recognition_sink(
    annotation,
    big_query_table="",
    big_query_service_address="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "BigQueryVideoActionRecognitionSink".

  TODO Fill long description for "BigQueryVideoActionRecognitionSink".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.VideoActionRecognitionPredictionResult".
    big_query_table: An attribute of type `string`. Defaults to ''.
    big_query_service_address: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="BigQueryVideoActionRecognitionSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={
          "big_query_table": big_query_table,
          "big_query_service_address": big_query_service_address,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def big_query_video_classification_sink(
    annotation,
    big_query_table="",
    big_query_service_address="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "BigQueryVideoClassificationSink".

  TODO Fill long description for "BigQueryVideoClassificationSink".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.VideoClassificationPredictionResult".
    big_query_table: An attribute of type `string`. Defaults to ''.
    big_query_service_address: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="BigQueryVideoClassificationSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={
          "big_query_table": big_query_table,
          "big_query_service_address": big_query_service_address,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def big_query_video_object_tracking_sink(
    annotation,
    big_query_table="",
    big_query_service_address="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "BigQueryVideoObjectTrackingSink".

  TODO Fill long description for "BigQueryVideoObjectTrackingSink".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.VideoObjectTrackingPredictionResult".
    big_query_table: An attribute of type `string`. Defaults to ''.
    big_query_service_address: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="BigQueryVideoObjectTrackingSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={
          "big_query_table": big_query_table,
          "big_query_service_address": big_query_service_address,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def cloud_function_bq_predict_response(
    annotation,
    cloud_function_http_trigger="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "CloudFunctionBqPredictResponse".

  TODO Fill long description for "CloudFunctionBqPredictResponse".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.aiplatform.v1.PredictResponse".
    cloud_function_http_trigger: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type
    "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest".
  """
  n = graph.Node(
      name=name,
      operator="CloudFunctionBqPredictResponse",
      input_ports=["annotation"],
      output_ports=["processed_annotation"],
      attributes={
          "cloud_function_http_trigger": cloud_function_http_trigger,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def cloud_function_bq_god(
    annotation,
    cloud_function_http_trigger="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "CloudFunctionBqGod".

  TODO Fill long description for "CloudFunctionBqGod".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.ObjectDetectionPredictionResult".
    cloud_function_http_trigger: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type
    "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest".
  """
  n = graph.Node(
      name=name,
      operator="CloudFunctionBqGod",
      input_ports=["annotation"],
      output_ports=["processed_annotation"],
      attributes={
          "cloud_function_http_trigger": cloud_function_http_trigger,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def cloud_function_bq_ppe(
    annotation,
    cloud_function_http_trigger="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "CloudFunctionBqPpe".

  TODO Fill long description for "CloudFunctionBqPpe".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.PersonalProtectiveEquipmentDetectionOutput".
    cloud_function_http_trigger: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type
    "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest".
  """
  n = graph.Node(
      name=name,
      operator="CloudFunctionBqPpe",
      input_ports=["annotation"],
      output_ports=["processed_annotation"],
      attributes={
          "cloud_function_http_trigger": cloud_function_http_trigger,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def cloud_function_bq_icn(
    annotation,
    cloud_function_http_trigger="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "CloudFunctionBqIcn".

  TODO Fill long description for "CloudFunctionBqIcn".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.ClassificationPredictionResult".
    cloud_function_http_trigger: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type
    "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest".
  """
  n = graph.Node(
      name=name,
      operator="CloudFunctionBqIcn",
      input_ports=["annotation"],
      output_ports=["processed_annotation"],
      attributes={
          "cloud_function_http_trigger": cloud_function_http_trigger,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def cloud_function_bq_iod(
    annotation,
    cloud_function_http_trigger="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "CloudFunctionBqIod".

  TODO Fill long description for "CloudFunctionBqIod".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.ImageObjectDetectionPredictionResult".
    cloud_function_http_trigger: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type
    "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest".
  """
  n = graph.Node(
      name=name,
      operator="CloudFunctionBqIod",
      input_ports=["annotation"],
      output_ports=["processed_annotation"],
      attributes={
          "cloud_function_http_trigger": cloud_function_http_trigger,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def cloud_function_bq_oc(
    annotation,
    cloud_function_http_trigger="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "CloudFunctionBqOc".

  TODO Fill long description for "CloudFunctionBqOc".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.OccupancyCountingPredictionResult".
    cloud_function_http_trigger: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type
    "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest".
  """
  n = graph.Node(
      name=name,
      operator="CloudFunctionBqOc",
      input_ports=["annotation"],
      output_ports=["processed_annotation"],
      attributes={
          "cloud_function_http_trigger": cloud_function_http_trigger,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def cloud_function_bq_stream(
    video,
    cloud_function_http_trigger="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "CloudFunctionBqStream".

  TODO Fill long description for "CloudFunctionBqStream".

  Args:
    video: A `Port` that produce packets of type "gst/video".
    cloud_function_http_trigger: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type
    "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest".
  """
  n = graph.Node(
      name=name,
      operator="CloudFunctionBqStream",
      input_ports=["video"],
      output_ports=["processed_annotation"],
      attributes={
          "cloud_function_http_trigger": cloud_function_http_trigger,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(video, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def cloud_function_bq_var(
    annotation,
    cloud_function_http_trigger="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "CloudFunctionBqVar".

  TODO Fill long description for "CloudFunctionBqVar".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.VideoActionRecognitionPredictionResult".
    cloud_function_http_trigger: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type
    "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest".
  """
  n = graph.Node(
      name=name,
      operator="CloudFunctionBqVar",
      input_ports=["annotation"],
      output_ports=["processed_annotation"],
      attributes={
          "cloud_function_http_trigger": cloud_function_http_trigger,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def cloud_function_bq_vcn(
    annotation,
    cloud_function_http_trigger="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "CloudFunctionBqVcn".

  TODO Fill long description for "CloudFunctionBqVcn".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.VideoClassificationPredictionResult".
    cloud_function_http_trigger: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type
    "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest".
  """
  n = graph.Node(
      name=name,
      operator="CloudFunctionBqVcn",
      input_ports=["annotation"],
      output_ports=["processed_annotation"],
      attributes={
          "cloud_function_http_trigger": cloud_function_http_trigger,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def cloud_function_bq_vot(
    annotation,
    cloud_function_http_trigger="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "CloudFunctionBqVot".

  TODO Fill long description for "CloudFunctionBqVot".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.VideoObjectTrackingPredictionResult".
    cloud_function_http_trigger: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type
    "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest".
  """
  n = graph.Node(
      name=name,
      operator="CloudFunctionBqVot",
      input_ports=["annotation"],
      output_ports=["processed_annotation"],
      attributes={
          "cloud_function_http_trigger": cloud_function_http_trigger,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def cloud_function_predict_response(
    annotation,
    cloud_function_http_trigger="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "CloudFunctionPredictResponse".

  TODO Fill long description for "CloudFunctionPredictResponse".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.aiplatform.v1.PredictResponse".
    cloud_function_http_trigger: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A tuple of `Port` objects (processed_annotation, custom_event).

    processed_annotation: A `Port` that produce packets of type
    "protobuf/google.cloud.aiplatform.v1.PredictResponse".
    custom_event: A `Port` that produce packets of type
    "protobuf/google.cloud.visionai.v1.AppPlatformCloudFunctionResponse".
  """
  n = graph.Node(
      name=name,
      operator="CloudFunctionPredictResponse",
      input_ports=["annotation"],
      output_ports=["processed_annotation", "custom_event"],
      attributes={
          "cloud_function_http_trigger": cloud_function_http_trigger,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def cloud_function_god(
    annotation,
    cloud_function_http_trigger="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "CloudFunctionGod".

  TODO Fill long description for "CloudFunctionGod".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.ObjectDetectionPredictionResult".
    cloud_function_http_trigger: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A tuple of `Port` objects (processed_annotation, custom_event).

    processed_annotation: A `Port` that produce packets of type
    "protobuf/google.cloud.visionai.v1.ObjectDetectionPredictionResult".
    custom_event: A `Port` that produce packets of type
    "protobuf/google.cloud.visionai.v1.AppPlatformCloudFunctionResponse".
  """
  n = graph.Node(
      name=name,
      operator="CloudFunctionGod",
      input_ports=["annotation"],
      output_ports=["processed_annotation", "custom_event"],
      attributes={
          "cloud_function_http_trigger": cloud_function_http_trigger,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def cloud_function_occupancy_count(
    annotation,
    cloud_function_http_trigger="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "CloudFunctionOccupancyCount".

  TODO Fill long description for "CloudFunctionOccupancyCount".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.OccupancyCountingPredictionResult".
    cloud_function_http_trigger: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A tuple of `Port` objects (processed_annotation, custom_event).

    processed_annotation: A `Port` that produce packets of type
    "protobuf/google.cloud.visionai.v1.OccupancyCountingPredictionResult".
    custom_event: A `Port` that produce packets of type
    "protobuf/google.cloud.visionai.v1.AppPlatformCloudFunctionResponse".
  """
  n = graph.Node(
      name=name,
      operator="CloudFunctionOccupancyCount",
      input_ports=["annotation"],
      output_ports=["processed_annotation", "custom_event"],
      attributes={
          "cloud_function_http_trigger": cloud_function_http_trigger,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def generic_object_detection(input_stream, name="", g=None):
  """TODO Fill short description for "GenericObjectDetection".

  TODO Fill long description for "GenericObjectDetection".

  Args:
    input_stream: A `Port` that produce packets of type "gst/video".
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type
    "protobuf/google.cloud.visionai.v1.ObjectDetectionPredictionResult".
  """
  n = graph.Node(
      name=name,
      operator="GenericObjectDetection",
      input_ports=["input_stream"],
      output_ports=["output_stream"],
      attributes={},
  )
  g.add_node(n)
  g.add_edge(input_stream, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def person_vehicle_detection(
    input_stream, detect_person=True, detect_vehicle=True, name="", g=None
):
  """TODO Fill short description for "PersonVehicleDetection".

  TODO Fill long description for "PersonVehicleDetection".

  Args:
    input_stream: A `Port` that produce packets of type "gst/video".
    detect_person: An attribute of type `bool`. Defaults to True.
    detect_vehicle: An attribute of type `bool`. Defaults to True.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type
    "protobuf/google.cloud.visionai.v1.OccupancyCountingPredictionResult".
  """
  n = graph.Node(
      name=name,
      operator="PersonVehicleDetection",
      input_ports=["input_stream"],
      output_ports=["output_stream"],
      attributes={
          "detect_person": detect_person,
          "detect_vehicle": detect_vehicle,
      },
  )
  g.add_node(n)
  g.add_edge(input_stream, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def personal_protective_equipment_detection(
    input_stream,
    enable_face_coverage_detection=True,
    enable_head_coverage_detection=True,
    enable_hands_coverage_detection=True,
    name="",
    g=None,
):
  """TODO Fill short description for "PersonalProtectiveEquipmentDetection".

  TODO Fill long description for "PersonalProtectiveEquipmentDetection".

  Args:
    input_stream: A `Port` that produce packets of type "gst/video".
    enable_face_coverage_detection: An attribute of type `bool`. Defaults to
      True.
    enable_head_coverage_detection: An attribute of type `bool`. Defaults to
      True.
    enable_hands_coverage_detection: An attribute of type `bool`. Defaults to
      True.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type
    "protobuf/google.cloud.visionai.v1.PersonalProtectiveEquipmentDetectionOutput".
  """
  n = graph.Node(
      name=name,
      operator="PersonalProtectiveEquipmentDetection",
      input_ports=["input_stream"],
      output_ports=["output_stream"],
      attributes={
          "enable_face_coverage_detection": enable_face_coverage_detection,
          "enable_head_coverage_detection": enable_head_coverage_detection,
          "enable_hands_coverage_detection": enable_hands_coverage_detection,
      },
  )
  g.add_node(n)
  g.add_edge(input_stream, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def text_detection(
    input_stream,
    is_live_video=False,
    ocr_service_address="",
    language_hints="",
    name="",
    g=None,
):
  """TODO Fill short description for "TextDetection".

  TODO Fill long description for "TextDetection".

  Args:
    input_stream: A `Port` that produce packets of type "gst/video".
    is_live_video: An attribute of type `bool`. Defaults to False.
    ocr_service_address: An attribute of type `string`. Defaults to ''.
    language_hints: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type "protobuf/visionai.TextAnnotations".
  """
  n = graph.Node(
      name=name,
      operator="TextDetection",
      input_ports=["input_stream"],
      output_ports=["output_stream"],
      attributes={
          "is_live_video": is_live_video,
          "ocr_service_address": ocr_service_address,
          "language_hints": language_hints,
      },
  )
  g.add_node(n)
  g.add_edge(input_stream, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def svai_product_recognizer(
    input_stream,
    retail_service_endpoint="",
    retail_endpoint_name="",
    feature="CAESEAoOYnVpbHRpbi9zdGFibGU",
    gcs_destination="",
    name="",
    g=None,
):
  """TODO Fill short description for "SvaiProductRecognizer".

  TODO Fill long description for "SvaiProductRecognizer".

  Args:
    input_stream: A `Port` that produce packets of type "gst/video".
    retail_service_endpoint: An attribute of type `string`. Defaults to ''.
    retail_endpoint_name: An attribute of type `string`. Defaults to ''.
    feature: An attribute of type `string`. Defaults to
      'CAESEAoOYnVpbHRpbi9zdGFibGU'.
    gcs_destination: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type
    "protobuf/google.cloud.visionai.v1alpha1.RetailPredictResults".
  """
  n = graph.Node(
      name=name,
      operator="SvaiProductRecognizer",
      input_ports=["input_stream"],
      output_ports=["prediction_result"],
      attributes={
          "retail_service_endpoint": retail_service_endpoint,
          "retail_endpoint_name": retail_endpoint_name,
          "feature": feature,
          "gcs_destination": gcs_destination,
      },
  )
  g.add_node(n)
  g.add_edge(input_stream, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def big_query_svai_recognition_sink(
    annotation,
    big_query_table="",
    big_query_service_address="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "BigQuerySvaiRecognitionSink".

  TODO Fill long description for "BigQuerySvaiRecognitionSink".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1alpha1.RetailPredictResults".
    big_query_table: An attribute of type `string`. Defaults to ''.
    big_query_service_address: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="BigQuerySvaiRecognitionSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={
          "big_query_table": big_query_table,
          "big_query_service_address": big_query_service_address,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def cloud_function_bq_retail_pr(
    annotation,
    cloud_function_http_trigger="",
    app_platform_metadata="",
    name="",
    g=None,
):
  """TODO Fill short description for "CloudFunctionBqRetailPR".

  TODO Fill long description for "CloudFunctionBqRetailPR".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1alpha1.RetailPredictResults".
    cloud_function_http_trigger: An attribute of type `string`. Defaults to ''.
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type
    "protobuf/google.cloud.bigquery.storage.v1.AppendRowsRequest".
  """
  n = graph.Node(
      name=name,
      operator="CloudFunctionBqRetailPR",
      input_ports=["annotation"],
      output_ports=["processed_annotation"],
      attributes={
          "cloud_function_http_trigger": cloud_function_http_trigger,
          "app_platform_metadata": app_platform_metadata,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def svai_product_recognition_warehouse_sink(
    annotation,
    use_insecure_channel=False,
    warehouse_endpoint="",
    asset_name="",
    product_recognizer_data_schema_id="",
    name="",
    g=None,
):
  """TODO Fill short description for "SvaiProductRecognitionWarehouseSink".

  TODO Fill long description for "SvaiProductRecognitionWarehouseSink".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1alpha1.RetailPredictResults".
    use_insecure_channel: An attribute of type `bool`. Defaults to False.
    warehouse_endpoint: An attribute of type `string`. Defaults to ''.
    asset_name: An attribute of type `string`. Defaults to ''.
    product_recognizer_data_schema_id: An attribute of type `string`. Defaults
      to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="SvaiProductRecognitionWarehouseSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={
          "use_insecure_channel": use_insecure_channel,
          "warehouse_endpoint": warehouse_endpoint,
          "asset_name": asset_name,
          "product_recognizer_data_schema_id": (
              product_recognizer_data_schema_id
          ),
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def pub_sub_event(
    events,
    app_platform_metadata="",
    channel="",
    deliver_interval_ms=5000,
    name="",
    g=None,
):
  """TODO Fill short description for "PubSubEvent".

  TODO Fill long description for "PubSubEvent".

  Args:
    events: A `Port` that produce packets of type
      "protobuf/google.cloud.visionai.v1.AppPlatformCloudFunctionResponse".
    app_platform_metadata: An attribute of type `string`. Defaults to ''.
    channel: An attribute of type `string`. Defaults to ''.
    deliver_interval_ms: An attribute of type `int`. Defaults to 5000.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="PubSubEvent",
      input_ports=["events"],
      output_ports=[],
      attributes={
          "app_platform_metadata": app_platform_metadata,
          "channel": channel,
          "deliver_interval_ms": deliver_interval_ms,
      },
  )
  g.add_node(n)
  g.add_edge(events, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def warehouse_video_source(
    warehouse_endpoint="", asset_name="", name="", g=None
):
  """TODO Fill short description for "WarehouseVideoSource".

  TODO Fill long description for "WarehouseVideoSource".

  Args:
    warehouse_endpoint: An attribute of type `string`. Defaults to ''.
    asset_name: An attribute of type `string`. Defaults to ''.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    A `Port` that produce packets of type "gst/video".
  """
  n = graph.Node(
      name=name,
      operator="WarehouseVideoSource",
      input_ports=[],
      output_ports=["output_video_stream"],
      attributes={
          "warehouse_endpoint": warehouse_endpoint,
          "asset_name": asset_name,
      },
  )
  g.add_node(n)

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()


def text_detection_warehouse_sink(
    annotation,
    use_insecure_channel=False,
    warehouse_endpoint="",
    asset_name="",
    text_detection_data_schema_id="",
    is_live_video=False,
    name="",
    g=None,
):
  """TODO Fill short description for "TextDetectionWarehouseSink".

  TODO Fill long description for "TextDetectionWarehouseSink".

  Args:
    annotation: A `Port` that produce packets of type
      "protobuf/visionai.TextAnnotations".
    use_insecure_channel: An attribute of type `bool`. Defaults to False.
    warehouse_endpoint: An attribute of type `string`. Defaults to ''.
    asset_name: An attribute of type `string`. Defaults to ''.
    text_detection_data_schema_id: An attribute of type `string`. Defaults to
      ''.
    is_live_video: An attribute of type `bool`. Defaults to False.
    name: A parameter of type `string`. Defaults to "". A unique name to assign
      for this `Node`. A random unique name will be generated if left empty
    g: A parameter of type `Graph`. Defaults to None. The `Graph` into which
      this `Node` is inserted. The default `Graph` will be used if left as None.

  Returns:
    None.
  """
  n = graph.Node(
      name=name,
      operator="TextDetectionWarehouseSink",
      input_ports=["annotation"],
      output_ports=[],
      attributes={
          "use_insecure_channel": use_insecure_channel,
          "warehouse_endpoint": warehouse_endpoint,
          "asset_name": asset_name,
          "text_detection_data_schema_id": text_detection_data_schema_id,
          "is_live_video": is_live_video,
      },
  )
  g.add_node(n)
  g.add_edge(annotation, n.inputs()[0])

  num_outputs = len(n.outputs())
  if num_outputs == 0:
    return None
  elif num_outputs == 1:
    return n.outputs()[0]
  else:
    return n.outputs()
