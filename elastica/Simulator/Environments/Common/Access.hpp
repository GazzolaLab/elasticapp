//==============================================================================
/*!
//  \file
//  \brief Access API for all \elastica environments
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
#include <cstddef>  // size_t

namespace elastica {

  namespace environments {

    struct AnyEnvironment {};

    //==========================================================================
    //
    //  ACCESS API FUNCTIONS
    //
    //==========================================================================

    //**Reset function**********************************************************
    /*!\name Reset functions */
    //@{
    /*!\brief Resets the environment
     * \ingroup environments
     */
    template <typename TimeType>
    inline void reset(AnyEnvironment&, TimeType) noexcept;
    //@}
    //**************************************************************************

    //**Initialize parameter****************************************************
    /*!\name Parameter initialization functions */
    //@{
    /*!\brief Initializes parameters of the environment
     * \ingroup environments
     */
    template <typename CorrespondingEnvironmentConfiguration>
    inline void initialize_parameters(
        AnyEnvironment&, CorrespondingEnvironmentConfiguration const&) noexcept;
    //@}
    //**************************************************************************

  }  // namespace environments

}  // namespace elastica
