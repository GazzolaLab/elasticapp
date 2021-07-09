//==============================================================================
/*!
//  \file
//  \brief Type traits for the Time module
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

#include "Time/Helpers/TypeTraits.hpp"
#include "Time/Types.hpp"
#include "Utilities/TypeTraits.hpp"

namespace elastica {

  namespace tt {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Compile time check for time delta type
    // \ingroup type_traits
    //
    // This type trait tests whether or not the given template parameter is a
    // time delta type (as marked in the Time module). In case the type is a
    // time-delta type, the class inherit from \a std::true_type. Otherwise
    // it inherits \a std::false_type.

     \code
     elastica::IsTimeDelta< TimeDelta<int> >::value          // Evaluates to 1
     elastica::IsTimeDelta< const TimeDelta<double> >::type  // Results in
                                                             // std::true_type
     elastica::IsTimeDelta< volatile TimeDelta<float> >      // Is derived
                                                             // std::true_type

     elastica::IsTimeDelta< SO3<double> >::value             // Evaluates to 0
     elastica::IsTimeDelta< const SE3<double> >::type        // Results in
                                                             // std::false_type
     elastica::IsTimeDelta< volatile SO3<int> >              // Is derived from
                                                             // std::false_type
     \endcode
    */
    template <typename T>
    struct IsTimeDelta
        : public ::tt::is_a<::elastica::impl::SimulationTimeDelta, T> {};
    //**************************************************************************

    //**************************************************************************
    /*!\brief Auxiliary variable template for the IsTimeDelta type trait.
     * \ingroup type_traits
     *
     * The is_time_delta_v variable template provides a convenient shortcut to
     * access the nested \a value of the IsTimeDelta class template. For
     * instance, given the type \a T the following two statements are identical:
     *
     * \code
     * constexpr bool value1 = elastica::IsTimeDelta<T>::value;
     * constexpr bool value2 = elastica::is_time_delta_v<T>;
     * \endcode
     */
    template <typename T>
    constexpr bool is_time_delta_v = IsTimeDelta<T>::value;
    //**************************************************************************

  }  // namespace tt

}  // namespace elastica
