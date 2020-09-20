//==============================================================================
/*!
//  \file
//  \brief Block definition
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

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Implementation of \c ComputationalBlock concept in \elastica
  // \ingroup simulator
  //
  // The Block template class is \elastica 's implementation of the
  // computational \a block concept---it fits all the requirements of a
  // \c ComputationalBlock concept and is used throughout the code base. It IS
  // the gateway to perform efficient inter-element operations for (m)any
  // Lagrangian-based simulation codes, and is used primarily to implement
  // efficient intra-rod operations within \elastica. The fundamental idea is
  // to separate out \a data and \a operations on data into different orthogonal
  // modules (usually implemented as \a policy classes), but facilitate their
  // interaction, within the contained hierarchy, using Block. This then turns
  // conventional top-down class hierarchy into a \a star hierarchy, the Block
  // being the central \a hub in this topology. The hub serves as a central spot
  // for storing all \a data, to be used by \a operations from the nodes.
  //
  // Block heavily relies on some patterns used in modern C++---Curiously
  // Recurring Template Pattern (CRTP, across multiple hierarchies),
  // policy-based class hierarchy design, trait and type based customization.
  //
  // The Block is intended to be customized (specialized) for any Lagrangian
  // entity with complex data inter-communication patterns, for a concrete
  // example, please refer to the cosserat_rod module of \elastica.
  //
  // From a first look, the usage of block is very similar to an implementation
  // class of a conventional pattern CRTP, and hence is used as a template
  // parameter in most cases. Its intended usage is however more closer to a
  // CRTP class used as a policy class! While confusing at a first glance, this
  // pattern lets us imbibe the SOLID design principles in class design.
  //
  // \tparam Plugin The computational plugin modeling a Lagrangian entity
  //
  // \see BlockSlice
  */
  template <class Plugin>  // The computational plugin type
  class Block;
  //****************************************************************************

}  // namespace elastica