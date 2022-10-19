//==============================================================================
/*!
//  \file
//  \brief Common policy types
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
#include "Utilities/CRTP.hpp"
#include "Utilities/ForceInline.hpp"
#include "Utilities/Overloader.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace configuration {

    //**************************************************************************
    //! String type
    using String = const char* const;
    //**************************************************************************

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Quickly compose different criteria/strategies for blocking
     * \ingroup config
     *
     * \tparam Criteria Criteria to be composed
     *
     * \see Overloader
     */
    template <class... Criteria>
    struct CompositeCriteria : public Overloader<Criteria...> {
      //**Type definitions******************************************************
      //! Parent type
      using Parent = Overloader<Criteria...>;
      //************************************************************************

      //! Inherit constructors
      using Parent::Parent;
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief A blocking policy based on a single size parameter
     * \ingroup config
     *
     * \details
     * CRTP class for ease in implementing new policies
     *
     * \tparam Policy The actual implementation of the policy class
     */
    template <typename Policy>
    struct SingleSizePolicy : CRTPHelper<Policy, SingleSizePolicy> {
      //**Type definitions******************************************************
      //! This type
      using CRTP = CRTPHelper<Policy, SingleSizePolicy>;
      //! CRTP methods
      using CRTP::self;
      //! Size Argument
      struct Size {
        using type = std::size_t;
        static constexpr String help = {"Size parameter of the policy"};
      };
      //! List of options
      using options = tmpl::list<Size>;
      //************************************************************************

      //************************************************************************
      /*!\brief Default constructor
       * \note Default constructor is not deleted for options parsing
       */
      SingleSizePolicy() : SingleSizePolicy(0UL) {}
      //************************************************************************

      //************************************************************************
      /*!\brief Policy constructor.
       *
       * \param limit Max size per block
       */
      explicit ELASTICA_ALWAYS_INLINE SingleSizePolicy(const std::size_t limit)
          : max_dofs_per_block(limit){};
      //************************************************************************

      //************************************************************************
      /*!\brief Policy destructor.
       *
       * Explicitly inlined to ensure the object is not created
       */
      ELASTICA_ALWAYS_INLINE ~SingleSizePolicy() = default;
      //************************************************************************

      //**Value methods*********************************************************
      /*!\name Value methods*/
      //@{
      inline auto value() & noexcept -> std::size_t& {
        return max_dofs_per_block;
      }
      inline auto value() const& noexcept -> std::size_t {
        return max_dofs_per_block;
      }
      //@}
      //************************************************************************

      //**Member variables******************************************************
      //! Maximum dofs before which another block is allocated
      std::size_t max_dofs_per_block;
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica