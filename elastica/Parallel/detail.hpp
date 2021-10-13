//==============================================================================
/*!
//  \file
//  \brief Parallelism details
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

namespace elastica {

  namespace parallel {

    namespace detail {

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <bool B = true>
      struct [[deprecated(R"warn(
You requested a parallelized configuration but disabled parallelism during
CMake generation/compilation step!
)warn")]] warn_on_disabled_parallelism{};
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

  }  // namespace parallel

}  // namespace elastica
