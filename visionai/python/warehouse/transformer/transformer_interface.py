# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
"""Base class of the warehouse transformers.

The transformer interface to interact with warehouse.
"""

import abc
from visionai.python.gapic.visionai import visionai_v1
from visionai.python.warehouse.transformer.transform_progress import TransformProgress


class TransformerInterface(metaclass=abc.ABCMeta):
  """Base class of the transformers.

  Subclasses require:
  transform: transforms the given resource. Returns TransformProgress for
  polling status.
  """

  def __init__(
      self,
      warehouse_client: visionai_v1.WarehouseClient,
  ):
    """Constructor.

    Args:
      warehouse_client: the client to talk to warehouse.
    """
    self.warehouse_client = warehouse_client

  @abc.abstractmethod
  def initialize(self) -> None:
    """Initialize the transformer."""

  @abc.abstractmethod
  def transform(self, resource_id: str) -> TransformProgress:
    """Perform transform for the given resource.

    Args:
      resource_id: the resource that this transform will operate on.

    Returns:
      TransformProgress used to poll status for the transformation.
    """

  def teardown(self) -> bool:
    """Cleans up resources."""
    return True
