//==============================================================================
/*!
//  \file
//  \brief Spanwise Rod in the Geometry component of @ref cosserat_rod_component
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
#include "Systems/CosseratRods/Components/Geometry/detail/Types.hpp"
///
#include "Systems/CosseratRods/Components/Geometry/detail/Tags/RodSpanwiseGeometryTags.hpp"

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
        /*!\brief Variables corresponding to a spanwise rod component
         * \ingroup cosserat_rod_component
         *
         * \details
         * RodSpanwiseGeometryVariables contains the definitions of variables
         * used within the Blocks framework for a rod data-structure spanning a
         * single spatial dimension (i.e. along centerline coordinates)
         *
         * \tparam CRT A valid Cosserat Rod Traits class
         *
         * \see RodSpanwiseGeometry
         */
        template <typename CRT>
        class RodSpanwiseGeometryVariables {
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
          /*!\brief Variable marking Positions within the Cosserat rod hierarchy
           */
          struct Position
              : public Traits::template CosseratRodInitializedVariable<
                    ::elastica::tags::Position,         //
                    typename Traits::DataType::Vector,  //
                    typename Traits::Place::OnNode> {};
          //********************************************************************

          //********************************************************************
          /*!\brief Variable marking Director(Q) within the Cosserat rod
           * hierarchy
           */
          struct Director
              : public Traits::template CosseratRodInitializedVariable<
                    ::elastica::tags::Director,  //
                    typename Traits::DataType::Matrix,
                    typename Traits::Place::OnElement> {};
          //********************************************************************

          //@}
          //********************************************************************

          //**Type definitions**************************************************
          //! List of computed variables
          using ComputedVariables = tmpl::list<>;
          //! List of initialized variables
          using InitializedVariables = tmpl::list<Position, Director>;
          //! List of all variables
          using Variables =
              tmpl::append<InitializedVariables, ComputedVariables>;
          //********************************************************************
        };
        //**********************************************************************

        //======================================================================
        //
        //  CLASS DEFINITION
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Component corresponding to a spanwise rod
         * \ingroup cosserat_rod_component
         *
         * \details
         * RodSpanwiseGeometry implements a part of Geometry component
         * for use within Cosserat rods implemented in the Blocks framework. It
         * denotes a rod data-structure spanning a single spatial dimension
         * (i.e. along centerline coordinates)
         *
         * \tparam CRT A valid Cosserat Rod Traits class
         * \tparam ComputationalBlock The final block which is derived from the
         * current component
         *
         * \see RodSpanwiseGeometryVariables
         */
        template <typename CRT, typename ComputationalBlock>
        class RodSpanwiseGeometry
            : public CRTPHelper<ComputationalBlock, RodSpanwiseGeometry>,
              public RodSpanwiseGeometryVariables<CRT> {
         private:
          //**Type definitions**************************************************
          //! Traits type
          using Traits = CRT;
          //! This type
          using This = RodSpanwiseGeometry<Traits, ComputationalBlock>;
          //! Type of Variable definitions
          using VariableDefinitions = RodSpanwiseGeometryVariables<Traits>;
          //! CRTP Type
          using CRTP = CRTPHelper<ComputationalBlock, RodSpanwiseGeometry>;
          //! CRTP methods
          using CRTP::self;
          //! Index type
          using index_type = typename Traits::index_type;
          //********************************************************************

         protected:
          //**Type definitions**************************************************
          //! List of computed variables
          using typename VariableDefinitions::ComputedVariables;
          //! List of initialized variables
          using typename VariableDefinitions::InitializedVariables;
          //! List of all variables
          using typename VariableDefinitions::Variables;

          //! Current Variable definitions
          using typename VariableDefinitions::Director;
          using typename VariableDefinitions::Position;
          //********************************************************************

          //********************************************************************
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
          //********************************************************************

         public:
          //**Get methods*******************************************************
          /*!\name Get methods*/
          //@{

          //********************************************************************
          /*!\name Position methods*/
          //@{
          /*!\brief Gets position of the current rod
           */
          inline constexpr decltype(auto) get_position() & noexcept {
            return blocks::get<tags::Position>(self());
          }
          inline constexpr decltype(auto) get_position() const& noexcept {
            return blocks::get<tags::Position>(self());
          }
          inline constexpr decltype(auto) get_position(
              index_type idx) & noexcept {
            return Position::slice(get_position(), idx);
          }
          inline constexpr decltype(auto) get_position(
              index_type idx) const& noexcept {
            return Position::slice(get_position(), idx);
          }
          //@}
          //********************************************************************

          //********************************************************************
          /*!\name Director methods*/
          //@{
          /*!\brief Gets director of the current rod
           */
          inline constexpr decltype(auto) get_director() & noexcept {
            return blocks::get<tags::Director>(self());
          }
          inline constexpr decltype(auto) get_director() const& noexcept {
            return blocks::get<tags::Director>(self());
          }
          inline constexpr decltype(auto) get_director(
              index_type idx) & noexcept {
            return Director::slice(get_director(), idx);
          }
          inline constexpr decltype(auto) get_director(
              index_type idx) const& noexcept {
            return Director::slice(get_director(), idx);
          }
          //@}
          //********************************************************************

          //@}
          //********************************************************************
        };
        //**********************************************************************

      }  // namespace detail

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica
