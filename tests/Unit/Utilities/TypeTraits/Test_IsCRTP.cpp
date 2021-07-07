//
// Created by Tejaswin Parthasarathy on 6/14/21.
//

#include <type_traits>

#include "Utilities/TypeTraits/IsCRTP.hpp"

/// [is_crtp_eg]
template <typename D>
struct CRTPBase {};

struct Derived : public CRTPBase<Derived> {};
struct Derived2 : public Derived {};

struct NotDerived {};

static_assert(tt::IsCRTP<CRTPBase, Derived>::value,
              "IsCRTP should recognize Derived as derived from CRTP class!");
static_assert(
    not tt::IsCRTP<CRTPBase, Derived2>::value,
    "IsCRTP should not recognize Derived2 as derived from CRTP class!");
static_assert(
    not tt::IsCRTP<CRTPBase, NotDerived>::value,
    "IsCRTP should not recognize NotDerived as derived from CRTP class!");

template <typename D>
struct MiddleDerived : CRTPBase<D> {};
struct Derived3 : public MiddleDerived<Derived3> {};

static_assert(tt::IsCRTP<CRTPBase, Derived3>::value,
              "IsCRTP should recognize Derived3 as derived from CRTP class!");
/// [is_crtp_eg]
