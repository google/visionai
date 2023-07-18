# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

"""Base client library for the LVA service."""

import datetime
import random
import string
import time
from typing import Dict, List

from google.api_core import client_options as client_options_lib
from google.api_core import gapic_v1
from google.api_core.exceptions import NotFound

from visionai.python.gapic.visionai import visionai_v1
from visionai.python.lva import graph
from visionai.python.net import channel


_RESOURCE_ID_LENGTH = 8


def _create_lva_client(
    connection_options: channel.ConnectionOptions,
) -> visionai_v1.LiveVideoAnalyticsClient:
  return visionai_v1.LiveVideoAnalyticsClient(
      client_options=client_options_lib.ClientOptions(
          api_endpoint=channel.get_service_endpoint(connection_options.env),
      ),
      transport="grpc",
  )


class Analysis:
  """Analysis object abstracts the analysis resource in the service."""

  def __init__(self, analysis: visionai_v1.Analysis):
    self._analysis = analysis

  @property
  def analysis_id(self) -> str:
    return self._analysis.name.split("/")[-1]

  @property
  def name(self) -> str:
    return self._analysis.name

  @property
  def create_time(self) -> datetime.datetime:
    return datetime.datetime.fromtimestamp(self._analysis.create_time.seconds)

  @property
  def update_time(self) -> datetime.datetime:
    return datetime.datetime.fromtimestamp(self._analysis.update_time.seconds)

  @property
  def labels(self) -> Dict[str, str]:
    return self._analysis.labels

  @property
  def analysis_definition(self) -> visionai_v1.AnalysisDefinition:
    return self._analysis.analysis_definition

  @property
  def input_streams_mapping(self) -> Dict[str, str]:
    return self._analysis.input_streams_mapping

  @property
  def output_streams_mapping(self) -> Dict[str, str]:
    return self._analysis.output_streams_mapping

  @property
  def disable_event_watch(self) -> bool:
    return self._analysis.disable_event_watch

  def __eq__(self, other) -> bool:
    return other._analysis == self._analysis


class Process:
  """Process objects abstract process resources in the service."""

  _STATE_MAPPING = {
      0: "STATE_UNSPECIFIED",
      1: "INITIALIZING",
      2: "RUNNING",
      3: "COMPLETED",
      4: "FAILED",
      5: "PENDING",
  }

  def __init__(self, process: visionai_v1.Process):
    self._process = process

  @property
  def process_id(self) -> str:
    return self._process.name.split("/")[-1]

  @property
  def name(self) -> str:
    return self._process.name

  @property
  def create_time(self) -> datetime.datetime:
    return datetime.datetime.fromtimestamp(self._process.create_time.seconds)

  @property
  def update_time(self) -> datetime.datetime:
    return datetime.datetime.fromtimestamp(self._process.update_time.seconds)

  @property
  def analysis(self) -> str:
    return self._process.analysis

  @property
  def attribute_overrides(self) -> List[str]:
    return self._process.attribute_overrides

  @property
  def run_status(self) -> visionai_v1.RunStatus:
    return self._process.run_status

  @property
  def run_mode(self) -> visionai_v1.RunMode:
    return self._process.run_mode

  @property
  def event_id(self) -> str:
    return self._process.event_id

  @property
  def batch_id(self) -> str:
    return self._process.batch_id

  @property
  def retry_count(self) -> int:
    return self._process.retry_count

  @property
  def run_status_state(self) -> str:
    return self._STATE_MAPPING[self._process.run_status.state]

  def is_completed(self) -> bool:
    return self.run_status.state == visionai_v1.RunStatus.State.COMPLETED

  def is_failed(self) -> bool:
    return self.run_status.state == visionai_v1.RunStatus.State.FAILED

  def __eq__(self, other) -> bool:
    return other._process == self._process

  def wait(
      self,
      connection_options: channel.ConnectionOptions,
      timeout: datetime.timedelta = datetime.timedelta(days=1),
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
    client = _create_lva_client(connection_options)
    req = visionai_v1.GetProcessRequest(name=self.name)
    deadline = datetime.datetime.now() + timeout
    while datetime.datetime.now() < deadline:
      process = client.get_process(request=req)
      state = process.run_status.state
      if (
          state == visionai_v1.RunStatus.State.COMPLETED
          or state == visionai_v1.RunStatus.State.FAILED
      ):
        return Process(process)
      time.sleep(5)
    raise TimeoutError(
        "timeout wait for process {} after {} seconds".format(
            process.name, timeout.total_seconds()
        )
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
  client = _create_lva_client(connection_options)
  req = visionai_v1.GetAnalysisRequest(
      name="projects/{}/locations/{}/clusters/{}/analyses/{}".format(
          connection_options.project_id,
          connection_options.location_id,
          connection_options.cluster_id,
          analysis_id,
      )
  )
  return Analysis(client.get_analysis(request=req))


def delete_analysis(
    connection_options: channel.ConnectionOptions, analysis_id: str
) -> None:
  """Delete an analysis in the LVA.

  Args:
    connection_options: A `ConnectionOptions` targeting a specific Vertex AI
      Vision instance.
    analysis_id: The analysis ID.
  """
  client = _create_lva_client(connection_options)
  req = visionai_v1.DeleteAnalysisRequest(
      name="projects/{}/locations/{}/clusters/{}/analyses/{}".format(
          connection_options.project_id,
          connection_options.location_id,
          connection_options.cluster_id,
          analysis_id,
      )
  )
  client.delete_analysis(request=req).result()


def list_analyses(
    connection_options: channel.ConnectionOptions,
) -> List[Analysis]:
  """List analyses in the LVA.

  Args:
    connection_options:  A `ConnectionOptions` targeting a specific Vertex AI
      Vision instance.

  Returns:
    A list of `Analysis` objects.
  """
  client = _create_lva_client(connection_options)
  req = visionai_v1.ListAnalysesRequest(
      parent="projects/{}/locations/{}/clusters/{}".format(
          connection_options.project_id,
          connection_options.location_id,
          connection_options.cluster_id,
      )
  )
  return [Analysis(a) for a in list(client.list_analyses(request=req))]


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
  client = _create_lva_client(connection_options)
  analysis = visionai_v1.Analysis(
      analysis_definition=g.get_analysis_definition(),
      disable_event_watch=disable_event_watch,
  )
  req = visionai_v1.CreateAnalysisRequest(
      parent="projects/{}/locations/{}/clusters/{}".format(
          connection_options.project_id,
          connection_options.location_id,
          connection_options.cluster_id,
      ),
      analysis_id=analysis_id,
      analysis=analysis,
  )
  return Analysis(client.create_analysis(request=req).result())


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
  except NotFound:
    return create_analysis(
        connection_options, g, analysis_id, disable_event_watch
    )


def get_process(
    connection_options: channel.ConnectionOptions,
    process_id: str,
    retry=gapic_v1.method.DEFAULT,
) -> Process:
  """Get a process in the LVA.

  Args:
    connection_options:  A `ConnectionOptions` targeting a specific Vertex AI
      Vision instance.
    process_id: The process ID.
    retry: retry policy when GetProcess.

  Returns:
    A `Process` object.
  """
  client = _create_lva_client(connection_options)
  req = visionai_v1.GetProcessRequest(
      name="projects/{}/locations/{}/clusters/{}/processes/{}".format(
          connection_options.project_id,
          connection_options.location_id,
          connection_options.cluster_id,
          process_id,
      )
  )
  return Process(client.get_process(request=req, retry=retry))


def list_processes(
    connection_options: channel.ConnectionOptions,
    list_filter: str = None,
) -> List[Process]:
  """List processes in the LVA.

  Args:
    connection_options:  A `ConnectionOptions` targeting a specific Vertex AI
      Vision instance.
    list_filter: the filter for listing processes.
  Returns:
    A list of `Process` objects.
  """
  client = _create_lva_client(connection_options)
  req = visionai_v1.ListProcessesRequest(
      parent="projects/{}/locations/{}/clusters/{}".format(
          connection_options.project_id,
          connection_options.location_id,
          connection_options.cluster_id,
      )
  )
  if list_filter:
    req.filter = list_filter
  return [Process(p) for p in list(client.list_processes(request=req))]


def delete_process(
    connection_options: channel.ConnectionOptions, process_id: str
) -> None:
  """Delete a process in the LVA.

  Args:
    connection_options: A `ConnectionOptions` targeting a specific Vertex AI
      Vision instance.
    process_id: The process ID.
  """
  client = _create_lva_client(connection_options)
  req = visionai_v1.DeleteProcessRequest(
      name="projects/{}/locations/{}/clusters/{}/processes/{}".format(
          connection_options.project_id,
          connection_options.location_id,
          connection_options.cluster_id,
          process_id,
      )
  )
  client.delete_process(request=req).result()


def create_process(
    connection_options: channel.ConnectionOptions,
    analysis_id: str,
    attribute_overrides: Dict[str, str] = None,
    run_mode: visionai_v1.RunMode = visionai_v1.RunMode.SUBMISSION,
    retry_count: int = 3,
    event_id: str = None,
) -> Process:
  """Create process in the LVA.

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
  client = _create_lva_client(connection_options)
  parent = "projects/{}/locations/{}/clusters/{}".format(
      connection_options.project_id,
      connection_options.location_id,
      connection_options.cluster_id,
  )
  process = visionai_v1.Process(
      analysis="{}/analyses/{}".format(parent, analysis_id),
      attribute_overrides=[
          "{}={}".format(k, v) for (k, v) in attribute_overrides.items()
      ],
      run_mode=run_mode,
      retry_count=retry_count,
      event_id=event_id,
  )
  req = visionai_v1.CreateProcessRequest(
      parent=parent,
      process_id="".join(
          random.choices(string.ascii_lowercase, k=_RESOURCE_ID_LENGTH)
      ),
      process=process,
  )
  return Process(client.create_process(request=req).result())
