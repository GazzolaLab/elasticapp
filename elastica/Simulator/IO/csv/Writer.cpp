#include "Writer.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <utility>

#include "elastica/ErrorHandling/Assert.hpp"
#include "elastica/Utilities/Logging.hpp"

// Concrete types
#include "Systems/RigidBody/SphereCollection.hpp"

namespace elastica {

  namespace io {

    namespace csv {

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
                     std::size_t tstart, std::size_t tend)
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
            spheres_()                 // Registered spheres
      {
        using namespace fs;

        // 0. Sanity check if called multiple times
        ELASTICA_ASSERT(!active_,
                        "Multiple constructor calls for a singleton class");
        active_ = true;

        // 1. Test the path and mkdir
        path p(output_directory_);
        if (exists(p)) {
          std::cerr << "csv::Writer::Writer(): Directory exists: " << p
                    << ". - Files in this directory may be overwritten!\n";
        }
        if (!exists(p)) {
          create_directory(p);
        }

        // 3. Adding the registered visible spheres and boxes
        for (auto sphere : Visualization::spheres_) {
          add_sphere(sphere);
        }

        //        for (auto bounding_box : Visualization::bounding_boxes_) {
        //          add_bounding_box(bounding_box);
        //        }

        // Logging the successful setup of the CSV writer
        LOG_INFO_SECTION(log) {
          log << "Successfully initialized the CSV writer instance";
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
        // Logging the successful destruction of the CSV writer
        LOG_INFO_SECTION(log) {
          log << "Successfully destroyed the CSV writer instance";
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
      void Writer::add_sphere(SphereCollection const* sphere) {
        spheres_.push_back(sphere);
      }
      //*************************************************************************************************

      void Writer::add_bounding_box(Visualization::AABB const* bounding_box) {
        return;
      }

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
      void Writer::remove_sphere(SphereCollection const* sphere) {
        auto pos(std::find(spheres_.begin(), spheres_.end(), sphere));
        ELASTICA_ASSERT(pos != spheres_.end(),
                        "Sphere is not registered for visualization");
        spheres_.erase(pos);
      }
      //*************************************************************************************************

      void Writer::remove_bounding_box(
          Visualization::AABB const* bounding_box) {
        return;
      }

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
            ".csv";

        std::ostringstream spherefile;
        spherefile << output_directory_ << "/sphere_csv_data_"
                   << fn_with_suffix;
        write_spheres(spherefile.str().c_str());
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
      void Writer::write_spheres(fs::path const& filename) const {
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

          for (auto sph : spheres_) {
            write_sphere_data_in_ascii(out, sph);
          }

          // Closing the output file
          out.close();
        }
      }
      //*************************************************************************************************

      void Writer::write_sphere_data_in_ascii(
          std::ostream& out, SphereCollection const* sphere_collection) const {
        // write grid
        std::size_t n_spheres(sphere_collection->size());
        const char* tab = ",";

        // Write the sphere positions
        for (std::size_t i = 0UL; i < n_spheres; ++i) {
          auto pos(sphere_collection->get_position(i));
          out << pos[0] << tab << pos[1] << tab << pos[2] << "\n";
        }
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
        os << " CSV file                                : '"
           << output_directory_ << "'\n"
           << " Number of time steps                    : " << counter_ << "\n"
           << " Number of registered sphere collections : " << spheres_.size()
           << "\n";
      }
      //*************************************************************************************************

      //=================================================================================================
      //
      //  VTK WRITER SETUP FUNCTIONS
      //
      //=================================================================================================

      //*************************************************************************************************
      /*!\brief Activation of the CSV writer.
       * \ingroup vtk
       *
       * \param filename File name for the CSV output.
       * \param spacing Spacing between two written time steps \f$ [1..\infty) \f$.
       * \param tstart First time step index to write.
       * \param tend Last time step index to write.
       * \return Handle to the active CSV writer.
       * \exception std::invalid_argument Invalid spacing value or file name.
       *
       * The first call activates the writer and returns its handle; later calls
       * return the same handle and ignore the arguments.
       */
      std::shared_ptr<Writer> make_writer(std::string const& filename,
                                          std::size_t spacing,
                                          std::size_t tstart,
                                          std::size_t tend) {
#ifdef HAVE_TBB
        tbb::mutex::scoped_lock lock(_log_mutex);
#else
        std::lock_guard<std::mutex> lock(Writer::instance_mutex_,
                                         std::adopt_lock);
#endif
        static std::shared_ptr<Writer> dx(
            new Writer(filename, spacing, tstart, tend));
        return dx;
      }

    }  // namespace csv

  }  // namespace io

}  // namespace elastica
