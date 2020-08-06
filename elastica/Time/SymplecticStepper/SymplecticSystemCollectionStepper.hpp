#pragma once
#include <cstddef>
#include <utility>
#include <vector>

// #include "SymplecticStepperKernel.hpp"
#include "elastica/Utilities/StdHelpers.hpp"
#include "elastica/Utilities/TMPL.hpp"

namespace timestepper {

  namespace symplectic {

    // template <template <std::size_t> class SymplecticStepper, std::size_t
    // Order>
    template <class SymplecticKernel>
    class SystemCollectionStepper {
     public:
      SymplecticKernel stepper;
      using StepperCategoryTag = typename SymplecticKernel::StepperCategoryTag;
      static constexpr std::size_t get_number_of_steps() noexcept {
        return decltype(stepper)::n_steps();
      }

     private:
      // DONE : Pass by reference only and not a copy
      template <std::size_t CurrentStep, class HomogenousSystemCollection>
      inline void do_kinematic_step(HomogenousSystemCollection& hsc,
                                    double time_v, double dt) {
        for (auto& system : hsc) {
          stepper.template kinematic_step<CurrentStep>(system, time_v, dt);
        }
      }

      template <std::size_t CurrentStep, class HomogenousSystemCollection>
      inline void do_dynamic_step(HomogenousSystemCollection& hsc,
                                  double time_v, double dt) {
        for (auto& system : hsc) {
          stepper.template dynamic_step<CurrentStep>(system, time_v, dt);
        }
      }

      template <std::size_t CurrentStep, typename SystemCollection>
      inline double kinematic_step(SystemCollection& system_collection,
                                   double time_v, double dt) {
        index_apply<SystemCollection::get_number_of_system_collections()>(
            [this, &system_collection, time_v, dt](auto... Is) {
              EXPAND_PACK_LEFT_TO_RIGHT(this->do_kinematic_step<CurrentStep>(
                  std::get<Is>(system_collection), time_v, dt));
            });
        time_v += stepper.template kinematic_prefactor<CurrentStep>() * dt;
        system_collection.constrain_primitive_values(time_v);
        return time_v;
      }

      template <std::size_t CurrentStep, typename SystemCollection>
      inline double dynamic_step(SystemCollection& system_collection,
                                 double time_v, double dt) {
        system_collection.resolve_effects(time_v);
        index_apply<SystemCollection::get_number_of_system_collections()>(
            [this, &system_collection, time_v, dt](auto... Is) {
              EXPAND_PACK_LEFT_TO_RIGHT(this->do_dynamic_step<CurrentStep>(
                  std::get<Is>(system_collection), time_v, dt));
            });
        system_collection.constrain_primitive_rates(time_v);
        return time_v;
      }

      // TODO : Is there a interface independent way of integrating the system
      //      because the calls to boundary conditions, environments etc clutter
      //      the meaning of code
      template <std::size_t CurrentStep, typename SystemCollection>
      void do_step_impl_single_pass_unused(SystemCollection& system_collection,
                                           double time_v, double dt) {
        // Is this the best way to do it?
        //    for (auto &system : system_collection) {
        //      stepper.template kinematic_step<CurrentStep>(system, time_v,
        //      dt);
        //    }

        // TMPL For each
        // TODO : Parallel

        // Was earlier system_collection.get_number_of_system_collections(),
        // but clang complains about it not being constexpr (although it is)
        // so made it static constexpr and use it here
        index_apply<SystemCollection::get_number_of_system_collections()>(
            [this, &system_collection, time_v, dt](auto... Is) {
              EXPAND_PACK_LEFT_TO_RIGHT(do_kinematic_step<CurrentStep>(
                  std::get<Is>(system_collection), time_v, dt));
              //              using expander = int[];
              //              (void)expander{
              //                  0,
              //                  ((void)(this->do_kinematic_step<CurrentStep>(
              //                       std::get<Is>(*system_collection), time_v,
              //                       dt)),
              //                   0)...};

              //          using expander = int[];
              //              (void)expander {
              //                0, ((void)(auto& system_collection_vector =
              //                               std::get<Is>(system_collection)
              ////                           for (auto& system
              ////                                : system_collection_vector) {
              ////                             stepper.template
              /// kinematic_step<CurrentStep>( / system, time_v, dt);
              //                           }
              //                           ),
              //                    0)...
              //              };
            });

        /*
    for (std::size_t i_collection(0UL);
         i_collection <
         system_collection.get_number_of_system_collections();
         ++i_collection) {
      auto& system_collection_vector =
          std::get<i_collection>(system_collection);
      for (auto& system : system_collection_vector) {
        stepper.template kinematic_step<CurrentStep>(system, time_v, dt);
      }
    }
         */

        time_v += stepper.template kinematic_prefactor<CurrentStep>() * dt;
        // Apply boundary conditions that constrain solely the primitives
        // that is (r, Q)
        system_collection.constrain_primitive_values(time_v);
        // time += stepper.template;

        // Note : There's an inefficiency here.
        // In the last step, there's never a dynamic step applied
        // for any symplectic algorithm for preserving time-symmetry.
        // This implies that we can break the loop at this point and
        // the algorithm will still be the same.
        // So need not do unnecessary work here

        // Resolve environmental forces, connections etc.
        system_collection.resolve_effects(time_v);

        // For lack of a better term, we call it synchronize
        // it applies all external forces/torques, resolves joints
        // resolves contacts, controls etc.
        // system_collection.synchronize(time);

        //        for (auto& system : system_collection) {
        //          stepper.template dynamic_step<CurrentStep>(system, time_v,
        //          dt);
        //        }

        // TODO : Parallel
        // Was earlier system_collection.get_number_of_system_collections(),
        // but clang complains about it not being constexpr (although it is)
        // so made it static constexpr and use it here
        index_apply<SystemCollection::get_number_of_system_collections()>(
            [&system_collection, this, time_v, dt](auto... Is) {
              EXPAND_PACK_LEFT_TO_RIGHT(do_dynamic_step<CurrentStep>(
                  std::get<Is>(system_collection), time_v, dt));
              //              using expander = int[];
              //              (void)expander{
              //                  0, ((void)(this->do_dynamic_step<CurrentStep>(
              //                          std::get<Is>(*system_collection),
              //                          time_v, dt)),
              //                      0)...};
            });

        system_collection.constrain_primitive_rates(time_v);
      }

      //      template <typename SystemCollection, std::size_t... Steps>
      //      void do_step_impl(SystemCollection* system_collection, double
      //      time_v,
      //                        double dt, std::index_sequence<Steps...>) {
      //        using expander = int[];
      //
      //        (void)expander{0, ((void)do_step_impl_single_pass<Steps>(
      //                               system_collection, time_v, dt),
      //                           0)...};
      //      }

      template <std::size_t CurrentStep, typename SystemCollection>
      double do_step_impl_single_pass(SystemCollection& system_collection,
                                      double time_v, double dt) {
        time_v = kinematic_step<CurrentStep>(system_collection, time_v, dt);
        time_v = dynamic_step<CurrentStep>(system_collection, time_v, dt);
        return time_v;
      }

     public:
      SystemCollectionStepper() = default;
      ~SystemCollectionStepper() = default;

      // Look ma, no alias
      template <typename SystemCollection>
      inline void do_step(SystemCollection& system_collection, double time_v,
                          double dt) {
        //        std::cout << "Time at start" << time_v << " ";

        // For positoin verlet only, but see later
        // For step 0, do kinematic and dynamic
        index_apply<get_number_of_steps() - 1UL>(
            [this, &system_collection, &time_v, dt](auto... Is) {
              EXPAND_PACK_LEFT_TO_RIGHT(
                  (time_v = this->do_step_impl_single_pass<Is>(
                       system_collection, time_v, dt)));
            });

        //        std::cout << "Time in middle" << time_v << " ";

        // For step 1, do only kinematic
        index_apply<get_number_of_steps() - 1UL>([this, &system_collection,
                                                  &time_v, dt](auto... Is) {
          EXPAND_PACK_LEFT_TO_RIGHT(
              (time_v = this->kinematic_step<Is>(system_collection, time_v, dt))
          );
        });

        //        std::cout << "Time at end" << time_v << " ";

        // Correct
        //        index_apply<get_number_of_steps()>(
        //            [this, &system_collection, time_v, dt](auto... Is) {
        //              EXPAND_PACK_LEFT_TO_RIGHT(
        //                  do_step_impl_single_pass<Is>(system_collection,
        //                  time_v, dt));
        //            });

        //        do_step_impl(system_collection, time_v, dt,
        //                     std::make_index_sequence<get_number_of_steps()>());
      }
    };  // namespace symplectic
  }     // namespace symplectic

}  // namespace timestepper
