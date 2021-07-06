//==============================================================================
/*!
//  \file
//  \brief Wraps up a functor with an `with` accessor
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

#include <utility>

#include "Utilities/NamedType.hpp"
#include "Utilities/Requires.hpp"

namespace named_type {

  //****************************************************************************
  /*!\brief Class wrapping a functor with a `with` accessor
   * \ingroup interface
   *
   * \example
   * \snippet Test_WithWrapper.cpp WithWrapper_example
   *
   * \tparam DerivedCallable Any class with a call operator
   */
  template <typename DerivedCallable>
  class WithWrapper : public CRTPHelper<DerivedCallable, WithWrapper> {
   protected:
    //**Constructors************************************************************
    /*!\name Constructors */
    //@{
    //**************************************************************************
    /*!\brief The default constructor.
     */
    WithWrapper() = default;
    //**************************************************************************
    //@}
    //**************************************************************************

   private:
    //**Type definitions********************************************************
    using CRTP = CRTPHelper<DerivedCallable, WithWrapper>;  //<! CRTP Type
    using CRTP::self;                                       //<! CRTP methods
    //**************************************************************************

   public:
    //**Destructor**************************************************************
    /*!\name Destructor */
    //@{
    //**************************************************************************
    /*!\brief Destructor
     */
    ~WithWrapper() = default;
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
    decltype(auto) with(ClassArgs&&... args) /*const */ noexcept(
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
    constexpr decltype(auto) with(Args&&... args) /* const */
        noexcept(noexcept(self().get_value()(std::forward<Args>(args)...))) {
      return self().get_value()(std::forward<Args>(args)...);
    }
    //**************************************************************************
  };
  //****************************************************************************

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Specialization of Traits for a WithWrapped type
   * \ingroup named_type
   */
  template <>
  struct Traits<WithWrapper> : public Traits<Gettable> {};
  /*! \endcond */
  //****************************************************************************

  //****************************************************************************
  /*!\brief Makes a WrappedCallable with the WithWrapper
   *
   * \example
   * \snippet Test_WithWrapper.cpp make_wrapped_with_example
   *
   * \param func Functor to be wrapped
   */
  template <typename F>
  decltype(auto) make_wrapped_with(F&& func) noexcept(
      noexcept(make_named_with_default<WithWrapper, NonCopyable>(
          std::forward<F>(func)))) {
    return make_named_with_default<WithWrapper, NonCopyable>(
        std::forward<F>(func));
  }
  //****************************************************************************
}  // namespace named_type

// Introduce to elastica namespace

namespace elastica {

  namespace interface {

    using named_type::make_wrapped_with;

  }  // namespace interface

}  // namespace elastica
