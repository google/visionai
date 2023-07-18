import sys
import pytest

# if using 'bazel test ...'
if __name__ == "__main__":
  sys.exit(pytest.main(["-rA", "--color=yes", "-x"] + sys.argv[1:]))
