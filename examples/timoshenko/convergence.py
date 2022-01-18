from typing import NewType, List

import numpy as np
from elasticapp import Blaze  # noqa
from elasticapp.CaseStudies.PyTimoshenkoCaseStudy import TimoshenkoCaseStudy

from analytical import analytical_shearable, analytical_unshearable
from convergence_helpers import plot_convergence, calculate_error_norm

PositiveInt = NewType("PositiveInt", int)
Elements = list[PositiveInt]
ConvergenceResults = NewType("ConvergenceResults", List[float])


def simulate_timoshenko_beam_with(n_elem: PositiveInt, is_rod_shearable: bool = True):
    base_radius = 0.25
    base_area = np.pi * base_radius ** 2

    base_length = 3.0
    nu = 500.0
    youngs_modulus = 1e6

    shear_modulus = 1e4 if is_rod_shearable else 1e9
    analytical_solution = (
        analytical_shearable if is_rod_shearable else analytical_unshearable
    )

    end_force_transverse = -15.0
    dt = 0.01 * (base_length / n_elem)
    final_time = 500.0

    cs = TimoshenkoCaseStudy(
        n_elements=n_elem,
        damping=nu,
        youngs_modulus=youngs_modulus,
        shear_modulus=shear_modulus,
        applied_force=end_force_transverse,
        radius=base_radius,
        length=base_length,
        dt=dt,
    )

    cs.run_for(additional_time=final_time)

    final_positions, _ = cs.communicate()
    final_positions = np.copy(final_positions)

    _, y_analytical = analytical_solution(
        youngs_modulus=youngs_modulus,
        shear_modulus=shear_modulus,
        applied_force=end_force_transverse,
        radius=base_radius,
        length=base_length,
        n_elem=(n_elem + 1),
    )

    error, l1, l2, linf = calculate_error_norm(
        y_analytical,
        final_positions[0, ...],
        n_elem,
    )

    return {"n_elem": n_elem, "error": error, "l1": l1, "l2": l2, "linf": linf}


if __name__ == "__main__":
    import multiprocessing as mp
    from p_tqdm import p_map
    from functools import partial

    # 5, 6, ... 9
    convergence_elements = list(range(5, 10))
    # 10, 20, ... , 100
    convergence_elements.extend([10 * x for x in range(1, 11)])
    convergence_elements.extend([200])

    # Convergence study
    # for n_elem in [5, 6, 7, 8, 9, 10]
    simulate_shearable_timoshenko_beam_with = partial(
        simulate_timoshenko_beam_with, is_rod_shearable=True
    )

    results = p_map(
        simulate_shearable_timoshenko_beam_with,
        convergence_elements,
        num_cpus=len(convergence_elements),
    )

    if True:
        filename = "Timoshenko_convergence_test.png"
        plot_convergence(results, save_figure=False, filename=filename)

    if False:
        import pickle

        filename = "Timoshenko_convergence_test_data.dat"
        file = open(filename, "wb")
        pickle.dump([results], file)
        file.close()
