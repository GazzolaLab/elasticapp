from typing import NewType, List

import numpy as np
from elasticapp import Blaze  # noqa
from elasticapp.CaseStudies.PyAxialStretchingCaseStudy import AxialStretchingCaseStudy
from tqdm.auto import trange

PositiveInt = NewType('PositiveInt', int)
Elements = list[PositiveInt]
ConvergenceResults = NewType("ConvergenceResults", List[float])


def simulate_axial_stretching_with(
        n_elem: PositiveInt
) -> float:
    sample_interval = 200

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
        dt=dt,
    )

    times = []
    tip_displacements = []

    for _ in trange(n_steps):
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

    return np.abs(avg_tip_displacement - expected_tip_disp_improved - base_length)
    # print(np.abs(avg_tip_displacement - expected_tip_disp_improved))
    # norms = ['1', '2', 'inf']
    # errors = map(lambda x: np.linalg.norm(avg_tip_displacement - expected_tip_disp_improved, x), norms)
    # print(dict(zip(norms, errors)))


def convergence_for_axial_stretching_with(elements: Elements) -> ConvergenceResults:
    import multiprocessing as mp

    with mp.Pool(mp.cpu_count()) as pool:
        results = pool.map(simulate_axial_stretching_with, elements)

    return ConvergenceResults(results)


def _visualize_results(elements: Elements, results: ConvergenceResults):
    from matplotlib import pyplot as plt
    from matplotlib.colors import to_rgb

    fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)

    ax = fig.add_subplot(111)
    ax.grid(b=True, which="minor", color="k", linestyle="--")
    ax.grid(b=True, which="major", color="k", linestyle="-")
    ax.loglog(
        elements,
        results,
        marker="o",
        ms=10,
        c=to_rgb("xkcd:bluish"),
        lw=2,
    )
    # fig.legend(prop={"size": 20})
    # block for me to see
    plt.show()


if __name__ == "__main__":
    convergence_elements = Elements([20, 40, 80, 100, 200])
    convergence_results = convergence_for_axial_stretching_with(convergence_elements)
    _visualize_results(convergence_elements, convergence_results)
