//==============================================================================
/*!
//  \file
//  \brief A symplectic-time stepper facade for symplectic algorithms
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

#include <tuple>
#include <type_traits>

///
#include "Time/Helpers/TypeTraits.hpp"
#include "Time/SymplecticSteppers/StepperTags.hpp"
#include "Time/SymplecticSteppers/Types.hpp"
///
#include "Utilities/Invoke.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace timestepper {

    namespace symplectic {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief A facade for implementing the time-stepping algorithm
       * based on declarations in `SymplecticAlgorithm`
       * \ingroup symplectic
       *
       * \details
       * Implements the algorithmic update substeps declared in
       * `SymplecticAlgorithm`
       *
       * \usage
       *
       * \code
       * using ::elastica::timestepper::symplectic;
       * SymplecticStepper<PositionVerlet> position_verlet{};
       * \endcode
       *
       * \tparam
       * SymplecticAlgorithm Any symplectic algorithm (see examples below)
       *
       * \see PositionVerlet
       */
      template <class SymplecticAlgorithm>
      class SymplecticStepper : public NonCopyable {
       public:
        //**Type definitions****************************************************
        //! The category of the time-stepper
        using StepperCategory = elastica::timestepper::tags::SymplecticTag;
        //! The list of algorithmic substeps
        using SubSteps = typename SymplecticAlgorithm::SubSteps;
        //**********************************************************************

       private:
        //**Type definitions****************************************************
        //! Storage of of algorithmic substeps
        using AlgorithmSubSteps = tmpl::as_tuple<SubSteps>;
        //**********************************************************************

       public:
        //**********************************************************************
        /*!\brief Returns number of function evaluations per time-step
         */
        static inline constexpr auto n_feval_per_step() -> std::size_t {
          return ::elastica::tt::n_feval_per_step<SubSteps>();
        };
        //**********************************************************************

        //**********************************************************************
        /*!\brief Returns number of (state update) substeps per time-step
         */
        static inline constexpr auto n_sub_steps() -> std::size_t {
          return ::elastica::tt::n_substeps<SubSteps>();
        };
        //**********************************************************************

        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief Default constructor
         */
        SymplecticStepper() = default;
        //**********************************************************************

        //**********************************************************************
        /*!\brief Forwarding constructor
         *
         * \details
         * Useful for testing and if state is needed in the symplectic stepper
         */
        template <typename... Args>
        SymplecticStepper(Args&&... args)
            : substeps_(std::forward<Args>(args)...){}
        //**********************************************************************

        //@}
        //**********************************************************************


        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        ~SymplecticStepper() = default;
        //@}
        //**********************************************************************

       public:
        //**Substep access******************************************************
        /*!\name Substep access */
        //@{

        //**********************************************************************
        /*!\brief Access to the underlying substeps
        //
        // \return Mutable lvalue reference to the underlying substeps
        */
        inline constexpr AlgorithmSubSteps& substeps() & noexcept {
          return substeps_;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Access to the underlying substeps
        //
        // \return Constant lvalue reference to the underlying substeps
        */
        inline constexpr AlgorithmSubSteps const& substeps() const& noexcept {
          return substeps_;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Access to the underlying substeps
        //
        // \return Mutable rvalue reference to the underlying substeps
        */
        inline constexpr AlgorithmSubSteps&& substeps() && noexcept {
          return static_cast<AlgorithmSubSteps&&>(substeps_);
        }

        //**********************************************************************

        //**********************************************************************
        /*!\brief Access to the underlying substeps
        //
        // \return Const rvalue reference to the underlying substeps
        */
        inline constexpr AlgorithmSubSteps const&& substeps() const&& noexcept {
          return static_cast<AlgorithmSubSteps const&&>(substeps_);
        }
        //**********************************************************************

        //@}
        //**********************************************************************

       private:
        //**Member variables****************************************************
        //! Substep callables in the current symplectic stepper
        AlgorithmSubSteps substeps_;
        //**********************************************************************
      };
      //************************************************************************

      //**Get functions*********************************************************
      /*!\name Get functions for SymplecticStepper */
      //@{

      //************************************************************************
      /*!\brief Extract substep from a SymplecticStepper
       * \ingroup symplectic
       *
       * \details
       * Extracts the substep of the SymplecticStepper `time_stepper` whose tag
       * type is `SubStep`. Fails to compile unless the time_stepper has the
       * `SubStep` being extracted.
       *
       * \usage
       * The usage is similar to std::get(), shown below
       * \code
       * SymplecticStepper<...> b;
       * auto my_tag_data = time_steppers::get<typename
       * decltype(b)::FirstSubStep>(b);
       * \endcode
       *
       * \tparam SubStep SubStep to extract
       *
       * \param time_stepper The time_stepper to extract the tag from
       */
      template <typename SubStep, typename SymplecticAlgorithm>
      inline constexpr decltype(auto) get(
          SymplecticStepper<SymplecticAlgorithm> const& time_stepper) noexcept {
        return std::get<SubStep>(time_stepper.substeps());
      }
      template <typename SubStep, typename SymplecticAlgorithm>
      inline constexpr decltype(auto) get(
          SymplecticStepper<SymplecticAlgorithm> const&&
              time_stepper) noexcept {
        using TS = SymplecticStepper<SymplecticAlgorithm> const&&;
        return std::get<SubStep>(static_cast<TS>(time_stepper).substeps());
      }
      //************************************************************************

      //@}
      //************************************************************************

    }  // namespace symplectic

    // func returns void, but can have state
    // putting noexcept here, but it should ideally be meta

    //**************************************************************************
    /*!\brief Apply a callable on the substeps of a SymplecticStepper
     * \ingroup symplectic
     *
     * \details
     * Applies a callable `func` on the substeps of the SymplecticStepper
     * `time_stepper`.
     *
     * \usage
     * The usage is similar to std::apply(), shown below
     * \code
     * using ::elastica::timestepper::symplectic;
     * SymplecticStepper<...> const b;
     * apply_substeps_of(b, [](auto const& update){
     *  update( // custom logic);
     * })
     * \endcode
     *
     * \param time_stepper The time_stepper to extract the tag from
     * \param callable Callable to be applied on each update step
     */
    template <typename SymplecticAlgorithm, typename Callable>
    inline decltype(auto) apply_substeps_of(
        symplectic::SymplecticStepper<SymplecticAlgorithm> const& time_stepper,
        Callable callable) noexcept {
      /*
      using TS = SymplecticStepper<SymplecticAlgorithm>;
      tmpl::for_each<TS::SubSteps>([&time_stepper](auto s) {
        using Substep = tmpl::type_from<decltype(s)>;
        auto const& sub_step = symplectic::get<Substep>(time_stepper);
        func(sub_step);
      });
      */
      return cpp17::apply(
          [callable = std::move(callable)](auto const&... substep) noexcept(
              noexcept(EXPAND_PACK_LEFT_TO_RIGHT(callable(substep)))) {
            EXPAND_PACK_LEFT_TO_RIGHT(callable(substep));
            return callable;
          },
          time_stepper.substeps());
    }
    //**************************************************************************

  }  // namespace timestepper

}  // namespace elastica
