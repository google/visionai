# Copyright 2023 Google LLC
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
"""Vertex AI Vision Python SDK."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import setuptools

VERSION = "0.0.5"
REQUIRED_PACKAGES = []
with open("requirements.txt", "r") as f:
  REQUIRED_PACKAGES = f.read().splitlines()

CONSOLE_SCRIPTS = []

setuptools.setup(
    name="visionai",
    version=VERSION,
    author="Google Inc.",
    author_email="",
    description="Vertex AI Vision Python SDK",
    long_description="Vertex AI Vision Python SDK",
    long_description_content_type="text/markdown",
    url="https://cloud.google.com/vision-ai/docs/overview",
    download_url="https://github.com/google/visionai",
    packages=setuptools.find_packages(),
    install_requires=REQUIRED_PACKAGES,
    entry_points={"console_scripts": CONSOLE_SCRIPTS},
    include_package_data=True,
    classifiers=[
        "Programming Language :: Python :: 3",
    ],
    python_requires=">=3.6",
)
