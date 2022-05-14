import numpy as np
import pandas as pd
from elasticapp import Blaze  # noqa
from elasticapp.CaseStudies.PyRigidBodyCollisionCaseStudy import (
    RigidBodyCollisionCaseStudy,
)
from matplotlib import pyplot as plt

from cli.help import print_description
from cli.progress import track_simulation
from postprocess import plot_position, animate_scenario, plot_energies, Stats


def run_example(
    save_to_csv: bool = False,
    save_figure: bool = False,
):
    sample_interval = 100

    radius1 = 0.05
    radius2 = 0.05
    xvelocity1 = 0.2  # go right
    xvelocity2 = -0.1  # go left
    dt = 1e-3
    final_time = 10.0
    time_interval = sample_interval * dt
    n_steps = int(final_time / time_interval)

    cs = RigidBodyCollisionCaseStudy(
        radius1=radius1,
        radius2=radius2,
        xvelocity1=xvelocity1,
        xvelocity2=xvelocity2,
        dt=dt,
    )

    first_sphere_stats = Stats(radius1)
    second_sphere_stats = Stats(radius2)

    sphere_stats = (first_sphere_stats, second_sphere_stats)

    def collect(cas):
        for stats in sphere_stats:
            stats.time.append(cas.current_time())
            stats.step.append(cas.current_iteration())

    def add_to(s, p, v):
        s.position.append(np.copy(p))
        s.velocity.append(np.copy(v))

    def collect_more(d, pvs):
        for stat, pv in zip(sphere_stats, pvs):
            add_to(stat, *pv)
            stat.penetration_depth.append(d)

    def collect_stats_from(cas):
        collect(cas)
        collect_more(*cas.communicate())

    # initial
    collect_stats_from(cs)

    for _ in track_simulation(range(n_steps)):
        cs.run_for(additional_time=time_interval)
        collect_stats_from(cs)

    energy_fig = plot_energies(
        first_sphere_stats, second_sphere_stats, save_figure=save_figure
    )
    position_fig = plot_position(
        first_sphere_stats, second_sphere_stats, save_figure=save_figure
    )
    ani = animate_scenario(
        first_sphere_stats, second_sphere_stats, save_figure=save_figure, frame_skip=5
    )

    if save_to_csv:
        pd.DataFrame(first_sphere_stats).to_csv("first_sphere_states.csv")
        pd.DataFrame(second_sphere_stats).to_csv("second_sphere_states.csv")

    plt.show()


if __name__ == "__main__":
    additional_description = (
        "Additionally, in this case, the spheres are identical. Hence, upon collision, "
        "they simply exchange their velocity magnitudes with a change in the direction alone. "
    )
    print_description(
        RigidBodyCollisionCaseStudy, additional_description=additional_description
    )
    run_example()
