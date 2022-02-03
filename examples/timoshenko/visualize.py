import numpy as np
from elasticapp import Blaze  # noqa
from elasticapp.CaseStudies.PyTimoshenkoCaseStudy import TimoshenkoCaseStudy
from matplotlib import pyplot as plt
from tqdm import tqdm

from postprocess import plot_beam, plot_velocity
from analytical import analytical_shearable, analytical_unshearable


def run_example(
    is_rod_shearable: bool = True,
    save_to_csv: bool = False,
    save_figure: bool = False,
):
    sample_interval = 500

    n_elem = 100
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
    time_interval = sample_interval * dt
    n_steps = int(final_time / time_interval)

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

    times = []
    velocity_norms = []

    for _ in tqdm(range(n_steps)):
        cs.run_for(additional_time=time_interval)
        # cs.step()

        # if not (step % 200):
        curr_time = cs.current_time()
        _, curr_velocity_norms = cs.communicate()
        times.append(curr_time)
        velocity_norms.append(np.linalg.norm(np.asarray(curr_velocity_norms)))

    final_positions, _ = cs.communicate()
    final_positions = np.copy(final_positions)
    plot_beam(
        n_elem,
        final_positions,
        analytical_solution(
            youngs_modulus=youngs_modulus,
            shear_modulus=shear_modulus,
            applied_force=end_force_transverse,
            radius=base_radius,
            length=base_length,
        ),
        save_figure=save_figure,
    )
    plot_velocity(np.asarray(times), np.asarray(velocity_norms))

    # print(tip_displacements)
    if save_to_csv:
        np.savetxt(
            "tip_displacements.csv",
            final_positions,
            delimiter=",",
        )

    plt.show()


if __name__ == "__main__":
    run_example()
