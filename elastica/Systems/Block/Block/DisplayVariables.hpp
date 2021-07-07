//==============================================================================
/*!
//  \file
//  \brief Displays variables associated with Plugins
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
#include <iostream>
#include <ostream>
#include <string>

#include "Systems/Block/Block/TypeTraits.hpp"
#include "Systems/Block/BlockVariables/Types.hpp"
#include "Utilities/PrettyType.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/WidthStream.hpp"

namespace blocks {

  //****************************************************************************
  /*!\brief Customization point for displaying variable information in plugins
   * \ingroup blocks
   *
   * \details
   * DisplayVariables provides the customization point for specifying the
   * behavior of display_variable_summary() for any `Plugin` type.
   * If no customized implementation is provided, \elastica falls back to the
   * operations implemented by this default DisplayVariables class, which does
   * a no-operation.
   *
   * \usage
   * The no-operation above uses the static apply() function templated on the
   * variable
   * \code
   * std::ostream& os; // cout, string stream etc.
   * DisplayVariables<Plugin>::template apply<Variable>(os);
   * \endcode
   *
   * \section customization DisplayVariables customization
   * As seen from the example above, the DisplayVariables class needs to
   * define a static apply() function with the following signature
   * \snippet this apply_signature
   *
   * Hence to customize DisplayBehavior for your own plugin, we rely on template
   * specialization, typically done in the translation unit where the Plugin
   * type is defined. As an example of implementing DisplayVariables for a
   * custom plugin, consider the following example
   *
   * \example
   * With the setup for Plugin shown below
   * \snippet Test_DisplayVariables.cpp customized_plugin
   * the following code demonstrates the customization of DisplayVariables
   * \snippet Test_DisplayVariables.cpp customization_for_plugin
   *
   * \tparam Plugin Plugin for which display_variable_summary() need to be
   * customized
   *
   * \see Block, blocks::protocols::Plugin, blocks::Variable,
   * display_variable_summary()
   */
  template <typename Plugin>
  struct DisplayVariables {
    /// [apply_signature]
    template <typename Variable>
    static inline auto apply(std::ostream& /*out*/) noexcept -> void {}
    /// [apply_signature]
  };

  namespace detail {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Helper for displaying variables information
     * \ingroup blocks
     *
     * \tparam Plugin A valid plugin template conforming to protocols::Plugin
     */
    template <typename Plugin, bool B = is_plugin_v<Plugin>>
    struct DisplayVariablesHelper {
      //************************************************************************
      /*!\brief Apply function implementing the displaying
       */
      static void apply(std::ostream& out) {
        out << "Name : " << pretty_type::short_name<Plugin>() << "\n";
        out << "Not a plugin!" << std::endl;
      }
      //************************************************************************
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specializaiton of DisplayVariablesHelper for Plugin types
     * \ingroup blocks
     */
    template <typename Plugin>
    struct DisplayVariablesHelper<Plugin, true> {
      //************************************************************************
      /*!\brief Apply function implementing the displaying
       */
      static void apply(std::ostream& outstream) {
        using CustomizationPoint = DisplayVariables<Plugin>;

        const std::size_t line_length = 80UL;

        // similar function to Wraptext, but more flexible
        ::elastica::widthstream out(line_length, outstream);

        const std::string information("Variable information: ");
        out << "Plugin name : " << pretty_type::short_name<Plugin>() << "\n\n"
            << information << "\n"
            << std::string(information.size(), '*') << "\n";

        using Variables = variables_t<Plugin>;
        if (not tmpl::size<Variables>()) {
          out << "\n"
              << "No variables!";
        } else {
          tmpl::for_each<Variables>([&out, tab_width = 4,
                                     variable_index = 0UL](auto v) mutable {
            using Var = tmpl::type_from<decltype(v)>;
            out << "\n";
            out << std::setfill('0') << std::setw(2) << ++variable_index
                << ". ";
            out << "Name : " << pretty_type::get_name<Var>() << " \n";
            // sticky
            out.indent(tab_width);
            out << "Parameter : " << pretty_type::get_name<parameter_t<Var>>()
                << " \n";
            out << "Initialized? : " << std::boolalpha
                << tt::is_detected_v<initializer_t, Var> << " \n";
            out << "Rank : " << pretty_type::get_name<rank_t<Var>>() << " \n";
            out << "Data type : " << pretty_type::get_name<typename Var::type>()
                << " \n";
            CustomizationPoint::template apply<Var>(out);
            // reset
            out.indent(-tab_width);
          });
        }

        // remember to flush!
        out << std::endl;
      };
      //************************************************************************
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace detail

  //****************************************************************************
  /*!\brief Displays variable information associated with a Plugin
   *
   * \example
   * \code
   * using Plugin = MyCustomPlugin;
   * std::ostream os; // std::cout, std::stringstream
   * blocks::display_variable_summary<Plugin>(os);
   * \endcode
   *
   * \tparam Plugin A valid plugin template conforming to protocols::Plugin
   */
  template <typename Plugin>
  inline void display_variable_summary(std::ostream& os = std::cout) {
    detail::DisplayVariablesHelper<Plugin>::apply(os);
  }
  //****************************************************************************

}  // namespace blocks
