//==============================================================================
/*!
//  \file
//  \brief Data collection interface in H5
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
//
#include "Simulator/IO/H5/Interface/Types.hpp"
#include "Simulator/IO/Interface/Helpers.hpp"  // format
//
#include "Simulator/IO/H5/_Impl.hpp"  // we need all implementation here for read write
//
#include "Utilities/NonCopyable.hpp"
#include "Utilities/PrettyType.hpp"
//
#include <cstddef>
#include <highfive/H5File.hpp>
#include <highfive/H5Group.hpp>

namespace elastica {

  namespace io {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */

    //**************************************************************************
    /*!\brief Recorder for system data in H5
     * \ingroup io
     *
     * \details
     * Setups up the common data-structures needed for H5 read and write.
     */
    class H5InterfaceHelper {
      //**Type definitions******************************************************
      //! Map from systems to counters
      using Counters = std::unordered_map<std::string, std::size_t>;
      //************************************************************************

     public:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{

      //************************************************************************
      /*!\brief Constructor with a filename
       *
       * \param h5file H5 file to write data into
       */
      H5InterfaceHelper(HighFive::File h5file)
          : file_(std::move(h5file)), counters_(){};
      //************************************************************************

      //@}
      //************************************************************************

      //**Access operators******************************************************
      /*!\name Access operators */
      //@{

      //************************************************************************
      /*!\brief Access to the metadata file.
       */
      inline auto file() & noexcept -> HighFive::File& { return file_; }
      inline auto file() const& noexcept -> HighFive::File const& {
        return file_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Access to the metadata counter.
       */
      inline auto counters() & noexcept -> Counters& { return counters_; }
      inline auto counters() const& noexcept -> Counters const& {
        return counters_;
      }
      //************************************************************************

      //@}
      //************************************************************************

      //**Utility functions*****************************************************
      /*!\name Utility functions */
      //@{

      //************************************************************************
      /*!\brief Formats the counter.
       *
       * \note This supports upto a maximum of 10^10 systems.
       */
      static auto format(std::size_t count) -> std::string {
        return format_id(10, std::to_string(count), '0');
      }
      //@}
      //************************************************************************

     protected:
      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      //! File to record information into
      HighFive::File file_;
      //! Counters
      std::unordered_map<std::string, std::size_t> counters_{};
      //@}
      //************************************************************************
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*!\brief Export system data to  H5
     * \ingroup io
     *
     * \warning This class does NOT support thread-safe operations and is
     * expected to work correctly only on single-threaded environments.
     */
    class H5Writer : public H5InterfaceHelper {
     protected:
      //**Type definitions******************************************************
      //! Parent name
      using Parent = H5InterfaceHelper;
      //************************************************************************

     public:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{
      using Parent::Parent;
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Utility operator*/
      //@{

      //************************************************************************
      /*!\brief Records the system `T` data into a H5 file
       *
       * \param system System whose state is written to the archive.
       */
      template <typename System>
      auto operator()(System const& system) /*const noexcept*/ {
        auto const name = pretty_type::name<System>();
        auto& counter = counters_[name];
        auto group = file().exist(name)
                         ? file().getGroup(name)
                         : (counter = 0, file().createGroup(name));
        write_data(group, system, Parent::format(counter++));
      };
      //************************************************************************

      //@}
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Import system data from H5
     * \ingroup io
     *
     * \warning This class does NOT support thread-safe operations and is
     * expected to work correctly only on single-threaded environments.
     */
    class H5Reader : public H5InterfaceHelper {
     protected:
      //**Type definitions******************************************************
      //! Parent name
      using Parent = H5InterfaceHelper;
      //************************************************************************

     public:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{
      using Parent::Parent;
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Utility operator*/
      //@{

      //************************************************************************
      /*!\brief Reads the system `T` data from a H5 file
       *
       * \param system System whose state is restored from the archive.
       */
      template <typename System>
      auto operator()(System& system) /*const noexcept*/ {
        auto const name = pretty_type::name<System>();
        auto& counter = counters_[name];
        read_data(file().getGroup(name), system, Parent::format(counter++));
      };
      //************************************************************************

      //@}
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace io

}  // namespace elastica