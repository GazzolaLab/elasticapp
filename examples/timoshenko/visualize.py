import numpy as np
from elasticapp import Blaze  # noqa
from elasticapp.CaseStudies.PyTimoshenkoCaseStudy import TimoshenkoCaseStudy
from matplotlib import pyplot as plt

from cli.help import print_description
from cli.progress import track_simulation
from helpers.analytical import analytical_shearable, analytical_unshearable
from helpers.postprocess import plot_beam, plot_velocity


def run_example(
    is_rod_shearable: bool = True,
    save_to_csv: bool = False,
    save_figure: bool = False,
):
    sample_interval = 500

    n_elem = 100
    base_radius = 0.25
    base_area = np.pi * base_radius**2

    base_length = 3.0
    density = 5e3

    """
    We set this value higher than usual because it accelerates convergence
    to the quasi-steady state, where forces balance out each other. We are only 
    interested in this state, to enable comparison with theory. It is worth noting
    that in this state, rates (v and omega) are zero : hence the damping contribution 
    to force is zero---so this coefficient does not really matter.
    """
    damping_rate = 0.5
    youngs_modulus = 1e6

    shear_modulus = 1e4 if is_rod_shearable else 1e9
    analytical_solution = (
        analytical_shearable if is_rod_shearable else analytical_unshearable
    )

    end_force_transverse = -15.0
    dt = 0.01 * (base_length / n_elem)
    final_time = 200.0
    time_interval = sample_interval * dt
    n_steps = int(final_time / time_interval)

    cs = TimoshenkoCaseStudy(
        n_elements=n_elem,
        damping_rate=damping_rate,
        youngs_modulus=youngs_modulus,
        shear_modulus=shear_modulus,
        applied_force=end_force_transverse,
        radius=base_radius,
        length=base_length,
        dt=dt,
    )

    times = []
    velocity_norms = []

    for _ in track_simulation(range(n_steps)):
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
    print_description(TimoshenkoCaseStudy)
    run_example()
