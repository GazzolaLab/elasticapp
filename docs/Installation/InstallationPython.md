\cond NEVER Distributed under the MIT License. See LICENSE.txt for details. \endcond

# Installation of Py-binded case-studies {#python_installation}

In order to generate and install the Python bindings for case-studies make sure to run the following steps:

1. Compile the case study using Cmake.
2. Go to the `cmake-build` folder.
3. Go to `bin/python`
4. Run the command: `python3 -m pip install --editable .`

This should install a package called `elasticapp` system-wide in editable mode (https://stackoverflow.com/a/30306403).
You can also add `--user` in case you want to install it for a particular user.