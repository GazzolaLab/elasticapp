//==============================================================================
/*!
//  \file
//  \brief Magnetism component of Cosserat rod
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

#include <elastica/Utilities/CRTP.hpp>
#include <elastica/Utilities/TMPL.hpp>

#include "Extension/Systems/CosseratRods/Components/Tags.hpp"
#include "Extension/Systems/CosseratRods/Components/Types.hpp"
#include "Extension/Systems/CosseratRods/Components/helpers.hpp"

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
        class WithMagnetismVariables {
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
          /*!\brief Variable marking Magnetism within the Cosserat rod hierarchy
           */
          struct Magnetization
              : public Traits::template CosseratRodInitializedVariable<
                    ::elastica::tags::Magnetization,    //
                    typename Traits::DataType::Vector,  //
                    typename Traits::Place::OnElement> {};
          //********************************************************************

          //@}
          //********************************************************************

          //**Type definitions**************************************************
          //! List of computed variables
          using ComputedVariables = tmpl::list<>;
          //! List of initialized variables
          using InitializedVariables = tmpl::list<Magnetization>;
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

      template <typename CRT, typename ComputationalBlock>
      class WithMagnetization
          : public CRTPHelper<ComputationalBlock, WithMagnetization>,
            public detail::WithMagnetismVariables<CRT>,
            public MagnetismComponent<
                WithMagnetization<CRT, ComputationalBlock>> {
       private:
        //**Type definitions****************************************************
        //! Traits type
        using Traits = CRT;
        //! This type
        using This = WithMagnetization<Traits, ComputationalBlock>;
        //! Type of Variable definitions
        using VariableDefinitions = detail::WithMagnetismVariables<Traits>;
        //! CRTP Type
        using CRTP = CRTPHelper<ComputationalBlock, WithMagnetization>;
        //! CRTP methods
        using CRTP::self;
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
        using typename VariableDefinitions::Magnetization;
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
                  // Scaled by the volume here, pass in density
                  Variable::slice(variable, i) =
                      downstream_block.get_element_volume(i) *
                      // Magnetization is always in the local frame of reference
                      // But specified in the uniform frame of reference
                      (downstream_block.get_director(i) * initializer(i));
                }
              });
        }
        //**********************************************************************

       public:
        //**Get methods*******************************************************
        /*!\name Get methods*/
        //@{

        //********************************************************************
        /*!\name Magnetization methods*/
        //@{
        /*!\brief Gets magnetization of the current rod
         */
        inline constexpr decltype(auto) get_magnetization() & noexcept {
          return blocks::get<tags::Magnetization>(self());
        }
        inline constexpr decltype(auto) get_magnetization() const& noexcept {
          return blocks::get<tags::Magnetization>(self());
        }
        inline constexpr decltype(auto) get_magnetization(
            index_type idx) & noexcept {
          return Magnetization::slice(get_magnetization(), idx);
        }
        inline constexpr decltype(auto) get_magnetization(
            index_type idx) const& noexcept {
          return Magnetization::slice(get_magnetization(), idx);
        }
        //@}
        //********************************************************************

        //@}
        //********************************************************************
      };

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica