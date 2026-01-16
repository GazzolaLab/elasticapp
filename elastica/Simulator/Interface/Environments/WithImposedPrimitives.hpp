//==============================================================================
/*!
//  \file
//  \brief Imposed primitives component of the \elastica Environments module
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

#include <algorithm>   // for each
#include <functional>  // ref
#include <limits>      // numeric
#include <tuple>
#include <utility>  // move
#include <vector>   // begin/end

///
#include "Simulator/Interface/Environments/Types.hpp"
#include "Simulator/Interface/Environments/WithImposedPrimitivesExecutionPolicy.hpp"
/// Serialization support
#include "Simulator/Environments/Primitives/PrimitivesCollection.hpp"
#include "Simulator/IO/Serialization/Types.hpp"
///
#include "Utilities/CRTP.hpp"
#include "Utilities/GSL.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/Logging.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace interface {

    namespace environments {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Imposed primitives for the Environments module
       * \ingroup interface environments
       *
       * \details
       * This component to the Environments module allows for adding imposed
       * primitives (shapes) whose dynamics are not integrated/computed in time.
       *
       * \tparam EnvironmentTraits Options for configuration of the Environments
       * Module
       * \tparam Sim Simulator
       *
       * \see elastica::interface::Environments
       */
      template <class EnvironmentsTraits, class Sim>
      class WithImposedPrimitives
          : public CRTPHelper<Sim, WithImposedPrimitives>,
            public NonCopyable,
            protected WithImposedPrimitivesExecutionPolicy<
                EnvironmentsTraits,
                WithImposedPrimitives<EnvironmentsTraits, Sim>> {
       private:
        //**Type definitions****************************************************
        //! This type
        using This = WithImposedPrimitives<EnvironmentsTraits, Sim>;
        //! CRTP Type
        using CRTP = CRTPHelper<Sim, WithImposedPrimitives>;
        //! CRTP methods
        using CRTP::self;
        //! Internal size type
        using SizeType = typename EnvironmentsTraits::SizeType;
        //! Algorithm type
        using Algorithm = ::elastica::interface::environments::
            WithImposedPrimitivesExecutionPolicy<EnvironmentsTraits, This>;
        //**********************************************************************

        //**Friendships*********************************************************
        //! Friendship with Serialize
        friend struct ::elastica::io::Serialize<This>;
        //**********************************************************************

       protected:
        //**Type definitions****************************************************
        //! Typelist of admissible primitives
        using ListOfAdmissibleImposedPrimitives =
            typename EnvironmentsTraits::AdmissibleImposedPrimitives;
        //! Metafunction for container of imposed primitives
        // template <typename ImposedPrimitive>
        // using AddContainerType = typename EnvironmentsTraits::
        //     template AddImposedPrimitiveStorageType<ImposedPrimitive>;
        //! Typelist of admissible imposed primitive collections in the
        //! simulator
        // using ImposedPrimitiveCollection =
        //     tmpl::transform<ListOfAdmissibleImposedPrimitives,
        //                     tmpl::bind<AddContainerType, tmpl::_1>>;
        //! Type for storing imposed primitives
        using ImposedPrimitives = ::elastica::collision::PrimitivesCollection<
            ListOfAdmissibleImposedPrimitives>;
        //! Metafunction to check if a plugin is valid
        template <typename ImposedPrimitive>
        using an_admissible = std::bool_constant<
            // brigand's bool constant does not have constexpr bool() operator
            tmpl::list_contains<ListOfAdmissibleImposedPrimitives,
                                ImposedPrimitive>::value>;
        //**********************************************************************

       public:
        //**Type definitions****************************************************
        //! Type of simulation time
        using TimeType = typename EnvironmentsTraits::TimeType;
        // public for protocols
        //! Typelist of submodules
        using SubModules = tmpl::list<Algorithm>;
        //**********************************************************************

       protected:
        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief Constructor of the Imposed Primitives add-on class.
         *
         */
        WithImposedPrimitives() : imposed_primitives_() {
          using namespace ::elastica::logging;
          log<info>([](auto const& log) {
            log << "Successfully initialized the Imposed primitives add-on for "
                   "the Environments module";
          });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Move constructor
         */
        WithImposedPrimitives(WithImposedPrimitives&& other) noexcept
            : imposed_primitives_(std::move(other.imposed_primitives_)) {}
        //**********************************************************************

        //@}
        //**********************************************************************

       public:
        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        ~WithImposedPrimitives() {
          using namespace ::elastica::logging;
          log<info>([](auto const& log) {
            log << "Successfully cleaned up the Imposed primitives add-on for "
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
        inline constexpr auto imposed_primitives() & noexcept
            -> ImposedPrimitives& {
          return imposed_primitives_;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\copydoc imposed_primitives()
         */
        inline constexpr auto imposed_primitives() const& noexcept
            -> ImposedPrimitives const& {
          return imposed_primitives_;
        }
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Functor methods*****************************************************
        /*!\name Functor methods */
        //@{
       protected:
        //**********************************************************************
        template <typename F, typename Containers>
        inline auto for_each_imposed_primitive_impl(
            F func, Containers&& containers) const noexcept -> F {
          // TODO
          // for now the iteration is sequential, consider adding a policy later
          // on
          auto apply_per_primitive =
              [func = std::ref(func)](auto& prim_collection) {
                std::for_each(std::begin(prim_collection),
                              std::end(prim_collection), func);
              };

          std::apply(
              [apply_per_primitive](auto&&... collection) {
                EXPAND_PACK_LEFT_TO_RIGHT(apply_per_primitive(collection));
              },
              std::forward<Containers>(containers));

          return func;
        }
        //**********************************************************************

       public:
        //**********************************************************************
        /*!\brief Apply a callable over all imposed primitives
         *
         * \details
         * Applies a callable `func` over all imposed primitives
         * (across types and collections) registered in the current simulator.
         *
         * \note
         * We need strong exception safety guarantee from `func`. In C++14 this
         * cannot be strongly checked, so the user needs to satisfy this
         * guarantee
         *
         * \param func Callable to be applied over imposed primitives
         */
        template <typename F>
        inline auto for_each_imposed_primitive(F func) noexcept -> F {
          return for_each_imposed_primitive_impl(std::move(func),
                                                 imposed_primitives().data());
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Apply a callable over all imposed primitives
         *
         * \details
         * Applies a callable `func` over all imposed primitives
         * (across types and collections) registered in the current simulator.
         *
         * \note
         * We need strong exception safety guarantee from `func`. In C++14 this
         * cannot be strongly checked, so the user needs to satisfy this
         * guarantee
         *
         * \param func Callable to be applied over imposed
         * primitives
         * \param PrimitivesList A `tmpl::list` of Primitives over which to
         * apply the callable
         */
        template <typename F, typename... ImposedPrimitives>
        inline auto for_each_imposed_primitive(
            F func, tmpl::list<ImposedPrimitives...> /*PrimitiveList*/) noexcept
            -> F {
          using ::elastica::collision::get_collection;
          return for_each_imposed_primitive_impl(
              std::move(func), std::tie(get_collection<ImposedPrimitives>(
                                   imposed_primitives())...));
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Apply a callable over all imposed primitives
         *
         * \details
         * Applies a callable `func` over all imposed primitives
         * (across types and collections) registered in the current simulator.
         *
         * \note
         * We need strong exception safety guarantee from `func`. In C++14 this
         * cannot be strongly checked, so the user needs to satisfy this
         * guarantee
         *
         * \tparam ImposedPrimitivesList A `tmpl::list` of Primitives over which
         * to apply the callable
         * \param func Callable to be applied over imposed primitives
         */
        template <typename ImposedPrimitivesList, typename F>
        inline auto for_each_imposed_primitive(F func) noexcept -> F {
          return for_each_imposed_primitive(std::move(func),
                                            ImposedPrimitivesList{});
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Update the values of each imposed primitives
         *
         * \param time_v The current time of simulation
         */
        inline auto update_values_of_imposed_primitives(TimeType time_v) const
            -> void {
          // this is internal so a member function suffices.
          auto const& algorithm =
              self()
                  .template get_algorithm<
                      typename Algorithm::UpdateImposedPrimitiveKinematics>();
          algorithm(time_v);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Update the rates of each imposed primitives
         *
         * \param time_v The current time of simulation
         */
        inline auto update_rates_of_imposed_primitives(TimeType time_v) const
            -> void {
          // this is internal so a member function suffices.
          auto const& algorithm =
              self()
                  .template get_algorithm<
                      typename Algorithm::UpdateImposedPrimitiveDynamics>();
          algorithm(time_v);
        }
        //**********************************************************************

        //@}
        //**********************************************************************

       protected:
        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        template <typename ImposedPrimitive>
        class IndexedReferenceWrapper {
          // TODO : Refactor, getting hairy without testing
         private:
          friend This;
          using SourceCollection = std::remove_cvref_t<
              decltype(::elastica::collision::get_collection<ImposedPrimitive>(
                  std::declval<ImposedPrimitives&>()))>;

         protected:
          IndexedReferenceWrapper()
              : src_(nullptr),
                index_(std::numeric_limits<std::size_t>::max()) {}

          IndexedReferenceWrapper(SourceCollection& v, std::size_t index)
              : src_(&v), index_(index){};

         public:
          IndexedReferenceWrapper(IndexedReferenceWrapper const& other) =
              default;
          //
          IndexedReferenceWrapper& operator=(
              IndexedReferenceWrapper const& other) = default;

          // We move away from iterator semantics here and give a reference
          // wrapper semantics instead. To form an iterator, we use the function
          using type = ImposedPrimitive;
          using reference = type&;
          using const_reference = type const&;

          /* implicit conversion to a imposed primitive */
          inline operator reference() const /* noexcept*/ {
            return this->get();
          }
          inline auto get() /* noexcept*/ -> reference {
            gsl::not_null<SourceCollection*> ptr_to_collection(src_);
            return (*src_)[index_];
          }
          inline auto get() const /* noexcept*/ -> const_reference {
            gsl::not_null<SourceCollection*> ptr_to_collection(src_);
            return (*src_)[index_];
          }

         private:
          SourceCollection* src_;
          std::size_t index_;
        };
        /*! \endcond */
        //**********************************************************************

       public:
        //**********************************************************************
        /*!\brief Get an invalid reference type for a given primitive type
         *
         * \details
         * Convenience function to get a reference wrapper to a primitive added
         * to the simulator. Typical use is in case-studies.
         */
        template <typename ImposedPrimitive,
                  Requires<an_admissible<ImposedPrimitive>{}> = nullptr>
        decltype(auto) get_invalid_ref() const noexcept {
          using RT = IndexedReferenceWrapper<ImposedPrimitive>;
          return RT{};
        }
        //**********************************************************************

       protected:
        //**********************************************************************
        /*!\brief Implementation of emplace back method
         *
         * \tparam ImposedPrimitive Type of imposed primitive to emplace back
         *
         * \return Mutable reference to the imposed primitived emplaced in
         */
        template <class ImposedPrimitive, class... Args>
        inline auto emplace_back_imposed_impl(Args&&... args) {
          using ::elastica::collision::get_collection;
          auto& imposed_primitive_container =
              get_collection<ImposedPrimitive>(imposed_primitives());
          const std::size_t index = std::size(imposed_primitive_container);
          imposed_primitive_container.emplace_back(std::forward<Args>(args)...);
          // return non-owning handle to the System just pushed back
          using RT = IndexedReferenceWrapper<ImposedPrimitive>;
          return RT{imposed_primitive_container, index};
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Display error message
         *
         * \tparam ImposedPrimitive Type of imposed primitive to emplace back
         */
        template <typename ImposedPrimitive,
                  bool B = an_admissible<ImposedPrimitive>{}>
        static inline constexpr auto check_if_admissible() noexcept -> void {
          static_assert(B,
                        R"error(
The imposed primitive that you are trying to add to the current Simulator is
either unknown to Elastica++ or not in the type-list of admitted primitives!
Please add them to the above type-list, using EnvironmentsConfiguration
)error");
        }
        //**********************************************************************

       public:
        //**Interfaces**********************************************************
        /*!\name Interface methods */
        //@{

        //**********************************************************************
        /*!\brief Get number of types of imposed primitive (collections) in
         * this Simulator
         */
        static constexpr auto get_number_of_imposed_primitive_collections()
            -> std::size_t {
          return ImposedPrimitives::get_number_of_primitive_types();
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets the number of primitives registered in the present
         * simulator
         *
         * \tparam ImposedPrimitive The primitive you want to check
         */
        template <typename ImposedPrimitive>
        inline auto n_imposed_primitives() const noexcept -> SizeType {
          using ::elastica::collision::get_collection;
          return std::size(
              get_collection<ImposedPrimitive>(imposed_primitives()));
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets the total number of primitives registered in the present
         * simulator
         */
        inline auto n_imposed_primitives() const noexcept -> SizeType {
          auto count_primitives_in = [](auto const& container) -> SizeType {
            return std::size(container);
          };
          return std::apply(
              [n_primitives = 0UL,
               count_primitives_in](auto const&... container) mutable {
                EXPAND_PACK_LEFT_TO_RIGHT(
                    (n_primitives += count_primitives_in(container)));
                return n_primitives;
              },
              imposed_primitives().data());
        }
        //**********************************************************************

        // //**********************************************************************
        // /*!\brief Push back an imposed primitive
        //  *
        //  * \param imposed_primitive Imposed primitive to push back
        //  * \return Mutable reference to the primitive pushed in
        //  */
        // template <class ImposedPrimitive,
        //           class D = std::decay_t<ImposedPrimitive>>
        // inline auto push_back_imposed(ImposedPrimitive&& imposed_primitive)
        //     -> D& {
        //   self().check_if_not_final();
        //   check_if_admissible<D>();

        //   auto& imposed_primitive_container =
        //       get_imposed_primitive_container<D>();
        //   imposed_primitive_container.push_back(
        //       std::forward<ImposedPrimitive>(imposed_primitive));
        //   return imposed_primitive_container.back();
        // }
        // //**********************************************************************

        //**********************************************************************
        /*!\brief Emplace back an imposed primitive
         *
         * \tparam ImposedPrimitive Type of imposed primitive to emplace back
         * \param args Arguments for the Imposed Primitive
         * \return Mutable reference to the imposed primitive emplaced in
         */
        template <class ImposedPrimitive, class... Args,
                  Requires<an_admissible<ImposedPrimitive>{}> = nullptr>
        inline decltype(auto) emplace_back(Args&&... args) {
          self().check_if_not_final();

          return emplace_back_imposed_impl<ImposedPrimitive>(
              std::forward<Args>(args)...);
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
              imposed_primitives().data());
        }
        //**********************************************************************

        //@}
        //**********************************************************************

       private:
        //**Member variables****************************************************
        //! The container of different imposed primitives
        ImposedPrimitives imposed_primitives_;
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace environments

  }  // namespace interface

}  // namespace elastica