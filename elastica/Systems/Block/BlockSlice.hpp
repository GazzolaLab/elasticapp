//==============================================================================
/*!
//  \file
//  \brief Block slice definition
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
  /*!\brief Implementation of a slice/view on an \elastica Block
  // \ingroup simulator
  //
  // The BlockSlice template class provides a slice (or) view into a Block of
  // \a data and \operations. It also models the \c ComputationalBlock concept
  // and hence can be used across places where a \elastica Block is used as
  // a template parameter.
  //
  // Similar to a Block, the BlockSlice can also be customized (specialized)
  // for any Lagrangian entity, but experience suggests that extensive
  // customization is not needed. For a concrete example, the cosserat_rod
  // module of \elastica specializes BlockSlice
  //
  // The intended usage of a BlockSlice is when a view is required into the data
  // held by a block---this is frequently the case when a user adds an entity
  // into the Simulator. The pattern that most commonly used in \elastica is
  // for a BlockSlice of \c Plugin type to be only used with a Block of the same
  // \c Plugin type---this is enforced by making BlockSlice's constructors
  // private and allowing only Block to access it (via friendship). The Block
  // usually has a private alias for the BlockSlice (as, say, \c SliceType) and
  // whenever a new entity is emplaced into a block, a \c SliceType is returned
  // to the user.
  //
  // Also, with no additional coding effort, the view has exactly the same
  // operations as the mother Block, but now it operators only on that slice of
  // the data. This means that a BlockSlice can be used as a small Block in
  // itself which greatly simplifies interfacing different components of
  // \elastica---the user need not care or even know about a Block or a
  // BlockSlice ! For example,
     \code
         // .... make simulator etc ...//
         auto my_rod = simulator.emplace_back<CosseratRod>(//...args...//);
         // the args go and form a Block
         // which in turn returns a BlockSlice
         // which the user gets as my_rod

         // use my_rod like a regular cosserat rod
         simulator->constrain(my_rod)->using<SomeConstraint>(//...args...//);
     \endcode
  //
  // This abstraction helped us constrain \c my_rod, embedded in a Block of
  // data, using \c SomeConstraint just like any non-Blocked item of the
  // \elastica library.
  //
  // \note One can draw parallels of a BlockSlice to a std::iterator---
  // both have the ability to access \a data within a larger container. But the
  // resemblance stops there, as iterator semantics are much more broader,
  // and is usually not suited for BlockSlice.
  //
  // \tparam Plugin The computational plugin modeling a Lagrangian entity
  //
  // \see Block
  */
  template <class Plugin>  // The computational plugin type
  class BlockSlice;
  //****************************************************************************

}  // namespace elastica
