#pragma once

#include <algorithm>
#include <type_traits>

#include "ContactResolver.hpp"
#include "elastica/ErrorHandling/Assert.hpp"
#include "elastica/Parallel/blocked_range.hpp"
#include "elastica/Parallel/reduce.hpp"
#include "elastica/Utilities/DefineTypes.h"
#include "elastica/Utilities/Logging.hpp"
#include "elastica/Utilities/NonCopyable.hpp"

namespace elastica {

  namespace collision {

    namespace response {

      template <
          typename C,          // Type of the configuration
          typename U1 = void,  // First unused auxiliary template parameter
          typename U2 = void>  // Second unused auxiliary template parameter
      class DEMSolver : private NonCopyable {
       public:
        //**Type
        // definitions****************************************************************************
        using Config = C;  //!< Type of the configuration.
        using This =
            DEMSolver<C, U1, U2>;  //!< Type of this DEMSolver instance.
        using ContactID =
            typename Config::ContactID;  //!< Handle for a contact.
        using ConstContactID =
            typename Config::ConstContactID;  //!< Handle for a constant
                                              //!< contact.
        // TODO : should be default constructible
        // TODO : Traits in general
        using ForceModel = typename Config::ForceModel;

        //**********************************************************************************************

        //**Constructor*********************************************************************************
        /*!\name Constructor */
        //@{
        explicit DEMSolver() : force_model_(){};
        //@}
        //**********************************************************************************************

        //**Destructor**********************************************************************************
        /*!\name Destructor */
        //@{
        ~DEMSolver() = default;
        //@}
        //**********************************************************************************************

        //**Solver
        // functions****************************************************************************
        /*!\name Solver functions */
        //@{
        template <typename CC>
        inline real_t resolve_contacts(CC& contacts) noexcept;
        //@}
        //**********************************************************************************************

       private:
        ContactResolver<ForceModel> force_model_;
        static_assert(std::is_same<U1, void>::value,
                      "U1 in DEMSolver must be void");
        static_assert(std::is_same<U2, void>::value,
                      "U2 in DEMSolver must be void");
      };

      template <typename C,   // Type of the configuration
                typename U1,  // First unused auxiliary template parameter
                typename U2>  // Second unused auxiliary template parameter
      template <typename CC>
      inline real_t DEMSolver<C, U1, U2>::resolve_contacts(CC& contacts) noexcept {
        /* Need a transform reduce here, but we don't have C++17 support yet
        // So we do it the long way and mock TBB's API

        Can also do for each like so:

        std::for_each(contacts.begin(), contacts.end(), FM());
        return FM.overlap:

        Here FM is stateful and keeps track of max.
        */

        force_model_.set_max_overlap(0.0);

        // Reduction over all possible contacts to get maximum overlap
        // Can be used for diagnostics or for adjusting step size too

        // With a std vector a blocked range of iterator performs poorly
        // so prefer to do everything in terms of raw pointer instead
        //        using size_type = typename CC::size_type;
        using pointer = typename CC::pointer;

        // https://www.myphysicslab.com/engine2D/collision-en.html

        elastica::parallel::reduce(
            elastica::parallel::blocked_range<pointer>(
                contacts.data(), contacts.data() + contacts.size()),
            force_model_);
//        std::cout << " Max overlap " << force_model_.get_max_overlap();
        return force_model_.get_max_overlap();
      }

    }  // namespace response

  }  // namespace collision

}  // namespace elastica
