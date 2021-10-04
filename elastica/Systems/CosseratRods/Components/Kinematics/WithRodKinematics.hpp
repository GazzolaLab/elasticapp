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

          //@}
          //********************************************************************

          //**Type definitions**************************************************
          //! List of computed variables
          using ComputedVariables = tmpl::list<>;
          //! List of initialized variables
          using InitializedVariables = tmpl::list<Velocity, AngularVelocity>;
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
        using typename VariableDefinitions::AngularVelocity;
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
          return blocks::get<Velocity>(self());
        }
        inline constexpr decltype(auto) get_velocity() const& noexcept {
          return blocks::get<Velocity>(self());
        }
        //@}
        //**********************************************************************

        //**********************************************************************
        /*!\name Angular Velocity methods*/
        //@{
        /*!\brief Gets angular velocity of the current rod
         */
        inline constexpr decltype(auto) get_angular_velocity() & noexcept {
          return blocks::get<AngularVelocity>(self());
        }
        inline constexpr decltype(auto) get_angular_velocity() const& noexcept {
          return blocks::get<AngularVelocity>(self());
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
         * The following members are modified:
         * - dilatation_rate_batch(n_elems)
         *
         * with the following inputs:
         * - velocity_batch(n_nodes) \n
         * given inputs of compute_all_dilatations() are set.
         *
         * \example
         * The following shows a typical use of the compute_dilatation_rate()
         * function with the expected (correct) result also shown.
         * \snippet test_gov_eqns.cpp compute_dilatation_rate_example
         *
         * \return void/None
         *
         */
        inline constexpr decltype(auto) get_dilatation_rate() const& noexcept {
          auto& position(blocks::get<tags::Position>(self()));
          auto& length(blocks::get</* TODO: length tag */>(self()));
          auto& reference_length(blocks::get</* TODO: ref length tag */>(self()));
          auto& velocity(blocks::get<Velocity>(self()));
          auto position_diff_dot_velocity_diff = Traits::Operations::batch_dot(
              Traits::Operations::difference_kernel(position),
              Traits::Operations::difference_kernel(velocity));

          return position_diff_dot_velocity_diff / (length * reference_length);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets temporal rate of curvature based on the current state
         * of the current rod
         */
        inline constexpr decltype(auto) get_curvature_rate() const& noexcept {
          return 1.0;
        }
        //**********************************************************************

        //@}
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica