//==============================================================================
/*!
//  \file
//  \brief Adapter to make an imposed primitive
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Simulator/Environments/Primitives/Access.hpp"
#include "Simulator/Environments/Primitives/Types.hpp"
//
#include "Simulator/Environments/Kinematics.hpp"
#include "Simulator/Frames.hpp"
//
#include "Time/SimulationTime.hpp"

namespace elastica {

  namespace collision {

    //**************************************************************************
    /*!\brief Base class for all primitive geometries.
     * \ingroup primitives
     *
     * The ImposedPrimitiveAdapter class adapts a rigid body primitive of the
     * the \elastica engine. In addition to primitive entities, users can
     * impose the motion (translations for now) at every step, by coding
     * positions and velocities.
     *
     * \tparam Primitive_ Primitive to adapt
     *
     * \note
     * `Primitive_` is a elastica Primitive typically conforming to
     * protocols::ConvexPrimitive
     */
    template <typename Primitive_>
    class ImposedPrimitiveAdapter final : public Primitive_ {
     protected:
      //**Type definitions******************************************************
      //! Internal erased type
      using Func_ = std::function<real_t(real_t)>;
      //! Translation in the X direction
      using TX_ = kinematics::detail::TranslationKinematics<EulerianFrame::X,
                                                            Func_, Func_>;
      //! Translation in the Y direction
      using TY_ = kinematics::detail::TranslationKinematics<EulerianFrame::Y,
                                                            Func_, Func_>;
      //! Translation in the Z direction
      using TZ_ = kinematics::detail::TranslationKinematics<EulerianFrame::Z,
                                                            Func_, Func_>;
      //! Translation activations
      using TranslationsActivations = kinematics::Translations<TX_, TY_, TZ_>;
      //************************************************************************

      template <::elastica::EulerianFrame::DirectionType Direction_>
      static inline auto get_component_wrapped(
          ::elastica::Vec3 const& all) noexcept {
        using ::elastica::cast_along;
        return [val = all[cast_along<Direction_>()]](
                   real_t) noexcept -> real_t { return val; };
      }

      template <::elastica::EulerianFrame::DirectionType Direction_>
      static inline auto make_translation_(::elastica::Vec3 const& p,
                                           ::elastica::Vec3 const& v) {
        return kinematics::detail::translate_along<Direction_>(
            get_component_wrapped<Direction_>(p),
            get_component_wrapped<Direction_>(v));
      }

      static inline auto make_translations_(::elastica::Vec3 const& p,
                                            ::elastica::Vec3 const& v) {
        return TranslationsActivations{
            make_translation_<EulerianFrame::X>(p, v),
            make_translation_<EulerianFrame::Y>(p, v),
            make_translation_<EulerianFrame::Z>(p, v)};
      }

      void fill_rates_() noexcept {
        // In debug mode, v, w maybe set to nan for early failure, so we
        // reset it here
        Primitive::v_ = real_t(0.0);
        Primitive::w_ = real_t(0.0);
      }

     public:
      //**Type definitions******************************************************
      //! Primitive type
      using Primitive = Primitive_;
      //! Parent type
      using Parent = Primitive;
      //! This type
      using This = ImposedPrimitiveAdapter<Primitive>;
      //************************************************************************

      //**Constructor***********************************************************
      /*!\name Constructor */
      //@{

      //************************************************************************
      /*!\brief Constructor for the ImposedPrimitiveAdapter class.
       *
       * \param primitive The primitive information to initialize the imposed
       * primitive with
       */
      explicit ImposedPrimitiveAdapter(Primitive_ primitive)
          // here we force the positions to be copied
          : Primitive_(std::move(primitive)),
            translations_(make_translations_(::elastica::position(*this),
                                             ::elastica::velocity(*this))) {
        fill_rates_();
        // Removing results in a floating point exception
        translations_ = make_translations_(::elastica::position(*this),
                                           ::elastica::velocity(*this));
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Constructor for the ImposedPrimitiveAdapter class.
       *
       * \param primitive The primitive information to initialize the imposed
       * primitive with
       * \param translations Translation motion for imposition
       */
      template <typename... Kinematics>
      ImposedPrimitiveAdapter(
          Primitive_ primitive,
          kinematics::Translations<Kinematics...> translations)
          : Primitive_(std::move(primitive)),
            translations_(std::move(translations)) {
        fill_rates_();
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Default move constructor for the ImposedPrimitiveAdapter class.
       */
      ImposedPrimitiveAdapter(ImposedPrimitiveAdapter&&) noexcept = default;
      //************************************************************************

      //@}
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~ImposedPrimitiveAdapter() = default;
      //@}
      //************************************************************************

      //**Utility functions*****************************************************
      /*!\name Utility functions */
      //@{

      //************************************************************************
      /*!\brief Updates primitive information at some time
       *
       * \param time_v The time of update
       */
      void update_primitives(::elastica::Time time_v) /*noexcept*/ {
        real_t px =
            translations_.template compute_position_along<EulerianFrame::X>(
                time_v);
        real_t py =
            translations_.template compute_position_along<EulerianFrame::Y>(
                time_v);
        real_t pz =
            translations_.template compute_position_along<EulerianFrame::Z>(
                time_v);
        Parent::gpos_ = Vec3{px, py, pz};
        // Setting the axis-aligned bounding box
        Parent::calculate_bounding_box();
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Updates rate information at some time
       *
       * \param time_v The time of update
       */
      void update_rates(::elastica::Time time_v) /*noexcept*/ {
        real_t vx =
            translations_.template compute_velocity_along<EulerianFrame::X>(
                time_v);
        real_t vy =
            translations_.template compute_velocity_along<EulerianFrame::Y>(
                time_v);
        real_t vz =
            translations_.template compute_velocity_along<EulerianFrame::Z>(
                time_v);
        Parent::v_ = Vec3{vx, vy, vz};
      }
      //************************************************************************

      //@}
      //************************************************************************

     private:
      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      //! The buffer for translation activations
      TranslationsActivations translations_;
      //@}
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica