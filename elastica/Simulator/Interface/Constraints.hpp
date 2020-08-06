#pragma once
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "elastica/Simulator/BoundaryConditions.hpp"
#include "elastica/Utilities/Logging.hpp"

namespace elastica {

  namespace interface {

    // TODO : Replace idx in constraints by pointer to appropriate system type
    // This also involves detecting how many "types" of systems there are in the
    // case file (say rods, rigid-bodies) etc. and then
    // 1. create storage for these systems on the fly.
    // 2. Default to specific storage for a generic script parsing a YAML file

    template <class Sim,  // Simulator class, derived from this class using CRTP
              class SimTraits>  // Simulator options
    struct Constraints {
      using SizeType = typename SimTraits::SizeType;
      using IndexType = typename SimTraits::IndexType;
      using TimeType = typename SimTraits::TimeType;
      template <typename T>
      using SystemInterfaceType =
          typename SimTraits::template SystemInterfaceType<T>;

      // struct ConstraintImplBase_ {};
      using BoundaryConditionID =
          std::unique_ptr<boundary_conditions::BoundaryConditionBase>;

      //=================================================================================================
      //
      //  CONSTRUCTOR
      //
      //=================================================================================================

      //*************************************************************************************************
      /*!\brief Constructor of the Constraints class.
       */
      // TODO : mark protected
      Constraints(){
        LOG_INFO_SECTION(log) {
          log << "Successfully initialized the constraint instance";
        }
      };
      ~Constraints(){
        LOG_INFO_SECTION(log) {
            log << "Successfully destroyed the constraint instance";
          }
      };
      Constraints(Constraints const &) = delete;
      Constraints& operator= (Constraints const&) = delete;
      Constraints(Constraints &&) = delete;
      Constraints& operator= (Constraints &&) = delete;
      //*************************************************************************************************

      // template <class System>
      struct ConstraintImpl_
      // : public ConstraintImplBase_
      {
        IndexType idx_;
        std::vector<IndexType> position_indices_;
        std::vector<IndexType> director_indices_;
        std::function<BoundaryConditionID()> f_;

        explicit ConstraintImpl_(IndexType idx) : idx_(idx), f_(nullptr){};
        // ConstraintImpl() = default;
        ~ConstraintImpl_() = default;
        ConstraintImpl_(ConstraintImpl_ const& other) = delete;
        ConstraintImpl_& operator=(ConstraintImpl_ const&) = delete;
        ConstraintImpl_(ConstraintImpl_&& other) = delete;
        ConstraintImpl_& operator=(ConstraintImpl_&&) = delete;

        IndexType id() { return idx_; };

        // Benefits
        // --------
        // runtime construction of objects with compile-time code
        // checking

        // enable if with non-type template parameter to allow overloads
        // If possible to construct
        // TODO Make private
        template <typename BCType, typename... Args,
                  typename std::enable_if_t<
                      std::is_constructible<BCType, Args...>{}, int> = 0>
        ConstraintImpl_& with_impl(Args&&... args) {
          f_ = [=] { return std::make_unique<BCType>(std::move(args)...); };
          return *this;
        }

        // Make it private! Nice solution to inaccessible compile time
        // construction
        // TODO Make private
        // impossible to construct
        template <typename BCType, typename... Args,
                  typename std::enable_if_t<
                      !std::is_constructible<BCType, Args...>{}, int> = 1>
        ConstraintImpl_& with_impl(Args&&...) {
          // TODO : Needs more explanations (which boundary condition? which
          // line etc). Do it in the error handling library
          throw std::runtime_error(
              "Cannot create boundary condition with given args");
          f_ = [=] { return nullptr; };
          return *this;
        }

        // DONE overloaded with non-template type BCType for runtime code
        // This should show up in the interface
        template <typename BCType, typename... Args>
        auto with(Args&&... args) {
          // Does the same check before to print a useful compile-time message
          static_assert(
              std::is_constructible<BCType, Args...>::value,
              "Cannot create boundary conditions with given arguments");
          // DONE : Add static assert to see if BCType derives from DBCBase for
          // type-safety
          static_assert(
              std::is_base_of<boundary_conditions::BoundaryConditionBase,
                              BCType>::value,
              "Given boundary condition does not satisfy interface "
              "requirements, "
              "did you forget to derive from BoundaryConditionBase?");
          return with_impl<BCType>(std::forward<Args>(args)...);
        }

        // By default, only library boundary conditions can be specified as
        // options. This makes sense as if there are custom boundary conditions,
        // the user needs to write code for that, which would then be called
        // with the templated with method and not the with(std::string, args...)
        // method

        // In case extension of this method is needed (to specify custom
        // boundary conditions in runtime ), this class can be derived from.
        // Then the stringized method can be overriden in the derived class as
        // follows
        /// \code
        // template <typename... Args>
        // auto with(std::string bc_type, Args &&... args) {
        //   try {
        //     ConstraintImpl_::with(bc_type, std::forward<Args>(args)...);
        //   } catch (const std::runtime_error &e) {
        //     if (bc_type == "FreeBoundaryCondition")
        //       return
        //       with_impl<FreeBoundaryCondition>(std::forward<Args>(args)...);
        //     else
        //       throw;
        //   }
        // }
        ///
        template <typename... Args>
        auto with(const std::string& bc_type, Args&&... args) {
          if (bc_type == "FixedBoundaryCondition")
            return with_impl<boundary_conditions::FixedBoundaryCondition>(
                std::forward<Args>(args)...);
          else if (bc_type == "HingedBoundaryCondition")
            return with_impl<boundary_conditions::HingedBoundaryCondition>(
                std::forward<Args>(args)...);
          else {
            std::ostringstream s_stream;
            s_stream
                << bc_type << "is not a valid boundary condition"
                << "If you are trying to constrain using a custom boundary "
                   "condition, "
                   "consult documentation about extending the Constraints "
                   "add-on "
                   "of the simulator.";
            throw std::runtime_error(s_stream.str());
          }
        }

        // Generate list of indices here
        // Only run-time vectors of positions and director indices
        // compile-time list using IndexSequences possible, but
        // offers no advantages as all CosseratRods are run-time
        // entities
        // TODO Replace std::vector with a struct/typedef called Position
        // and DirectorIndices for explicit names
        // TODO Create an overload taking in a tagged tuple of p_idx and d_idx
        // Seems more complicated to do as we need to store the tuple somewhere
        // Maybe work with pointers to tuples instead? Can also create lambdas
        // that can return tags of the tuple which can then be forwarded to the
        // appropriate set method of the boundary condition
        void at(std::vector<IndexType>&& position_indices,
                std::vector<IndexType>&& director_indices) {
          // Runtime check with degrees of freedom
          auto ndofs = static_cast<Sim*>(this)->dofs(idx_);
          for (auto p_idx : position_indices) {
            if (p_idx > ndofs) {
              std::ostringstream s_stream;
              s_stream << "Attempts to constrain position at index " << p_idx
                       << " for system number " << idx_
                       << " fails as the number of degrees of freedom is only "
                       << ndofs;
              throw std::range_error(s_stream.str());
            }
          }
          for (auto d_idx : director_indices) {
            if (d_idx > ndofs) {
              std::ostringstream s_stream;
              s_stream << "Attempts to constrain directors at index " << d_idx
                       << " for system number " << idx_
                       << " fails as the number of degrees of freedom is only "
                       << ndofs;
              throw std::range_error(s_stream.str());
            }
          }
          position_indices_ = position_indices;
          director_indices_ = director_indices;
        }

        // Given the BC upon construction and the system, pass
        // on copies of system's positions and directors to the BC
        template <class System>
        void finalize(BoundaryConditionID& bc, System* /*system*/) {
          // Creates a new buffer of vectors everytime, seems wasteful
          std::vector<double> positions, velocities, omegas;
          std::vector<double> directors;
          std::size_t n_positions = position_indices_.size();
//          std::size_t n_directors = director_indices_.size();

          positions.reserve(n_positions);
          velocities.reserve(n_positions);

          /*
      std::transform(position_indices_.begin(), position_indices_.end(),
                     std::back_inserter(positions),
                     [&system](IndexType pos_idx) {
                       return system->position_collection[pos_idx];
                     });
      std::transform(position_indices_.begin(), position_indices_.end(),
                     std::back_inserter(velocities),
                     [&system](IndexType pos_idx) {
                       return system->velocity_collection[pos_idx];
                     });

      directors.reserve(n_directors);
      omegas.reserve(n_directors);

      std::transform(director_indices_.begin(), director_indices_.end(),
                     std::back_inserter(directors),
                     [&system](IndexType dir_idx) {
                       return system->director_collection[dir_idx];
                     });
      std::transform(director_indices_.begin(), director_indices_.end(),
                     std::back_inserter(omegas), [&system](IndexType dir_idx) {
                       return system->omega_collection[dir_idx];
                     });
          */

          bc->set(positions, velocities, directors, omegas);
        }
      };

      using ConstraintID = std::unique_ptr<ConstraintImpl_>;

      template <typename System>
      ConstraintImpl_& constrain(const SystemInterfaceType<System>& system) {
        // check if systems is in the list
        auto sys_idx = static_cast<Sim*>(this)->check(system);

        // if so, good create a constraint object, append and return to user
        // ConstraintType constraint_ = std::make_unique<ConstraintImpl>(idx);
        constraints_.push_back(std::make_unique<ConstraintImpl_>(sys_idx));
        return *(constraints_.back());
      }

      // TODO Make private
      void finalize(TimeType time_v) {
        // Here we should check if f_ is empty
        // This happens when people do
        // e = rod()
        // sim.push_back(e)
        // sim.constrain(e)
        // !! Notice that they just attempt to constrain and do not constrain
        // the rod The simulator thinks they have constrained, and tries to
        // access the functor f_ unfortunately its a nullptr which upon
        // dereferencing throws an error
        std::vector<std::reference_wrapper<ConstraintID>> faulty_constraints;
        std::for_each(constraints_.begin(), constraints_.end(),
                      [&faulty_constraints](ConstraintID& constraint) {
                        if (not constraint->f_)
                          faulty_constraints.push_back(std::ref(constraint));
                      });
        // Prune list of faulty constraints
        // TODO Have tags for each rod so that its easier to detect which
        // rod/constrain has errors. Even a raw pointer would be fine at this
        // stage
        if (!faulty_constraints.empty()) {
          std::ostringstream s_stream;
          s_stream << "Faults detected in the following constraints!.\n"
                   << "They are listed by the system ids below \n";

          std::for_each(faulty_constraints.begin(), faulty_constraints.end(),
                        [&s_stream](auto constraint) {
                          s_stream << "\t" << constraint.get()->id() << "\n";
                        });

          s_stream << "\n Did you forget to call the `with()` method while "
                      "attempting "
                      "to constrain?";
          throw std::runtime_error(s_stream.str());
        }

        std::transform(constraints_.begin(), constraints_.end(),
                       std::back_inserter(boundary_conditions_),
                       [](const ConstraintID& constraint) {
                         return std::move(constraint->f_());
                       });

        // Iterate over the position and velocity indices and pass on copies to
        // the boundary conditions
        std::size_t n_constraints(constraints_.size());
        for (std::size_t i_constraint = 0UL; i_constraint < n_constraints;
             ++i_constraint) {
          IndexType sys_idx = constraints_[i_constraint]->id();
          constraints_[i_constraint]->finalize(
              boundary_conditions_[i_constraint],
              static_cast<Sim*>(this)->operator[](sys_idx));
        }

        constrain_primitive_values(time_v);
        constrain_primitive_rates(time_v);
      }

      void constrain_primitive_values(double /*time_v*/) {
        std::cout << "Constraining primitive values\n";
      };
      void constrain_primitive_rates(double /*time_v*/) {
        std::cout << "Constraining primitive rates\n";
      };

      // private:
      IndexType constraint_id_;
      std::vector<ConstraintID> constraints_;
      std::vector<BoundaryConditionID> boundary_conditions_;
    };

  }  // namespace interface

}  // namespace elastica
