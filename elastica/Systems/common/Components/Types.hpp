//==============================================================================
/*!
//  \file
//  \brief All helper types within @ref cosserat_rod_component
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

namespace elastica {

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  template <typename /* DerivedComponent*/>
  struct Component;
  template <typename /* DerivedComponent*/>
  struct GeometryComponent;
  template <typename /* DerivedComponent*/>
  struct KinematicsComponent;
  template <typename /* DerivedComponent*/>
  struct NameComponent;
  /*! \endcond */
  //****************************************************************************

  // This is an anti-pattern, but is required for aliases to not include
  // definition
  namespace detail {
    template <typename /* Traits */, typename /* Block */,
              typename /*NameImpl*/>
    struct NameAdapter;
  }

}  // namespace elastica
