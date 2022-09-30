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

#include <elastica/Systems/CosseratRods/Components/Initialization.hpp>
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

        //********************************************************************
        /*!\copydoc ComponentInitializationDocStub
         */
        template <typename BlockLike, typename CosseratInitializer>
        static void initialize(
            WithMagnetization<Traits, BlockLike>& this_component,
            CosseratInitializer&& initializer) {
          initialize_component<This::InitializedVariables>(
              this_component.self(),
              std::forward<CosseratInitializer>(initializer));

          auto const n_elem = blocks::get<elastica::tags::NElement>(
              cpp17::as_const(initializer))();

          {
            using Variable = Magnetization;
            using Tag = blocks::parameter_t<Variable>;
            using real_type = typename Traits::real_type;

            auto const dofs = Variable::get_dofs(n_elem);

            auto&& variable(blocks::get<Tag>(this_component.self()));

            for (std::size_t idx = 0U; idx < dofs; ++idx) {
              const auto mag_density = Variable::slice(variable, idx);
              Variable::slice(variable, idx) =
                  this_component.self().get_element_volume(idx) *
                  (this_component.self().get_director(idx) * mag_density);
            }
          }
        }
        //**********************************************************************

       public:
        //**CRTP method*********************************************************
        /*!\name CRTP method*/
        //@{
        using CRTP::self;
        //@}
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