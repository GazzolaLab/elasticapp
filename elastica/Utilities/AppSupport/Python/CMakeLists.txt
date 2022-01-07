# Distributed under the MIT License.
# See LICENSE.txt for details.

include(SetupPybind11)
pybind11_add_module(PyBindings Bindings.cpp BlazeVector.cpp BlazeMatrix.cpp BlazeTensor.cpp)
target_link_libraries(PyBindings PRIVATE Blaze BlazeTensor Utilities)