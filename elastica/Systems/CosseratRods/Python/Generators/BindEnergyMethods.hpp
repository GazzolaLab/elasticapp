//==============================================================================
/*!
//  \file
//  \brief Defines python bindings for energy methods of CosseratRod
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
//
#include "Systems/CosseratRods/CosseratRods.hpp"
#include "Systems/CosseratRods/Utility.hpp"
//
#include <pybind11/pybind11.h>
//
#include <type_traits>

namespace py_bindings {

  namespace detail {

    template <typename ModelsBlock>
    void bind_energy_methods(pybind11::class_<ModelsBlock>& rod) {
      constexpr const char* translational_mode_doc = R"doc(
Computes translational mode energy of the cosserat_rod.
)doc";
      constexpr const char* rotational_mode_doc = R"doc(
Computes rotational mode energy of the cosserat_rod.
)doc";
      constexpr const char* shearing_mode_doc = R"doc(
Computes shearing mode energy of the cosserat_rod.
)doc";
      constexpr const char* bending_mode_doc = R"doc(
Computes bending mode energy of the cosserat_rod.
)doc";

      using Arg = std::remove_reference_t<decltype(rod)>;
      using type = typename Arg::type;

      rod.def(
          "compute_translational_energy",
          +[](type const& self) {
            return ::elastica::compute_translational_energy(self);
          },
          translational_mode_doc);
      rod.def(
          "compute_rotational_energy",
          +[](type const& self) {
            return ::elastica::compute_rotational_energy(self);
          },
          rotational_mode_doc);
      rod.def(
          "compute_shearing_energy",
          +[](type const& self) {
            return ::elastica::compute_shear_energy(self);
          },
          shearing_mode_doc);
      rod.def(
          "compute_bending_energy",
          +[](type const& self) {
            return ::elastica::compute_bend_energy(self);
          },
          bending_mode_doc);
    }

  }  // namespace detail

  //****************************************************************************
  /*!\brief Helps bind energy methods to a CosseratRod in Elastica++
   * \ingroup python_bindings
   *
   * \details
   * Adds methods to compute energies to the cosserat rod
   *
   * \param rod a Pybind11 class of a CosseratRod
   */
  template <typename CRT,
            template <typename /*CRT*/, typename /*InitializedBlock*/>
            class... Components>
  void bind_energy_methods(
      pybind11::class_<
          ::blocks::BlockSlice<::elastica::cosserat_rod::CosseratRodPlugin<
              CRT, ::blocks::BlockSlice, Components...>>>& rod) {
    detail::bind_energy_methods(rod);
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Helps bind energy methods to a CosseratRod in Elastica++
   * \ingroup python_bindings
   *
   * \details
   * Adds methods to compute energies to the cosserat rod
   *
   * \param rod a Pybind11 class of a CosseratRod
   */
  template <typename CRT, typename Tag,
            template <typename /*CRT*/, typename /*InitializedBlock*/>
            class... Components>
  void bind_energy_methods(
      pybind11::class_<::blocks::BlockSlice<
          ::elastica::cosserat_rod::TaggedCosseratRodPlugin<
              CRT, ::blocks::BlockSlice, Tag, Components...>>>& rod) {
    detail::bind_energy_methods(rod);
  }
  //****************************************************************************

}  // namespace py_bindings
