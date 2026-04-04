//==============================================================================
/*!
//  \file
//  \brief Wraps up a functor with an `at` accessor
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

#include <utility>

#include "Utilities/NamedType.hpp"
#include "Utilities/Requires.hpp"

namespace named_type {

  //****************************************************************************
  /*!\brief Class wrapping a functor with an `at` accessor
   * \ingroup interface named_type
   *
   * \example
   * \snippet Test_AtWrapper.cpp AtWrapper_example
   *
   * \tparam DerivedCallable Any class with a call operator
   *
   * \see make_wrapped_at()
   */
  template <typename DerivedCallable>
  class AtWrapper : public CRTPHelper<DerivedCallable, AtWrapper> {
   protected:
    //**Constructors************************************************************
    /*!\name Constructors */
    //@{
    //**************************************************************************
    /*!\brief The default constructor.
     */
    AtWrapper() = default;
    //**************************************************************************
    //@}
    //**************************************************************************

   private:
    //**Type definitions********************************************************
    using CRTP = CRTPHelper<DerivedCallable, AtWrapper>;  //<! CRTP Type
    using CRTP::self;                                     //<! CRTP methods
    //**************************************************************************

   public:
    //**Destructor**************************************************************
    /*!\name Destructor */
    //@{
    //**************************************************************************
    /*!\brief Destructor
     */
    ~AtWrapper() = default;
    //**************************************************************************
    //@}
    //**************************************************************************

    //**Call method*************************************************************
    /*!\brief Access to the underlying callable's function
     *
     * \details
     * Mimics the interface of a factory function (i.e. `make_*`) functions
     *
     * \tparam Class Class to be factory-made
     * \param args arguments for constructing the class
     * \returns `auto` from the calling the underlying Callable with the
     * constructed Class passed in as argument
     */
    template <
        typename Class, typename... ClassArgs,
        Requires<std::is_constructible<Class, ClassArgs...>::value> = nullptr>
    decltype(auto) at(ClassArgs&&... args) /*const*/ noexcept(
        noexcept(self().get_value()(Class(std::forward<ClassArgs>(args)...)))) {
      return self().get_value()(Class(std::forward<ClassArgs>(args)...));
    }
    //**************************************************************************

    //**Call method*************************************************************
    /*!\brief Access to the underlying callable's function
     *
     * \details
     * Perfectly forwarding operator
     *
     * \param args arguments to be forwarded to the underlying callable
     * \returns `auto` from the calling the underlying Callable with the
     * forwarded arguments
     */
    template <typename... Args>
    constexpr decltype(auto) at(Args&&... args) /*const*/
        noexcept(noexcept(self().get_value()(std::forward<Args>(args)...))) {
      //      return self().get_value()(std::forward<Args>(args)...);
      auto& x = self().get_value();
      return x(std::forward<Args>(args)...);
    }
    //**************************************************************************
  };
  //****************************************************************************

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Specialization of Traits for a AtWrapped type
   * \ingroup named_type
   */
  template <>
  struct Traits<AtWrapper> : Traits<Callable> {};
  /*! \endcond */
  //****************************************************************************

  //****************************************************************************
  /*!\brief Makes a WrappedCallable with the WithWrapper
   *
   * \example
   * \snippet Test_AtWrapper.cpp make_wrapped_at_example
   *
   * \param func Functor to be wrapped
   */
  template <typename F>
  decltype(auto) make_wrapped_at(F&& func) noexcept(noexcept(
      make_named_with_default<AtWrapper, NonCopyable>(std::forward<F>(func)))) {
    return make_named_with_default<AtWrapper, NonCopyable>(
        std::forward<F>(func));
  }
  //****************************************************************************

}  // namespace named_type

namespace elastica {

  namespace interface {

    using named_type::make_wrapped_at;

  }  // namespace interface

}  // namespace elastica
