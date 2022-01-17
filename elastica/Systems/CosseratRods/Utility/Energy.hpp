//
// Created by Seung Hyun Kim on 10/9/21.
//

#pragma once

#include "Systems/Block.hpp"
#include "Systems/CosseratRods.hpp"
#include "Systems/CosseratRods/Components/helpers.hpp"
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
#include "Systems/CosseratRods/Tags.hpp"
#include "Utilities/CRTP.hpp"
#include "Utilities/Get.hpp"
#include "Utilities/TMPL.hpp"
#include "blaze/Blaze.h"
#include "blaze_tensor/Blaze.h"

namespace elastica {
  namespace cosserat_rod {

    template <typename CRT = elastica::cosserat_rod::DefaultCosseratRodTraits>
    class RodEnergy {
     private:
      using Traits = CRT;
      using real_type = typename Traits::real_type;

     public:
      //************************************************************************
      /*!\brief Computes translational kinetic energy of the body.
      //
      // \details
      // Computes translational kinetic energy of the body given nodal
      // masses and velocities. \n
      // translational_KE = 0.5 * sum(body.mass_batch{i} *
      // body.velocity_batch{i}^2)
      //
      // \example
      // The following shows a typical use of the compute_translational_energy()
      // function with the expected (correct) result also shown.
      // \snippet test_gov_eqns.cpp compute_translational_energy_example
      //
      // \param[in] body object whose translational kinetic energy is to be
      computed
      //
      // \return translational kinetic energy expression type
      //
      // \see fill later?
      */
      template <typename MassType, typename VelocityType>
      static auto compute_translational_energy(
          const MassType& mass_batch, const VelocityType& velocity_batch) {
        return real_type(0.5) *
               blaze::sum(mass_batch * blaze::sum<blaze::columnwise>(
                                           velocity_batch % velocity_batch));
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Computes rotational kinetic energy of the body.
      //
      // \details
      // Computes rotational kinetic energy of the body given elemental
      // mass moment of inertia and angular velocities. \n
      // rotational_KE = 0.5 * sum((body.mass_second_moment_of_inertia{i} *
      // body.omega_batch{i}) dot
      // body.omega_batch{i} / body.dilatation_batch{i})
      //
      // \example
      // The following shows a typical use of the compute_rotational_energy()
      // function with the expected (correct) result also shown.
      // \snippet test_gov_eqns.cpp compute_rotational_energy_example
      //
      // \param[in] body object whose rotational kinetic energy is to be
      computed
      //
      // \return rotational kinetic energy expression type
      //
      // \see fill later?
      */
      template <typename BodyType>
      static auto compute_rotational_energy(BodyType& body) {
        const std::size_t dimension = 3UL;  // to avoid linalg kernels fail
        // allocating temp container (3, body.n_elems) for temporary storage
        // TODO refactor, assign variable name to dummy variable if it appears
        // frequently
        // TODO remove blaze operation
        auto J_omega_batch = blaze::submatrix(body.temp_vec_batch, 0UL, 0UL,
                                              dimension, body.n_elems);
        Traits::Operations::batch_matvec(
            J_omega_batch, body.mass_second_moment_of_inertia_batch,
            body.omega_batch);
        auto J_omega_upon_e_batch =
            J_omega_batch %
            blaze::expand(blaze::pow(body.dilatation_batch, -1), dimension);
        auto rotational_energy_batch = Traits::Operations::batch_dot(
            body.omega_batch, J_omega_upon_e_batch);
        return real_type(0.5) * blaze::sum(rotational_energy_batch);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Computes shearing mode energy of the body.
      //
      // \details
      // Computes shearing mode energy of the body given elemental
      // shear matrix and shear strain. \n
      // shear_energy = 0.5 * sum((body.shear_matrix_batch{i} *
      // body.strain_diff_batch{i}) dot
      // body.strain_diff_batch{i} / body.rest_length_batch{i}) \n
      // where body.strain_diff_batch = body.strain_batch -
      body.rest_strain_batch
      //
      // \example
      // The following shows a typical use of the compute_shear_energy()
      // function with the expected (correct) result also shown.
      // \snippet test_gov_eqns.cpp compute_shear_energy_example
      //
      // \param[in] body object whose shear mode energy is to be computed
      //
      // \return shear mode energy expression type
      //
      // \see fill later?
      */
      template <typename R>
      static auto compute_shear_energy(R rod) {
        return compute_shear_energy(
            ::elastica::get<elastica::tags::ShearStretchStrain>(rod),
            ::elastica::get<elastica::tags::ReferenceShearStretchStrain>(rod),
            ::elastica::get<elastica::tags::ShearStretchRigidityMatrix>(rod),
            ::elastica::get<elastica::tags::ReferenceElementLength>(rod));
      }
      template <typename StrainType, typename ShearMatrixType,
                typename LengthType>
      static auto compute_shear_energy(
          const StrainType& strain, const StrainType& rest_strain,
          const ShearMatrixType& shear_matrix_batch,
          const LengthType& rest_length_batch) {
        const std::size_t n_elems = ::elastica::cosserat_rod::get_batch_elems(
            strain);  // TODO: Something is wrong here. why get_batch_elems is
                      // global?
        const std::size_t dimension = 3UL;  // to avoid linalg kernels fail
        auto strain_diff_batch = strain - rest_strain;
        // allocating temp container (3, n_elems) for temporary storage
        // TODO refactor to avoid temp memory allocations
        auto shear_internal_stress_batch =
            blaze::DynamicMatrix<real_type>(dimension, n_elems);
        Traits::Operations::batch_matvec(shear_internal_stress_batch,
                                         shear_matrix_batch, strain_diff_batch);
        auto shear_internal_stress_dot_strain_diff_batch =
            Traits::Operations::batch_dot(strain_diff_batch,
                                          shear_internal_stress_batch);
        return real_type(0.5) *
               blaze::sum(shear_internal_stress_dot_strain_diff_batch *
                          rest_length_batch);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Computes bending mode energy of the system
      //
      // \details
      // Computes bending mode energy of the system given voronoidal
      // bend matrix and curvature. \n
      // bend_energy = 0.5 * sum((body.bend_matrix_batch{i} *
      // body.curvature_diff_batch{i}) dot
      // body.curvature_diff_batch{i} /
      // body.rest_voronoi_length_batch{i}) \n
      // where body.curvature_diff_batch = body.curvature_batch -
      // body.rest_curvature_batch
      //
      // \example
      // The following shows a typical use of the compute_bend_energy()
      // function with the expected (correct) result also shown.
      // \snippet test_gov_eqns.cpp compute_bend_energy_example
      //
      // \param[in] body object whose bend mode energy is to be computed
      //
      // \return bend mode energy expression type
      //
      // \see fill later?
      */
      template <typename BodyType>
      static auto compute_bend_energy(BodyType& body) {
        const std::size_t dimension = 3UL;  // to avoid linalg kernels fail
        auto curvature_diff_batch =
            body.curvature_batch - body.rest_curvature_batch;
        // allocating temp container (3, n_elems) for temporary storage
        // TODO refactor to avoid temp memory allocations
        auto bend_internal_torque_batch = blaze::submatrix(
            body.temp_vec_batch, 0UL, 0UL, dimension, body.n_voronoi);
        Traits::Operations::batch_matvec(bend_internal_torque_batch,
                                         body.bend_matrix_batch,
                                         curvature_diff_batch);
        auto bend_internal_torque_dot_curvature_diff_batch =
            Traits::Operations::batch_dot(curvature_diff_batch,
                                          bend_internal_torque_batch);
        return real_type(0.5) *
               blaze::sum(bend_internal_torque_dot_curvature_diff_batch *
                          body.rest_voronoi_length_batch);
      }
      //************************************************************************
    };
  }  // namespace cosserat_rod

}  // namespace elastica