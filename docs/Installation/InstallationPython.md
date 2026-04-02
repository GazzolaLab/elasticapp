\cond NEVER Distributed under the MIT License. See LICENSE.txt for details. \endcond

# Installation of Python-bound case studies {#python_installation}

To generate and install the Python bindings for case studies:

1. Configure and build the project with `ELASTICA_BUILD_PYTHON_BINDINGS=ON`.
2. Change into the build tree's `bin/python` directory.
3. Run `python3 -m pip install --editable .`.

This installs the `elasticapp` package in editable mode. Add `--user` if you want
to install it for only the current user.
