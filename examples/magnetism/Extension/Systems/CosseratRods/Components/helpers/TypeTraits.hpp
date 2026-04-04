//==============================================================================
/*!
//  \file
//  \brief Type traits for @ref cosserat_rod_component
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
#include <elastica/Systems/CosseratRods/Components/helpers/TypeTraits.hpp>

#include "Extension/Systems/CosseratRods/Components/helpers/Types.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace component {

      namespace tt {

        //**************************************************************************
        /*!\brief Check whether a given type `C` is a valid MagnetismComponent
         * \ingroup systems
         *
         * \details
         * Inherits from std::true_type if `C` is a valid magnetism component
         * template, deriving from MagnetismComponent, otherwise inherits from
         * std::false_type.
         *
         * \usage
         * For any type `C`,
         * \code
         * using result = IsMagnetismComponent<C>;
         * \endcode
         *
         * \metareturns
         * cpp17::bool_constant
         *
         * \semantics
         * If the type `C` is derived from MagnetismComponent<C>, then
         * \code
         * typename result::type = std::true_type;
         * \endcode
         * otherwise
         * \code
         * typename result::type = std::false_type;
         * \endcode
         *
         * \tparam C : the type to check
         *
         * \see MagnetismComponent
         */
        template <typename C>
        struct IsMagnetismComponent
            : public ::tt::IsCRTP<
                  ::elastica::cosserat_rod::component::MagnetismComponent, C> {
        };
        //**************************************************************************

        template <typename T>
        using magnetism_component_t = typename T::magnetism_component;

        template <typename T>
        struct MagnetismComponentTrait
            : public ::tt::detected_or<NoSuchType, magnetism_component_t, T> {};

        template <typename T>
        using magnetism_component_trait_t =
            typename MagnetismComponentTrait<T>::type;

      }  // namespace tt

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica
