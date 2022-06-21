//==============================================================================
/*!
//  \file
//  \brief All systems in \elastica
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
//
#include "Systems/Types.hpp"
//
#include "Systems/Protocols.hpp"
#include "Systems/Tags.hpp"
//
#include "Systems/Block.hpp"
#include "Systems/CosseratRods.hpp"
#include "Systems/RigidBody.hpp"
#include "Systems/States/States.hpp"
//
#include "Utilities/TMPL.hpp"

//==============================================================================
//
//  DOXYGEN DOCUMENTATION
//
//==============================================================================

//******************************************************************************
/*!\defgroup systems Physical systems
 * \brief Physical systems (that occupy space and evolve in time) in \elastica
 */
//******************************************************************************

namespace elastica {

  //****************************************************************************
  /*!\brief All implemented CosseratRod plugins in \elastica
   * \ingroup systems
   */
  using CosseratRodPlugins =
      tmpl::list<::elastica::cosserat_rod::CosseratRod,
                 ::elastica::cosserat_rod::CosseratRodWithoutDamping>;
  //****************************************************************************

  //****************************************************************************
  /*!\brief All implemented RigidBody plugins in \elastica
   * \ingroup systems
   */
  using RigidBodyPlugins = tmpl::list<::elastica::rigid_body::Sphere>;
  //****************************************************************************

  //****************************************************************************
  /*!\brief All implemented physical system plugins in \elastica
   * \ingroup systems
   *
   * \see elastica::CosseratRodPlugins, elastica::RigidBodyPlugins
   */
  using PhysicalSystemPlugins =
      tmpl::append<CosseratRodPlugins, RigidBodyPlugins>;
  //****************************************************************************

}  // namespace elastica