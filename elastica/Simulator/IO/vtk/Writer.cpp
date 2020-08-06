//=================================================================================================
/*!
 *  \file src/vtk/Writer.cpp
 *  \brief VTK file writer for the VTK visualization
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

#include "Writer.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <utility>

#include "elastica/ErrorHandling/Assert.hpp"
#include "elastica/Utilities/Logging.hpp"

namespace elastica {
  namespace io {
    namespace vtk {

      //=================================================================================================
      //
      //  DEFINITION AND INITIALIZATION OF THE STATIC MEMBER VARIABLES
      //
      //=================================================================================================

      bool Writer::active_(false);
      Writer::MutexType Writer::instance_mutex_;

      //=================================================================================================
      //
      //  CONSTRUCTOR
      //
      //=================================================================================================

      //*************************************************************************************************
      /*!\brief The default constructor of the Writer class.
       *
       * \param filename File name for the VTK visualization file.
       * \param spacing Spacing between two visualized time steps \f$
       * [1..\infty) \f$. \exception std::invalid_argument Invalid file name.
       */
      Writer::Writer(std::string filename, std::size_t spacing,
                     std::size_t tstart, std::size_t tend, bool binary)
          : Visualization(),  // Initialization of the Visualization base object
            Dependency<logging::Logger>(),  // Initialization of the logger
                                            // lifetime dependency
            tspacing_(spacing),  // Spacing between two visualized time steps
            tstart_(tstart),
            tend_(tend),
            steps_(0),    // Time step counter between two time steps
            counter_(0),  // Visualization counter for the number of visualized
                          // time steps
            output_directory_(
                std::move(filename)),  // The output directory for the VTK files
            binary_(binary),
            spheres_(),        // Registered spheres
            bounding_boxes_()  // Registered AABBs
      {
        using namespace fs;

        // 0. Sanity check if called multiple times
        ELASTICA_ASSERT(!active_,
                        "Multiple constructor calls for a singleton class");
        active_ = true;

        // 1. Test the path and mkdir
        path p(output_directory_);
        if (exists(p)) {
          std::cerr << "vtk::Writer::Writer(): Directory exists: " << p
                    << ". - Files in this directory may be overwritten!\n";
        }
        if (!exists(p)) {
          create_directory(p);
        }

        // 2. Add a collector file to take information from other files
        // Caveats:
        // 1. If somehow the constructor call is in a multi-process environment
        // only the root process should do this (should be wrapped in an
        // exclusive section)
        if (true) {
          // Write the pvd collector file (Paraview) for all time steps
          std::stringstream pvdfile;
          pvdfile << output_directory_ << "/collector.pvd";
          std::ofstream pvd(pvdfile.str().c_str(),
                            std::ofstream::out | std::ofstream::trunc);
          if (!pvd.is_open())
            throw std::invalid_argument("Invalid file name: ");

          // write header
          pvd << "<?xml version=\"1.0\"?>" << std::endl;
          pvd << "<!-- This pvd file references all the written time steps. "
                 "-->\n";
          pvd << "<VTKFile type=\"Collection\" version=\"0.1\" \n"
                 "byte_order=\"LittleEndian\" \n"
                 "compressor=\"vtkZLibDataCompressor\">"
              << std::endl;
          pvd << "<Collection>" << std::endl;

          // TODO : Move this to the destructor for changing time-step
          // implementations write filenames of datasets
          int timeCount = 1;  // for numbering of the files
          for (unsigned int t = tstart_; t < tend_;
               t += tspacing_, timeCount++) {
            // TODO : Multiple threads, change procs

            // To pad the counter with zeros
            // input 45, output 000045
            std::string count_string(std::to_string(timeCount));
            std::string new_string =
                std::string(6 - count_string.length(), '0') + count_string;

            for (int proc = 0; proc < 1; proc++) {
              // Write spheres entry
              pvd << "<DataSet timestep=\"" << t << "\" part=\"" << 2 * proc
                  << "\" file=\"" << proc << "/spheres" << new_string
                  << ".vtu\"/>\n";

              // Write boxes entry
              pvd << "<DataSet timestep=\"" << t << "\" part=\"" << 2 * proc + 1
                  << "\" file=\"" << proc << "/boxes" << new_string
                  << ".vtu\"/>\n";

              // TODO: Write other entries
            }
          }

          // close tags
          pvd << "</Collection>" << std::endl;
          pvd << "</VTKFile>" << std::endl;
          pvd.close();
        }

        // 3. Adding the registered visible spheres and boxes
        for (auto sphere : Visualization::spheres_) {
          add_sphere(sphere);
        }

        for (auto bounding_box : Visualization::bounding_boxes_) {
          add_bounding_box(bounding_box);
        }
        // Logging the successful setup of the VTK writer
        LOG_INFO_SECTION(log) {
          log << "Successfully initialized the VTK writer instance";
        }
      }

      //=================================================================================================
      //
      //  DESTRUCTOR
      //
      //=================================================================================================

      //*************************************************************************************************
      /*!\brief The destructor of the Writer class.
       */
      Writer::~Writer() {
        // Logging the successful destruction of the VTK writer
        LOG_INFO_SECTION(log) {
          log << "Successfully destroyed the VTK writer instance";
        }
      }
      //*************************************************************************************************

      //=================================================================================================
      //
      //  SET FUNCTIONS
      //
      //=================================================================================================

      //*************************************************************************************************
      /*!\brief Setting the spacing of the VTK visualization.
       *
       * \param spacing Spacing between two visualized time steps \f$
       * [1..\infty) \f$. \exception std::invalid_argument Invalid spacing
       * value.
       */
      void Writer::set_spacing(std::size_t spacing) {
        // Checking the spacing value
        if (spacing == 0)
          throw std::invalid_argument("Invalid zero spacing value");

        tspacing_ = spacing;
      }
      //*************************************************************************************************

      //=================================================================================================
      //
      //  ADD FUNCTIONS
      //
      //=================================================================================================

      //*************************************************************************************************
      /*!\brief Registering a single sphere for the VTK visualization.
       *
       * \param sphere The sphere to be registered.
       * \return void
       */
      void Writer::add_sphere(rigidbody::Sphere const* sphere) {
        spheres_.push_back(sphere);
      }
      //*************************************************************************************************

      //*************************************************************************************************
      /*!\brief Registering a single box for the VTK visualization.
       *
       * \param box The box to be registered.
       * \return void
       */
      void Writer::add_bounding_box(AABB const* box) {
        // The Writer is not able to visualize boxes. Therefore the box doesn't
        // have to be registered.
        bounding_boxes_.push_back(box);
      }
      //*************************************************************************************************

      //=================================================================================================
      //
      //  REMOVE FUNCTIONS
      //
      //=================================================================================================

      //*************************************************************************************************
      /*!\brief Removing a single sphere from the VTK visualization.
       *
       * \param sphere The sphere to be removed.
       * \return void
       */
      void Writer::remove_sphere(const rigidbody::Sphere* sphere) {
        auto pos(std::find(spheres_.begin(), spheres_.end(), sphere));
        ELASTICA_ASSERT(pos != spheres_.end(),
                        "Sphere is not registered for visualization");
        spheres_.erase(pos);
      }
      //*************************************************************************************************

      //*************************************************************************************************
      /*!\brief Removing a single bounding box from the VTK visualization.
       *
       * \param bounding box The bounding box to be removed.
       * \return void
       */
      void Writer::remove_bounding_box(const AABB* bounding_box) {
        auto pos(std::find(bounding_boxes_.begin(), bounding_boxes_.end(),
                           bounding_box));
        ELASTICA_ASSERT(pos != bounding_boxes_.end(),
                        "Bounding box is not registered for visualization");
        bounding_boxes_.erase(pos);
      }
      //*************************************************************************************************

      //=================================================================================================
      //
      //  OUTPUT FUNCTIONS
      //
      //=================================================================================================

      //*************************************************************************************************
      /*!\brief Visualizing the current state of the registered rigid bodies.
       *
       * \return void
       *
       * This function is automatically called every time step. It extends the
       * VTK visualization file by the current state of all registered spheres.
       */
      void Writer::trigger(double time_v, std::size_t simulation_step) {
        // Skipping the visualization for intermediate time steps
        if (++steps_ < tspacing_)
          return;

        // Adjusing the counters
        steps_ = 0;
        ++counter_;

        // To pad the counter with zeros
        // input 45, output 000045
        std::string counter_string(std::to_string(counter_));
        std::string fn_with_suffix =
            std::string(6 - counter_string.length(), '0') + counter_string +
            ".vtu";

        std::ostringstream spherefile;
        spherefile << output_directory_ << "/spheres" << fn_with_suffix;
        write_spheres(spherefile.str().c_str());

        std::ostringstream boxfile;
        boxfile << output_directory_ << "/boxes" << fn_with_suffix;
        write_bounding_boxes(boxfile.str().c_str());
      }
      //*************************************************************************************************

      //*************************************************************************************************
      /*!\brief Visualizing the current state of the registered spheres.
       *
       * \return void
       *
       * This function creates a new file containing the sphere data of the
       * current timestep
       */
      void Writer::write_spheres(const fs::path& filename) const {
        using namespace fs;

        // Determining the directory and the filename for the POV-Ray
        // visualization
        const path directory(filename.parent_path());
        const path file(filename.filename());

        // Checking the directory and the filename
        if (!directory.empty() && !exists(directory))
          throw std::runtime_error(
              "Directory for VTK-Ray files does not exist");
        if (file.empty())
          throw std::runtime_error("Invalid file name");

        // Generation of a single VTK-Ray file
        // In case the 'singleFile' flag is set to 'true' all processes append
        // their local, finite rigid bodies to the main POV-Ray file 'filename'.
        // This task is performed serially in ascending order.
        //        if (false /*singleFile_  || MPISettings::size() == 1 */) {
        //          pe_SERIALIZATION {
        //            throw std::runtime_error(
        //                "src/vtk/Writer.cpp::writeSpheres(): not yet
        //                implemented");
        //
        //            // Opening the output file
        //            std::ofstream out(filename.string().c_str(),
        //                              std::ofstream::out | std::ostream::app);
        //            if (!out.is_open()) {
        //              std::ostringstream oss;
        //              oss << " Error opening VTK-Ray file '" << filename << "'
        //              !\n"; throw std::runtime_error(oss.str());
        //            }
        //
        //            // Writing the registered spheres
        //            for (Spheres::ConstIterator s = spheres_.begin();
        //                 s != spheres_.end(); ++s) {
        //            }
        //
        //            // Closing the output file
        //            out.close();
        //          }
        //        }

        // Generation of multiple VTK-Ray files
        // In case the 'singleFile' flag is set to 'false' each process creates
        // an individual POV-Ray file (containing their local rigid bodies) that
        // is included in the main POV-Ray file 'filename'.
        {
          // Creating the process-specific extended directory
          path extended(directory);
          //          extended /=
          //          lexical_cast<std::string>(MPISettings::rank());
          extended /= std::to_string(0);
          if (!exists(extended))
            create_directory(extended);
          extended /= file;

          // Opening the output file
          std::ofstream out(extended.string().c_str(),
                            std::ofstream::out | std::ostream::trunc);
          if (!out.is_open()) {
            std::ostringstream oss;
            oss << " Error opening VTK-Ray file '" << filename << "' !\n";
            throw std::runtime_error(oss.str());
          }

          /*
          if (binary_) {
            // std::cout << "writing spheres binary";
            writeSphereDataBinary(out);
          } else {
            // std::cout << "writing spheres ascii";
            writeSphereDataAscii(out);
          }
          */

          for (auto sph : spheres_) {
            write_sphere_data_in_ascii(out, sph);
          }

          // Closing the output file
          out.close();
        }
      }
      //*************************************************************************************************

      void Writer::write_sphere_data_in_ascii(
          std::ostream& out, const rigidbody::Sphere* sphere_collection) const {
        // write grid
        std::size_t n_spheres(sphere_collection->size());
        out << "<?xml version=\"1.0\"?>\n";
        out << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" "
               "byte_order=\"LittleEndian\">\n";
        out << " <UnstructuredGrid>\n";
        out << "  <Piece NumberOfPoints=\"" << n_spheres
            << "\" NumberOfCells=\"" << n_spheres << "\">\n";
        out << "   <Points>\n";
        out << "    <DataArray type=\""
            << "Float32"
            << "\" NumberOfComponents=\"" << 3 << "\" format=\"ascii\">\n";

        // Write the sphere positions
        for (std::size_t i = 0UL; i < n_spheres; ++i) {
          auto pos(sphere_collection->get_position(i));
          out << "\t" << pos[0] << "\t" << pos[1] << "\t" << pos[2] << "\n";
        }

        // Declare grid as point cloud
        out << "    </DataArray>\n";
        out << "   </Points>\n";
        out << "   <Cells>\n";
        out << "    <DataArray type=\"Int32\" Name=\"connectivity\">\n";
        for (std::size_t i = 0; i < n_spheres; i++)
          out << " " << i << "\n";
        out << "    </DataArray>\n";
        out << "    <DataArray type=\"Int32\" Name=\"offsets\">\n";
        for (std::size_t i = 0; i < n_spheres; i++)
          out << " " << i + 1 << "\n";
        out << "    </DataArray>\n";
        out << "    <DataArray type=\"UInt8\" Name=\"types\">\n";
        for (std::size_t i = 0; i < n_spheres; i++)
          out << " " << 1 << "\n";
        out << "    </DataArray>\n";
        out << "   </Cells>\n";

        // Data at each point
        out << "   <PointData Scalars=\"scalars\" Vectors=\"vectors\">\n";

        /*
        // write IDs
        out << "    <DataArray type=\""
            << "UInt32"
            << "\" Name=\""
            << "ID"
            << "\" NumberOfComponents=\"" << 1 << "\" format=\"ascii\">\n";
        for (Spheres::ConstIterator s = spheres_.begin(); s != spheres_.end();
             ++s) {
          out << "\t" << s->getID() << "\n";
        }  // end for all Spheres
        out << "    </DataArray>\n";
        */

        // write Radii
        out << "    <DataArray type=\""
            << "Float32"
            << "\" Name=\""
            << "Radius"
            << "\" NumberOfComponents=\"" << 1 << "\" format=\"ascii\">\n";
        for (std::size_t i = 0UL; i < n_spheres; ++i) {
          auto radius(sphere_collection->get_radius(i));
          out << "\t" << radius << "\n";
        }  // end for all Spheres
        out << "    </DataArray>\n";

        // write Mass
        out << "    <DataArray type=\""
            << "Float32"
            << "\" Name=\""
            << "Mass"
            << "\" NumberOfComponents=\"" << 1 << "\" format=\"ascii\">\n";
        for (std::size_t i = 0UL; i < n_spheres; ++i) {
          auto mass(sphere_collection->get_mass(i));
          out << "\t" << mass << "\n";
        }  // end for all Spheres
        out << "    </DataArray>\n";

        /*
        // write Orientation
        out << "    <DataArray type=\""
            << "Float32"
            << "\" Name=\""
            << "Orientation"
            << "\" NumberOfComponents=\"" << 9 << "\" format=\"ascii\">\n";
        for (Spheres::ConstIterator s = spheres_.begin(); s != spheres_.end();
             ++s) {
          const pe::Rot3& rot = s->getRotation();
          // Vector3<Real> o = calcEulerAngles(rot);
          out << "\t" << rot[0] << " " << rot[1] << " " << rot[2] << "\t"
              << rot[3] << " " << rot[4] << " " << rot[5] << "\t" << rot[6]
              << " " << rot[7] << " " << rot[8] << "\n";
        }  // end for all Spheres
        out << "    </DataArray>\n";
         */

        /*
        // write Euler angles
        out << "    <DataArray type=\""
            << "Float32"
            << "\" Name=\""
            << "Euler Rotation"
            << "\" NumberOfComponents=\"" << 3 << "\" format=\"ascii\">\n";
        for (Spheres::ConstIterator s = spheres_.begin(); s != spheres_.end();
             ++s) {
          const pe::Vec3& rot = s->getRotation().getEulerAnglesXYZ();
          // Vector3<Real> o = calcEulerAngles(rot);
          out << "\t" << rot[0] << "\t" << rot[1] << "\t" << rot[2] << "\n";
          // out << "\t" << o[0] << "\t" << o[1] << "\t" << o[2] << "\n";
        }  // end for all Boxes
        out << "    </DataArray>\n";
         */

        // write Net Force
        out << "    <DataArray type=\""
            << "Float32"
            << "\" Name=\""
            << "Net Force"
            << "\" NumberOfComponents=\"" << 3 << "\" format=\"ascii\">\n";
        for (std::size_t i = 0UL; i < n_spheres; ++i) {
          auto force(sphere_collection->get_force(i));
          out << "\t" << force[0] << "\t" << force[1] << "\t" << force[2]
              << "\n";
        }  // end for all Spheres
        out << "    </DataArray>\n";

        // write Velocsy
        out << "    <DataArray type=\""
            << "Float32"
            << "\" Name=\""
            << "Velocity"
            << "\" NumberOfComponents=\"" << 3 << "\" format=\"ascii\">\n";
        for (std::size_t i = 0UL; i < n_spheres; ++i) {
          auto velocity(sphere_collection->get_velocity(i));
          out << "\t" << velocity[0] << "\t" << velocity[1] << "\t"
              << velocity[2] << "\n";
        }  // end for all Spheres
        out << "    </DataArray>\n";

        /*
        // write angular Velocity
        out << "    <DataArray type=\""
            << "Float32"
            << "\" Name=\""
            << "Angular Velocity"
            << "\" NumberOfComponents=\"" << 3 << "\" format=\"ascii\">\n";
        for (std::size_t i = 0UL; i < n_spheres; ++i) {
          auto angular_velocity(sphere_collection->get_angular_velocity(i));
          out << "\t" << angular_velocity[0] << "\t" << angular_velocity[1]
              << "\t" << angular_velocity[2] << "\n";
        } // end for all Spheres
        out << "    </DataArray>\n";
        */

        out << "   </PointData>\n";
        out << "   <CellData>  </CellData>\n";
        out << "  </Piece>\n";
        out << " </UnstructuredGrid>\n";
        out << "</VTKFile>\n";
      }

      //*************************************************************************************************
      /*!\brief Visualizing the current state of the registered spheres.
       *
       * \return void
       *
       * This function creates a new file containing the sphere data of the
       * current timestep
       */
      // TODO : Much code repeated here, can be compressed
      void Writer::write_bounding_boxes(const fs::path& filename) const {
        using namespace fs;

        // Determining the directory and the filename for the POV-Ray
        // visualization
        const path directory(filename.parent_path());
        const path file(filename.filename());

        // Checking the directory and the filename
        if ((not directory.empty()) and (not exists(directory)))
          throw std::runtime_error(
              "Directory for VTK-Ray files does not exist");
        if (file.empty())
          throw std::runtime_error("Invalid file name");

        // Generation of a single VTK-Ray file
        // In case the 'singleFile' flag is set to 'true' all processes append
        // their local, finite rigid bodies to the main POV-Ray file 'filename'.
        // This task is performed serially in ascending order.
        //        if (false /*singleFile_  || MPISettings::size() == 1 */) {
        //          pe_SERIALIZATION {
        //            throw std::runtime_error(
        //                "src/vtk/Writer.cpp::writeSpheres(): not yet
        //                implemented");
        //
        //            // Opening the output file
        //            std::ofstream out(filename.string().c_str(),
        //                              std::ofstream::out | std::ostream::app);
        //            if (!out.is_open()) {
        //              std::ostringstream oss;
        //              oss << " Error opening VTK-Ray file '" << filename << "'
        //              !\n"; throw std::runtime_error(oss.str());
        //            }
        //
        //            // Writing the registered spheres
        //            for (Spheres::ConstIterator s = spheres_.begin();
        //                 s != spheres_.end(); ++s) {
        //            }
        //
        //            // Closing the output file
        //            out.close();
        //          }
        //        }

        // Generation of multiple VTK-Ray files
        // In case the 'singleFile' flag is set to 'false' each process creates
        // an individual POV-Ray file (containing their local rigid bodies) that
        // is included in the main POV-Ray file 'filename'.
        {
          // Creating the process-specific extended directory
          path extended(directory);
          //          extended /=
          //          lexical_cast<std::string>(MPISettings::rank());
          extended /= std::to_string(0);
          if (!exists(extended))
            create_directory(extended);
          extended /= file;

          // Opening the output file
          std::ofstream out(extended.string().c_str(),
                            std::ofstream::out | std::ostream::trunc);
          if (!out.is_open()) {
            std::ostringstream oss;
            oss << " Error opening VTK-Ray file '" << filename << "' !\n";
            throw std::runtime_error(oss.str());
          }

          /*
          if (binary_) {
            // std::cout << "writing spheres binary";
            writeSphereDataBinary(out);
          } else {
            // std::cout << "writing spheres ascii";
            writeSphereDataAscii(out);
          }
          */

          write_bounding_box_data_in_ascii(out);
          //          for (auto aabb : bounding_boxes_) {
          //            write_bounding_box_data_in_ascii(out, aabb);
          //          }

          // Closing the output file
          out.close();
        }
      }

      void Writer::write_bounding_box_data_in_ascii(std::ostream& out) const {
        // write grid
        out << "<?xml version=\"1.0\"?>\n";
        out << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" "
               "byte_order=\"LittleEndian\">\n";
        out << " <UnstructuredGrid>\n";
        out << "  <Piece NumberOfPoints=\"" << bounding_boxes_.size()
            << "\" NumberOfCells=\"" << bounding_boxes_.size() << "\">\n";
        out << "   <Points>\n";
        out << "    <DataArray type=\""
            << "Float32"
            << "\" NumberOfComponents=\"" << 3 << "\" format=\"ascii\">\n";

        // Write the sphere positions
        for (auto bbox : bounding_boxes_) {
          out << "\t" << bbox->get_position(0) << "\t" << bbox->get_position(1)
              << "\t" << bbox->get_position(2) << "\n";
        }

        // Declare grid as point cloud
        out << "    </DataArray>\n";
        out << "   </Points>\n";
        out << "   <Cells>\n";
        out << "    <DataArray type=\"Int32\" Name=\"connectivity\">\n";
        for (unsigned int i = 0; i < bounding_boxes_.size(); i++)
          out << " " << i << "\n";
        out << "    </DataArray>\n";
        out << "    <DataArray type=\"Int32\" Name=\"offsets\">\n";
        for (unsigned int i = 0; i < bounding_boxes_.size(); i++)
          out << " " << i + 1 << "\n";
        out << "    </DataArray>\n";
        out << "    <DataArray type=\"UInt8\" Name=\"types\">\n";
        for (unsigned int i = 0; i < bounding_boxes_.size(); i++)
          out << " " << 1 << "\n";
        out << "    </DataArray>\n";
        out << "   </Cells>\n";

        // Data at each point
        out << "   <PointData Scalars=\"scalars\" Vectors=\"vectors\">\n";

        /*
        // write IDs
        out << "    <DataArray type=\""
            << "UInt32"
            << "\" Name=\""
            << "ID"
            << "\" NumberOfComponents=\"" << 1 << "\" format=\"ascii\">\n";
        for (Boxes::ConstIterator s = bounding_boxes_.begin();
             s != bounding_boxes_.end(); ++s) {
          out << "\t" << s->getID() << "\n";
        }  // end for all Boxes
        out << "    </DataArray>\n";
         */

        // write Radii
        out << "    <DataArray type=\""
            << "Float32"
            << "\" Name=\""
            << "Lengths"
            << "\" NumberOfComponents=\"" << 3 << "\" format=\"ascii\">\n";
        for (auto bbox : bounding_boxes_) {
          out << "\t" << bbox->get_length(0) << "\t" << bbox->get_length(1)
              << "\t" << bbox->get_length(2) << "\n";
        }  // end for all Boxes
        out << "    </DataArray>\n";

        /*
        // write Mass
        out << "    <DataArray type=\""
            << "Float32"
            << "\" Name=\""
            << "Mass"
            << "\" NumberOfComponents=\"" << 1 << "\" format=\"ascii\">\n";
        for (auto bbox : bounding_boxes_) {
          out << "\t" << bbox->is_colliding() << "\n";
        }// end for all Boxes
        out << "    </DataArray>\n";
         */

        out << "   </PointData>\n";
        out << "   <CellData>  </CellData>\n";
        out << "  </Piece>\n";
        out << " </UnstructuredGrid>\n";
        out << "</VTKFile>\n";
      }
      //=================================================================================================
      //
      //  OUTPUT FUNCTIONS
      //
      //=================================================================================================

      //*************************************************************************************************
      /*!\brief Output of the current state of an VTK writer.
       *
       * \param os Reference to the output stream.
       * \return void
       */
      void Writer::print(std::ostream& os) const {
        os << " VTK file                                : '"
           << output_directory_ << "'\n"
           << " Number of time steps                    : " << counter_ << "\n"
           << " Number of registered sphere collections : " << spheres_.size()
           << "\n"
           << " Number of registered AABBs              : "
           << bounding_boxes_.size() << "\n";
      }
      //*************************************************************************************************

      //=================================================================================================
      //
      //  VTK WRITER SETUP FUNCTIONS
      //
      //=================================================================================================

      //*************************************************************************************************
      /*!\brief Activation of the VTK writer.
       * \ingroup vtk
       *
       * \param filename File name for the VTK visualization file.
       * \param spacing Spacing between two visualized time steps \f$
       * [1..\infty) \f$. \return Handle to the active VTK writer. \exception
       * std::invalid_argument Invalid spacing value. \exception
       * std::invalid_argument Invalid file name.
       *
       * This function activates the VTK writer for an VTK visualization. The
       * first call to this function will activate the VTK writer and return the
       * handle to the active writer, subsequent calls will ignore the
       * parameters and only return the handle to the VTK writer.
       */
      std::shared_ptr<Writer> make_writer(const std::string& filename,
                                          std::size_t spacing,
                                          std::size_t tstart, std::size_t tend,
                                          bool binary = false) {
#ifdef HAVE_TBB
        tbb::mutex::scoped_lock lock(_log_mutex);
#else
        std::lock_guard<std::mutex> lock(Writer::instance_mutex_,
                                         std::adopt_lock);
#endif
        static std::shared_ptr<Writer> dx(
            new Writer(filename, spacing, tstart, tend, binary));
        return dx;
      }
      //*************************************************************************************************

      //=================================================================================================
      //
      //  OPERATORS
      //
      //=================================================================================================

      //*************************************************************************************************
      /*!\brief Global output operator for VTK writers.
       * \ingroup vtk
       *
       * \param os Reference to the output stream.
       * \param dx Reference to a constant VTK writer object.
       * \return Reference to the output stream.
       */
      std::ostream& operator<<(std::ostream& os, const Writer& dx) {
        os << "--"
           << "VTK FILE WRITER"
           << "------------------------------------------------------------\n";
        dx.print(os);
        os << "----------------------------------------------------------------"
              "----------------\n"
           << std::endl;
        return os;
      }
      //*************************************************************************************************

      //*************************************************************************************************
      /*!\brief Global output operator for VTK writer handles.
       * \ingroup vtk
       *
       * \param os Reference to the output stream.
       * \param dx VTK writer handle.
       * \return Reference to the output stream.
       */
      std::ostream& operator<<(std::ostream& os,
                               const std::shared_ptr<Writer>& dx) {
        os << "--"
           << "VTK FILE WRITER"
           << "------------------------------------------------------------\n";
        dx->print(os);
        os << "----------------------------------------------------------------"
              "----------------\n"
           << std::endl;
        return os;
      }
      //*************************************************************************************************

      //*************************************************************************************************
      /*!\brief Global output operator for constant VTK writer handles.
       * \ingroup vtk
       *
       * \param os Reference to the output stream.
       * \param dx Constant VTK writer handle.
       * \return Reference to the output stream.
       */
      std::ostream& operator<<(std::ostream& os,
                               const std::shared_ptr<const Writer>& dx) {
        os << "--"
           << "VTK FILE WRITER"
           << "------------------------------------------------------------\n";
        dx->print(os);
        os << "----------------------------------------------------------------"
              "----------------\n"
           << std::endl;
        return os;
      }
      //*************************************************************************************************

    }  // namespace vtk
  }    // namespace io
}  // namespace elastica
