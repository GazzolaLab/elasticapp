//==============================================================================
/*!
//  \file
//  \brief Interface class for a Geometry component of
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
#include <cmath>
#include <utility>

#include "Simulator/Frames/LagrangianFrame.hpp"
#include "Utilities/CRTP.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/TMPL.hpp"

///// Types always first
#include "Systems/CosseratRods/Components/Geometry/detail/Types.hpp"
/////
#include "Systems/CosseratRods/Components/Geometry/detail/CosseratRodSpanwiseGeometry.hpp"
#include "Systems/CosseratRods/Components/Geometry/detail/Tags/CosseratRodCrossSectionInterfaceTags.hpp"

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
        /*!\brief Variables corresponding to the geometry interface component
         * \ingroup cosserat_rod_component
         *
         * \details
         * CosseratRodCrossSectionInterfaceVariables contains the definitions of
         * variables used within the Blocks framework for a rod data-structure
         * with an interface expected by other components.
         *
         * \tparam CRT A valid Cosserat Rod Traits class
         *
         * \see CosseratRodCrossSectionInterface
         */
        template <typename CRT>
        class CosseratRodCrossSectionInterfaceVariables {
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
          /*!\brief Variable marking ElementDimension within the Cosserat rod
           * hierarchy
           */
          struct ElementDimension
              : public Traits::template CosseratRodInitializedVariable<
                    ::elastica::tags::ElementDimension,  //
                    typename Traits::DataType::Scalar,   //
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

          //********************************************************************
          /*!\brief Variable marking ElementVolume within the Cosserat rod
           * hierarchy
           */
          struct ElementVolume
              : public Traits::template CosseratRodInitializedVariable<
                    ::elastica::tags::ElementVolume,    //
                    typename Traits::DataType::Scalar,  //
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
          using ComputedVariables = tmpl::list<ElementVolume,
                                               MassSecondMomentOfInertia,
                                               InvMassSecondMomentOfInertia>;
          //! List of initialized variables
          using InitializedVariables = tmpl::list<ElementDimension>;
          //! List of all variables
          using Variables =
              tmpl::append<InitializedVariables, ComputedVariables>;
          //********************************************************************
        };
        //**********************************************************************

        // TODO : Protocol for cross section trait
        //======================================================================
        //
        //  CLASS DEFINITION
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Interface component corresponding to a spanwise rod with a
         * particular shape of cross section
         * \ingroup cosserat_rod_component
         *
         * \details
         * CosseratRodCrossSectionInterface implements the penultimate Geometry
         * component use within Cosserat rods implemented in the Blocks
         * framework. It denotes a Cosserat rod data-structure with a particular
         * shape of cross section (such as a square or a circle) spanning a
         * single spatial dimension (i.e. along centerline coordinates)
         *
         * \tparam CRT A valid Cosserat Rod Traits class
         * \tparam ComputationalBlock The final block which is derived from the
         * current component
         * \tparam CrossSectionOperations Operations for particular cross
         * section shapes (such as a square or a circle)
         *
         * \see CosseratRodCrossSectionInterfaceVariables
         */
        template <typename CRT, typename ComputationalBlock,
                  typename CrossSectionOperations>
        class CosseratRodCrossSectionInterface
            : public CosseratRodSpanwiseGeometry<CRT, ComputationalBlock>,
              public CosseratRodCrossSectionInterfaceVariables<CRT>,
              public CrossSectionOperations,
              public CRTPHelper<ComputationalBlock,
                                CosseratRodCrossSectionInterface> {
         protected:
          //**Type definitions**************************************************
          //! Frame type
          using Frame = ::elastica::detail::LagrangianFrame;
          //! Frame direction type
          using DirectionType = typename Frame::DirectionType;
          //! Cross section type
          // this is protected for access in CrossSectionOperations
          using CrossSection = CrossSectionOperations;
          //********************************************************************

         private:
          //**Type definitions**************************************************
          //! Traits type
          using Traits = CRT;
          //! This type
          using This =
              CosseratRodCrossSectionInterface<Traits, ComputationalBlock,
                                               CrossSection>;
          //! Parent type
          using Parent =
              CosseratRodSpanwiseGeometry<Traits, ComputationalBlock>;
          //! Variable definitions
          using VariableDefinitions =
              CosseratRodCrossSectionInterfaceVariables<Traits>;
          //! CRTP Type
          using CRTP =
              CRTPHelper<ComputationalBlock, CosseratRodCrossSectionInterface>;
          //! CRTP methods
          using CRTP::self;
          //! Real number type
          using real_type = typename Traits::real_type;
          //! Index type
          using index_type = typename Traits::index_type;
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
          //! Current Variable definitions
          using typename Parent::ReferenceCurvature;
          using typename Parent::ReferenceElementLength;
          using typename Parent::Curvature;
          using typename Parent::ElementDilatation;
          using typename Parent::ReferenceVoronoiLength;
          using typename Parent::ShearStretchStrain;
          using typename Parent::Tangent;
          using typename Parent::VoronoiDilatation;
          using typename Parent::VoronoiLength;
          using typename Parent::ElementLength;

          using typename VariableDefinitions::MassSecondMomentOfInertia;
          using typename VariableDefinitions::InvMassSecondMomentOfInertia;
          using typename VariableDefinitions::ElementDimension;
          using typename VariableDefinitions::ElementVolume;
          //********************************************************************

         private:
          //**Parent methods****************************************************
          //! Methods inherited from parent class
          using CrossSection::compute_cross_section_area;
          using CrossSection::compute_second_moment_of_area;
          using CrossSection::update_dimension;

         public:
          using CrossSection::shape_factor;
          //********************************************************************

          //**Static members****************************************************
          //! unique ID for D1 direction
          static constexpr DirectionType D1 = Frame::D1;
          //! unique ID for D2 direction
          static constexpr DirectionType D2 = Frame::D2;
          //! unique ID for D3 direction
          static constexpr DirectionType D3 = Frame::D3;
          //********************************************************************

         protected:
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

            // 2. Initialize generators first
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

                  auto dofs = Variable::get_dofs(cap.value.n_elems());
                  for (std::size_t i = 0; i < dofs; ++i) {
                    Variable::slice(variable, i) = initializer(i);
                  }
                });

            // 3. Computed tags

            {
              auto& volumes(blocks::get<::elastica::tags::ElementVolume>(
                  downstream_block));
              auto& dimensions(blocks::get<::elastica::tags::ElementDimension>(
                  downstream_block));
              auto& lengths(blocks::get<::elastica::tags::ElementLength>(
                  downstream_block));

              volumes = compute_cross_section_area(dimensions) * lengths;
            }

            /* mass second moment of inertia */
            {
              using Variable = MassSecondMomentOfInertia;
              using Tag = blocks::initializer_t<Variable>;


              auto& variable(blocks::get<Tag>(downstream_block));
              const auto dofs = Variable::get_dofs(initializers.n_elems());
              for (std::size_t idx = 0U; idx < dofs; ++idx) {
                // (I_1, I_2, I_3)
                Variable::slice(variable, idx) = real_type(0.0);
                Variable::diagonal_assign(variable, idx,
                    downstream_block.get_second_moment_of_area(idx));
              }
            }

            /* inverse mass second moment of inertia */
            {
              using Variable = InvMassSecondMomentOfInertia;
              using Tag = blocks::initializer_t<Variable>;

              auto& variable(blocks::get<Tag>(downstream_block));
              const auto dofs = Variable::get_dofs(initializers.n_elems());
              for (std::size_t idx = 0U; idx < dofs; ++idx) {
                // inv(I_1, I_2, I_3)
                Variable::slice(variable, idx) = real_type(0.0);
                  Variable::diagonal_assign(variable, idx,
                      real_type(1.0)/downstream_block.get_second_moment_of_area(idx));
              }
            }
          }
          //********************************************************************

         public:
          //**Get methods*******************************************************
          /*!\name Get methods*/
          //@{

          // These are all interface functions, explicitly disallow on temporary
          // objects by non overload on &&
          //********************************************************************
          /*!\name dimension methods*/
          //@{
          /*!\brief Gets dimension of the current rod
           */
          inline constexpr decltype(auto) get_element_dimension() & noexcept {
            return blocks::get<ElementDimension>(self());
          }
          inline constexpr decltype(auto) get_element_dimension()
              const& noexcept {
            return blocks::get<ElementDimension>(self());
          }
          inline constexpr decltype(auto) get_element_dimension(
              index_type idx) & noexcept {
            return ElementDimension::slice(get_element_dimension(), idx);
          }
          inline constexpr decltype(auto) get_element_dimension(
              index_type idx) const& noexcept {
            return ElementDimension::slice(get_element_dimension(), idx);
          }
          //@}
          //********************************************************************

          //********************************************************************
          /*!\name volume methods*/
          //@{
          /*!\brief Gets volume of the current rod
           */
          inline constexpr decltype(auto) get_element_volume() & /*noexcept*/ {
            // Do we need it here, or should it be under update volume?
            //          (blocks::get<ElementVolume>(self())) =
            //              (blocks::get<ElementDimension>(self())) *
            //              (blocks::get<ElementLength>(self()));
            return blocks::get<ElementVolume>(self());
          }
          inline constexpr decltype(auto) get_element_volume() const& noexcept {
            return blocks::get<ElementVolume>(self());
          }
          inline constexpr decltype(auto) get_element_volume(
              index_type idx) & noexcept {
            return ElementVolume::slice(get_element_volume(), idx);
          }
          inline constexpr decltype(auto) get_element_volume(
              index_type idx) const& noexcept {
            return ElementVolume::slice(get_element_volume(), idx);
          }
          //@}
          //********************************************************************

          //********************************************************************
          /*!\name area methods*/
          //@{
          /*!\brief Gets area of the current rod
           */
          inline constexpr auto get_area() const& /*noexcept*/ {
            // Should give out a VecVecExpr rather than Dimensions object
            // for expression template mechanisms
            return compute_cross_section_area(This::get_element_dimension());
          }
          inline constexpr auto get_area(index_type idx) const& /*noexcept*/ {
            // Should give out a VecVecExpr rather than Dimensions object
            // for expression template mechanisms
            return compute_cross_section_area(This::get_element_dimension(idx));
          }
          //@}
          //********************************************************************

          //********************************************************************
          /*!\name second moment of area methods*/
          //@{
          /*!\brief Gets second moment of area of the current rod
           */
          template <DirectionType Dim, Requires<(Dim < Frame::D3)> = nullptr>
          inline constexpr decltype(auto) get_second_moment_of_area() const
          /*noexcept*/ {
            // Should give out a VecVecExpr rather than Dimensions object
            // for expression template mechanisms
            return compute_second_moment_of_area(This::get_element_dimension());
          }
          template <DirectionType Dim, Requires<(Dim == Frame::D3)> = nullptr>
          inline constexpr decltype(auto) get_second_moment_of_area() const
          /*noexcept*/ {
            // Should give out a VecVecExpr rather than Dimensions object
            // for expression template mechanisms
            return real_t(2.0) *
                   compute_second_moment_of_area(This::get_element_dimension());
          }
          template <DirectionType Dim>
          inline constexpr decltype(auto) get_second_moment_of_area(
              index_type idx) const /*noexcept*/ {
            // Should give out a VecVecExpr rather than Dimensions object
            // for expression template mechanisms
            return ElementDimension::slice(
                This::get_second_moment_of_area<Dim>(), idx);
          }

          // do not have APIs return vec3, seems to reduce customizability and
          // promotes bad code
          inline constexpr auto get_second_moment_of_area(index_type idx) const
          /*noexcept*/ {
            return Vec3{real_t(This::get_second_moment_of_area<D1>(idx)),
                        real_t(This::get_second_moment_of_area<D2>(idx)),
                        real_t(This::get_second_moment_of_area<D3>(idx))};
          }

          //@}
          //********************************************************************

          //@}
          //********************************************************************

          /*
          inline constexpr auto get_volumes(std::size_t i) const noexcept
          -> float_type {
            return This::get<VolumeCollectionTag>()[i];
          }

          inline constexpr auto get_area(std::size_t i) const noexcept
              -> float_type {
            return self().area_impl(This::get_dimension(i));
          }

          template <DirectorType Dim, Requires<(Dim < This::D3)> = nullptr>
          inline constexpr auto get_second_moment_of_area(
              std::size_t i) const noexcept -> float_type {
            return self().second_moment_of_area_impl(This::get_dimension(i));
          }

          template <DirectorType Dim, Requires<(Dim == This::D3)> = nullptr>
          inline constexpr auto get_second_moment_of_area(
              std::size_t i) const noexcept -> float_type {
            // Perpendicular axis theorem
            return float_type(2.0) *
                   self().second_moment_of_area_impl(This::get_dimension(i));
          }
          inline constexpr auto get_second_moment_of_area(
              std::size_t i) const noexcept {
            return Vec3{get_second_moment_of_area<D1>(i),
                        get_second_moment_of_area<D2>(i),
                        get_second_moment_of_area<D3>(i)};
          }

          */

          //      inline constexpr auto get_second_moment_of_area() const
          //      noexcept
          //      {
          //        return Vec3{get_second_moment_of_area<D1>(),
          //                    get_second_moment_of_area<D2>(),
          //                    get_second_moment_of_area<D3>()};
          //      }
          //

          // Technically these can be put in an interface class that has
          // the ability to compute curvature/shear etc., but we can leave
          // it aside for now.

         private:
          //**Computation methods***********************************************
          /*!\name Computation methods*/
          //@{

          //**************************************************************************
          /*!\brief Updates rod geometric properties given state.
          //
          // \details
          // Translational functions
          // Updates rod elemental geometric properties (elemental lengths,
          // tangents and radii) from given nodal positions (state).
          // Ref: eq (3.3) from \cite gazzola2018forward. \n
          // length{i} = || position{i+1} - position{i} || \n
          // tangent{i} = (position{i+1} - position{i}) / length{i} \n
          // radius{i} = (volume{i} / M_PI / length{i})^0.5
          //
          // The following members are modified:
          //  - length_batch(n_elems)
          //  - tangent_batch(3, n_elems)
          //  - radius_batch(n_elems)
          //
          // with the following inputs:
          //  - position_batch(3, n_nodes)
          //  - volume_batch(n_elems)
          //
          // \example
          // The following shows a typical use of the compute_geometry_from_state()
          // function with the expected (correct) result also shown.
          //
          // \return void/None
          //
          */
          void compute_translation_diagnostics() noexcept {
            // Compute eq (3.3) from 2018 RSOS paper
            // 0. Create reference variables
            auto& position(blocks::get<::elastica::tags::Position>(self()));
            auto& director(blocks::get<::elastica::tags::Director>(self()));

            // 1. Compute new lengths
            auto position_diff(Traits::Operations::difference_kernel(position));
            auto& element_length_collection(blocks::get<ElementLength>(self()));
            element_length_collection = Traits::Operations::batch_norm(position_diff);

            // 2. Update tangents
            auto& tangent_collection(blocks::get<Tangent>(self()));
            auto& reference_length_collection(blocks::get<ReferenceElementLength>(self()));
            Traits::Operations::batch_division_matvec(tangent_collection, position_diff, reference_length_collection);

            // 3. Compute new radius based on above lengths
            // volume conservation
            auto& volume_collection(blocks::get<ElementVolume>(self()));
            auto& dimensions_collection(blocks::get<ElementDimension>(self()));
            dimensions_collection =
                CrossSection::update_dimension(volume_collection, element_length_collection);

            // 4. Compute dilatation of elements
            auto& element_dilatation_collection(blocks::get<ElementDilatation>(self()));
            element_dilatation_collection = element_length_collection /
                                            reference_length_collection;

            // 5. Compute dilatation of voronoi regions
            auto& voronoi_dilatation(blocks::get<VoronoiDilatation>(self()));
            voronoi_dilatation = blocks::get<VoronoiLength>(self()) /
                                 blocks::get<ReferenceVoronoiLength>(self());
          }

         public:
          //**************************************************************************
          /*!\brief Compute elemental shear and stretch mode strains
          //
          // \details
          // Computes elemental shear and stretch modes strains given the rod
          // positions and directors (state), calls compute_all_dilatations() inside.
          // Ref: eq (2.9) from \cite gazzola2018forward. \n
          // strain_batch{i} =
          // dilatation_batch{i} * director_collection_batch{i} *
          // tangent_batch{i} - {0.0, 0.0, 1.0}
          //
          // The following members are modified:
          // - strain_batch(3, n_elems)
          //
          // with the following inputs:
          // - director_collection_batch(3, 3, n_elems)
          // - temp container of (3, n_elems) for intermediate storage \n
          // given inputs of compute_all_dilatations() are set.
          //
          // \example
          // The following shows a typical use of the compute_shear_stretch_strains()
          // function with the expected (correct) result also shown.
          // \snippet test_gov_eqns.cpp compute_shear_stretch_strains_example
          //
          // \return void/None
          //
          // \see fill later?
          */
          void compute_shear_strains() noexcept {
            // Q (et - d_3)
            // Q is always latest, only e and t need to be computed
            compute_translation_diagnostics(); // Dilatation updated here!

            //z_vector = np.array([0.0, 0.0, 1.0]).reshape(3, -1)
            //sigma[:] = dilatation * _batch_matvec(director_collection, tangents) - z_vector
            auto& shear_stretch_strains_collection(
                blocks::get<::elastica::tags::ShearStretchStrain>(self()));
            auto expanded_transposed_EDC = Traits::Operations::expand_for_broadcast(Traits::Operations::transpose(
                blocks::get<::elastica::tags::ElementDilatation>(self())));
            Traits::Operations::batch_matvec(
                shear_stretch_strains_collection,
                blocks::get<::elastica::tags::Director>(self()),
                blocks::get<::elastica::tags::Tangent>(self()) % expanded_transposed_EDC // matvecexpr
            );
            Traits::Operations::batch_subtract_z_unit(shear_stretch_strains_collection);
          }

          void compute_curvature() noexcept {
            // kappa is the only rotational diagnostic, so we direcelty compute
            // that here

            auto& curvature_collection(
                blocks::get<::elastica::tags::Curvature>(self()));
            auto& reference_voronoi_length_collection(
                blocks::get<::elastica::tags::ReferenceVoronoiLength>(self()));
            auto& director(blocks::get<::elastica::tags::Director>(self()));
            Traits::Operations::spanwise_inv_rotate_and_divide(
                curvature_collection, director,
                reference_voronoi_length_collection);
            //auto rot_vec = Traits::Operations::batch_inv_rotate();
            //curvature_collection = rot_vec / reference_voronoi_length_collection;

            // Deprecated Code:
            // std::get<typename GeometryParent::CurvatureCollectionTag>(
            // self().data()) = 1.0;
            // get_curvature() = log(get_rotation_matrices())
          }

          //@}
          //********************************************************************
        };
        //**********************************************************************

      }  // namespace detail

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica

//  public RodKinematics<RodTraits>
// Sparse_strategy
// Dense strategy
/* Lessons learnt:
 * We wanted to CRTP multiple times to achieve multiple-inheritance
 * during compile-time. However putting the tag structs inside the
 * CRTP base class is problematic as the tags are also stamped with
 * the CRTP-ed derived class (aka the CrossSection template) below.
 * For some reason (still unknown), this causes the tuples to not find
 * the tags initialized from the block ||-> plugin || -> circle geometry
 * which is then specially CRTped to the CrossSectionBase class.
 *
 * The error comes because
 * We search for
    elastica::detail_cosserat_rod::CosseratRodTraits::VectorSlice<elastica::detail_cosserat_rod::CrossSectionBase<elastica::detail_cosserat_rod::CosseratRodTraits,
    elastica::detail_cosserat_rod::CircleCrossSection<elastica::detail_cosserat_rod::CosseratRodTraits,
    elastica::Block<elastica::CosseratRodPlugin<elastica::detail_cosserat_rod::CosseratRodTraits,
    elastica::Block, elastica::detail_cosserat_rod::CircleCrossSection> > >
    >::VolumeCollectionTag, elastica::detail_cosserat_rod::ElementTagImpl>;

    But in the tuple we only have
    elastica::detail_cosserat_rod::CosseratRodTraits::VectorSlice<elastica::detail_cosserat_rod::CrossSectionBase<elastica::detail_cosserat_rod::CosseratRodTraits,
    elastica::detail_cosserat_rod::CircleCrossSection<elastica::detail_cosserat_rod::CosseratRodTraits,
    elastica::CosseratRodPlugin<elastica::detail_cosserat_rod::CosseratRodTraits,
    elastica::Block, elastica::detail_cosserat_rod::CircleCrossSection> >
    >::VolumeCollectionTag, elastica::detail_cosserat_rod::ElementTagImpl>,

    Notice how an extra "Block" template gets added to the search parameter
   around the CosseratRodPlugin. This seems to be the right type to be
   searched for, at least while looking at the hierarchy that we have.
 However in the tuple types there's a deficiency of the block types for some
 reason.

   To avoid these hassle, we again split the CrossSectionBase into
 CrossSectionOperations templated on the CosseratRodTraits. The sole purpose
 of this parent is just to provide meta-data on what the cross-section base
 should contain.
 *
 */

/*
 * In this multiple CRTP inheritance, another trick is used. We know
 * that the CrossSection class will only have static members. Then
 * instead of templating on the ComputaionalBlock (the impl) class
 * we template on the CrossSection instead and give it a friend access.
 * Thie leads to a more "sensible" code since only CrossSection can
 * be used as the final class (aka CrossSectionBase cannot be used
 * to instantiate a Cosserat Rod), thus preventing wrong misuse. Also
 * the CrossSection hierarchy is self contained and can be reused
 * without worrying about blocks getting interchanged in the
 * CrossSection Base and CrossSection (since only one block is
 * templated)
 *
 *
 */
