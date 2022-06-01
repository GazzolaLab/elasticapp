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

# 2. Import the Python-binded case study bindings as modules. Please note
# the following:
# 2.1 Blaze needs to be imported first before any case study can be
# imported.
# 2.2 All case studies registered with the CMake case-study helper sit in
# elasticapp.CaseStudies.
from elasticapp import Blaze  # noqa
from elasticapp.CaseStudies.PyAxialStretchingCaseStudy import AxialStretchingCaseStudy
from matplotlib import pyplot as plt
from matplotlib.colors import to_rgb

from cli import print_description, track_simulation


def run_example(
    pyelastica_file: os.PathLike = "",
    save_to_csv: bool = False,
    save_figure: bool = False,
):
    np.set_printoptions(floatmode="unique")

    # 3. Define the input parameters to the case study.
    sample_interval = 200

    n_elem = 19
    base_radius = 0.025
    base_area = np.pi * base_radius**2

    base_length = 1.0
    """
    We set this value higher than usual because it accelerates convergence
    to the quasi-steady state, where forces balance out each other. We are only 
    interested in this state, to enable comparison with theory. It is worth noting
    that in this state, rates (v and omega) are zero : hence the damping contribution 
    to force is zero---so this coefficient does not really matter.
    """
    damping_rate = 1.0
    youngs_modulus = 1e4
    end_force_x = 1.0
    dt = 0.01 * (base_length / n_elem)
    final_time = 120.0
    rampup_interval = 10.0
    rampup_profile = "linear"  # or otherwise sinusoidal
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
        damping_rate=damping_rate,
        youngs_modulus=youngs_modulus,
        applied_force=end_force_x,
        radius=base_radius,
        length=base_length,
        rampup_interval=rampup_interval,
        rampup_profile=rampup_profile,
        dt=dt,
    )

    times = []
    iterations = []
    tip_displacements = []
    velocity_norms = []

    # 5. Run the case study. Different ways to run the case study are
    # 5.1 `cs.run()` : Runs the simulator till the user-defined criterion.
    # 5.2 `cs.iteration()` : Steps forward the simulator one iteration.
    # 5.3 `cs.run_for(additional_time=time_interval)` : Runs the simulator
    # for additional "time_interval" seconds.
    # 5.4 `cs.run_for(n_iterations=20)` : Runs the simulator for additional
    # 20 iterations.
    # 5.5 `cs.run_for_n_iterations(20)` : Same as above
    for _ in track_simulation(range(n_iterations)):
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
        curr_positions, curr_velocities = cs.communicate()
        times.append(curr_time)
        iterations.append(curr_iteration)

        # 6. To use your numpy-skills, just use np.asarray()! Memory is not
        # allocated, rather it uses the memory in the C++ buffer. So changes
        # are reflected on the C++ side too!
        tip_displacements.append(np.asarray(curr_positions)[0, -1])
        velocity_norms.append(np.linalg.norm(np.asarray(curr_velocities)))

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

    with plt.style.context("ggplot"):
        fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)
        ax = fig.add_subplot(111)
        ax.plot(times, tip_displacements, lw=2.0, c=to_rgb("xkcd:bluish"))
        ax.hlines(
            base_length + expected_tip_disp, 0.0, final_time, "k", "dashdot", lw=1.0
        )
        ax.hlines(
            base_length + expected_tip_disp_improved,
            0.0,
            final_time,
            "k",
            "dashed",
            lw=2.0,
        )
        ax.hlines(avg_tip_displacement, 0.0, final_time, "r", lw=0.5)
        ax.set_xlabel("Time")
        ax.set_ylabel("Tip displacement")
        fig.show()

        new_fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)
        new_ax = new_fig.add_subplot(111)
        new_ax.semilogy(times, velocity_norms, c=to_rgb("xkcd:reddish"), lw=2.0)

        new_ax.set_xlabel("t")
        new_ax.set_ylabel("|v|")

        new_fig.show()

    if os.path.exists(pyelastica_file):
        pyelastica_results = np.loadtxt(pyelastica_file, delimiter=",")
        ax.plot(pyelastica_results[:, 0], pyelastica_results[:, 1], "r-")

    if save_to_csv:
        np.savetxt(
            "tip_displacements.csv",
            np.c_[np.asarray(times), np.asarray(tip_displacements)],
            delimiter=",",
        )
        np.savetxt(
            "velocity_norms.csv",
            np.c_[np.asarray(times), np.asarray(velocity_norms)],
            delimiter=",",
        )

    if save_figure:
        fig.savefig("axial_stretching.pdf")

    plt.show()


if __name__ == "__main__":
    print_description(AxialStretchingCaseStudy)
    run_example(save_to_csv=False)
