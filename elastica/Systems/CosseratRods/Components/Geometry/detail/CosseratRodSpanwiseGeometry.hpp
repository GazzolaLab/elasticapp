//==============================================================================
/*!
//  \file
//  \brief Spanwise Cosserat Rods in the Geometry component of
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

//****************************************************************************
// Includes
//****************************************************************************
#include <cstddef>
#include <utility>

#include "ErrorHandling/Assert.hpp"
#include "Systems/Block.hpp"
#include "Utilities/CRTP.hpp"
#include "Utilities/TMPL.hpp"

// module
/// Types always first
#include "Systems/CosseratRods/Components/Geometry/detail/Types.hpp"
///
#include "Systems/CosseratRods/Components/Geometry/detail/RodSpanwiseGeometry.hpp"
#include "Systems/CosseratRods/Components/Geometry/detail/Tags/CosseratRodSpanwiseGeometryTags.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace component {

      namespace detail {

        //**********************************************************************
        /*!\brief Variables corresponding to a spanwise Cosserat rod component
         * \ingroup cosserat_rod_component
         *
         * \details
         * CosseratRodSpanwiseGeometryVariables contains the definitions of
         * variables used within the Blocks framework for a Cosserat rod
         * data-structure spanning a single spatial dimension (i.e. along
         * centerline coordinates)
         *
         * \tparam CRT A valid Cosserat Rod Traits class
         *
         * \see CosseratRodSpanwiseGeometry
         */
        template <typename CRT>
        class CosseratRodSpanwiseGeometryVariables {
         private:
          //**Type definitions**************************************************
          //! Traits type
          using Traits = CRT;
          //********************************************************************

         protected:
          //**Variable definitions**********************************************
          /*!\name Variable definitions*/
          //@{

          //**Initialized Variable definitions**********************************
          /*!\name Initialized Variable definitions*/
          //@{

          //********************************************************************
          /*!\brief Variable marking ReferenceElementLengths within the Cosserat
           * rod hierarchy
           */
          struct ReferenceElementLength
              : public Traits::template CosseratRodInitializedVariable<
                    ::elastica::tags::ReferenceElementLength,  //
                    typename Traits::DataType::Scalar,         //
                    typename Traits::Place::OnElement> {
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

          //@}
          //********************************************************************

          //**Computed Variable definitions*************************************
          /*!\name Computed Variable definitions*/
          //@{

          //********************************************************************
          /*!\brief Variable marking ReferenceCurvatures within the Cosserat
           * rod hierarchy
           */
          struct ReferenceCurvature
              : public Traits::template CosseratRodInitializedVariable<
                    ::elastica::tags::ReferenceCurvature,  //
                    typename Traits::DataType::Vector,     //
                    typename Traits::Place::OnVoronoi> {};
          //********************************************************************

          //********************************************************************
          /*!\brief Variable marking ReferenceVoronoiLengths within the Cosserat
           * rod hierarchy
           */
          struct ReferenceVoronoiLength
              : public Traits::template CosseratRodVariable<
                    ::elastica::tags::ReferenceVoronoiLength,  //
                    typename Traits::DataType::Scalar,         //
                    typename Traits::Place::OnVoronoi> {
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
          /*!\brief Variable marking ElementLength within the Cosserat rod
           * hierarchy
           */
          struct ElementLength : public Traits::template CosseratRodVariable<
                                     ::elastica::tags::ElementLength,    //
                                     typename Traits::DataType::Scalar,  //
                                     typename Traits::Place::OnElement> {};
          //********************************************************************

          //********************************************************************
          /*!\brief Variable marking ElementDilatation within the Cosserat
           * rod hierarchy
           */
          struct ElementDilatation
              : public Traits::template CosseratRodVariable<
                    ::elastica::tags::ElementDilatation,  //
                    typename Traits::DataType::Scalar,    //
                    typename Traits::Place::OnElement> {};
          //********************************************************************

          //********************************************************************
          /*!\brief Variable marking VoronoiLength within the Cosserat rod
           * hierarchy
           */
          struct VoronoiLength : public Traits::template CosseratRodVariable<
                                     ::elastica::tags::VoronoiLength,    //
                                     typename Traits::DataType::Scalar,  //
                                     typename Traits::Place::OnVoronoi> {};
          //********************************************************************

          //********************************************************************
          /*!\brief Variable marking VoronoiDilatation within the Cosserat
           * rod hierarchy
           */
          struct VoronoiDilatation
              : public Traits::template CosseratRodVariable<
                    ::elastica::tags::VoronoiDilatation,  //
                    typename Traits::DataType::Scalar,    //
                    typename Traits::Place::OnVoronoi> {};
          //********************************************************************

          //********************************************************************
          /*!\brief Variable marking Curvature within the Cosserat rod hierarchy
           */
          struct Curvature : public Traits::template CosseratRodVariable<
                                 ::elastica::tags::Curvature,        //
                                 typename Traits::DataType::Vector,  //
                                 typename Traits::Place::OnVoronoi> {};
          //********************************************************************

          //********************************************************************
          /*!\brief Variable marking Tangent within the Cosserat rod hierarchy
           */
          struct Tangent : public Traits::template CosseratRodVariable<
                               ::elastica::tags::Tangent,          //
                               typename Traits::DataType::Vector,  //
                               typename Traits::Place::OnElement> {};
          //********************************************************************

          //********************************************************************
          /*!\brief Variable marking ShearStretchStrain within the Cosserat
           * rod hierarchy
           */
          struct ShearStretchStrain
              : public Traits::template CosseratRodVariable<
                    ::elastica::tags::ShearStretchStrain,  //
                    typename Traits::DataType::Vector,     //
                    typename Traits::Place::OnElement> {};
          //********************************************************************

          //@}
          //********************************************************************

          //@}
          //********************************************************************

          //**Type definitions**************************************************
          //! List of computed variables
          using ComputedVariables =
              tmpl::list<ReferenceVoronoiLength, ElementLength,
                         ElementDilatation, VoronoiLength, VoronoiDilatation,
                         Curvature, Tangent, ShearStretchStrain>;
          //! List of initialized variables
          using InitializedVariables =
              tmpl::list<ReferenceElementLength, ReferenceCurvature>;
          //! List of all variables
          using Variables =
              tmpl::append<InitializedVariables, ComputedVariables>;
          //********************************************************************
        };
        //**********************************************************************

        // TODO Check for EBO
        //======================================================================
        //
        //  CLASS DEFINITION
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Component corresponding to a spanwise Cosserat rod with a
         * \ingroup cosserat_rod_component
         *
         * \details
         * CosseratRodSpanwiseGeometry implements an intermediate Geometry
         * component for use within Cosserat rods implemented in the Blocks
         * framework. It denotes a Cosserat rod data-structure without any
         * details on the shape of cross section spanning a
         * single spatial dimension (i.e. along centerline coordinates)
         *
         * \tparam CRT A valid Cosserat Rod Traits class
         * \tparam ComputationalBlock The final block which is derived from the
         * current component
         *
         * \see CosseratRodSpanwiseGeometryVariables
         */
        template <typename CRT, typename ComputationalBlock>
        class CosseratRodSpanwiseGeometry
            : public RodSpanwiseGeometry<CRT, ComputationalBlock>,
              public CosseratRodSpanwiseGeometryVariables<CRT>,
              public CRTPHelper<ComputationalBlock,
                                CosseratRodSpanwiseGeometry> {
         private:
          //**Type definitions**************************************************
          //! Traits type
          using Traits = CRT;
          //! This type
          using This = CosseratRodSpanwiseGeometry<Traits, ComputationalBlock>;
          //! Parent type
          using Parent = RodSpanwiseGeometry<Traits, ComputationalBlock>;
          //! Type of Variable definitions
          using VariableDefinitions =
              CosseratRodSpanwiseGeometryVariables<Traits>;
          //! CRTP Type
          using CRTP =
              CRTPHelper<ComputationalBlock, CosseratRodSpanwiseGeometry>;
          //! CRTP methods
          using CRTP::self;
          //********************************************************************

         protected:
          //**Type definitions**************************************************
          //! List of initialized variables
          using InitializedVariables =
              tmpl::append<typename Parent::InitializedVariables,
                           typename VariableDefinitions::InitializedVariables>;
          //! List of computed variables
          using ComputedVariables =
              tmpl::append<typename Parent::ComputedVariables,
                           typename VariableDefinitions::ComputedVariables>;
          //! List of all variables
          using Variables =
              tmpl::append<typename Parent::Variables,
                           typename VariableDefinitions::Variables>;
          //! Initialized variables
          using typename VariableDefinitions::ReferenceCurvature;
          using typename VariableDefinitions::ReferenceElementLength;
          //! Computed variables
          using typename VariableDefinitions::Curvature;
          using typename VariableDefinitions::ElementDilatation;
          using typename VariableDefinitions::ElementLength;
          using typename VariableDefinitions::ReferenceVoronoiLength;
          using typename VariableDefinitions::ShearStretchStrain;
          using typename VariableDefinitions::Tangent;
          using typename VariableDefinitions::VoronoiDilatation;
          using typename VariableDefinitions::VoronoiLength;
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
            // 1. Initialize parent
            Parent::initialize(downstream_block,
                               std::forward<CosseratInitializer>(initializers));

            // 2. Initialize generated variables first
            struct {
              CosseratInitializer value;
            } cap{std::forward<CosseratInitializer>(initializers)};
            tmpl::for_each<typename VariableDefinitions::InitializedVariables>(
                [&downstream_block, &cap
                 // cap = std::move(cap)
            ](auto v) mutable -> void {
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

            // 3. Finally do the computed variables (Initialization)
            {
              // position difference
              auto& position(blocks::get<::elastica::tags::Position>(downstream_block));
              auto& director(blocks::get<::elastica::tags::Director>(downstream_block));
              auto position_diff(Traits::Operations::difference_kernel(position));

              // reference_length_collection
              auto& reference_length_collection(
                  blocks::get<ReferenceElementLength>(downstream_block));
              reference_length_collection = Traits::Operations::batch_norm(position_diff);

              // reference_voronoi_length_collection
              auto& reference_voronoi_length_collection(
                  blocks::get<ReferenceVoronoiLength>(downstream_block));
              reference_voronoi_length_collection = Traits::Operations::voronoi_length(reference_length_collection);

              // element_length_collection
              auto& element_length_collection(
                  blocks::get<ElementLength>(downstream_block));
              element_length_collection = Traits::Operations::batch_norm(position_diff);

              // element_dilatation
              auto& element_dilatation_collection(
                  blocks::get<ElementDilatation>(downstream_block));
              element_dilatation_collection = element_length_collection /
                                              reference_length_collection;
              /* Deprecated Code
                get_slice(ElementDilatationCollectionTag{}) =
                get_slice(ElementLengthCollectionTag{}) /
                get_slice(ReferenceElementLengthCollectionTag{});
               */

              // voronoi_length_collection
              auto& voronoi_length_collection(
                  blocks::get<VoronoiLength>(downstream_block));
              voronoi_length_collection = Traits::Operations::voronoi_length(element_length_collection);

              // voronoi_dilatation
              auto& voronoi_dilatation(
                  blocks::get<VoronoiDilatation>(downstream_block));
              voronoi_dilatation = voronoi_length_collection / reference_voronoi_length_collection;
                /* Deprecated Code
                 get_slice(VoronoiDilatationCollectionTag{}) =
                 get_slice(VoronoiLengthCollectionTag{}) /
                 get_slice(ReferenceVoronoiLengthCollectionTag{});
               */

              // tangent_collection
              auto& tangent_collection(
                  blocks::get<Tangent>(downstream_block));
              tangent_collection = position_diff / reference_length_collection;

              // curvature_collection (kappa)
              auto& curvature_collection(
                  blocks::get<Curvature>(downstream_block));
              Traits::Operations::batch_inv_rotate_and_divide(curvature_collection,
                                                              director,
                                                              reference_voronoi_length_collection);
              //auto rot_vec = Traits::Operations::batch_inv_rotate(director);
              //curvature_collection = rot_vec / reference_voronoi_length_collection;

              // shear_stretch_strains_collection (sigma)
              // eqt-z
              auto& shear_stretch_strains_collection(
                  blocks::get<ShearStretchStrain>(downstream_block));
              Traits::Operations::batch_matvec(
                  shear_stretch_strains_collection,
                  director,
                  tangent_collection * element_dilatation_collection // matvecexpr
                );
              Traits::Operations::batch_subtract_z_unit(shear_stretch_strains_collection);
            }
          }
          //********************************************************************

         public:
          // decide the API bro

          //**Get methods*******************************************************
          /*!\name Get methods*/
          //@{

          //********************************************************************
          /*!\name element length methods*/
          //@{
          /*!\brief Gets element lengths of the current rod
           */
          inline constexpr decltype(auto) get_element_length() & noexcept {
            return blocks::get<ElementLength>(self());
          }
          inline constexpr decltype(auto) get_element_length() const& noexcept {
            return blocks::get<ElementLength>(self());
          }
          //@}
          //********************************************************************

          //********************************************************************
          /*!\name reference element length methods*/
          //@{
          /*!\brief Gets reference element lengths of the current rod
           */
          inline constexpr decltype(auto)
          get_reference_element_length() & noexcept {
            return blocks::get<ReferenceElementLength>(self());
          }
          inline constexpr decltype(auto) get_reference_element_length()
              const& noexcept {
            return blocks::get<ReferenceElementLength>(self());
          }
          //@}
          //********************************************************************

          //********************************************************************
          /*!\name reference curvature methods*/
          //@{
          /*!\brief Gets reference curvature of the current rod
           */
          inline constexpr decltype(auto) get_reference_curvature() & noexcept {
            return blocks::get<ReferenceCurvature>(self());
          }

          inline constexpr decltype(auto) get_reference_curvature()
              const& noexcept {
            return blocks::get<ReferenceCurvature>(self());
          }
          //@}
          //********************************************************************
        };
        //**********************************************************************

      }  // namespace detail

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica
