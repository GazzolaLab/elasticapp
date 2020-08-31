#pragma once

#include <blaze/math.h>

namespace elastica {

  //****************************************************************************
  /*!\brief Import of the blaze::trans() and blaze::ctrans() function into the
   * elastica namespace.
  // \ingroup math_shims
  */
  using blaze::ctrans;
  using blaze::trans;
  //****************************************************************************

}  // namespace elastica
