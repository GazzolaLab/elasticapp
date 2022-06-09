# Distributed under the MIT License.
# See LICENSE.txt for details.

import inspect
import unittest

from elasticapp.Systems.Tags import _PyTags


class TestTags(unittest.TestCase):
    def test_tags(self):
        # smh, whats the point of metaprogramming :/
        all_tags = [
            "Acceleration",
            "AngularAcceleration",
            "AngularVelocity",
            "BendingTwistRigidityMatrix",
            "Curvature",
            "Director",
            "ElementDilatation",
            "ElementDimension",
            "ElementLength",
            "ElementVolume",
            "ExternalLoads",
            "ExternalTorques",
            "ForceDampingRate",
            "InternalCouple",
            "InternalLoads",
            "InternalStress",
            "InternalTorques",
            "InvMassSecondMomentOfInertia",
            "Mass",
            "MassSecondMomentOfInertia",
            "Material",
            "NElement",
            "Position",
            "Radius",
            "ReferenceCurvature",
            "ReferenceElementLength",
            "ReferenceShearStretchStrain",
            "ReferenceVoronoiLength",
            "ShearStretchRigidityMatrix",
            "ShearStretchStrain",
            "Tangent",
            "TorqueDampingRate",
            "Velocity",
            "VoronoiDilatation",
            "VoronoiLength",
        ]
        all_tags = map(lambda x: "_" + x, all_tags)

        def test_tag(tag):
            with self.subTest(tag=tag):
                self.assertEqual(
                    len(
                        tuple(
                            filter(lambda x: x[0] == tag, inspect.getmembers(_PyTags))
                        )
                    ),
                    1,
                )
            return True

        # force evaluate
        test_results = list(map(test_tag, all_tags))


if __name__ == "__main__":
    unittest.main(verbosity=2)
