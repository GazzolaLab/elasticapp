# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_EnvironmentsPrimitives")

set(LIBRARY_SOURCES
        Test_CapsulePrimitiveBase.cpp
        Test_CapsulePrimitive.cpp
        Test_CapsuleFromRodPrimitive.cpp
        Test_CylinderPrimitive.cpp
        Test_PlanePrimitive.cpp
        Test_PrimitivesCollection.cpp
        Test_RigidBodyPrimitive.cpp
        Test_SpherePrimitive.cpp
        Test_TypeTraits.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Environments/Primitives"
        "${LIBRARY_SOURCES}"
        "Primitives;Utilities;Materials"
)
