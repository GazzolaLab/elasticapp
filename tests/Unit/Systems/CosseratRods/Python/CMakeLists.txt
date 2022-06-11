# Distributed under the MIT License.
# See LICENSE.txt for details.

elastica_add_python_bindings_test(
        "Unit.Systems.Python.CosseratRod"
        Test_CosseratRod.py
        "Unit;Systems;Python"
        PyCosseratRods)

elastica_add_python_bindings_test(
        "Unit.Systems.Python.CosseratRodWithoutDamping"
        Test_CosseratRodWithoutDamping.py
        "Unit;Systems;Python"
        PyCosseratRods)
