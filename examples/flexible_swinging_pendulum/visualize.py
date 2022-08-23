import os

import numpy as np
from elasticapp.Systems import CosseratRods  # noqa
from elasticapp.CaseStudies.PyFlexibleSwingingPendulumCaseStudy import (
    FlexibleSwingingPendulumCaseStudy,
)
from matplotlib import pyplot as plt
from matplotlib.colors import to_rgb

from cli import track_simulation, replace_docstring, clickify


def run_example(
    reference_file: os.PathLike = "",
    save_to_csv: bool = False,
    save_figure: bool = False,
    plot_cpp_py_deviation: bool = False,
):
    np.set_printoptions(floatmode="unique")
    n_elem = 50
    base_radius = 0.005
    base_length = 1.0

    expected_density = 1.1e3
    base_area = np.pi * base_radius**2
    damping_rate = 1e-4
    youngs_modulus = 5e6
    shear_modulus = 5e6 / 1.5
    gravitational_acc = -9.80665
    dt = 0.002 * (base_length / n_elem)
    final_time = 5.0
    n_iterations = 200
    time_interval = final_time / n_iterations

    params = FlexibleSwingingPendulumCaseStudy.params(
        n_elements=n_elem,
        damping_rate=damping_rate,
        youngs_modulus=youngs_modulus,
        shear_modulus=shear_modulus,
        gravitational_acc=gravitational_acc,
        radius=base_radius,
        length=base_length,
        dt=dt,
    )
    cs = FlexibleSwingingPendulumCaseStudy(params)

    times = []
    tip_displacements_x = []
    tip_displacements_z = []

    def collect_stats(cs):
        times.append(cs.current_time())
        r = cs.communicate()
        tip_displacements_x.append(np.asarray(r.get_position())[0, -1])
        tip_displacements_z.append(np.asarray(r.get_position())[2, -1])

    collect_stats(cs)

    for _ in track_simulation(range(n_iterations)):
        cs.run_for(additional_time=time_interval)
        collect_stats(cs)

    times = np.asarray(times)
    tip_displacements_x = np.asarray(tip_displacements_x)
    tip_displacements_z = np.asarray(tip_displacements_z)

    with plt.style.context("ggplot"):
        fig = plt.figure(figsize=(8, 4), frameon=True, dpi=150)
        ax = fig.add_subplot(111)
        ax.plot(
            times,
            tip_displacements_x,
            lw=2.0,
            c=to_rgb("xkcd:bluish"),
            label="cpp : X disp.",
        )
        ax.plot(
            times,
            tip_displacements_z,
            "--",
            lw=2.0,
            c=to_rgb("xkcd:bluish"),
            label="cpp : Z disp.",
        )
        ax.set_xlabel("Time")
        ax.set_ylabel("Tip displacements")
        if os.path.exists(reference_file):
            reference_results = np.loadtxt(reference_file, delimiter=",")
            ax.plot(
                reference_results[:, 0],
                reference_results[:, 1],
                c="r",
                label="py : X disp.",
            )
            ax.plot(
                reference_results[:, 0],
                reference_results[:, 2],
                "--",
                c="r",
                label="py : Z disp.",
            )
            if plot_cpp_py_deviation:
                assert np.allclose(
                    times.shape, reference_results[:, 0].shape
                ), "C++ and Python data not of equal size"
                cpp_py_tip_deviation_x = (
                    np.fabs(tip_displacements_x - reference_results[:, 1]) / base_length
                )
                cpp_py_tip_deviation_z = (
                    np.fabs(tip_displacements_z - reference_results[:, 2]) / base_length
                )
                fig2 = plt.figure(figsize=(8, 4), frameon=True, dpi=150)
                ax2 = fig2.add_subplot(111)
                ax2.semilogy(
                    times,
                    cpp_py_tip_deviation_x,
                    lw=2.0,
                    c=to_rgb("xkcd:bluish"),
                    label="cpp-py : X deviation",
                )
                ax2.semilogy(
                    times,
                    cpp_py_tip_deviation_z,
                    lw=2.0,
                    c="r",
                    label="cpp-py : Z deviation",
                )
                ax2.set_xlabel("Time")
                ax2.set_ylabel("cpp-py tip dispacement deviation")
                ax2.legend(
                    loc="upper center",
                    bbox_to_anchor=(0.5, 1.1),
                    fancybox=True,
                    shadow=True,
                    ncol=2,
                )
                if plot_cpp_py_deviation:
                    fig2.savefig(
                        "flexible_swinging_pendulum_tip_position_cpp_py_deviation.png"
                    )

        ax.legend(
            loc="upper center",
            bbox_to_anchor=(0.5, 1.1),
            fancybox=True,
            shadow=True,
            ncol=4,
        )

    if save_to_csv:
        np.savetxt(
            "tip_displacements.csv",
            np.c_[
                np.asarray(times),
                np.asarray(tip_displacements_x),
                np.array(tip_displacements_z),
            ],
            delimiter=",",
        )
    if save_figure:
        fig.savefig("flexible_swinging_pendulum_tip_position.png")

    plt.show()

@clickify()
@replace_docstring(FlexibleSwingingPendulumCaseStudy.__doc__)
def run_case(
    data: bool,
    fig: bool,
):
    run_example("", data, fig, True)


if __name__ == "__main__":
    run_case()
