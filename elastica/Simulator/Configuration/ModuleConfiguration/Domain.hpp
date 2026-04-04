//==============================================================================
/*!
//  \file
//  \brief Configuration for the simulation domain of an \elastica Simulator
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

#include <type_traits>
#include <utility>
///
// TODO : Code Smell : Configuration should not depend on implementation details.
// HEALTH WARNING : We need a complete implementation of Domain here to let
// the compiler know that it conforms to the protocol (the types header only
// forward declares it without conformance)
#include "Simulator/Domain.hpp"
// This is an implementation detail and hence isn't exposed in Types
#include "Simulator/Frames/EulerianFrame.hpp"
// Complete implementation of this type is needed to do manipulations
#include "Simulator/Configuration/Protocols/Domain.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace elastica {

  namespace configuration {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Configuration of domain of the simulation
     * \ingroup config
     *
     * DomainConfiguration is the compile-time customization point point of
     * domain related properties in an \elastica simulation. It offers
     * the ability to customize the simulation with different boundary
     * conditions based on the template parameters.
     *
     * \tparam BoundaryConditionInX Boundary condition in the X direction
     * \tparam BoundaryConditionInY Boundary condition in the Y direction
     * \tparam BoundaryConditionInZ Boundary condition in the Z direction
     *
     * \see make_domain_configuration() for idiomatic construction
     */
    template <typename BoundaryConditionInX, typename BoundaryConditionInY,
              typename BoundaryConditionInZ>
    class DomainConfiguration
        : public ::tt::ConformsTo<protocols::DomainConfiguration> {
     private:
      //**Type definitions******************************************************
      //! This type
      using This =
          DomainConfiguration<BoundaryConditionInX, BoundaryConditionInY,
                              BoundaryConditionInZ>;
      //************************************************************************

     public:
      //**Type definitions******************************************************
      //! Type of direction
      using DirectionType = ::elastica::detail::EulerianFrame::DirectionType;
      //************************************************************************

     private:
      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Helper struct for accessing boundary types along different
       * directions.
       * \ingroup domain
       *
       * The BoundaryTypeAlongHelper class template offers the ability to obtain
       * the boundary type along the X, Y and Z directions in the current
       * simulation. It defines the nested type `type`, which
       * represents the boundary type for X, Y and Z directions. In case the
       * direction is not one of X, Y and Z, there is no nested type `type`.
       * Note that \c const and \c volatile qualifiers and reference modifiers
       * are generally ignored.
       */
      template <::elastica::detail::EulerianFrame::DirectionType,
                class Dummy = void>
      struct BoundaryTypeAlongHelper;
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Specialization of the BoundaryTypeAlongHelper class template for
       * the X direction.
       * \ingroup domain
       */
      template <class Dummy>
      struct BoundaryTypeAlongHelper<::elastica::detail::EulerianFrame::X,
                                     Dummy> {
        using type = std::remove_cvref_t<BoundaryConditionInX>;
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Specialization of the BoundaryTypeAlongHelper class template for
       * the Y direction.
       * \ingroup domain
       */
      template <class Dummy>
      struct BoundaryTypeAlongHelper<::elastica::detail::EulerianFrame::Y,
                                     Dummy> {
        using type = std::remove_cvref_t<BoundaryConditionInY>;
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Specialization of the BoundaryTypeAlongHelper class template for
       * the Z direction.
       * \ingroup domain
       */
      template <class Dummy>
      struct BoundaryTypeAlongHelper<::elastica::detail::EulerianFrame::Z,
                                     Dummy> {
        using type = std::remove_cvref_t<BoundaryConditionInZ>;
      };
      /*! \endcond */
      //************************************************************************

     public:
      //**Type definitions******************************************************
      //! meta function for obtaining boundary type along direction D
      template <DirectionType D>  // Direction
      using BoundaryTypeAlong = typename BoundaryTypeAlongHelper<D>::type;
      //! Type of simulation domain
      using DomainType = ::elastica::tt::SimulationDomainType<
          BoundaryTypeAlong<::elastica::detail::EulerianFrame::X>,
          BoundaryTypeAlong<::elastica::detail::EulerianFrame::Y>,
          BoundaryTypeAlong<::elastica::detail::EulerianFrame::Z>>;
      //************************************************************************

      //**Compile-time checks **************************************************
      // sanity check direction types
      static_assert(std::is_same<typename DomainType::DirectionType,
                                 DirectionType>::value,
                    "The direction type found within"
                    "the DomainType and the BoundaryTypeAlong doesn't match, "
                    "please contact the developers with the error log!");
      //************************************************************************

      //**Constructor***********************************************************
      /*!\name Constructor*/
      //@{
     public:
      //************************************************************************
      /*!\brief The default constructor.
       */
      constexpr DomainConfiguration() = delete;
      //************************************************************************

      //************************************************************************
      /*!\brief The Domain constructor.
       *
       * \param d the type of the domain
       */
      constexpr explicit DomainConfiguration(DomainType d)
          : domain_(std::move(d)){};
      //************************************************************************

      //************************************************************************
      /*!\brief Constructor.
       *
       * \param args Arguments forwarded to the domain type d
       */
      template <typename... Args>
      constexpr explicit DomainConfiguration(Args&&... args)
          : DomainConfiguration(DomainType(std::forward<Args>(args)...)) {}
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Obtains the domain
       */
      inline auto domain() const& noexcept -> DomainType const& {
        return This::domain_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Obtains the domain
       */
      inline auto domain() && noexcept -> DomainType&& {
        return static_cast<DomainType&&>(This::domain_);
      }
      //************************************************************************

     private:
      //**Member variables******************************************************
      //! Domain parameter
      DomainType domain_;
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Makes a domain configuration
     * \ingroup config
     *
     * \example
     * To make a fully unbounded simulation
     * \snippet ModuleConfiguration/Test_Domain.cpp unbounded_eg
     * To make a periodic simulation, periodic in all directions
     * \snippet ModuleConfiguration/Test_Domain.cpp periodic_eg
     * To make a mixed simulation, which is periodic in some directions and
     * unbounded in others
     * \snippet ModuleConfiguration/Test_Domain.cpp mixed_eg
     * The code becomes more readable when you use the ::elastica namespace
     * \snippet ModuleConfiguration/Test_Domain.cpp shorthand_eg
     *
     * \param along_x Configuration along the x direction,
     * returned from elastica::boundary_along
     * \param along_y Configuration along the y direction,
     * returned from elastica::boundary_along
     * \param along_z Configuration along the z direction,
     * returned from elastica::boundary_along
     * \return A domain configuration
     */
    template <typename BoundaryConditionInX, typename BoundaryConditionInY,
              typename BoundaryConditionInZ, typename Real>
    inline constexpr auto make_domain_configuration(
        ::elastica::detail::BoundaryAlongWrapper<
            ::elastica::detail::EulerianFrame::X, BoundaryConditionInX, Real>
            along_x,
        ::elastica::detail::BoundaryAlongWrapper<
            ::elastica::detail::EulerianFrame::Y, BoundaryConditionInY, Real>
            along_y,
        ::elastica::detail::BoundaryAlongWrapper<
            ::elastica::detail::EulerianFrame::Z, BoundaryConditionInZ, Real>
            along_z) noexcept {
      using RT = DomainConfiguration<BoundaryConditionInX, BoundaryConditionInY,
                                     BoundaryConditionInZ>;
      return RT{std::move(along_x), std::move(along_y), std::move(along_z)};
    }
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica