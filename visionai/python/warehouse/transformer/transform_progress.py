# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""TransformProgress is used for polling status for transformations.

TransformProgress is a type of google.api_core.PollingFuture.
"""

import abc
import logging
import threading
from typing import Sequence
from google.api_core import exceptions
from google.api_core import operation
from google.api_core import retry as retry_lib
from google.api_core.exceptions import GoogleAPICallError
from google.api_core.future import polling as polling_lib
from visionai.python.gapic.visionai import visionai_v1
from visionai.python.lva import client


_logger = logging.getLogger(__name__)

# Timeout in seconds to wait for the transform progress.
DEFAULT_TIMEOUT = 7200

# Default retry when the exceptions in the list happens, this controls the retry
# behavior for the polling RPC.
DEFAULT_RETRY = retry_lib.Retry(
    initial=1.0,
    maximum=120.0,
    multiplier=2.5,
    predicate=retry_lib.if_exception_type(
        exceptions.DeadlineExceeded,
        exceptions.ServiceUnavailable,
        exceptions.ResourceExhausted,
        exceptions.TooManyRequests,
        exceptions.InternalServerError,
        exceptions.BadGateway,
    ),
    deadline=120.0,
)

# Default polling configuration, this controls the behavior of polling. See
# https://googleapis.dev/python/google-api-core/latest/_modules/google/api_core/future/polling.html
# for more info.
DEFAULT_POLLING = retry_lib.Retry(
    predicate=polling_lib.RETRY_PREDICATE,
    initial=60,  # seconds
    maximum=600,  # seconds
    multiplier=2,
    deadline=7200,  # seconds
)


class TransformError(Exception):
  """Exception raised from TransformProgress.result()."""

  def __init__(self, identifier: str, error_message: str):
    self._identifier = identifier
    self._error_message = error_message

  def __str__(self):
    return self._identifier + ": " + self._error_message


class TransformProgress(polling_lib.PollingFuture):
  """Base class of the TransformProgress.

  Subclasses requires:

  get_identifier: gets the identifier for the transform.
  """

  @abc.abstractmethod
  def get_identifier(self) -> str:
    """Gets an identifer for this transform."""

  def result(
      self,
      timeout=DEFAULT_TIMEOUT,
      retry=DEFAULT_RETRY,
  ):
    try:
      return super().result(timeout=timeout, retry=retry)
    except GoogleAPICallError as err:
      raise TransformError(self.get_identifier(), err) from err


class LroTransformProgress(TransformProgress):
  """LroTransformProgress is helper class to get state or wait for long-running operations.

  Attributes:
    lro: Future to interact with Google long-running operations
  """

  def __init__(self, lro: operation.Operation):
    """Constructor.

    Args:
      lro: The lro that the transform about.
    """
    super().__init__(retry=DEFAULT_POLLING)
    self.lro = lro

  def get_identifier(self) -> str:
    return "LroTransformProgress(%s)" % self.lro.operation.name

  def done(self, retry=DEFAULT_RETRY):
    return self.lro.done(retry=retry)

  def result(self, timeout=DEFAULT_TIMEOUT, retry=DEFAULT_RETRY):
    try:
      r = self.lro.result(timeout=timeout, retry=retry)
    except GoogleAPICallError as err:
      raise TransformError(self.get_identifier(), err.message) from err
    else:
      return r

  def cancel(self):
    self.lro.cancel()

  def cancelled(self):
    return self.lro.cancelled()

  def metadata(self):
    return self.lro.metadata

  def operation(self):
    return self.lro.operation

  def response(self):
    if self.lro.operation.HasField("response"):
      return self.lro.operation.response
    else:
      return None


class SpeechTransformProgress(TransformProgress):
  """The future for the speech transcription transform."""

  def __init__(self, annotate_video_lro: operation.Operation):
    super().__init__(retry=DEFAULT_POLLING)
    self._annotate_video_lro = annotate_video_lro

  def done(self, retry=None):
    """Transform is done when either result or exception is set."""
    return self._exception is not None or self._result is not None

  def cancel(self):
    if self.done():
      return False
    self._annotate_video_lro.cancel()
    self._cancelled = True

  def cancelled(self):
    return self._annotate_video_lro.cancelled() and self._cancelled

  def get_identifier(self) -> str:
    return (
        "SpeechTransformProgress(%s And write to warehouse)"
        % self._annotate_video_lro.operation.name
    )


class LvaTransformProgress(TransformProgress):
  """The future to poll Lva process state."""

  def __init__(self, connection_options, process_id: str):
    super().__init__(retry=DEFAULT_POLLING)
    self._connection_options = connection_options
    self._process_id = process_id
    self._completion_lock = threading.Lock()

  def done(self, retry=DEFAULT_RETRY):
    """Transform is done when process state is completed or failed."""
    process = client.get_process(
        self._connection_options, self._process_id, retry=retry
    )
    with self._completion_lock:
      if process.is_completed():
        self.set_result(process)
        return True
      if process.is_failed():
        self.set_exception(
            TransformError(
                self.get_identifier(),
                "failed with reason :%s" % process.run_status.reason,
            )
        )
        return True
    return False

  def get_identifier(self) -> str:
    return "LvaTransformProgress(%s)" % self._process_id

  # Cancel is not supported.
  def cancel(self):
    return False

  # Cancel is not supported.
  def cancelled(self):
    return False


class CombinedTransformProgress(TransformProgress):
  """Given a list of transform progress, waits for all of them finishes."""

  def __init__(self, transform_progresses: Sequence[TransformProgress]):
    """Constructor.

    Args:
      transform_progresses: A list of transform progresses.
    """
    super().__init__(retry=DEFAULT_POLLING)
    self._transform_progresses = transform_progresses
    self._completion_lock = threading.Lock()

  def done(self, retry=DEFAULT_RETRY):
    for transform_progress in self._transform_progresses:
      if not transform_progress.done(retry=retry):
        _logger.info("%s not done", transform_progress.get_identifier())
        return False
      else:
        _logger.info("%s done", transform_progress.get_identifier())

    _logger.info(
        "CombinedTransformProgress [ %s ] is done", self.get_identifier()
    )
    result = {}
    # This must be done in a lock to prevent the polling thread
    # and main thread from both executing the completion logic
    # at the same time.
    with self._completion_lock:
      try:
        for transform_progress in self._transform_progresses:
          result[transform_progress.get_identifier()] = (
              transform_progress.result()
          )
      except TransformError as err:
        self.set_exception(err)
      else:
        self.set_result(result)
    return True

  def cancel(self):
    if self.done():
      return False
    for op in self._operations:
      if op.done():
        continue
      op.cancel()

  def cancelled(self):
    for op in self._operations:
      if not op.cancelled():
        return False

  def get_identifier(self) -> str:
    id_str = ""
    for transform_progress in self._transform_progresses:
      id_str += transform_progress.get_identifier()
      id_str += "/n"
    return id_str
