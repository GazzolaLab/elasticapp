//==============================================================================
/*!
//  \file
//  \brief Define a function that acts similarly to a virtual
//  function, but can take template parameters.
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
//
//  Reused from SpECTRE : https://spectre-code.org/
*/
//==============================================================================

// Reused from SpECTRE : https://spectre-code.org/
// Distributed under the MIT License.
// See LICENSE.txt for details.

#pragma once

#include "Utilities/TMPL.hpp"

/// \ingroup UtilitiesGroup
/// \brief Helpers for derived class registration
///
/// SpECTRE's factory mechanism requires that the base class contain a
/// list of all derived classes (in the special nested type alias
/// `creatable_classes`).  This can be problematic when the desired
/// list of derived classes can vary with use of the base class.
/// These helpers provide a method to simplify handling these
/// compile-time registrations.
///
/// Each optional derived class defines a registrar helper and both
/// the base class and derived classes are templated on the list of
/// registrars.  The typical structure of these classes is:
/// \snippet Test_Registration.cpp registrar_structure
/// A concrete base class type with a specific set of derived classes
/// is constructed like
/// \snippet Test_Registration.cpp registrar_use
///
/// It is frequently useful to default the registrar list in a derived
/// class to the registrar for that class.  This ensures that any
/// methods in the base class using the registrar list (such as those
/// using `DEFINE_FAKE_VIRTUAL()` or `call_with_dynamic_type()`) work
/// as expected on an explicitly constructed derived class with the
/// list omitted.
namespace Registration {
  /// A template for defining a registrar.
  ///
  /// A registrar for a class can be defined by making a type alias to
  /// this struct, filling in the registrant.
  /// \snippet Test_Registration.cpp registrar
  /// In more complex cases (such as with non-type template
  /// parameters) defining a registrar manually may be necessary.
  /// \snippet Test_Registration.cpp custom_registrar
  template <template <typename...> class Registrant, typename... Args>
  struct Registrar {
    // Final registrant type, specialized with input Args... and the
    // RegistrarList it was extracted from.  This will be a full
    // specialization of a derived class that uses the registration
    // framework.
    template <typename RegistrarList>
    using f = Registrant<Args..., RegistrarList>;
  };

  namespace detail {
    template <typename RegistrarList, typename Registrar>
    struct registrant {
      using type = typename Registrar::template f<RegistrarList>;
    };
  }  // namespace detail

  /// Transform a list of registrars into the list of associated
  /// registrants.  This is usually used to define the
  /// `creatable_classes` type list.
  template <typename RegistrarList>
  using registrants = tmpl::transform<
      tmpl::remove_duplicates<RegistrarList>,
      detail::registrant<tmpl::pin<tmpl::remove_duplicates<RegistrarList>>,
                         tmpl::_1>>;
}  // namespace Registration

// automatic registation
namespace Registration {

  constexpr unsigned int max_registered_types() { return 10U; }

  template <unsigned int N>
  struct Rank : Rank<N - 1> {};

  template <>
  struct Rank<0> {};

  template <class Tag>
  tmpl::list<> get_registered_types(Tag*, Rank<0>) {
    return {};
  }

}  // namespace Registration

#define GET_REGISTERED_TYPES(Tag)              \
  decltype(::Registration::get_registered_types( \
      static_cast<Tag*>(nullptr),              \
      ::Registration::Rank<::Registration::max_registered_types()>()))

// Appends Type to GET_REGISTERED_TYPES(Tag).
#define REGISTER_TYPE(Tag, Type)                                          \
  namespace Registration {                                                \
    inline tmpl::push_back<GET_REGISTERED_TYPES(Tag), Type>               \
    get_registered_types(                                                 \
        Tag*,                                                             \
        Registration::Rank<tmpl::size<GET_REGISTERED_TYPES(Tag)>::value + \
                           1U>) {                                         \
      static_assert(                                                      \
          tmpl::size<GET_REGISTERED_TYPES(Tag)>::value <                  \
              Registration::max_registered_types(),                       \
          "Cannot register this type as maximum capacity exceeded!");     \
      return {};                                                          \
    }                                                                     \
  }
