import numpy as np
import pandas as pd
from elasticapp.Systems import RigidBody  # noqa
from elasticapp.CaseStudies.PyImposedPrimitiveCaseStudy import (
    ImposedPrimitiveCaseStudy,
)
from matplotlib import pyplot as plt

from cli import print_description, track_simulation
from postprocess import (
    animate_scenario,
    Stats,
    SphereStats,
    PlaneStats,
    plot_energies,
    plot_position,
)


def run_example(
    save_to_csv: bool = False,
    save_figure: bool = False,
):
    # Parameters below are one-to-one from PRL nest
    radius = 0.1  # m
    dt = 4e-5
    gravitational_acc = -9.81
    contact_stiffness = 1e5
    contact_damping = 1e-3
    initial_height_from_plane = 1.0

    sample_interval = 1e2
    time_interval = sample_interval * dt
    final_time = 2.0
    n_steps = int(final_time / time_interval)

    cs = ImposedPrimitiveCaseStudy(
        radius=radius,
        gravitational_acc=gravitational_acc,
        contact_stiffness=contact_stiffness,
        contact_damping=contact_damping,
        initial_height_from_plane=initial_height_from_plane,
        dt=dt,
    )

    stats = Stats(
        SphereStats(radius),
        PlaneStats(),
        contact_stiffness=contact_stiffness,
        gravitational_acc=gravitational_acc,
    )

    def collect_global_stats_from(cas):
        stats.time.append(cas.current_time())
        stats.step.append(cas.current_iteration())

    def collect_stats(prim_stats, p, v):
        prim_stats.position.append(np.copy(p))
        prim_stats.velocity.append(np.copy(v))

    def collect_stats_from(cas):
        collect_global_stats_from(cas)
        collected = cas.communicate()
        stats.penetration_depth.append(collected[0])
        collect_stats(
            stats.sphere, collected[1].get_position(), collected[1].get_velocity()
        )
        collect_stats(stats.plane, *(collected[2]))

    # initial
    collect_stats_from(cs)

    for _ in track_simulation(range(n_steps)):
        cs.run_for(additional_time=time_interval)
        collect_stats_from(cs)

    if save_to_csv:
        pd.DataFrame(stats).to_csv(f"stats.csv")

    energy_fig = plot_energies(stats=stats, save_figure=save_figure)
    position_fig = plot_position(stats=stats, save_figure=save_figure)
    if True:
        ani = animate_scenario(stats, save_figure=save_figure, frame_skip=5)
    plt.show()


if __name__ == "__main__":
    print_description(ImposedPrimitiveCaseStudy)
    run_example(save_figure=False)
