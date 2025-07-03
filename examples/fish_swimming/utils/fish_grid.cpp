#include <array>
#include <cmath>
#include <iostream>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>

#define PI 3.14159265358979323846

namespace py = pybind11;

std::array<double, 3> matvec(double a11, double a12, double a13, double a21,
                             double a22, double a23, double a31, double a32,
                             double a33, const std::array<double, 3>& vec) {
  return {a11 * vec[0] + a12 * vec[1] + a13 * vec[2],
          a21 * vec[0] + a22 * vec[1] + a23 * vec[2],
          a31 * vec[0] + a32 * vec[1] + a33 * vec[2]};
}

std::array<double, 3> cross(const std::array<double, 3>& a,
                            const std::array<double, 3>& b) {
  return {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
          a[0] * b[1] - a[1] * b[0]};
}

struct FishGridHelper {
  FishGridHelper(py::list positions, py::list velocities, py::list directors,
                 py::list omegas, py::size_t max_surface_grid_density,
                 py::array_t<double> widths, py::array_t<double> heights)
      : num_fish_(positions.size()),
        max_surface_grid_density_(max_surface_grid_density) {
    /* We assume all fish have the same initial geometry */
    for (const auto& pos : positions)
      positions_.emplace_back(pos.cast<py::array_t<double>>().unchecked<2>());
    for (const auto& vel : velocities)
      velocities_.emplace_back(vel.cast<py::array_t<double>>().unchecked<2>());
    for (const auto& dir : directors)
      directors_.emplace_back(dir.cast<py::array_t<double>>().unchecked<3>());
    for (const auto& omega : omegas)
      omegas_.emplace_back(omega.cast<py::array_t<double>>().unchecked<2>());
    num_elements_ = omegas_[0].shape(1);

    /* Compute number of lagrangian nodes */
    auto widths_unchecked = widths.unchecked<1>();
    auto heights_unchecked = heights.unchecked<1>();
    std::vector<double> circumference(num_elements_);
    double max_circumference = 0.0;
    for (unsigned i = 0; i < num_elements_; ++i) {
      circumference[i] =
          PI * (3 * (widths_unchecked[i] + heights_unchecked[i]) -
                std::sqrt((3 * widths_unchecked[i] + heights_unchecked[i]) *
                          (3 * heights_unchecked[i] + widths_unchecked[i])));
      max_circumference = std::max(max_circumference, circumference[i]);
    }
    max_grid_size_ = max_circumference / max_surface_grid_density_;

    std::vector<int> surface_grid_density(num_elements_, 0);
    start_index_.reserve(num_elements_ + 1);
    start_index_.push_back(0);
    for (unsigned i = 0; i < num_elements_; ++i) {
      surface_grid_density[i] = std::round(
          circumference[i] * max_surface_grid_density_ / max_circumference);
      // If there are less than 1 point then set it equal to 1 since we will
      // place it on the element center.
      if (surface_grid_density[i] < 3)
        surface_grid_density[i] = 1;

      num_lag_nodes_ += surface_grid_density[i];
      start_index_.push_back(start_index_.back() + surface_grid_density[i]);
    }

    /* Compute the grid positions in the local frame. Note that
     * "heights"/"semi_minor_axis" is aligned with director 1, while
     * "widths"/"semi_major_axis" is aligned with director 2 */
    local_frame_surface_points_.reserve(num_lag_nodes_);
    for (unsigned i = 0; i < num_elements_; ++i) {
      // If the element has only one grid point, assign it to element center
      if (surface_grid_density[i] == 1) {
        local_frame_surface_points_.push_back({0, 0, 0});
        continue;
      }

      // Otherwise linspace distribution (no end point)
      double dt = 2 * PI / surface_grid_density[i];
      for (unsigned j = 0; j < surface_grid_density[i]; ++j) {
        local_frame_surface_points_.push_back(
            {heights_unchecked[i] * std::cos(j * dt),
             widths_unchecked[i] * std::sin(j * dt), 0.0});
      }
    }

    moment_arms_.reserve(num_fish_);
    for (unsigned fi = 0; fi < num_fish_; ++fi) {
      moment_arms_.push_back({num_lag_nodes_, {0, 0, 0}});
    }
  }

  py::size_t get_num_lag_nodes() const { return num_fish_ * num_lag_nodes_; }

  void set_element_length(double length) {
    max_grid_size_ = std::max(max_grid_size_, length);
  }

  void compute_lag_grid_position_field(py::array_t<double> lag_positions) {
    auto lag_pos = lag_positions.mutable_unchecked<2>();
    compute_moment_arms_();
    for (unsigned fi = 0; fi < num_fish_; ++fi) {  // for each fish
      unsigned start = fi * num_lag_nodes_;
      for (unsigned i = 0; i < num_elements_; ++i) {  // for each element
        std::array<double, 3> elem_pos = {
            0.5 * (positions_[fi](0, i) + positions_[fi](0, i + 1)),
            0.5 * (positions_[fi](1, i) + positions_[fi](1, i + 1)),
            0.5 * (positions_[fi](2, i) + positions_[fi](2, i + 1))};
        for (unsigned j = start_index_[i]; j < start_index_[i + 1]; ++j) {
          // for each lagrangian grid point
          lag_pos(0, start + j) = elem_pos[0] + moment_arms_[fi][j][0];
          lag_pos(1, start + j) = elem_pos[1] + moment_arms_[fi][j][1];
          lag_pos(2, start + j) = elem_pos[2] + moment_arms_[fi][j][2];
        }  // j
      }  // i
    }  // fi
  }

  void compute_lag_grid_velocity_field(py::array_t<double> lag_velocities) {
    auto lag_vel = lag_velocities.mutable_unchecked<2>();
    for (unsigned fi = 0; fi < num_fish_; ++fi) {  // for each fish
      unsigned start = fi * num_lag_nodes_;
      for (unsigned i = 0; i < num_elements_; ++i) {  // for each element
        std::array<double, 3> elem_vel = {
            0.5 * (velocities_[fi](0, i) + velocities_[fi](0, i + 1)),
            0.5 * (velocities_[fi](1, i) + velocities_[fi](1, i + 1)),
            0.5 * (velocities_[fi](2, i) + velocities_[fi](2, i + 1))};
        std::array<double, 3> elem_glob_omega =
            matvec(directors_[fi](0, 0, i), directors_[fi](1, 0, i),
                   directors_[fi](2, 0, i), directors_[fi](0, 1, i),
                   directors_[fi](1, 1, i), directors_[fi](2, 1, i),
                   directors_[fi](0, 2, i), directors_[fi](1, 2, i),
                   directors_[fi](2, 2, i),
                   {omegas_[fi](0, i), omegas_[fi](1, i), omegas_[fi](2, i)});
        for (unsigned j = start_index_[i]; j < start_index_[i + 1]; ++j) {
          // for each lagrangian grid point
          auto local_vel = cross(elem_glob_omega, moment_arms_[fi][j]);
          lag_vel(0, start + j) = elem_vel[0] + local_vel[0];
          lag_vel(1, start + j) = elem_vel[1] + local_vel[1];
          lag_vel(2, start + j) = elem_vel[2] + local_vel[2];
        }  // j
      }  // i
    }  // fi
  }

  void transfer_forcing_from_grid_to_body(
      py::array_t<double> body_flow_forces,
      py::array_t<double> body_flow_torques,
      py::array_t<double> lag_grid_forcing_field) {
    // Assume body_flow_forces and body_flow_torques are zeroed out
    auto body_force = body_flow_forces.mutable_unchecked<2>();
    auto body_torque = body_flow_torques.mutable_unchecked<2>();
    auto lag_forcing = lag_grid_forcing_field.unchecked<2>();
    for (unsigned fi = 0; fi < num_fish_; ++fi) {  // for each fish
      unsigned start = fi * num_lag_nodes_;
      unsigned start_elem = fi * num_elements_;
      unsigned start_node = fi * (num_elements_ + 1);

      for (unsigned i = 0; i < num_elements_; ++i) {  // for each element
        unsigned elem_index = start_elem + i;
        unsigned node_index = start_node + i;

        std::array<double, 3> body_force_on_elem{0.0, 0.0, 0.0};
        std::array<double, 3> body_torque_on_elem{0.0, 0.0, 0.0};
        for (unsigned j = start_index_[i]; j < start_index_[i + 1]; ++j) {
          // for each lagrangian grid point
          unsigned lag_index = start + j;
          body_force_on_elem[0] += lag_forcing(0, lag_index);
          body_force_on_elem[1] += lag_forcing(1, lag_index);
          body_force_on_elem[2] += lag_forcing(2, lag_index);

          auto local_torque =
              cross(moment_arms_[fi][j],
                    {-lag_forcing(0, lag_index), -lag_forcing(1, lag_index),
                     -lag_forcing(2, lag_index)});
          body_torque_on_elem[0] += local_torque[0];
          body_torque_on_elem[1] += local_torque[1];
          body_torque_on_elem[2] += local_torque[2];
        }  // j

        body_force(0, node_index) -= 0.5 * body_force_on_elem[0];
        body_force(1, node_index) -= 0.5 * body_force_on_elem[1];
        body_force(2, node_index) -= 0.5 * body_force_on_elem[2];

        body_force(0, node_index + 1) -= 0.5 * body_force_on_elem[0];
        body_force(1, node_index + 1) -= 0.5 * body_force_on_elem[1];
        body_force(2, node_index + 1) -= 0.5 * body_force_on_elem[2];

        body_torque_on_elem =
            matvec(directors_[fi](0, 0, i), directors_[fi](0, 1, i),
                   directors_[fi](0, 2, i), directors_[fi](1, 0, i),
                   directors_[fi](1, 1, i), directors_[fi](1, 2, i),
                   directors_[fi](2, 0, i), directors_[fi](2, 1, i),
                   directors_[fi](2, 2, i), body_torque_on_elem);
        body_torque(0, elem_index) = body_torque_on_elem[0];
        body_torque(1, elem_index) = body_torque_on_elem[1];
        body_torque(2, elem_index) = body_torque_on_elem[2];
      }  // i
    }  // fi
  }

  double get_maximum_lagrangian_grid_spacing() {
    // for (unsigned fi = 0; fi < num_fish_; ++fi) {
    //   auto lengths_unchecked =
    //       lengths[fi].cast<py::array_t<double>>().unchecked<1>();
    //   for (unsigned i = 0; i < num_elements_; ++i) {
    //     max_grid_size_ = std::max(max_grid_size_, lengths_unchecked[i]);
    //   }
    // }
    return max_grid_size_;
  }

 private:
  unsigned num_fish_, num_elements_, max_surface_grid_density_, num_lag_nodes_;
  double max_grid_size_;
  std::vector<py::detail::unchecked_reference<double, 2>> positions_;
  std::vector<py::detail::unchecked_reference<double, 2>> velocities_;
  std::vector<py::detail::unchecked_reference<double, 3>> directors_;
  std::vector<py::detail::unchecked_reference<double, 2>> omegas_;
  std::vector<std::array<double, 3>> local_frame_surface_points_;
  std::vector<std::vector<std::array<double, 3>>> moment_arms_;
  std::vector<size_t> start_index_;

  void compute_moment_arms_() {
    for (unsigned fi = 0; fi < num_fish_; ++fi) {
      for (unsigned i = 0; i < num_elements_; ++i) {
        for (unsigned j = start_index_[i]; j < start_index_[i + 1]; ++j) {
          moment_arms_[fi][j] =
              matvec(directors_[fi](0, 0, i), directors_[fi](1, 0, i),
                     directors_[fi](2, 0, i), directors_[fi](0, 1, i),
                     directors_[fi](1, 1, i), directors_[fi](2, 1, i),
                     directors_[fi](0, 2, i), directors_[fi](1, 2, i),
                     directors_[fi](2, 2, i), local_frame_surface_points_[j]);
        }  // j
      }  // i
    }  // fi
  }
};

PYBIND11_MODULE(_fish_grid_helper, m) {
  py::class_<FishGridHelper>(m, "FishGridHelper")
      .def(py::init<py::list, py::list, py::list, py::list, py::size_t,
                    py::array_t<double>, py::array_t<double>>())
      .def("get_num_lag_nodes", &FishGridHelper::get_num_lag_nodes)
      .def("set_element_length", &FishGridHelper::set_element_length)
      .def("compute_lag_grid_position_field",
           &FishGridHelper::compute_lag_grid_position_field)
      .def("compute_lag_grid_velocity_field",
           &FishGridHelper::compute_lag_grid_velocity_field)
      .def("transfer_forcing_from_grid_to_body",
           &FishGridHelper::transfer_forcing_from_grid_to_body)
      .def("get_maximum_lagrangian_grid_spacing",
           &FishGridHelper::get_maximum_lagrangian_grid_spacing);
}