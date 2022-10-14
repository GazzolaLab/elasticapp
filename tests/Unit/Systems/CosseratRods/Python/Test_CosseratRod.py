# Distributed under the MIT License.
# See LICENSE.txt for details.

import unittest

import numpy as np
import numpy.testing as npt
from elasticapp.Arrays import Vector, Matrix, Tensor
from elasticapp.Systems import CosseratRods  # noqa


class TestCosseratRod(unittest.TestCase):
    def get_a_block(self):
        # This is something internal that is not part of the
        # public API but something that I need for testing temporarily
        CRB = CosseratRods._PyCosseratRods._CosseratRodBlock
        return CRB(n_rods=4, n_elems=10)

    def test_block(self):
        crb = self.get_a_block()
        # why test this?
        self.assertEqual(len(crb), 4 * 10 + (4 - 1) * 2)

    def test_length(self):
        crb = self.get_a_block()

        # slices
        for i in range(4):
            rod = crb[i]
            self.assertEqual(len(rod), 10)

        # sub-slices equaling a slice
        for i in range(4):
            rod = crb[i: i + 1]
            self.assertEqual(len(rod), 10)

        # proper sub-slices
        for i in range(2):
            rod = crb[i: i + 2]
            self.assertEqual(len(rod), 2 * 10 + 2)

        # slices of sub-slices
        for i in range(2):
            rod = crb[i: i + 3]
            self.assertEqual(len(rod), 3 * 10 + 2 * 2)
            arod = rod[0:2]  # has rod 0, 1, 2
            self.assertEqual(type(arod), type(rod))
            self.assertEqual(len(arod), 2 * 10 + 2)
            rod = arod[0]  # has rod 0, 1, 2
            self.assertEqual(len(rod), 10)

    def test_member_access(self):
        def setup_test(node_size=11, element_size=10, voronoi_size=9):
            def assert_is_nodal(x):
                self.assertEqual(x.shape[-1], node_size)
                return True

            def assert_is_elemental(x):
                self.assertEqual(x.shape[-1], element_size)
                return True

            def assert_is_voronoi(x):
                self.assertEqual(x.shape[-1], voronoi_size)
                return True

            def assert_is_scalar(x):
                self.assertEqual(len(x.shape), 1)
                return True

            def assert_is_vector(x):
                self.assertEqual(len(x.shape), 2)
                self.assertEqual(x.shape[0], 3)
                return True

            def assert_is_matrix(x):
                self.assertEqual(len(x.shape), 3)
                self.assertTupleEqual(x.shape[:2], (3, 3))
                return True

            def run_test(rod):
                # rod variables
                assert_is_scalar(rod.get_n_element())
                # Turned off because we also test views now
                # self.assertTupleEqual(rod.get_n_element().shape, (1,))
                self.assertEqual(rod.get_n_element()[0], 10)

                # geometrical variables
                assert_is_nodal(rod.get_position()) and assert_is_vector(
                    rod.get_position()
                )
                assert_is_elemental(rod.get_director()) and assert_is_matrix(
                    rod.get_director()
                )
                assert_is_elemental(rod.get_element_dilatation()) and assert_is_scalar(
                    rod.get_element_dilatation()
                )
                assert_is_elemental(rod.get_element_dimension()) and assert_is_scalar(
                    rod.get_element_dimension()
                )
                assert_is_elemental(rod.get_element_length()) and assert_is_scalar(
                    rod.get_element_length()
                )
                assert_is_elemental(rod.get_element_volume()) and assert_is_scalar(
                    rod.get_element_volume()
                )
                assert_is_elemental(
                    rod.get_reference_element_length()
                ) and assert_is_scalar(rod.get_reference_element_length())
                assert_is_elemental(
                    rod.get_reference_shear_stretch_strain()
                ) and assert_is_vector(rod.get_reference_shear_stretch_strain())
                assert_is_elemental(
                    rod.get_shear_stretch_strain()
                ) and assert_is_vector(rod.get_shear_stretch_strain())
                assert_is_elemental(rod.get_tangent()) and assert_is_vector(
                    rod.get_tangent()
                )
                assert_is_voronoi(rod.get_curvature()) and assert_is_vector(
                    rod.get_curvature()
                )
                assert_is_voronoi(rod.get_reference_curvature()) and assert_is_vector(
                    rod.get_reference_curvature()
                )
                assert_is_voronoi(
                    rod.get_reference_voronoi_length()
                ) and assert_is_scalar(rod.get_reference_voronoi_length())
                assert_is_voronoi(rod.get_voronoi_dilatation()) and assert_is_scalar(
                    rod.get_voronoi_dilatation()
                )
                assert_is_voronoi(rod.get_voronoi_length()) and assert_is_scalar(
                    rod.get_voronoi_length()
                )

                # elasticity variables
                assert_is_nodal(rod.get_force_damping_rate()) and assert_is_scalar(
                    rod.get_force_damping_rate()
                )
                assert_is_nodal(rod.get_internal_loads()) and assert_is_vector(
                    rod.get_internal_loads()
                )
                assert_is_elemental(rod.get_internal_torques()) and assert_is_vector(
                    rod.get_internal_torques()
                )
                assert_is_elemental(rod.get_internal_stress()) and assert_is_vector(
                    rod.get_internal_stress()
                )
                assert_is_elemental(
                    rod.get_shear_stretch_rigidity_matrix()
                ) and assert_is_vector(rod.get_shear_stretch_rigidity_matrix())
                assert_is_elemental(rod.get_torque_damping_rate()) and assert_is_scalar(
                    rod.get_torque_damping_rate()
                )
                assert_is_voronoi(rod.get_internal_couple()) and assert_is_vector(
                    rod.get_internal_couple()
                )
                assert_is_voronoi(
                    rod.get_bending_twist_rigidity_matrix()
                ) and assert_is_vector(rod.get_bending_twist_rigidity_matrix())

                # dynamic variables
                assert_is_nodal(rod.get_acceleration()) and assert_is_vector(
                    rod.get_acceleration()
                )
                assert_is_nodal(rod.get_external_loads()) and assert_is_vector(
                    rod.get_external_loads()
                )
                assert_is_nodal(rod.get_mass()) and assert_is_scalar(rod.get_mass())
                assert_is_nodal(rod.get_velocity()) and assert_is_vector(
                    rod.get_velocity()
                )
                assert_is_elemental(rod.get_angular_velocity()) and assert_is_vector(
                    rod.get_angular_velocity()
                )
                assert_is_elemental(
                    rod.get_angular_acceleration()
                ) and assert_is_vector(rod.get_angular_acceleration())
                assert_is_elemental(rod.get_external_torques()) and assert_is_vector(
                    rod.get_external_torques()
                )
                assert_is_elemental(
                    rod.get_mass_second_moment_of_inertia()
                ) and assert_is_matrix(rod.get_mass_second_moment_of_inertia())

                assert_is_elemental(rod.get_material()) and assert_is_scalar(
                    rod.get_material()
                )

                assert_is_elemental(
                    rod.get_inv_mass_second_moment_of_inertia()
                ) and assert_is_vector(rod.get_inv_mass_second_moment_of_inertia())

            return run_test

        crb = self.get_a_block()

        run_slice_test = setup_test()
        for slice_idx in range(0, 4):
            with self.subTest(slice_idx=slice_idx):
                run_slice_test(crb[slice_idx])

        slice_size = 2
        element_size = slice_size * 10 + (slice_size - 1) * 2
        run_view_test = setup_test(
            node_size=element_size + 1,
            element_size=element_size,
            voronoi_size=element_size - 1,
        )
        for view_idx in range(0, 2):
            with self.subTest(view_idx=view_idx):
                view = crb[view_idx: view_idx + slice_size]  # 0, 1, 2 & # 1, 2, 3
                run_view_test(view)

    def test_variable_locator(self):
        is_well_formed = lambda x: self.assertIsInstance(x.shape, tuple)

        def run_test(rod):
            all_tags = [
                rod.Acceleration,
                rod.AngularAcceleration,
                rod.AngularVelocity,
                rod.BendingTwistRigidityMatrix,
                rod.Curvature,
                rod.Director,
                rod.ElementDilatation,
                rod.ElementDimension,
                rod.ElementLength,
                rod.ElementVolume,
                rod.ExternalLoads,
                rod.ExternalTorques,
                rod.ForceDampingRate,
                rod.InternalCouple,
                rod.InternalLoads,
                rod.InternalStress,
                rod.InternalTorques,
                rod.InvMassSecondMomentOfInertia,
                rod.Mass,
                rod.MassSecondMomentOfInertia,
                rod.Material,
                rod.NElement,
                rod.Position,
                rod.ReferenceCurvature,
                rod.ReferenceElementLength,
                rod.ReferenceShearStretchStrain,
                rod.ReferenceVoronoiLength,
                rod.ShearStretchRigidityMatrix,
                rod.ShearStretchStrain,
                rod.Tangent,
                rod.TorqueDampingRate,
                rod.Velocity,
                rod.VoronoiDilatation,
                rod.VoronoiLength,
            ]

            return (is_well_formed(rod.loc[tag]) for tag in all_tags)

        crb = self.get_a_block()
        for slice_idx in range(0, 4):
            with self.subTest(slice_idx=slice_idx):
                run_test(crb[slice_idx])

        slice_size = 2
        for view_idx in range(0, 2):
            with self.subTest(view_idx=view_idx):
                run_test(crb[view_idx: view_idx + slice_size])

    def test_variable_location_settable(self):
        def setup_test(node_size=11, element_size=10, voronoi_size=9):
            def run_test(rod):
                rod.loc[rod.Position] = Matrix(2.0 * np.ones((3, node_size)))
                rod.loc[rod.Director] = Tensor(3.0 * np.ones((3, 3, element_size)))
                rod.loc[rod.VoronoiLength] = Vector(
                    4.0
                    * np.ones(
                        voronoi_size,
                    )
                )

                # The buffer should be modified after tests
                npt.assert_allclose(np.asarray(rod.get_position()), 2.0)
                npt.assert_allclose(np.asarray(rod.get_director()), 3.0)
                npt.assert_allclose(np.asarray(rod.get_voronoi_length()), 4.0)

            return run_test

        crb = self.get_a_block()
        run_slice_test = setup_test()
        for slice_idx in range(0, 4):
            with self.subTest(slice_idx=slice_idx):
                run_slice_test(crb[slice_idx])

        slice_size = 2
        element_size = slice_size * 10 + (slice_size - 1) * 2
        run_view_test = setup_test(
            node_size=element_size + 1,
            element_size=element_size,
            voronoi_size=element_size - 1,
        )
        for view_idx in range(0, 2):
            with self.subTest(view_idx=view_idx):
                run_view_test(crb[view_idx: view_idx + slice_size])

    def test_pyelastica_interface(self):
        is_well_formed = lambda x: self.assertIsInstance(x.shape, tuple)

        def run_test(rod):
            is_well_formed(rod.n_elems)
            is_well_formed(rod.position_collection)
            is_well_formed(rod.velocity_collection)
            is_well_formed(rod.acceleration_collection)
            is_well_formed(rod.omega_collection)
            is_well_formed(rod.alpha_collection)
            is_well_formed(rod.director_collection)
            is_well_formed(rod.rest_lengths)
            is_well_formed(rod.material)
            is_well_formed(rod.volume)
            is_well_formed(rod.mass)
            is_well_formed(rod.mass_second_moment_of_inertia)
            is_well_formed(rod.inv_mass_second_moment_of_inertia)
            is_well_formed(rod.dissipation_constant_for_forces)
            is_well_formed(rod.dissipation_constant_for_torques)
            is_well_formed(rod.rest_voronoi_lengths)
            is_well_formed(rod.internal_forces)
            is_well_formed(rod.internal_torques)
            is_well_formed(rod.external_forces)
            is_well_formed(rod.external_torques)
            is_well_formed(rod.lengths)
            is_well_formed(rod.tangents)
            is_well_formed(rod.radius)
            is_well_formed(rod.dilatation)
            is_well_formed(rod.voronoi_dilatation)

            # These are not officially supported by pyelastica but
            # we provide them for consistency
            is_well_formed(rod.curvature)
            is_well_formed(rod.reference_curvature)

        crb = self.get_a_block()
        for slice_idx in range(0, 4):
            with self.subTest(slice_idx=slice_idx):
                run_test(crb[slice_idx])

        slice_size = 2
        for view_idx in range(0, 2):
            with self.subTest(view_idx=view_idx):
                run_test(crb[view_idx: view_idx + slice_size])

    def test_energy_methods(self):
        is_well_formed = lambda x: self.assertIsInstance(x, float)

        def run_test(rod):
            is_well_formed(rod.compute_translational_energy())
            is_well_formed(rod.compute_rotational_energy())
            is_well_formed(rod.compute_shearing_energy())
            is_well_formed(rod.compute_bending_energy())

        crb = self.get_a_block()
        for slice_idx in range(0, 4):
            with self.subTest(slice_idx=slice_idx):
                run_test(crb[slice_idx])


if __name__ == "__main__":
    unittest.main(verbosity=2)
