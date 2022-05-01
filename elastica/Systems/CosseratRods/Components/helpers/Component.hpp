//==============================================================================
/*!
//  \file
//  \brief Helper to mark a valid component within @ref cosserat_rod_component
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
#include "Systems/common/Components.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace component {

      using ::elastica::Component;
      using ::elastica::GeometryComponent;
      using ::elastica::KinematicsComponent;

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Base class for all elasticity components.
       * \ingroup systems
       *
       * \details
       * The Elasticity component is the base class for all elasticity
       * components used in programming systems within \elastica. All classes
       * that represent a elasticity component and that are used within the @ref
       * blocks environment of \elastica library have to derive publicly from
       * this class in order to qualify as a component.
       *
       * Only in case a class is derived publicly from the Component base class,
       * the IsComponent type trait recognizes the class as valid component.
       *
       * \tparam DerivedElasticityComponent A downstream component that derives
       * from the Component class using the CRTP pattern.
       */
      template <typename DerivedElasticityComponent>
      struct ElasticityComponent
          : public Component<DerivedElasticityComponent> {
        // for ease in programming, ideally we shouldnt need it
        //**Type definitions****************************************************
        //! Type of elasticity component
        using elasticity_component = DerivedElasticityComponent;
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica
