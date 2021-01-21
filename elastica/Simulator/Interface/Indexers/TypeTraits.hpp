//==============================================================================
/*!
//  \file
//  \brief Type traits for the indexing routines
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
#include <type_traits>

///
#include "Simulator/Interface/Indexers/Types.hpp"
///
#include "Utilities/End.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"
#include "Utilities/TypeTraits/IsA.hpp"

namespace elastica {

  namespace interface {

    namespace tt {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Helper to check whether requirements of an index
      // \ingroup interface
      //
      // \example
      // \snippet Indexers/Test_TypeTraits.cpp is_admissible_index_example
      */
      template <typename T>
      struct IsAdmissibleIndex
          : cpp17::disjunction<std::is_integral<T>, std::is_same<from_end, T>> {
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Helper to check whether a given type is an inteface Range
      // \ingroup interface
      //
      // \example
      // \snippet Indexers/Test_TypeTraits.cpp is_range_example
      */
      template <typename T>
      struct IsRange : ::tt::is_a<detail::Range, T> {};
      /*! \endcond */
      //************************************************************************

    }  // namespace tt

  }  // namespace interface

}  // namespace elastica