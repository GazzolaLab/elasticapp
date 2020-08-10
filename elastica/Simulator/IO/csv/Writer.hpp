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

#include "Systems/RigidBody/RigidBodyCollection.hpp"
#include "elastica/Simulator/Visualization.hpp"
#include "elastica/Utilities/Logging/Logger.hpp"
#include "elastica/Utilities/Singleton/Dependency.hpp"

namespace elastica {

  namespace io {

    namespace csv {

#if __cplusplus >= 201703L && __has_include(<filesystem>)
      namespace fs = std::__fs::filesystem;
#else
      namespace fs = std::experimental::filesystem;
#endif

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

        explicit Writer(std::string filename, std::size_t spacing,
                        std::size_t tstart, std::size_t tend);

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

        virtual void write_spheres(fs::path const& filename) const;
        //        virtual void write_bounding_boxes(const fs::path& filename)
        //        const;

        virtual void write_sphere_data_in_ascii(
            std::ostream& out, SphereCollection const* sph) const;
        //        virtual void write_bounding_box_data_in_ascii(std::ostream&
        //        out) const;
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

        Spheres spheres_;  //!< Registered spheres for the visualization.
                           //        BoundingBoxes
        //            bounding_boxes_;  //!< Registered spheres for the
        //            visualization.

        static bool active_;  //!< Active flag of the VTK writer.

        //!< Synchronization mutex for !< access to the VTK writer.
        //@}
        static MutexType instance_mutex_;
        //**********************************************************************

        //**Friend declarations*************************************************
        /*! \cond PE_INTERNAL */
        friend bool is_active();
        // TODO : NamedTypes here instead of just types
        friend std::shared_ptr<Writer> make_writer(std::string const& filename,
                                                   std::size_t spacing,
                                                   std::size_t tstart,
                                                   std::size_t tend);
        /*! \endcond */
      };

      inline bool is_active();
      std::shared_ptr<Writer> make_writer(std::string const& filename,
                                          std::size_t spacing,
                                          std::size_t tstart, std::size_t tend);

      inline bool is_active() { return Writer::active_; }

    }  // namespace csv

  }  // namespace io

}  // namespace elastica