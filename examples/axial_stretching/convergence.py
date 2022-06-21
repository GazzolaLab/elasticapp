from typing import NewType, List

import numpy as np
from elasticapp import Arrays  # noqa
from elasticapp.CaseStudies.PyAxialStretchingCaseStudy import AxialStretchingCaseStudy

from cli.progress import p_map, ProgressionStatus

PositiveInt = NewType("PositiveInt", int)
Elements = List[PositiveInt]
ConvergenceResults = NewType("ConvergenceResults", List[float])


def simulate_axial_stretching_with(n_elem: PositiveInt, progress, uuid: int) -> float:
    sample_interval = 200

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
    damping_rate = 10.0
    youngs_modulus = 1e4
    end_force_x = 1.0
    dt = 0.01 * (base_length / n_elem)
    final_time = 50.0
    time_interval = sample_interval * dt
    n_steps = int(final_time / time_interval)

    cs = AxialStretchingCaseStudy(
        n_elements=n_elem,
        damping_rate=damping_rate,
        youngs_modulus=youngs_modulus,
        applied_force=end_force_x,
        radius=base_radius,
        length=base_length,
        dt=dt,
    )

    times = []
    tip_displacements = []

    for i_step in range(n_steps):
        cs.run_for(additional_time=time_interval)
        # cs.step()

        # if not (step % 200):
        curr_time = cs.current_time()
        curr_positions = cs.communicate()
        times.append(curr_time)
        tip_displacements.append(np.asarray(curr_positions)[0, -1])

        # state
        progress[uuid] = ProgressionStatus(current=i_step + 1, total=n_steps)

    # First-order theory with base-length
    expected_tip_disp = end_force_x * base_length / base_area / youngs_modulus
    # First-order theory with modified-length, gives better estimates
    expected_tip_disp_improved = (
        end_force_x * base_length / (base_area * youngs_modulus - end_force_x)
    )

    times = np.asarray(times)
    tip_displacements = np.asarray(tip_displacements)

    avg_tip_displacement = np.mean(tip_displacements[-200:])

    return np.abs(avg_tip_displacement - expected_tip_disp_improved - base_length)
    # print(np.abs(avg_tip_displacement - expected_tip_disp_improved))
    # norms = ['1', '2', 'inf']
    # errors = map(lambda x: np.linalg.norm(avg_tip_displacement - expected_tip_disp_improved, x), norms)
    # print(dict(zip(norms, errors)))


def convergence_for_axial_stretching_with(elements: Elements) -> ConvergenceResults:
    return ConvergenceResults(p_map(simulate_axial_stretching_with, elements))


def _visualize_results(elements: Elements, results: ConvergenceResults):
    from matplotlib import pyplot as plt
    from matplotlib.colors import to_rgb

    fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)

    ax = fig.add_subplot(111)
    ax.grid(visible=True, which="minor", color="k", linestyle="--", lw=0.5)
    ax.grid(visible=True, which="major", color="k", linestyle="-", lw=0.5)
    ax.loglog(
        elements,
        results,
        marker="o",
        ms=10,
        c=to_rgb("xkcd:bluish"),
        lw=2,
    )
    ax.set_xlabel("Number of elements")
    ax.set_ylabel("Error")
    # fig.legend(prop={"size": 20})
    # block for me to see
    plt.show()


if __name__ == "__main__":
    convergence_elements = Elements([20, 40, 80, 100, 200])
    convergence_results = convergence_for_axial_stretching_with(convergence_elements)
    _visualize_results(convergence_elements, convergence_results)
