//=================================================================================================
/*!
 *  \file pe/util/singleton/Singleton.h
 *  \brief Header file for the Singleton class
 *
 *  Copyright (C) 2009 Klaus Iglberger
 *
 *  This file is part of pe.
 *
 *  pe is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 *  pe is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with
 * pe. If not, see <http://www.gnu.org/licenses/>.
 */
// //=================================================================================================
#pragma once

#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

#include "Utilities/NonCopyable.hpp"
#include "Utilities/StdHelpers.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits.hpp"

namespace elastica {

  namespace tt {

    //==========================================================================
    //
    // TYPE TRAITS FOR SINGLETON CLASS
    //
    //==========================================================================

    /*
     * The following is perfectly valid for IsSingleton, however
     * it creates issues with friend scope. IsSingleton will be
     * a friend of all classes that use our Singleton, using the
     * BEFRIEND_SINGLETON macro below (this is to have access to
     * T::SingletonType, which may be private in the child class
     * context). The issue arises because we try and derive directly
     * gtom is_base_of. Rather, we push the is_base_of inside the
     * definition of the struct.
     *
     * template <typename T>
     * struct IsSingleton : public std::is_base_of<typename
     * T::SingletonType,T> {};
     */
    template <typename T>
    struct IsSingleton {
      constexpr static bool value =
          std::is_base_of<typename T::SingletonType, T>::value;
    };

    template <typename... Args>
    constexpr bool derives_from_singleton =
        cpp17::conjunction_v<IsSingleton<Args>...>;

    //==========================================================================
    //
    // RESOLVING CYCLIC DEPENDENCIES
    //
    //==========================================================================

    //****************************************************************************
    // Forward declarations
    //****************************************************************************
    template <typename T, typename TL, bool C>
    struct HasCyclicDependency;
    template <typename TL,  // Type list of checked lifetime dependencies
              typename D>   // Type list of lifetime dependencies checked for
    struct HasCyclicDependencyHelper;

    template <typename TL,  // Type list of checked lifetime dependencies
              template <typename...>
              class DL,       // Type list of lifetime dependencies checked for
              typename... D>  // Dependencies contained in the Type List
    struct HasCyclicDependencyHelper<TL, DL<D...>> {
      static constexpr bool value = cpp17::disjunction_v<
          HasCyclicDependency<D, TL, tmpl::list_contains_v<TL, D>>...>;
    };

    /*
     * Check any type from the typelist for cyclic dependencies
     */
    template <typename T,   // Type to be checked for cyclic lifetime dependency
              typename TL,  // Type list of checked lifetime dependencies
              bool C = tmpl::list_contains_v<TL, T>>  // Is T contained in TL?
    struct HasCyclicDependency {
      using ETL = tmpl::push_back<TL, T>;  // Extended Type List to be checked
      static constexpr bool value =
          HasCyclicDependencyHelper<ETL, typename T::Dependencies>::value;
      /* // Optional, to mimic std::false/true types
      using value_type = bool;
      constexpr operator value_type() const noexcept { return value; }
      constexpr value_type operator()() const noexcept { return value; }
       */
    };

    // Specialization in case T is contained in TL, which implies a
    // cyclic dependency, hence deriving from true type
    template <typename T,   // Type to be checked for cyclic lifetime dependency
              typename TL>  // Type list of checked lifetime dependencies
    struct HasCyclicDependency<T, TL, true> : std::true_type {};

    // Is true if any one of the types has a cylic dependency
    template <typename... D>  // Dependencies (types) to be checked
    constexpr bool has_cyclic_dependency =
        cpp17::disjunction_v<HasCyclicDependency<D, tmpl::empty_sequence>...>;

    template <typename T>
    struct SingletonStorage {
      using type = std::shared_ptr<T>;
    };

    template <typename T>
    struct AddSingletonStorage : public IsSingleton<T> {
      using IsSingleton<T>::value;
      static_assert(value, "Not a singleton");
      // TODO : Is there a way to use a common
      using type = typename SingletonStorage<T>::type;
    };

  }  // namespace tt

  //****************************************************************************
  // Forward declarations
  //****************************************************************************
  template <typename T>  // Type of the lifetime dependency
  class Dependency;

// Need to add Dependency to the list too, as dependencies rely
// on instantiating the dependent singleton derived class using the static
// instance method()
#define BEFRIEND_SINGLETON                         \
  template <typename, class...>                    \
  friend class elastica::Singleton;                \
  template <typename, typename, bool>              \
  friend struct elastica::tt::HasCyclicDependency; \
  template <typename>                              \
  friend struct elastica::tt::IsSingleton;         \
  template <typename>                              \
  friend class elastica::Dependency

  template <class T, class... Deps>
  class Singleton : private NonCopyable {
   public:
    using SingletonType = Singleton<T, Deps...>;
    using Dependencies = tmpl::list<Deps...>;
    using SingletonStorageType = typename tt::SingletonStorage<T>::type;
    using DependencyStorage = tmpl::as_tuple<
        tmpl::transform<Dependencies, tt::SingletonStorage<tmpl::_1>>>;

    // Allowing friend access to IsSingleton to access
    // Singleton type evem when Singleton is privately/publicly derived.
    template <typename>
    friend struct tt::IsSingleton;

    constexpr static std::size_t get_number_of_dependencies() noexcept {
      return tmpl::size<Dependencies>::value;
    }

   public:
    // Access via instance method
    static SingletonStorageType instance() {
      static SingletonStorageType object(new T());
      return object;
    }

   protected:
    Singleton() {
      // Fill with instances
      //      using expander = int[];
      //      expander{0, ((void)(std::get<std::shared_ptr<Dependencies>>(
      //                              dependencies_) =
      //                              Dependencies::instance()),
      //                   0)...};
      //      EXPAND_PACK_LEFT_TO_RIGHT(std::get<std::shared_ptr<Deps>>(dependencies_)
      //      =
      //                                    Deps::instance());
      index_apply<get_number_of_dependencies()>([this](auto... Is) {
        EXPAND_PACK_LEFT_TO_RIGHT(std::get<Is>(dependencies_) =
                                      tmpl::at_c<Dependencies, Is>::instance());
      });

      // Assert that T and Dependencies derive from singleton
      static_assert(tt::derives_from_singleton<T, Deps...>,
                    "Type does not derive from Singleton");

      // Assert that dependencies do not depend on other dependencies
      // cyclically
      static_assert(!tt::has_cyclic_dependency<Deps...>,
                    "Cyclic dependency detected!");
    };

   private:
    // Store handles to all dependencies
    // std::tuple<std::shared_ptr<Dependencies>...>
    DependencyStorage dependencies_;
  };

}  // namespace elastica
