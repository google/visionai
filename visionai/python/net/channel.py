# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

"""Library for creating authenticated gRPC channels."""

import dataclasses
import enum
import logging

_logger = logging.getLogger(__name__)


class Environment(enum.Enum):
  AUTOPUSH = 1
  STAGING = 2
  PROD = 3


@dataclasses.dataclass
class ConnectionOptions:
  project_id: str
  location_id: str
  cluster_id: str
  env: Environment = Environment.PROD


def get_service_endpoint(env: Environment) -> str:
  """Gets the visionai endpoint according to specified environment.

  Args:
    env: Environment, either AUTOPUSH, STAGING OR PROD.

  Returns:
    The visionai endpoint to talk to.
  """
  if env == Environment.AUTOPUSH:
    return 'autopush-visionai.sandbox.googleapis.com'
  if env == Environment.STAGING:
    return 'staging-visionai.sandbox.googleapis.com'
  if env == Environment.PROD:
    return 'visionai.googleapis.com'

  _logger.error('Unsupported environment %s', env)
  raise ValueError('Invalid environment.')


# TODO(zhangxiaotian): switch to regional endpoint when it is ready.
def get_warehouse_service_endpoint(env: Environment) -> str:
  """Gets the warehouse endpoint according to specified environment.

  Args:
    env: Environment, either AUTOPUSH, STAGING OR PROD.

  Returns:
    The warehouse endpoint to talk to.
  """
  if env == Environment.AUTOPUSH:
    return 'autopush-warehouse-visionai.sandbox.googleapis.com'
  if env == Environment.STAGING:
    return 'staging-warehouse-visionai.sandbox.googleapis.com'
  if env == Environment.PROD:
    return 'warehouse-visionai.googleapis.com'

  _logger.error('Unsupported environment %s', env)
  raise ValueError('Invalid environment.')
