//=================================================================================================
/*!
 *  \file elastica/Simulator/IO/vtk/Writer.hpp
 *  \brief Header of the VTK file writer for the VTK visualization
 *
 *  Copyright (C) 2012 Simon Bogner
 *
 *  This file is part of pe.
 *
 *  pe is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 *  pe is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with
 * pe. If not, see <http://www.gnu.org/licenses/>.
 */
//=================================================================================================

#pragma once

#include <string>
#include <vector>
// TODO Check flags, is it reliable?
#if __cplusplus >= 201703L && __has_include(<filesystem>)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif

#ifdef HAVE_TBB
#include <tbb/mutex.h>
#else
#include <mutex>
#endif

#include "elastica/Simulator/Visualization.hpp"
#include "elastica/Utilities/Logging/Logger.hpp"
#include "elastica/Utilities/Singleton/Dependency.hpp"

namespace elastica {

  namespace io {

    namespace vtk {

#if __cplusplus >= 201703L && __has_include(<filesystem>)
      namespace fs = std::__fs::filesystem;
#else
      namespace fs = std::experimental::filesystem;
#endif

      // TODO : Derive from a CRTP instance that takes care of the trigger
      //  interface so that things like spacing, path management etc. can be
      //  reused between povray writer, checkpoint writer etc.
      // TODO : Revamp the entire Trigger structure : seems very brittle
      class Writer : public Visualization, private Dependency<logging::Logger> {
       private:
        //**Type
        // definitions**********************************************************
        //        using Visualization::Spheres;
        //        using Visualization::BoundingBoxes;
        using Spheres =
            std::vector<const SphereCollection*>;  //!< Vector for registered
                                                    //!< sphere primitives.
        using BoundingBoxes =
            std::vector<const AABB*>;  //!< Vector for registered
                                       //!< sphere primitives.

#ifdef HAVE_TBB
        using MutexType = tbb::mutex;
#else
        using MutexType = std::mutex;
#endif
        //**********************************************************************

        //**Constructor*********************************************************
        /*!\name Constructor */
        //@{
        explicit Writer(std::string filename, std::size_t spacing,
                        std::size_t tstart, std::size_t tend,
                        bool binary = true);
        //@}
        //**********************************************************************
       public:
        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        ~Writer() override;
        //@}
        //**********************************************************************

        //**Set
        // functions************************************************************
        /*!\name Set functions */
        //@{
        void set_spacing(std::size_t spacing);
        //@}
        //**********************************************************************

        //**Output
        // functions************************************************************
        /*!\name Output functions */
        //@{
        void print(std::ostream& os) const;
        //@}
        //**********************************************************************

       private:
        //**Add functions*******************************************************
        /*!\name Add functions */
        //@{
        void add_sphere(SphereCollection const* sphere) final;
        void add_bounding_box(AABB const* bounding_box) final;
        //@}
        //**********************************************************************

        //**Remove
        // functions*************************************************************
        /*!\name remove functions */
        //@{
        void remove_sphere(SphereCollection const* sphere) final;
        void remove_bounding_box(AABB const* bounding_box) final;
        //@}
        //**********************************************************************

        //**Visualization functions*********************************************
        /*!\name Visualization functions */
        //@{
        void trigger(double time_v, std::size_t simulation_step) final;

        virtual void write_spheres(const fs::path& filename) const;
        virtual void write_bounding_boxes(const fs::path& filename) const;

        virtual void write_sphere_data_in_ascii(
            std::ostream& out, const SphereCollection* sph) const;
        virtual void write_bounding_box_data_in_ascii(std::ostream& out) const;
        /*
                virtual void writeSphereDataAscii(std::ostream& out) const;
                virtual void writeSphereDataBinary(std::ostream& out) const;

                virtual void writeBoxDataAscii(std::ostream& out) const;
                virtual void writeBoxDataBinary(std::ostream& out) const;
                */
        //@}
        //**********************************************************************

        //**Member
        // variables************************************************************
        /*!\name Member variables */
        //@{
        std::size_t tspacing_;  //!< Spacing between two visualized time steps.
        std::size_t tstart_;    //!< First time step to be written
        std::size_t tend_;      //!< last time step to be written
        std::size_t steps_;     //!< Time step counter between two time steps.
        std::size_t counter_;   //!< Visualization counter for the number of
                                //!< visualized time steps.
        std::string output_directory_;  //!< The name of the VTK visualization
                                        //!< output directory.
        std::string
            prefix_;  //!< Prefix of the file name for the POV-Ray files.
        std::string
            postfix_;  //!< Postfix of the file name for the POV-Ray files.
        bool binary_;  //!< write binary if true; write ascii else

        Spheres spheres_;  //!< Registered spheres for the visualization.
        BoundingBoxes
            bounding_boxes_;  //!< Registered spheres for the visualization.

        static bool active_;  //!< Active flag of the VTK writer.

        //!< Synchronization mutex for !< access to the VTK writer.
        //@}
        static MutexType instance_mutex_;
        //**********************************************************************

        //**Friend declarations*************************************************
        /*! \cond PE_INTERNAL */
        friend bool is_active();
        // TODO : NamedTypes here instead of just types
        friend std::shared_ptr<Writer> make_writer(const std::string& filename,
                                                   std::size_t spacing,
                                                   std::size_t tstart,
                                                   std::size_t tend,
                                                   bool binary);
        /*! \endcond */
        //**********************************************************************
      };

      //=================================================================================================
      //
      //  VTK WRITER SETUP FUNCTIONS
      //
      //=================================================================================================

      //*************************************************************************************************
      /*!\name VTK writer setup functions */
      //@{
      inline bool is_active();
      std::shared_ptr<Writer> make_writer(const std::string& filename,
                                          std::size_t spacing,
                                          std::size_t tstart, std::size_t tend,
                                          bool binary);
      //@}
      //*************************************************************************************************

      //*************************************************************************************************
      /*!\brief Returns whether the VTK visualization is active or not.
       * \ingroup vtk
       *
       * \return \a true if the VTK visualization is active, \a false if not.
       */
      inline bool is_active() { return Writer::active_; }
      //*************************************************************************************************

      //========================================================================
      //
      //  OPERATORS
      //
      //========================================================================

      //************************************************************************
      /*!\name VTK writer operators */
      //@{
      std::ostream& operator<<(std::ostream& os, const Writer& dx);
      std::ostream& operator<<(std::ostream& os,
                               const std::shared_ptr<Writer>& dx);
      std::ostream& operator<<(std::ostream& os,
                               const std::shared_ptr<const Writer>& dx);
      //@}
      //************************************************************************

    }  // namespace vtk

  }  // namespace io

}  // namespace elastica
