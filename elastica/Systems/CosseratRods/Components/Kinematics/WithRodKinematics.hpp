//==============================================================================
/*!
//  \file
//  \brief Kinematics of a rod in the Kinematics component of
//  @ref cosserat_rod_component
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <cstdint>  // for size_t
#include <utility>  // for move

#include "Systems/Block.hpp"
#include "Utilities/CRTP.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/Math/Vec3.hpp"

// module
/// Types always first
#include "Systems/CosseratRods/Components/Kinematics/Types.hpp"
///
#include "Systems/CosseratRods/Components/Kinematics/Protocols.hpp"
#include "Systems/CosseratRods/Components/Kinematics/Tags.hpp"  // there is only one tags here
#include "Systems/CosseratRods/Components/helpers.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace component {

      namespace detail {

        //======================================================================
        //
        //  CLASS DEFINITION
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Variables corresponding to kinematics of a span-wise rod
         * component within the Cosserat rod hierarchy
         * \ingroup cosserat_rod_component
         *
         * \details
         * RodKinematicsVariables contains the definitions of variables
         * used within the Blocks framework for the kinematics of a rod
         * data-structure spanning a single spatial dimension (i.e. along
         * center-line coordinates)
         *
         * \tparam CRT A valid Cosserat Rod Traits class
         *
         * \see WithRodKinematics
         */
        template <typename CRT>
        class RodKinematicsVariables {
         private:
          //**Type definitions**************************************************
          //! Traits type
          using Traits = CRT;
          //********************************************************************

         protected:
          //**Variable definitions**********************************************
          /*!\name Variable definitions*/
          //@{

          //********************************************************************
          /*!\brief Variable scalar mass within the Cosserat rod hierarchy
           *  Lumped mass on node. (should be in element)
           */
          struct Mass : public Traits::template CosseratRodInitializedVariable<
                            ::elastica::tags::Mass,             //
                            typename Traits::DataType::Scalar,  //
                            typename Traits::Place::OnNode> {
            //**Type definitions************************************************
            //! The element type of a ghost
            using ghost_type = typename Traits::DataType::Scalar::ghost_type;
            //******************************************************************

            //******************************************************************
            /*!\brief Obtain the ghost value
             *
             * \details
             * Overrides the default value for putting in ghost elements.
             */
            static inline constexpr auto ghost_value() noexcept -> ghost_type {
              return ghost_type(1.0);
            }
            //******************************************************************
          };
          //********************************************************************

          //********************************************************************
          /*!\brief Variable marking Velocity within the Cosserat rod hierarchy
           */
          struct Velocity
              : public Traits::template CosseratRodInitializedVariable<
                    ::elastica::tags::Velocity,         //
                    typename Traits::DataType::Vector,  //
                    typename Traits::Place::OnNode> {};
          //********************************************************************

          //********************************************************************
          /*!\brief Variable marking AngularVelocity (\f$ \omega \f$) within the
           * Cosserat rod hierarchy
           */
          struct AngularVelocity
              : public Traits::template CosseratRodInitializedVariable<
                    ::elastica::tags::AngularVelocity,  //
                    typename Traits::DataType::Vector,
                    typename Traits::Place::OnElement> {};
          //********************************************************************

          //********************************************************************
          /*!\brief Variable marking Acceleration within the Cosserat rod
           * hierarchy
           */
          struct Acceleration
              : public Traits::template CosseratRodInitializedVariable<
                    ::elastica::tags::Acceleration,     //
                    typename Traits::DataType::Vector,  //
                    typename Traits::Place::OnNode> {};
          //********************************************************************

          //********************************************************************
          /*!\brief Variable marking AngularAcceleration within the
           * Cosserat rod hierarchy
           */
          struct AngularAcceleration
              : public Traits::template CosseratRodInitializedVariable<
                    ::elastica::tags::AngularAcceleration,  //
                    typename Traits::DataType::Vector,
                    typename Traits::Place::OnElement> {};
          //********************************************************************

          //********************************************************************
          /*!\brief Variable marking external load within the Cosserat rod
           * hierarchy
           */
          struct ExternalLoads : public Traits::template CosseratRodVariable<
                                     ::elastica::tags::ExternalLoads,    //
                                     typename Traits::DataType::Vector,  //
                                     typename Traits::Place::OnNode> {};
          //********************************************************************

          //********************************************************************
          /*!\brief Variable marking external torque within the Cosserat rod
           * hierarchy
           */
          struct ExternalTorques : public Traits::template CosseratRodVariable<
                                       ::elastica::tags::ExternalTorques,  //
                                       typename Traits::DataType::Vector,  //
                                       typename Traits::Place::OnElement> {};
          //********************************************************************

          //********************************************************************
          /*!\brief Variable marking MassSecondMomentOfInertia within the
           * Cosserat rod hierarchy
           */
          struct MassSecondMomentOfInertia
              : public Traits::template CosseratRodVariable<
                    ::elastica::tags::MassSecondMomentOfInertia,  //
                    typename Traits::DataType::Matrix,
                    typename Traits::Place::OnElement> {};
          //********************************************************************

          //********************************************************************
          /*!\brief Variable marking InvMassSecondMomentOfInertia within the
           * Cosserat rod hierarchy
           */
          struct InvMassSecondMomentOfInertia
              : public Traits::template CosseratRodVariable<
                    ::elastica::tags::InvMassSecondMomentOfInertia,  //
                    typename Traits::DataType::Matrix,
                    typename Traits::Place::OnElement> {};
          //********************************************************************

          //@}
          //********************************************************************

          //**Type definitions**************************************************
          //! List of computed variables
          using ComputedVariables = tmpl::list<ExternalLoads, ExternalTorques,
                                               MassSecondMomentOfInertia,
                                               InvMassSecondMomentOfInertia>;
          //! List of initialized variables
          using InitializedVariables =
              tmpl::list<Mass, Velocity, AngularVelocity, Acceleration,
                         AngularAcceleration>;
          //! List of all variables
          using Variables =
              tmpl::append<InitializedVariables, ComputedVariables>;
          //********************************************************************
        };
        //**********************************************************************

      }  // namespace detail

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Kinematics component corresponding to a spanwise rod
       * \ingroup cosserat_rod_component
       *
       * \details
       * WithRodKinematics implements the final Kinematics component for use
       * with Cosserat rods implemented in the Blocks framework. It denotes the
       * kinematic variables and methods (anything temporally varying) needed
       * for a Cosserat rod data-structure, as defined by the Geometry
       * component.
       *
       * \note
       * It requires a valid Geometry component declared in the Blocks
       * Hierarchy to ensure it is properly used. Else, a compilation error is
       * thrown.
       *
       * \usage
       * Since WithRodKinematics is a valid and complete Kinematics component
       * adhering to protocols::Kinematics, one can use it to declare a
       * CosseratRodPlugin within the @ref blocks framework
       * \code
       * // pre-declare RodTraits, Blocks
       * using CircularCosseratRod = CosseratRodPlugin<RodTraits, Block,
       * // Conforms to protocols::Geometry1D!
       * components::WithCircularCosseratRod,
       * components::WithRodKinematics>;
       * \endcode
       *
       * \tparam CRT A valid Cosserat Rod Traits class
       * \tparam ComputationalBlock The final block which is derived from the
       * current component
       *
       * \see CosseratRodPlugin, RodKinematicsVariables
       */
      // Forward declare all Layers and policies
      template <typename CRT, typename ComputationalBlock>
      class WithRodKinematics
          : public CRTPHelper<ComputationalBlock, WithRodKinematics>,
            public detail::RodKinematicsVariables<CRT>,
            public ::tt::ConformsTo<protocols::Kinematics1D>,
            public Component<WithRodKinematics<CRT, ComputationalBlock>> {
       private:
        //**Type definitions****************************************************
        //! Traits type
        using Traits = CRT;
        //! This type
        using This = WithRodKinematics<Traits, ComputationalBlock>;
        //! Type of Variable definitions
        using VariableDefinitions = detail::RodKinematicsVariables<Traits>;
        //! CRTP Type
        using CRTP = CRTPHelper<ComputationalBlock, WithRodKinematics>;
        //! CRTP methods
        using CRTP::self;
        //! Real number type
        using real_type = typename Traits::real_type;
        //! Index type
        using index_type = typename Traits::index_type;
        //**********************************************************************

       protected:
        //**Type definitions****************************************************
        //! List of computed variables
        using typename VariableDefinitions::ComputedVariables;
        //! List of initialized variables
        using typename VariableDefinitions::InitializedVariables;
        //! List of all variables
        using typename VariableDefinitions::Variables;

        //! Current Variable definitions
        using typename VariableDefinitions::Acceleration;
        using typename VariableDefinitions::AngularAcceleration;
        using typename VariableDefinitions::AngularVelocity;
        using typename VariableDefinitions::ExternalLoads;
        using typename VariableDefinitions::ExternalTorques;
        using typename VariableDefinitions::InvMassSecondMomentOfInertia;
        using typename VariableDefinitions::Mass;
        using typename VariableDefinitions::MassSecondMomentOfInertia;
        using typename VariableDefinitions::Velocity;
        //**********************************************************************

        //**********************************************************************
        /*!\brief Initialize method for the current component
         *
         * \details
         * initialize() is responsible for initializing the variables present
         * in the current component in the final block of data. For
         * InitializedVariables, it fetches the corresponding initializer from
         * the set of `initializers` to fill in the data. For
         * ComputedVariables the default value is usually set. In this way,
         * each component is responsible for initializing its own data in a
         * manner that does not intrude with initialization from other
         * components.
         *
         * \tparam DownstreamBlock The final block-like object which is
         * derived from the current component
         * \tparam CosseratInitializer An Initializer corresponding to the
         * Cosserat rod hierarchy
         */
        template <typename DownstreamBlock, typename CosseratInitializer>
        static void initialize(DownstreamBlock& downstream_block,
                               CosseratInitializer&& initializers) {
          //        https://stackoverflow.com/a/53144382
          struct {
            CosseratInitializer value;
          } cap{std::forward<CosseratInitializer>(initializers)};
          // Note : need a mutable lambda as we can now "move" from cap
          tmpl::for_each<This::InitializedVariables>(
              [&downstream_block,
               //               cap = std::move(cap)
               &cap](auto v) mutable -> void {
                using Variable = tmpl::type_from<decltype(v)>;
                using Tag = blocks::initializer_t<Variable>;

                auto& variable(blocks::get<Tag>(downstream_block));
                auto&& initializer(blocks::get<Tag>(std::move(cap.value)));

                const auto dofs = Variable::get_dofs(cap.value.n_elems());
                for (std::size_t i = 0; i < dofs; ++i) {
                  // TODO : This doesn't work on slices, consider having the
                  // slice member in the initializer itself
                  Variable::slice(variable, i) = initializer(i);
                }
              });

          /* mass second moment of inertia */
          {
            using Variable = MassSecondMomentOfInertia;
            using Tag = blocks::parameter_t<Variable>;

            auto& variable(blocks::get<Tag>(downstream_block));
            auto& density(blocks::get<tags::Mass>(downstream_block));
            auto& reference_lengths(
                blocks::get<tags::ReferenceElementLength>(downstream_block));
            const auto dofs = Variable::get_dofs(initializers.n_elems());
            for (std::size_t idx = 0U; idx < dofs; ++idx) {
              // (I_1, I_2, I_3)
              Variable::slice(variable, idx) = real_type(0.0);
              const Vec3 I_idx =
                  downstream_block.get_second_moment_of_area(idx) *
                  Traits::DataType::Scalar::slice(density, idx) *
                  Traits::DataType::Scalar::slice(reference_lengths, idx);
              Variable::diagonal_assign(variable, idx, I_idx);
            }
          }

          /* inverse mass second moment of inertia */
          {
            using Variable = InvMassSecondMomentOfInertia;
            using Tag = blocks::parameter_t<Variable>;

            auto& variable(blocks::get<Tag>(downstream_block));
            auto& density(blocks::get<tags::Mass>(downstream_block));
            auto& reference_lengths(
                blocks::get<tags::ReferenceElementLength>(downstream_block));
            const auto dofs = Variable::get_dofs(initializers.n_elems());
            for (std::size_t idx = 0U; idx < dofs; ++idx) {
              // inv(I_1, I_2, I_3)
              Variable::slice(variable, idx) = real_type(0.0);
              const Vec3 I_idx =
                  real_type(1.0) /
                  (downstream_block.get_second_moment_of_area(idx) *
                   Traits::DataType::Scalar::slice(density, idx) *
                   Traits::DataType::Scalar::slice(reference_lengths, idx));
              Variable::diagonal_assign(variable, idx, I_idx);
            }
          }

          // Variables Initialization
          {
            using MassVariable = typename Traits::DataType::Scalar;

            auto& mass(blocks::get<::elastica::tags::Mass>(downstream_block));
            auto& volume = downstream_block.get_element_volume();
            auto n_elem = volume.size();
            typename MassVariable::type mass_on_element(n_elem);
            mass_on_element =
                0.5 * MassVariable::slice(mass, 0, n_elem) * volume;
            // Traits::Operations::quadrature_kernel(mass, mass_on_element);
            MassVariable::slice(mass, 0UL, n_elem) = mass_on_element;
            MassVariable::slice(mass, n_elem) = 0.0;
            MassVariable::slice(mass, 1UL, n_elem) += mass_on_element;
            MassVariable::slice(mass, 0UL) += 0.0;
          }

          /* Reset forces and torques */
          {
            auto& external_loads(
                blocks::get<::elastica::tags::ExternalLoads>(downstream_block));
            auto& external_torques(
                blocks::get<::elastica::tags::ExternalTorques>(
                    downstream_block));
            external_loads = 0.0;
            external_torques = 0.0;
          }
        }
        //**********************************************************************

       public:
        //**Get methods*********************************************************
        /*!\name Get methods*/
        //@{

        //**********************************************************************
        /*!\name Velocity methods*/
        //@{
        /*!\brief Gets velocity of the current rod
         */
        inline constexpr decltype(auto) get_velocity() & noexcept {
          return blocks::get<tags::Velocity>(self());
        }
        inline constexpr decltype(auto) get_velocity() const& noexcept {
          return blocks::get<tags::Velocity>(self());
        }
        inline constexpr decltype(auto) get_velocity(
            index_type idx) & noexcept {
          return Velocity::slice(get_velocity(), idx);
        }
        inline constexpr decltype(auto) get_velocity(
            index_type idx) const& noexcept {
          return Velocity::slice(get_velocity(), idx);
        }
        //@}
        //**********************************************************************

        //**********************************************************************
        /*!\name Angular Velocity methods*/
        //@{
        /*!\brief Gets angular velocity of the current rod
         */
        inline constexpr decltype(auto) get_angular_velocity() & noexcept {
          return blocks::get<tags::AngularVelocity>(self());
        }
        inline constexpr decltype(auto) get_angular_velocity() const& noexcept {
          return blocks::get<tags::AngularVelocity>(self());
        }
        inline constexpr decltype(auto) get_angular_velocity(
            index_type idx) & noexcept {
          return AngularVelocity::slice(get_angular_velocity(), idx);
        }
        inline constexpr decltype(auto) get_angular_velocity(
            index_type idx) const& noexcept {
          return AngularVelocity::slice(get_angular_velocity(), idx);
        }
        //@}
        //**********************************************************************

        //**********************************************************************
        /*!\name Aceeleration methods*/
        //@{
        /*!\brief Gets acceleration of the current rod
         */
        inline constexpr decltype(auto) get_acceleration() & noexcept {
          return blocks::get<tags::Acceleration>(self());
        }
        inline constexpr decltype(auto) get_acceleration() const& noexcept {
          return blocks::get<tags::Acceleration>(self());
        }
        //@}
        //**********************************************************************

        //**********************************************************************
        /*!\name Angular acceleration methods*/
        //@{
        /*!\brief Gets angular acceleration of the current rod
         */
        inline constexpr decltype(auto) get_angular_acceleration() & noexcept {
          return blocks::get<tags::AngularAcceleration>(self());
        }
        inline constexpr decltype(auto) get_angular_acceleration()
            const& noexcept {
          return blocks::get<tags::AngularAcceleration>(self());
        }
        //@}
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Computation methods*************************************************
        /*!\name Compute methods*/
        //@{

        //**********************************************************************
        /*!\brief Gets temporal rate of dilatation based on the current state
         * of the current rod
         *
         * \details
         * Computes rate of change of elemental dilatations (de / dt) given the
         * rod positions and velocities (state). \n
         * dilatation_rate{i} =
         * (position{i+1} - position{i}) dot
         * (velocity{i+1} - velocity{i}) /
         * (length{i} * rest_length{i}) \n
         * Assumes compute_all_dilatations() has been called.
         *
         * \return dilatation rate
         *
         */
        inline constexpr decltype(auto) get_dilatation_rate() const& noexcept {
          auto& position(blocks::get<tags::Position>(self()));
          auto& length(blocks::get<tags::ElementLength>(self()));
          auto& reference_length(
              blocks::get<tags::ReferenceElementLength>(self()));
          auto& velocity(blocks::get<tags::Velocity>(self()));
          return Traits::Operations::batch_dot(
                     Traits::Operations::difference_kernel(position),
                     Traits::Operations::difference_kernel(velocity)) /
                 (length * reference_length);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets temporal rate of curvature based on the current state
         * of the current rod
         */
        inline decltype(auto) get_curvature_rate() const noexcept {
          // TODO get curvature rate
          auto& curvature(blocks::get<tags::Curvature>(self()));
          auto& omega(blocks::get<tags::AngularAcceleration>(self()));
          auto averaged_omega = Traits::Operations::average_kernel(omega);
          return Traits::Operations::difference_kernel(omega) +
                 Traits::Operations::batch_cross(curvature, averaged_omega);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets temporal rate of curvature based on the current state
         * of the current rod
         */
        inline decltype(auto) get_shear_stretch_strain_rate() const noexcept {
          // TODO get shear stretch strain rate
          auto& director(blocks::get<tags::Director>(self()));
          auto& velocity(blocks::get<tags::Velocity>(self()));
          auto& reference_length(
              blocks::get<tags::ReferenceElementLength>(self()));
          auto& sigma(blocks::get<tags::ShearStretchStrain>(self()));
          auto& omega(blocks::get<tags::AngularAcceleration>(self()));

          auto sigma_rate(sigma);
          auto diff_velocity = Traits::Operations::difference_kernel(velocity);
          Traits::Operations::batch_add_z_unit(sigma_rate);
          // z_vector = np.array([ 0.0, 0.0, 1.0 ]).reshape(3, -1);
          return Traits::Operations::batch_cross(sigma_rate, omega) -
                 Traits::Operations::batch_matvec(director, diff_velocity) /
                     reference_length;
        }
        //**********************************************************************

       public:  // TODO: temporary change from protected
        //**********************************************************************
        /*!\brief Update acceleration based on the current state
         *
         * \details
         * Updates accelerations of the system (translational and angular) given
         * the state of the system. \n
         * acceleration_batch{i} =
         * (internal_force_batch{i} + external_force_batch{i})
         * / mass_batch{i} \n
         * alpha_batch{i} = inv_mass_second_moment_of_inertia_batch{i} *
         * (internal_torque_batch{i} + external_torque_batch{i}) *
         * dilatation_batch{i} \n
         * Also resets external_force_batch and external_torque_batch = 0.0
         *
         * \example
         * The following shows a typical use of the update_acceleration()
         * function with the expected (correct) result also shown.
         * \snippet test_gov_eqns.cpp update_acceleration_example
         *
         * \return void/None
         *
         * \see fill later?
         */

        // TODO: double check how external force and external torque comes into
        // play
        inline void update_acceleration() noexcept {
          auto& mass(blocks::get<tags::Mass>(self()));
          auto& dilatation(blocks::get<tags::ElementDilatation>(self()));
          auto& inv_mass_SMOI(
              blocks::get<tags::InvMassSecondMomentOfInertia>(self()));
          auto& acceleration(blocks::get<tags::Acceleration>(self()));
          auto& angular_acceleration(
              blocks::get<tags::AngularAcceleration>(self()));
          auto& internal_loads(blocks::get<tags::InternalLoads>(self()));
          auto& internal_torques(blocks::get<tags::InternalTorques>(self()));
          auto& external_loads(blocks::get<tags::ExternalLoads>(self()));
          auto& external_torques(blocks::get<tags::ExternalTorques>(self()));

          auto total_loads = internal_loads + external_loads;
          // TODO: Revisit division vs power+multiplication
          // TODO: remove transpose if possible
          // acceleration = total_loads /
          // Traits::Operations::expand_for_broadcast(mass);
          Traits::Operations::batch_division_matvec(acceleration, total_loads,
                                                    mass);

          auto total_torque = internal_torques + external_torques;
          Traits::Operations::batch_matvec(
              angular_acceleration, inv_mass_SMOI,
              total_torque % Traits::Operations::expand_for_broadcast(
                                 Traits::Operations::transpose(dilatation)));

          /* Reset forces and torques */
          // TODO: what if someone wants to check external load/torques before
          // reset
          external_loads = 0.0;
          external_torques = 0.0;
        }
        //*********************************************************************

        //@}
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica
