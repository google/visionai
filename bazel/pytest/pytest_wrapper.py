# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

import sys
import pytest

# if using 'bazel test ...'
if __name__ == "__main__":
  sys.exit(pytest.main(["-rA", "--color=yes", "-x"] + sys.argv[1:]))
