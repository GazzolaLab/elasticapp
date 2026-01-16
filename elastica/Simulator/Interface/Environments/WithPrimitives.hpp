//==============================================================================
/*!
//  \file
//  \brief Primitives component of the \elastica Environments module
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

///
#include "Simulator/Interface/Environments/Types.hpp"
//
#include "Simulator/Interface/Environments/Emplacer.hpp"
#include "Simulator/Interface/Environments/WithPrimitivesExecutionPolicy.hpp"
/// Serialization support
#include "Simulator/IO/Serialization/Types.hpp"
/// Profiling support
#include "ModuleSettings/Profiling.hpp"
/// Primitives
#include "Simulator/Environments/Primitives/Types.hpp"
//
#include "Simulator/Environments/Primitives/PrimitivesCollection.hpp"
#include "Simulator/Environments/Primitives/TypeTraits.hpp"
#include "Simulator/Environments/Protocols.hpp"
///
#include "Simulator/Utilities/Offsets.hpp"
//
#include "Utilities/CRTP.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/Logging.hpp"
#include "Utilities/NoSuchType.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TMPL.hpp"
///

namespace elastica {

  namespace interface {

    namespace environments {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename EnvironmentsTraits>
      struct WithPrimitivesTraits {
       private:
        using SC =
            typename EnvironmentsTraits::ListOfAdmissibleSystemCollections;

       public:
        //**Type definitions****************************************************
        //! Typelist of admissible primitives
        // Can be made lazy : any primitives marked with NoSuchType are removed
        using AdmissiblePrimitives = tmpl::remove_duplicates<tmpl::remove<
            tmpl::transform<
                SC, ::elastica::collision::tt::PrimitiveTrait<tmpl::_1>>,
            NoSuchType>>;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*!\brief Primitives for the Environments module
       * \ingroup interface environments
       *
       * \details
       * This component to the Environments module allows for adding
       * primitives (shapes) whose dynamics are integrated/computed in time.
       *
       * \tparam EnvironmentTraits Options for configuration of the Environments
       * Module
       * \tparam Sim Simulator
       *
       * \see elastica::interface::Environments
       */
      template <class EnvironmentsTraits, class Sim>
      class WithPrimitives
          : public CRTPHelper<Sim, WithPrimitives>,
            protected WithPrimitivesTraits<EnvironmentsTraits>,
            /* Here I derive from Sim instead of
             * WithPrimitives<EnvironmentsTraits, Sim>
             * because I need access to the iteration policies from the systems
             * module.
             */
            protected WithPrimitivesExecutionPolicy<EnvironmentsTraits, Sim>,
            public NonCopyable {
       private:
        //**Type definitions****************************************************
        //! This type
        using This = WithPrimitives<EnvironmentsTraits, Sim>;
        //! Custom traits type
        using CustomTraits = WithPrimitivesTraits<EnvironmentsTraits>;
        //! CRTP Type
        using CRTP = CRTPHelper<Sim, WithPrimitives>;
        //! CRTP methods
        using CRTP::self;
        //! Internal size type
        using SizeType = typename EnvironmentsTraits::SizeType;
        //! Algorithm type
        using Algorithm =
            ::elastica::interface::environments::WithPrimitivesExecutionPolicy<
                EnvironmentsTraits, Sim>;
        //**********************************************************************

        // Serialization purely for n_primitives and restore

        //**Friendships*********************************************************
        //! Friendship with Serialize
        friend struct ::elastica::io::Serialize<This>;
        //**********************************************************************

       protected:
        //**Type definitions****************************************************
        //! Type of traits
        using Traits = CustomTraits;
        //! Type of admissible primitives
        using typename CustomTraits::AdmissiblePrimitives;
        //**********************************************************************

       public:
        //**Type definitions****************************************************
        //! Tags of admissible environments
        using AdmissibleEnvironmentTags =
            typename EnvironmentsTraits::AdmissibleEnvironmentTags;
        //! Type for storing imposed primitives
        using Primitives_ = elastica::collision::PrimitivesCollection<
            AdmissiblePrimitives /*Can do a container policy here*/>;
        //! Type for storing offsets
        using OffsetsMap_ = elastica::OffsetsMap<
            typename EnvironmentsTraits::ListOfAdmissibleSystemCollections>;
        //! Typelist of submodules
        using SubModules = tmpl::list<Algorithm>;
        //**********************************************************************

       public:
        //**Type definitions****************************************************
        //! Type of simulation time
        using TimeType = typename EnvironmentsTraits::TimeType;
        // made public for protocols
        //**********************************************************************
       protected:
        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief Constructor of the Primitives add-on class.
         */
        WithPrimitives() : primitives_(), offsets_map_() {
          using namespace ::elastica::logging;
          log<info>([](auto const& log) {
            log << "Successfully initialized the primitives add-on for "
                   "the Environments module";
          });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Move constructor
         */
        WithPrimitives(WithPrimitives&& other) noexcept
            : primitives_(std::move(other.primitives_)),
              offsets_map_(std::move(other.offsets_map_)) {}
        //**********************************************************************

        //@}
        //**********************************************************************

       public:
        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        ~WithPrimitives() {
          using namespace ::elastica::logging;
          log<info>([](auto const& log) {
            log << "Successfully cleaned up the primitives add-on for "
                   "the Environments module";
          });
        };
        //@}
        //**********************************************************************

       public:
        //**Get methods*********************************************************
        /*!\name Get methods */
        //@{

        //**********************************************************************
        /*!\brief Gets the net imposed primitives buffer
         */
        inline constexpr auto primitives() & noexcept -> Primitives_& {
          return primitives_;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\copydoc WithPrimitives::primitives()
         */
        inline constexpr auto primitives() const& noexcept
            -> Primitives_ const& {
          return primitives_;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets a primitives buffer for a particular Primitive
         *
         * \tparam Primitive The type of primitive
         */
        template <typename Primitive>
        inline constexpr decltype(auto) primitives() & noexcept {
          using elastica::collision::get_collection;
          return get_collection<Primitive>(primitives());
        }
        //**********************************************************************

        //**********************************************************************
        /*!\copydoc WithPrimitives::primitives()
         */
        template <typename Primitive>
        inline constexpr decltype(auto) primitives() const& noexcept {
          using elastica::collision::get_collection;
          return get_collection<Primitive>(primitives());
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets the offsets buffer
         */
        inline constexpr auto offsets() & noexcept -> OffsetsMap_& {
          return offsets_map_;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\copydoc WithPrimitives::offsets()
         */
        inline constexpr auto offsets() const& noexcept -> OffsetsMap_ const& {
          return offsets_map_;
        }
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Functor methods*****************************************************
        /*!\name Functor methods */
        //@{

        //**********************************************************************
        /*!\brief Apply a callable over all primitives
         *
         * \details
         * Applies a callable `func` over all primitives
         * (across types and collections)
         *
         * \note
         * We need strong exception safety guarantee from `func`. In C++14 this
         * cannot be strongly checked, so the user needs to satisfy this
         * guarantee
         *
         * \param func Callable to be applied over primitives
         */
        template <typename F>
        inline decltype(auto) for_each_primitive(F func) /*const*/ noexcept {
          using Alg = typename Algorithm::ApplyOverPrimitives;

          ELASTICA_PROFILING_SECTION {
            self().template get_timer<Alg>().start();
          }
          auto const& algorithm = self().template get_algorithm<Alg>();
          F another_func(algorithm(std::move(func)));
          ELASTICA_PROFILING_SECTION {
            self().template get_timer<Alg>().stop();
          }
          return another_func;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Apply a callable over a subset of primitives
         *
         * \details
         * Applies a callable `func` over all primitives
         * (across types and collections)
         *
         * \note
         * We need strong exception safety guarantee from `func`. In C++14 this
         * cannot be strongly checked, so the user needs to satisfy this
         * guarantee
         *
         * \tparam PrimitivesList A `tmpl::list` of Primitives over which to
         * apply the callable
         * \param func Callable to be applied over primitives
         */
        template <typename PrimitivesTypeList, typename F>
        inline decltype(auto) for_each_primitive(F func) /*const*/ noexcept {
          using Alg = typename Algorithm::ApplyOverPrimitives;

          ELASTICA_PROFILING_SECTION {
            self().template get_timer<Alg>().start();
          }
          auto const& algorithm = self().template get_algorithm<Alg>();
          F another_func(algorithm.template operator()<PrimitivesTypeList>(
              std::move(func)));
          ELASTICA_PROFILING_SECTION {
            self().template get_timer<Alg>().stop();
          }

          return another_func;
        }
        //**********************************************************************

        //@}
        //**********************************************************************

       public:
        //**Interfaces**********************************************************
        /*!\name Interface methods */
        //@{

        //**********************************************************************
        /*!\brief Gets the number of primitives registered in the present
         * simulator
         *
         * \tparam Primitive The primitive you want to check
         */
        template <typename Primitive>
        inline auto n_primitives() const noexcept -> SizeType {
          using ::elastica::collision::get_collection;
          return std::size(get_collection<Primitive>(primitives()));
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets the total number of primitives registered in the present
         * simulator
         */
        inline auto n_primitives() const noexcept -> SizeType {
          std::size_t number_primitives = 0UL;
          tmpl::for_each<AdmissiblePrimitives>([&](auto p) {
            using Primitive = tmpl::type_from<decltype(p)>;
            number_primitives += n_primitives<Primitive>();
          });
          return number_primitives;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Populates the primitives from the registered systems
         */
        void populate_primitives() {
          using ::elastica::collision::Emplacer;
          Emplacer::create_primitives(self());
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Check if any of the registered environments needs primitives
         * support
         *
         * \returns std::bool_constant
         */
        static constexpr auto primitives_are_required() {
          using Test =
              ::elastica::environments::protocols::EnvironmentTag::Signatures;
          return std::bool_constant<tmpl::fold<
              tmpl::transform<
                  AdmissibleEnvironmentTags,
                  tmpl::bind<Test::template requires_primitives_result,
                             tmpl::_1>>  // transform
              ,
              // Even if there are no tags, we need false here
              std::false_type,
              tmpl::or_<tmpl::_state, tmpl::_element>>::value>{};
        }
        //**********************************************************************

        //@}
        //**********************************************************************

       protected:
        //**Reset methods*******************************************************
        /*!\name Reset methods*/
        //@{

        //**********************************************************************
        /*!\brief Resets the imposed primitives module
         *
         * \param time_v The time of reset
         * \exception std::runtime_error
         */
        void reset_module(TimeType /*time_v*/) {
          // we don't shrink to fit here since it may well be used again
          std::apply(
              [](auto&... primitive_container) {
                EXPAND_PACK_LEFT_TO_RIGHT(primitive_container.clear());
              },
              primitives().data());
          offsets_map_ = OffsetsMap_();
        }
        //**********************************************************************

        //@}
        //**********************************************************************

       protected:
        //**Finalize methods****************************************************
        /*!\name Finalize methods*/
        //@{

        //**********************************************************************
        /*!\brief The finalize method.
         *
         * \details
         * The finalize method is the interface to finalize the primitives
         * module of elastica before launching into temporal integration.
         *
         * \param time_v The time to finalize, usually 0.0
         * \exception std::runtime_error If any constraint information is
         * missing/faulty
         */
        void finalize(TimeType /*time_v*/) {
          if (primitives_are_required()) {
            populate_primitives();
          }
        }
        //**********************************************************************

        //@}
        //**********************************************************************

       private:
        //**Member variables****************************************************
        //! The container of different primitives
        Primitives_ primitives_;
        //! The type of mapping from systems to offsets into primitives buffer
        OffsetsMap_ offsets_map_;
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace environments

  }  // namespace interface

}  // namespace elastica
