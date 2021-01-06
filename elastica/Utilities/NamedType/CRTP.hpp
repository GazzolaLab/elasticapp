//==============================================================================
/*!
//  \file
//  \brief CRTP helper for access to derived classes
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

// Thanks :
// https://github.com/joboccara/NamedType/blob/master/include/NamedType/crtp.hpp

#pragma once

namespace named_type {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*!\brief CRTP helper class
  // \ingroup util
  //
  // CRTPHelper eases coding CRTP classes in \elastica by providing a common
  // interface to the underlying data type, using the self() methods
  //
  // \example
  // \snippet Test_CRTP.cpp crtp_example
  //
  // \tparam Derived The final derived class
  // \tparam Mixin The interface mixed-in class, templated to prevent diamond
  // hierarchies with multiple inheritance
  */
  template <typename Derived, template <typename> class Mixin>
  class CRTPHelper {
   protected:
    //**Constructors************************************************************
    /*!\name Constructors */
    //@{

    //**************************************************************************
    /*!\brief The default constructor.
     */
    CRTPHelper() = default;
    //**************************************************************************
    //@}
    //**************************************************************************

   private:
    //**Type definitions********************************************************
    //! Type of the bottom level derived class
    using Self = Derived;
    //! Reference type of the bottom level derived class
    using Reference = Self&;
    //! const reference type of the bottom level derived class
    using ConstReference = Self const&;
    //**************************************************************************

   protected:
    //**Self method*************************************************************
    /*!\brief Access to the underlying derived
    //
    // \return Mutable reference to the underlying derived
    //
    // Safely down-casts this module to the underlying derived type, using
    // the Curiously Recurring Template Pattern (CRTP).
    */
    inline constexpr auto self() noexcept -> Reference {
      return static_cast<Reference>(*this);
    }
    //**************************************************************************

    //**Self method*************************************************************
    /*!\brief Access to the underlying derived
    //
    // \return Const reference to the underlying derived
    //
    // Safely down-casts this module to the underlying derived type, using
    // the Curiously Recurring Template Pattern (CRTP).
    */
    inline constexpr auto self() const noexcept -> ConstReference {
      return static_cast<ConstReference>(*this);
    }
    //**************************************************************************
  };
  //****************************************************************************

}  // namespace named_type
