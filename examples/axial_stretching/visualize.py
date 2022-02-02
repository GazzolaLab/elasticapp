"""
==========================================================================
Brief tutorial for setting up python script to run Py-binded case studies.
==========================================================================

The purpose of this tutorial is to demonstrate setting up a python script
to run a Py-binded case study. Before running this script, in order to
generate and install the py-binded case studies, follow the iterations in
given in /docs/README_python.md
"""

# 1. Import relevant Python libraries.
import os

import numpy as np
from matplotlib import pyplot as plt
from tqdm import tqdm

# 2. Import the Python-binded case study bindings as modules. Please note
# the following:
# 2.1 Blaze needs to be imported first before any case study can be
# imported.
# 2.2 All case studies registered with the CMake case-study helper sit in
# elasticapp.CaseStudies.
from elasticapp import Blaze  # noqa
from elasticapp.CaseStudies.PyAxialStretchingCaseStudy import AxialStretchingCaseStudy


def run_example(
    pyelastica_file: os.PathLike = "",
    save_to_csv: bool = False,
    save_figure: bool = False,
):
    # 3. Define the input parameters to the case study.
    sample_interval = 200

    n_elem = 49
    base_radius = 0.025
    base_area = np.pi * base_radius ** 2

    base_length = 1.0
    nu = 20.0
    youngs_modulus = 1e4
    end_force_x = 1.0
    dt = 0.01 * (base_length / n_elem)
    final_time = 50.0
    time_interval = sample_interval * dt
    n_iterations = int(final_time / time_interval)

    # 4. Generate the Py-binded case study object. For a pythonic way
    # of looking at the help for the case study, one can do
    # pythonic help(AxialStretchingCaseStudy) or even
    # AxialStretchingCaseStudy.help().
    # For C++ implementation details refer to:
    # elastica/examples/axial_stretching/AxialStretchingCaseStudy.cpp
    cs = AxialStretchingCaseStudy(
        n_elements=n_elem,
        damping=nu,
        youngs_modulus=youngs_modulus,
        applied_force=end_force_x,
        radius=base_radius,
        length=base_length,
        dt=dt,
    )

    times = []
    iterations = []
    tip_displacements = []

    # 5. Run the case study. Different ways to run the case study are
    # 5.1 `cs.run()` : Runs the simulator till the user-defined criterion.
    # 5.2 `cs.iteration()` : Steps forward the simulator one iteration.
    # 5.3 `cs.run_for(additional_time=time_interval)` : Runs the simulator
    # for additional "time_interval" seconds.
    # 5.4 `cs.run_till(final_time)` : Runs the simulator till given
    # "final_time" seconds.
    for iteration in tqdm(range(n_iterations)):
        cs.run_for(additional_time=time_interval)
        # cs.iteration()

        # Certain useful methods while running a simulation are:
        # 5.5 `cs.current_time()` : Gets the current time of the simulation.
        # 5.6 `cs.current_dt()` : Gets the current timeiteration of the simulation.
        # 5.7 `cs.current_iteration()` : Gets the current iteration of the
        # simulation.
        curr_time = cs.current_time()
        curr_iteration = cs.current_iteration()

        # 5.8 `cs.communicate()` : Gets the system properties marked in the
        # function of the same name, defined in AxialStretchingCaseStudy.hpp
        curr_positions = cs.communicate()
        times.append(curr_time)
        iterations.append(curr_iteration)

        # 6. To use your numpy-skills, just use np.asarray()! Memory is not
        # allocated, rather it uses the memory in the C++ buffer. So changes
        # are reflected on the C++ side too!
        tip_displacements.append(np.asarray(curr_positions)[0, -1])

    # 7. Postprocess and visualize the results.
    # First-order theory with base-length
    expected_tip_disp = end_force_x * base_length / base_area / youngs_modulus
    # First-order theory with modified-length, gives better estimates
    expected_tip_disp_improved = (
        end_force_x * base_length / (base_area * youngs_modulus - end_force_x)
    )

    times = np.asarray(times)
    tip_displacements = np.asarray(tip_displacements)
    iterations = np.asarray(iterations)

    avg_tip_displacement = np.mean(tip_displacements[-200:])

    fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)
    ax = fig.add_subplot(111)
    ax.plot(times, tip_displacements, lw=2.0)
    ax.hlines(base_length + expected_tip_disp, 0.0, final_time, "k", "dashdot", lw=1.0)
    ax.hlines(
        base_length + expected_tip_disp_improved, 0.0, final_time, "k", "dashed", lw=2.0
    )
    ax.hlines(avg_tip_displacement, 0.0, final_time, "r", lw=0.5)

    if os.path.exists(pyelastica_file):
        pyelastica_results = np.loadtxt(pyelastica_file, delimiter=",")
        ax.plot(pyelastica_results[:, 0], pyelastica_results[:, 1], "r-")

    # print(tip_displacements)
    if save_to_csv:
        np.savetxt(
            "tip_displacements.csv",
            np.c_[np.asarray(times), np.asarray(tip_displacements)],
            delimiter=",",
        )
    if save_figure:
        fig.savefig("axial_stretching.pdf")

    plt.show()


if __name__ == "__main__":
    eg_pyelastica_file = "/Users/tp5/code/PrivatePyElasticaMyFork/examples/AxialStretchingCase/pyelastic_samples.csv"
    # eg_pyelastica_file = None
    run_example()
