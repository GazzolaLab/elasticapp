import numpy as np
from elasticapp import Blaze  # noqa
from elasticapp.CaseStudies.PyTimoshenkoCaseStudy import TimoshenkoCaseStudy
from matplotlib import pyplot as plt
from matplotlib.colors import to_rgb
from tqdm import tqdm

from analytical import analytical_shearable, analytical_unshearable


def _plot_timoshenko(n_elem, numerical_positions, analytical, save_figure):
    # import seaborn as sns
    # sns.set_style("whitegrid")

    fig = plt.figure(figsize=(5, 4), dpi=150)
    ax = fig.add_subplot(111)
    ax.plot(
        analytical[0],
        analytical[1],
        "k--",
    )
    ax.plot(
        numerical_positions[2, ...],
        numerical_positions[0, ...],
        c=to_rgb("xkcd:bluish"),
        label=f"n={n_elem}",
    )
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    fig.show()
    if save_figure:
        fig.savefig(f"Timoshenko_beam_test_{n_elem}.png")

    return


def _plot_velocity(times, velocity_norms):
    fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)
    ax = fig.add_subplot(111)
    ax.semilogy(times, velocity_norms, c=to_rgb("xkcd:reddish"), lw=2.0)

    ax.set_xlabel("t")
    ax.set_ylabel("|v|")

    fig.show()

    return


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
    _plot_timoshenko(
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
    _plot_velocity(np.asarray(times), np.asarray(velocity_norms))

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
