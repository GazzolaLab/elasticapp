# Distributed under the MIT License.
# See LICENSE.txt for details.

# [example_add_pybindings_test]
elastica_add_python_bindings_test(
        "Unit.Blaze.Python.Vector"
        Test_Vector.py
        "Unit;Blaze;Python"
        PyArrays)
# [example_add_pybindings_test]
elastica_add_python_bindings_test(
        "Unit.Blaze.Python.VectorView"
        Test_VectorView.py
        "Unit;Blaze;Python"
        PyArrays)

elastica_add_python_bindings_test(
        "Unit.Blaze.Python.Matrix"
        Test_Matrix.py
        "Unit;Blaze;Python"
        PyArrays)
elastica_add_python_bindings_test(
        "Unit.Blaze.Python.MatrixView"
        Test_MatrixView.py
        "Unit;Blaze;Python"
        PyArrays)

elastica_add_python_bindings_test(
        "Unit.Blaze.Python.Tensor"
        Test_Tensor.py
        "Unit;Blaze;Python"
        PyArrays)
elastica_add_python_bindings_test(
        "Unit.Blaze.Python.TensorView"
        Test_TensorView.py
        "Unit;Blaze;Python"
        PyArrays)
