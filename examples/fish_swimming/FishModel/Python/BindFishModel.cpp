#include <cstddef>
#include <pybind11/pybind11.h>
#include <string>
#include <utility>
//
#include <pybind11/pybind11.h>
//
#include <elastica/PythonBindings/BoundChecks.hpp>
//
#include <elastica/Systems/Block/Block.hpp>  // slice()
#include <elastica/Systems/CosseratRods/CosseratRods.hpp>
#include <elastica/Systems/CosseratRods/Python/Generators/BindEnergyMethods.hpp>
#include <elastica/Systems/CosseratRods/Python/Generators/BindMemberAccess.hpp>
#include <elastica/Systems/CosseratRods/Python/Generators/BindPyElasticaInterface.hpp>
#include <elastica/Systems/common/Python/Generators/BindVariableLocator.hpp>
//
#include <elastica/Utilities/Math/Python/SliceHelpers.hpp>
#include <elastica/Utilities/Size.hpp>
#include <elastica/Utilities/TMPL.hpp>
//
#include <examples/fish_swimming/FishModel/Aliases.hpp>

namespace py = pybind11;

namespace py_bindings {
  void bind_fish_model(py::module& m) {  // NOLINT
    using UnwantedVariableTags =
        tmpl::list<::elastica::tags::_DummyElementVector,
                   ::elastica::tags::_DummyElementVector2,
                   ::elastica::tags::_DummyVoronoiVector>;

    {
      using type = ::fish::details::FishBlockSlice;

      // Wrapper for basic type operations
      auto py_cosserat_rod =
          py::class_<type>(m, "Fish")
              .def("__len__", [](const type& t) { return cpp17::size(t); })
              .def(
                  "__str__",
                  +[](const type& /*meta*/) { return std::string("Fish"); });
      bind_member_access<UnwantedVariableTags>(py_cosserat_rod);
      bind_variable_locator<UnwantedVariableTags>(py_cosserat_rod);
      bind_pyelastica_interface<UnwantedVariableTags>(py_cosserat_rod);
      //   bind_energy_methods(py_cosserat_rod);
    }
    {
      using type = ::fish::details::FishBlockView;

      // Wrapper for basic type operations
      auto py_cosserat_rod =
          py::class_<type>(m, "FishView")
              .def("__len__", [](const type& t) { return cpp17::size(t); })
              .def(
                  "__str__",
                  +[](const type& /*meta*/) { return std::string("FishView"); })
              .def(
                  "__getitem__",
                  +[](type& t, std::size_t index) {
                    variable_bounds_check(::blocks::n_units(t), index);
                    return ::blocks::slice(t, index);
                  })
              .def(
                  "__getitem__", +[](type& t, const py::slice slice) {
                    auto v = check_slice<0UL>(::blocks::n_units(t),
                                              std::move(slice));
                    return ::blocks::slice(t, v.start, v.start + v.slicelength);
                  });
      bind_member_access<UnwantedVariableTags>(py_cosserat_rod);
      bind_variable_locator<UnwantedVariableTags>(py_cosserat_rod);
      bind_pyelastica_interface<UnwantedVariableTags>(py_cosserat_rod);
    }
  }
}  // namespace py_bindings

PYBIND11_MODULE(_PyFishModel, m) {  // NOLINT
  m.doc() = R"pbdoc(
    Bindings for Elastica++ fish model
    )pbdoc";
  // Experimental : what are the drawbacks
  py::module::import("elasticapp.Arrays");
  py::module::import("elasticapp.Systems.Tags");
  //
  py_bindings::bind_fish_model(m);
}