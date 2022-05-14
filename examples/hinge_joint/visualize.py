import numpy as np
import pandas as pd
from elasticapp import Blaze  # noqa
from elasticapp.CaseStudies.PyHingeJointCaseStudy import HingeJointCaseStudy
from matplotlib import pyplot as plt

from cli.help import print_description
from cli.progress import track_simulation
from postprocess import plot_position, animate_scenario, Stats


def run_example(
    save_to_csv: bool = False,
    save_figure: bool = False,
):
    sample_interval = 1000

    n_elem = 10
    base_length = 0.2
    base_radius = 0.007
    dt = 1e-5
    final_time = 1.0
    time_interval = sample_interval * dt
    n_steps = int(final_time / time_interval)

    cs = HingeJointCaseStudy(
        n_elements=n_elem,
        radius=base_radius,
        length=base_length,
        dt=dt,
    )

    first_rod_stats = Stats()
    second_rod_stats = Stats()
    rod_stats = (first_rod_stats, second_rod_stats)

    def collect(cas):
        for stats in rod_stats:
            stats.time.append(cas.current_time())
            stats.step.append(cas.current_iteration())

    def add_to(s, p, v):
        s.position.append(np.copy(p))
        s.velocity.append(np.copy(v))

    def collect_more(*pvs):
        for stat, pv in zip(rod_stats, pvs):
            add_to(stat, *pv)

    def collect_stats_from(cas):
        collect(cas)
        collect_more(*cas.communicate())

    # initial
    collect_stats_from(cs)

    for _ in track_simulation(range(n_steps)):
        cs.run_for(additional_time=time_interval)
        collect_stats_from(cs)

    plot_position(first_rod_stats, second_rod_stats, save_figure=save_figure)
    animation = animate_scenario(
        first_rod_stats, second_rod_stats, save_figure=save_figure, frame_skip=5
    )

    if save_to_csv:
        pd.DataFrame(first_rod_stats).to_csv("hinge_joint_first.csv")
        pd.DataFrame(second_rod_stats).to_csv("hinge_joint_second.csv")

    plt.show()


if __name__ == "__main__":
    print_description(HingeJointCaseStudy)
    run_example()
