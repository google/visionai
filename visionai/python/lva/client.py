# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

"""Base client library for the LVA service."""

from datetime import datetime
from datetime import timedelta
import time
from typing import Mapping, Optional
from concurrent import futures
import asyncio
import string
import random

import grpc

from visionai.python.protos.googleapis.v1 import lva_pb2
from visionai.python.protos.googleapis.v1 import lva_resources_pb2
from visionai.python.protos.googleapis.v1 import lva_service_pb2
from visionai.python.protos.googleapis.v1 import lva_service_pb2_grpc
from google.longrunning import operations_pb2
from google.longrunning import operations_pb2_grpc
from visionai.python.lva import graph
from visionai.python.net import channel


_RESOURCE_ID_LENGTH = 8


class Analysis:
  """Analysis object abstracts the analysis resource in the service."""

  def __init__(
      self,
      analysis_id: str,
      analysis_name: str,
      create_time: Optional[datetime] = ...,
      update_time: Optional[datetime] = ...,
      labels: Optional[Mapping[str, str]] = ...,
      analysis_definition: Optional[lva_pb2.AnalysisDefinition] = ...,
      input_streams_mapping: Optional[Mapping[str, str]] = ...,
      output_streams_mapping: Optional[Mapping[str, str]] = ...,
      disable_event_watch: bool = True,
  ):
    self.analysis_id = analysis_id
    self.analysis_name = analysis_name
    self.create_time = create_time
    self.update_time = update_time
    self.labels = labels
    self.analysis_definition = analysis_definition
    self.input_streams_mapping = input_streams_mapping
    self.output_streams_mapping = output_streams_mapping
    self.disable_event_watch = disable_event_watch


def _resource_to_analysis(resource: lva_resources_pb2.Analysis) -> Analysis:
  """Convert an API resource to the Analysis object.

  Args:
    resource: The Analysis resource.

  Returns:
    The Analysis object.
  """
  return Analysis(
      analysis_id=resource.name.split("/")[-1],
      analysis_name=resource.name,
      labels=resource.labels,
      create_time=datetime.fromtimestamp(
          resource.create_time.seconds + resource.create_time.nanos / 1e9
      ),
      update_time=datetime.fromtimestamp(
          resource.update_time.seconds + resource.update_time.nanos / 1e9
      ),
      analysis_definition=resource.analysis_definition,
      input_streams_mapping=resource.input_streams_mapping,
      output_streams_mapping=resource.output_streams_mapping,
      disable_event_watch=resource.disable_event_watch,
  )


class Process:
  """Process objects abstract process resources in the service."""

  STATE_MAPPING = {
      0: "STATE_UNSPECIFIED",
      1: "INITIALIZING",
      2: "RUNNING",
      3: "COMPLETED",
      4: "FAILED",
      5: "PENDING",
  }

  def __init__(
      self,
      process_id: str,
      process_name: str,
      analysis_name: str,
      create_time: datetime = None,
      update_time: datetime = None,
      attribute_overrides: dict[str, str] = None,
      run_status_state: str = None,
      run_status_reason: str = None,
      run_mode: str = None,
      event_id: str = None,
      batch_id: str = None,
      retry_count: int = 0,
  ):
    """Initilize a Process object.

    Args:
      process_id: The process ID.
      process_name: The process name.
      analysis_name: The analysis name.
      create_time: The resource creation time.
      update_time: Last update time.
      attribute_overrides: A dictionary of attributes to override.
      run_status_state: The process status state.
      run_status_reason: The process stats reason.
      run_mode: The process mode.
      event_id: Event ID of the input/output streams.
      batch_id: Batch ID of the Process.
      retry_count: The number of retries for a process in submission mode the
        system should try before declaring failure.
    """
    self.process_id = process_id
    self.process_name = process_name
    self.analysis_name = analysis_name
    self.create_time = create_time
    self.update_time = update_time
    self.attribute_overrides = attribute_overrides
    self.run_status_state = run_status_state
    self.run_status_reason = run_status_reason
    self.run_mode = run_mode
    self.event_id = event_id
    self.batch_id = batch_id
    self.retry_count = retry_count

  def wait(
      self,
      connection_options: channel.ConnectionOptions,
      timeout: timedelta = timedelta(days=1),
  ):
    """Wait for the process state to be COMPLETED or ERROR within the given timeout.


    Args:
      connection_options: A `ConnectionOptions` targeting a specific Vertex AI
        Vision instance.
      timeout: The maximum time waiting for the LRO.

    Returns:
      The `Process` object.

    Raises:
      TimeoutError: Process is not done within the given timeout.
    """
    deadline = datetime.now() + timeout
    while datetime.now() < deadline:
      process = get_process(connection_options, self.process_id)
      state = process.run_status_state
      if state == "COMPLETED" or state == "FAILED":
        return process
      time.sleep(5)
    raise TimeoutError(
        "timeout wait for process {} after {} seconds".format(
            self.process_id, timeout.total_seconds()
        )
    )


def _resource_to_process(resource: lva_resources_pb2.Process) -> Process:
  """Convert an API resource to the Process object.

  Args:
    resource: The Process resource.

  Returns:
    The `Process` object.
  """
  return Process(
      process_id=resource.name.split("/")[-1],
      process_name=resource.name,
      analysis_name=resource.analysis,
      create_time=datetime.fromtimestamp(
          resource.create_time.seconds + resource.create_time.nanos / 1e9
      ),
      update_time=datetime.fromtimestamp(
          resource.update_time.seconds + resource.update_time.nanos / 1e9
      ),
      attribute_overrides=dict(
          map(lambda p: p.split("="), resource.attribute_overrides)
      ),
      run_status_state=Process.STATE_MAPPING[resource.run_status.state],
      run_status_reason=resource.run_status.reason,
      run_mode=resource.run_mode,
      event_id=resource.event_id,
      batch_id=resource.batch_id,
      retry_count=resource.retry_count,
  )


def get_analysis(
    connection_options: channel.ConnectionOptions, analysis_id: str
) -> Analysis:
  """Get an analysis in the LVA.

  Args:
    connection_options:  A `ConnectionOptions` targeting a specific Vertex AI
      Vision instance.
    analysis_id: The analysis ID.

  Returns:
    An `Analysis` object.
  """
  stub = lva_service_pb2_grpc.LiveVideoAnalyticsStub(
      channel.create_channel(connection_options)
  )
  req = lva_service_pb2.GetAnalysisRequest(
      name="projects/{}/locations/{}/clusters/{}/analyses/{}".format(
          connection_options.project_id,
          connection_options.location_id,
          connection_options.cluster_id,
          analysis_id,
      )
  )
  metadata = [("x-goog-request-params", "name={}".format(req.name))]
  return _resource_to_analysis(stub.GetAnalysis(request=req, metadata=metadata))


def delete_analysis(
    connection_options: channel.ConnectionOptions, analysis_id: str
) -> None:
  """Delete an analysis in the LVA.

  Args:
    connection_options: A `ConnectionOptions` targeting a specific Vertex AI
      Vision instance.
    analysis_id: The analysis ID.
  """
  stub = lva_service_pb2_grpc.LiveVideoAnalyticsStub(
      channel.create_channel(connection_options)
  )
  req = lva_service_pb2.DeleteAnalysisRequest(
      name="projects/{}/locations/{}/clusters/{}/analyses/{}".format(
          connection_options.project_id,
          connection_options.location_id,
          connection_options.cluster_id,
          analysis_id,
      )
  )
  metadata = [("x-goog-request-params", "name={}".format(req.name))]
  lro = stub.DeleteAnalysis(request=req, metadata=metadata)
  lro_stub = operations_pb2_grpc.OperationsStub(
      channel.create_channel(connection_options)
  )
  _wait_for_lro(lro_stub, lro.name, timedelta(seconds=60))


def list_analyses(
    connection_options: channel.ConnectionOptions,
) -> list[Analysis]:
  """List analyses in the LVA.

  Args:
    connection_options:  A `ConnectionOptions` targeting a specific Vertex AI
      Vision instance.

  Returns:
    A list of `Analysis` objects.
  """
  stub = lva_service_pb2_grpc.LiveVideoAnalyticsStub(
      channel.create_channel(connection_options)
  )
  req = lva_service_pb2.ListAnalysesRequest(
      parent="projects/{}/locations/{}/clusters/{}".format(
          connection_options.project_id,
          connection_options.location_id,
          connection_options.cluster_id,
      )
  )
  metadata = [("x-goog-request-params", "parent={}".format(req.parent))]
  return list(
      map(
          _resource_to_analysis,
          stub.ListAnalyses(request=req, metadata=metadata).analyses,
      )
  )


def create_analysis(
    connection_options: channel.ConnectionOptions,
    g: graph.Graph,
    analysis_id: str = "",
    disable_event_watch: bool = True,
) -> Analysis:
  """Create an analysis in the LVA.

  Args:
    connection_options: A `ConnectionOptions` targeting a specific Vertex AI
      Vision instance.
    g: A `Graph` representing the analysis logic.
    analysis_id: An ID that uniquely identifies the analysis.
    disable_event_watch: The flag to toggle the event watching.

  Returns:
    An `Analysis` object.
  """
  if g is None:
    raise ValueError("graph is not specified")
  if not analysis_id:
    raise ValueError("analysis_id is not specified")
  stub = lva_service_pb2_grpc.LiveVideoAnalyticsStub(
      channel.create_channel(connection_options)
  )
  lro_stub = operations_pb2_grpc.OperationsStub(
      channel.create_channel(connection_options)
  )
  analysis = lva_resources_pb2.Analysis(
      analysis_definition=g.get_analysis_definition(),
      disable_event_watch=disable_event_watch,
  )
  req = lva_service_pb2.CreateAnalysisRequest(
      parent="projects/{}/locations/{}/clusters/{}".format(
          connection_options.project_id,
          connection_options.location_id,
          connection_options.cluster_id,
      ),
      analysis_id=analysis_id,
      analysis=analysis,
  )
  metadata = [("x-goog-request-params", f"parent={req.parent}")]
  lro = stub.CreateAnalysis(request=req, metadata=metadata)
  _wait_for_lro(lro_stub, lro.name, timedelta(seconds=60)).response.Unpack(
      analysis
  )
  return _resource_to_analysis(analysis)


def get_or_create_analysis(
    connection_options: channel.ConnectionOptions,
    g: graph.Graph,
    analysis_id: str = "",
    disable_event_watch: bool = True,
) -> Analysis:
  """Get or create an analysis in the LVA.

  If an analysis with the given `analysis_id` already exists, then simply return
  it. Otherwise, it is created and returned.

  Args:
    connection_options: A `ConnectionOptions` targeting a specific Vertex AI
      Vision instance.
    g: A `Graph` representing the analysis logic.
    analysis_id: An ID that uniquely identifies the analysis.
    disable_event_watch: The flag to toggle the event watching.

  Returns:
    An `Analysis` object.
  """
  try:
    return get_analysis(connection_options, analysis_id)
  except grpc.RpcError as rpc_error:
    if rpc_error.code() != grpc.StatusCode.NOT_FOUND:
      raise
  return create_analysis(
      connection_options, g, analysis_id, disable_event_watch
  )


def get_process(
    connection_options: channel.ConnectionOptions, process_id: str
) -> Process:
  """Get a process in the LVA.

  Args:
    connection_options:  A `ConnectionOptions` targeting a specific Vertex AI
      Vision instance.
    process_id: The process ID.

  Returns:
    A `Process` object.
  """
  stub = lva_service_pb2_grpc.LiveVideoAnalyticsStub(
      channel.create_channel(connection_options)
  )
  req = lva_service_pb2.GetProcessRequest(
      name="projects/{}/locations/{}/clusters/{}/processes/{}".format(
          connection_options.project_id,
          connection_options.location_id,
          connection_options.cluster_id,
          process_id,
      )
  )
  metadata = [("x-goog-request-params", "name={}".format(req.name))]
  return _resource_to_process(stub.GetProcess(request=req, metadata=metadata))


def list_processes(
    connection_options: channel.ConnectionOptions,
) -> list[Process]:
  """List processes in the LVA.

  Args:
    connection_options:  A `ConnectionOptions` targeting a specific Vertex AI
      Vision instance.

  Returns:
    A list of `Process` objects.
  """
  stub = lva_service_pb2_grpc.LiveVideoAnalyticsStub(
      channel.create_channel(connection_options)
  )
  req = lva_service_pb2.ListProcessesRequest(
      parent="projects/{}/locations/{}/clusters/{}".format(
          connection_options.project_id,
          connection_options.location_id,
          connection_options.cluster_id,
      )
  )
  metadata = [("x-goog-request-params", "parent={}".format(req.parent))]
  return list(
      map(
          _resource_to_process,
          stub.ListProcesses(request=req, metadata=metadata).processes,
      )
  )


def delete_process(
    connection_options: channel.ConnectionOptions, process_id: str
) -> None:
  """Delete a process in the LVA.

  Args:
    connection_options: A `ConnectionOptions` targeting a specific Vertex AI
      Vision instance.
    process_id: The process ID.
  """
  stub = lva_service_pb2_grpc.LiveVideoAnalyticsStub(
      channel.create_channel(connection_options)
  )
  req = lva_service_pb2.DeleteProcessRequest(
      name="projects/{}/locations/{}/clusters/{}/processes/{}".format(
          connection_options.project_id,
          connection_options.location_id,
          connection_options.cluster_id,
          process_id,
      )
  )
  metadata = [("x-goog-request-params", "name={}".format(req.name))]
  lro = stub.DeleteProcess(request=req, metadata=metadata)
  lro_stub = operations_pb2_grpc.OperationsStub(
      channel.create_channel(connection_options)
  )
  _wait_for_lro(lro_stub, lro.name, timedelta(seconds=60))


def create_process(
    connection_options: channel.ConnectionOptions,
    analysis_id: str,
    attribute_overrides: dict[str, str] = None,
    run_mode: lva_pb2.RunMode = lva_pb2.RunMode.SUBMISSION,
    retry_count: int = 3,
    event_id: str = None,
) -> Process:
  """Create process in the LVA synchronously.

  Args:
    connection_options: A `ConnectionOptions` targeting a specific Vertex AI
      Vision instance.
    analysis_id: The analysis ID.
    attribute_overrides: A dictionary of attributes to override.
    run_mode: The process mode.
    retry_count: The number of retries for a process in submission mode the
    event_id: Event ID of the input/output streams.

  Returns:
    A `Process` object.
  """
  return asyncio.run(
      _create_process_async(
          connection_options,
          analysis_id,
          attribute_overrides,
          run_mode,
          retry_count,
          event_id,
      )
  ).result()


async def _create_process_async(
    connection_options: channel.ConnectionOptions,
    analysis_id: str,
    attribute_overrides: dict[str, str] = None,
    run_mode: lva_pb2.RunMode = lva_pb2.RunMode.SUBMISSION,
    retry_count: int = 3,
    event_id: str = None,
) -> asyncio.Task[Process]:
  """Create process in the LVA asynchronously.

  Sample usage:
  ```
  task = create_process_async(...)
  ...
  process = await task()
  ```

  Args:
    connection_options: A `ConnectionOptions` targeting a specific Vertex AI
      Vision instance.
    analysis_id: The analysis ID.
    attribute_overrides: A dictionary of attributes to override.
    run_mode: The process mode.
    retry_count: The number of retries for a process in submission mode.
    event_id: Event ID of the input/output streams.

  Returns:
    The task object for creating a `Process` object.
  """
  if not analysis_id:
    raise ValueError("analysis_id is not specified")
  stub = lva_service_pb2_grpc.LiveVideoAnalyticsStub(
      channel.create_channel(connection_options)
  )
  parent = "projects/{}/locations/{}/clusters/{}".format(
      connection_options.project_id,
      connection_options.location_id,
      connection_options.cluster_id,
  )
  process = lva_resources_pb2.Process(
      analysis="{}/analyses/{}".format(parent, analysis_id),
      attribute_overrides=[
          "{}={}".format(k, v) for (k, v) in attribute_overrides.items()
      ],
      run_mode=run_mode,
      retry_count=retry_count,
      event_id=event_id,
  )
  req = lva_service_pb2.CreateProcessRequest(
      parent=parent,
      process_id="".join(
          random.choices(string.ascii_lowercase, k=_RESOURCE_ID_LENGTH)
      ),
      process=process,
  )
  metadata = [("x-goog-request-params", f"parent={req.parent}")]
  lro = stub.CreateProcess(request=req, metadata=metadata)
  lro_stub = operations_pb2_grpc.OperationsStub(
      channel.create_channel(connection_options)
  )
  return asyncio.create_task(_wait_for_process(process, lro_stub, lro.name))


async def _wait_for_process(
    process: lva_resources_pb2.Process,
    lro_stub: operations_pb2_grpc.OperationsStub,
    op_name: str,
) -> Process:
  """Wait for the longrunning operation to be done.

  Args:
    process: Process object to be created.
    lro_stub: RPC stub for longrunning operation service.
    op_name: The operation name.

  Returns:
    The Process object.
  """
  _wait_for_lro(lro_stub, op_name, timedelta(days=1)).response.Unpack(process)
  return _resource_to_process(process)


def _wait_for_lro(
    lro_stub: operations_pb2_grpc.OperationsStub,
    op_name: str,
    timeout: timedelta,
) -> operations_pb2.Operation:
  """Wait for the longrunning operation to be done.

  Args:
    lro_stub: RPC stub for longrunning operation service.
    op_name: The operation name.
    timeout: The maximum time waiting for the LRO.

  Returns:
    The completed or error operation.

  Raises:
    ValueError: LRO returns error.
    TimeoutError: LRO is not completed within the given timeout.
  """
  metadata = [("x-goog-request-params", f"name={op_name}")]
  req = operations_pb2.GetOperationRequest(name=op_name)
  deadline = datetime.now() + timeout
  while datetime.now() < deadline:
    operation: operations_pb2.Operation = lro_stub.GetOperation(
        request=req, metadata=metadata
    )
    if not operation.done:
      time.sleep(1)
    else:
      if not operation.error:
        raise ValueError(operation.error)
      return operation
  raise TimeoutError(
      "timeout wait for operation {} after {} seconds".format(
          op_name, timeout.total_seconds()
      )
  )
