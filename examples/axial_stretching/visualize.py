import os

import numpy as np
from matplotlib import pyplot as plt
from tqdm import tqdm

from elasticapp import Blaze  # noqa
from elasticapp.CaseStudies.PyAxialStretchingCaseStudy import AxialStretchingCaseStudy


def run_example(
    pyelastica_file: os.PathLike = "",
    save_to_csv: bool = False,
    save_figure: bool = False,
):
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
    n_steps = int(final_time / time_interval)

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
    tip_displacements = []

    for step in tqdm(range(n_steps)):
        cs.run_for(additional_time=time_interval)
        # cs.step()

        # if not (step % 200):
        curr_time = cs.current_time()
        curr_positions = cs.communicate()
        times.append(curr_time)
        tip_displacements.append(np.asarray(curr_positions)[0, -1])

    # First-order theory with base-length
    expected_tip_disp = end_force_x * base_length / base_area / youngs_modulus
    # First-order theory with modified-length, gives better estimates
    expected_tip_disp_improved = (
        end_force_x * base_length / (base_area * youngs_modulus - end_force_x)
    )

    times = np.asarray(times)
    tip_displacements = np.asarray(tip_displacements)

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
