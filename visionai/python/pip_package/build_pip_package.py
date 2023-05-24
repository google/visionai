# Copyright 2020 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""Build a pip package for Vertex AI Vision."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import argparse
import contextlib
import os
import shlex
import shutil
import subprocess as sp
import tempfile


@contextlib.contextmanager
def cd(dst):
  """Change directory in a context, but return to the original afterwards."""
  src = os.getcwd()
  os.chdir(os.path.expanduser(dst))
  try:
    yield
  finally:
    os.chdir(src)


def _copytree(src_dir, dst_dir):
  """Copy files from one directory to another.

  This worksaround shutil.copytree's inability to cope with existing
  destinations in python verions earlier than 3.8.

  Args:
    src_dir: The directory whose contents we want to copy from.
    dst_dir: The destination directory we want to copy contents to. It must
      already exist and be empty.
  """
  for f in os.listdir(src_dir):
    src = os.path.join(src_dir, f)
    dst = os.path.join(dst_dir, f)
    if os.path.isdir(src):
      shutil.copytree(src, dst)
    else:
      shutil.copy(src, dst)


def find_pip_package_src_reldir():
  """Find the source directory that contains the setuptools boilerplate."""
  src_reldir = os.path.dirname(
      os.path.relpath(os.path.realpath(__file__), os.getcwd())
  )
  return src_reldir


def find_runfiles_package_dir():
  """Find the bazel runfiles directory for this script."""
  pip_package_src_reldir = find_pip_package_src_reldir()
  runfiles_package_dir = os.path.dirname(__file__)
  for _ in range(len(pip_package_src_reldir.split("/"))):
    runfiles_package_dir = os.path.dirname(runfiles_package_dir)
  return runfiles_package_dir


def populate_staging_directory(staging_dir):
  """Populate the staging directory.

  Args:
    staging_dir: The path to the empty staging directory to be populated.  The
      staging directory contains all python package contents. Its root level
      contains the main module as well as all boilerplate files required by
      setuptools.
  """
  runfiles_package_dir = find_runfiles_package_dir()
  _copytree(os.path.join(runfiles_package_dir), staging_dir)
  shutil.rmtree(os.path.join(staging_dir, "external"))

  pip_package_src_reldir = find_pip_package_src_reldir()
  shutil.rmtree(os.path.join(staging_dir, pip_package_src_reldir))

  for f in os.listdir(pip_package_src_reldir):
    src = os.path.join(pip_package_src_reldir, f)
    dst = os.path.join(staging_dir, f)
    shutil.copy(src, dst)


def build_pip_wheel(staging_dir):
  """Create a pip wheel from an empty staging directory."""

  # Populate the staging directory.
  populate_staging_directory(staging_dir)

  # Build the pip wheel.
  with cd(staging_dir):
    sp.check_call(shlex.split("python3 setup.py bdist_wheel"))

  # Copy the pip wheel to where this script is run.
  whl_abs_path = None
  dist_dir_path = os.path.join(staging_dir, "dist")
  for f in os.listdir(dist_dir_path):
    if f.endswith(".whl"):
      whl_abs_path = os.path.join(dist_dir_path, f)
  shutil.copy(whl_abs_path, os.getcwd())


def main(args):
  if args.scratch_dir is not None:
    if os.path.isdir(args.scratch_dir):
      raise FileExistsError("{} already exists.".format(args.scratch_dir))
    build_pip_wheel(args.scratch_dir)
  else:
    with tempfile.TemporaryDirectory() as scratch_dir:
      build_pip_wheel(scratch_dir)


if __name__ == "__main__":
  parser = argparse.ArgumentParser(
      description="Build the Vertex AI Vision pip package."
  )
  parser.add_argument(
      "--scratch-dir", type=str, default=None, help="The scratch directory"
  )
  parsed_args = parser.parse_args()
  main(parsed_args)
