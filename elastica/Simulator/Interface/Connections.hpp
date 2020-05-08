#pragma once
#include <functional>
#include <memory>
#include <sstream>
#include <vector>

#include "elastica/Simulator/Joints.hpp"
#include "elastica/Utilities/NamedType.hpp"

namespace elastica {

  namespace interface {

    template <class Sim,  // Simulator class, derived from this class using CRTP
              class SimTraits>  // Simulator options
    struct Connections {
      // TODO : mark protected
      Connections() = default;
      ~Connections() = default;

      using SizeType = typename SimTraits::SizeType;
      using IndexType = typename SimTraits::IndexType;
      using TimeType = typename SimTraits::TimeType;
      template <typename T>
      using SystemInterfaceType =
          typename SimTraits::template SystemInterfaceType<T>;

      using JointID = std::unique_ptr<joints::JointBase>;

      struct PackedConnectionIndex_ {
        IndexType idx_first_sys_, idx_second_sys_;
        IndexType idx_first_sys_connection_, idx_second_sys_connection_;
        PackedConnectionIndex_(IndexType idx_first, IndexType idx_second)
            : idx_first_sys_(idx_first),
              idx_second_sys_(idx_second),
              idx_first_sys_connection_(std::numeric_limits<IndexType>::max()),
              idx_second_sys_connection_(
                  std::numeric_limits<IndexType>::max()){};
        ~PackedConnectionIndex_() = default;

        inline IndexType first_system_index() noexcept {
          return idx_first_sys_;
        };
        inline IndexType second_system_index() noexcept {
          return idx_second_sys_;
        };
        inline IndexType& first_system_connection_idx() noexcept {
          return idx_first_sys_connection_;
        };
        inline const IndexType& first_system_connection_idx() const noexcept {
          return idx_first_sys_connection_;
        };
        inline IndexType& second_system_connection_idx() noexcept {
          return idx_second_sys_connection_;
        };
        inline const IndexType& second_system_connection_idx() const noexcept {
          return idx_second_sys_connection_;
        };
      };

      /*
      struct FirstSystemIdx {
        IndexType idx_;
        // Not marked explicit by design
        FirstSystemIdx(IndexType id) : idx_(id){};
        IndexType id() { return idx_; };
        ~FirstSystemIdx();
      };
      struct SecondSystemIdx {
        IndexType idx_;
        // Not marked explicit by design
        SecondSystemIdx(IndexType id) : idx_(id){};
        IndexType id() { return idx_; };
        ~SecondSystemIdx();
      };
      */
      using FirstSystemIdx =
          utilities::NamedType<IndexType, struct FirstSystemIndexTag_>;
      using SecondSystemIdx =
          utilities::NamedType<IndexType, struct SecondSystemIndexTag_>;

      struct ConnectImpl_ {
        PackedConnectionIndex_ connectivity_info_;
        SizeType dofs_first_, dofs_second_;
        std::function<JointID()> f_;

        ConnectImpl_(IndexType idx_first, IndexType idx_second,
                     SizeType dofs_first, SizeType dofs_second)
            : connectivity_info_(idx_first, idx_second),
              // dofs_first_(std::numeric_limits<SizeType>::max()),
              // dofs_second_(std::numeric_limits<SizeType>::max()){};
              dofs_first_(dofs_first),
              dofs_second_(dofs_second){};
        ~ConnectImpl_() = default;

        // IndexType id() { return idx_o; };

        // DONE Make it slightly better in terms of having a struct that takes
        // First and Second instead of sizetype directly.
        // void at(SizeType i_dof, SizeType j_dof) {
        void at(FirstSystemIdx i_dof, SecondSystemIdx j_dof) {
          // Templates are too much trouble for specializing in a deeply
          // nested scope. Swtiching to use explicit functions instead.
          // check_limits<0UL>(i_dof);
          // check_limits<1UL>(j_dof);
          IndexType i_dof_id = i_dof.get();
          IndexType j_dof_id = j_dof.get();
          check_system_limits(i_dof_id, dofs_first_,
                              connectivity_info_.first_system_index());
          check_system_limits(j_dof_id, dofs_second_,
                              connectivity_info_.second_system_index());

          connectivity_info_.first_system_connection_idx() = i_dof_id;
          connectivity_info_.second_system_connection_idx() = j_dof_id;
        }

        // Overloads that delegate
        void at(SecondSystemIdx j_dof, FirstSystemIdx i_dof) {
          at(i_dof, j_dof);
        }
        void at(IndexType first_sys_dof, IndexType second_sys_dof) {
          at(FirstSystemIdx(first_sys_dof), SecondSystemIdx(second_sys_dof));
        }

        // template <std::size_t> void check_limits(SizeType idx);
        // TODO Make private
        void check_system_limits(SizeType idof, SizeType ndofs,
                                 IndexType sys_idx) {
          if (idof > ndofs) {
            std::ostringstream s_stream;
            s_stream << "Attempts to connect position at index " << idof
                     << " for system number " << sys_idx
                     << " fails as the number of degrees of freedom is only "
                     << ndofs << " in the connection between system number "
                     << connectivity_info_.first_system_index() << "and "
                     << connectivity_info_.second_system_index();
            throw std::range_error(s_stream.str());
          }
        }

        // TODO Make private
        template <typename JointType, typename... Args,
                  typename std::enable_if_t<
                      std::is_constructible<JointType, Args...>{}, int> = 0>
        ConnectImpl_& with_impl(Args&&... args) {
          f_ = [=] { return std::make_unique<JointType>(std::move(args)...); };
          return *this;
        }

        // Make it private! Nice solution to inaccessible compile time
        // construction
        // TODO Make private
        // impossible to construct
        template <typename JointType, typename... Args,
                  typename std::enable_if_t<
                      !std::is_constructible<JointType, Args...>{}, int> = 1>
        ConnectImpl_& with_impl(Args&&...) {
          // TODO : Needs more explanations (which boundary condition? which
          // line etc). Do it in the error handling library
          throw std::runtime_error(
              "Cannot create a valid joint with given args");
          f_ = [=] { return nullptr; };
          return *this;
        }

        template <typename JointType, typename... Args>
        auto with(Args&&... args) {
          // Does the same check before to print a useful compile-time message
          static_assert(std::is_constructible<JointType, Args...>::value,
                        "Cannot create a valid joint with given args");
          // DONE : Add static assert to see if JointType derives from DBCBase
          // for type-safety
          static_assert(std::is_base_of<joints::JointBase, JointType>::value,
                        "Given joint does not satisfy interface requirements, "
                        "did you forget to derive from JointBase?");
          return with_impl<JointType>(std::forward<Args>(args)...);
        };

        template <typename... Args>
        auto with(const std::string& connection_type, Args&&... args) {
          if (connection_type == "FixedJoint")
            return with_impl<joints::FixedJoint>(std::forward<Args>(args)...);
          else if (connection_type == "HingeJoint")
            return with_impl<joints::HingeJoint>(std::forward<Args>(args)...);
          else {
            std::ostringstream s_stream;
            s_stream
                << connection_type << "is an unknown/invalid joint. "
                << "If you are trying to connect with a custom joint type, "
                   "see documentation about extending the Connect add-on of "
                   "the simulator.";
            throw std::runtime_error(s_stream.str());
          }
        };
      };

      using ConnectionID = std::unique_ptr<ConnectImpl_>;

      template <typename SystemOne, typename SystemTwo>
      ConnectImpl_& connect(SystemInterfaceType<SystemOne> first_system,
                            SystemInterfaceType<SystemTwo> second_system) {
        // check if systems is in the list
        auto first_idx = static_cast<Sim*>(this)->check(first_system);
        auto second_idx = static_cast<Sim*>(this)->check(second_system);

        auto first_n_dofs = static_cast<Sim*>(this)->dofs(first_idx);
        auto second_n_dofs = static_cast<Sim*>(this)->dofs(second_idx);

        // if so, good create a constraint object, append and return to user
        // ConstraintType constraint_ = std::make_unique<ConstraintImpl>(idx);
        connections_.push_back(std::make_unique<ConnectImpl_>(
            first_idx, second_idx, first_n_dofs, second_n_dofs));
        // connections_.back().set_limits();
        return *(connections_.back());
      }

      // TODO Make private
      void finalize(TimeType time_v) {
        std::vector<std::reference_wrapper<ConnectionID>> faulty_connections;
        std::for_each(connections_.begin(), connections_.end(),
                      [&faulty_connections](ConnectionID& connection) {
                        if (not connection->f_)
                          faulty_connections.push_back(std::ref(connection));
                      });

        // Prune list of faulty constraints
        // TODO Have tags for each rod so that its easier to detect which
        // rod/constrain has errors. Even a raw pointer would be fine at this
        // stage
        if (!faulty_connections.empty()) {
          std::ostringstream s_stream;
          s_stream << "Faults detected in the following connections!.\n"
                   << "They are listed by the system connectionids below \n";

          std::for_each(faulty_connections.begin(), faulty_connections.end(),
                        [&s_stream](auto connection) {
                          auto cinfo = connection.get()->connectivity_info_;
                          s_stream << "\t" << cinfo.first_system_index() << "--"
                                   << cinfo.second_system_index() << "\n";
                        });

          s_stream << "\n Did you forget to call the `with()` method while "
                      "attempting "
                      "to connect?";
          throw std::runtime_error(s_stream.str());
        }

        std::transform(
            connections_.begin(), connections_.end(),
            std::back_inserter(joints_),
            [](ConnectionID& connection) { return connection->f_(); });
        std::transform(connections_.begin(), connections_.end(),
                       std::back_inserter(connection_idx_),
                       [](ConnectionID& connection) {
                         return connection->connectivity_info_;
                       });
      }

      std::vector<ConnectionID> connections_;
      // TODO Have a vector of tagged tuple here instead of just vector of
      // structs The tagged tuple needs to be TaggedTuple<FirstSystem,
      // SecondSystem, FirstConnectionIndex, SecondConnectionIndex> The first
      // and second system can be a direct pointer to the system The indices are
      // just indices
      std::vector<PackedConnectionIndex_> connection_idx_;
      std::vector<JointID> joints_;
    };

  }  // namespace interface
}  // namespace elastica
