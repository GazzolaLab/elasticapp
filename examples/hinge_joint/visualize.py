from dataclasses import dataclass, field
from typing import NewType

import numpy as np
import pandas as pd
from elasticapp import Blaze  # noqa
from elasticapp.CaseStudies.PyHingeJointCaseStudy import HingeJointCaseStudy
from matplotlib import pyplot as plt
from tqdm import tqdm
from postprocess import plot_position, plot_video

PositiveInt = NewType("PositiveInt", int)


@dataclass
class Stats:
    time: list[float] = field(default_factory=list)
    step: list[PositiveInt] = field(default_factory=list)
    position: list[np.ndarray] = field(default_factory=list)
    velocity: list[np.ndarray] = field(default_factory=list)


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

    def collect_more(first_pv, second_pv):
        add_to(first_rod_stats, *first_pv)
        add_to(second_rod_stats, *second_pv)

    def collect_stats_from(cas):
        collect(cas)
        collect_more(*cas.communicate())

    # initial
    collect_stats_from(cs)

    for _ in tqdm(range(n_steps)):
        cs.run_for(additional_time=time_interval)
        collect_stats_from(cs)

    plot_position(first_rod_stats, second_rod_stats, save_figure=save_figure)
    plot_video(first_rod_stats, second_rod_stats)

    if save_to_csv:
        pd.DataFrame(first_rod_stats).to_csv("hinge_joint_first.csv")
        pd.DataFrame(second_rod_stats).to_csv("hinge_joint_second.csv")

    plt.show()


if __name__ == "__main__":
    run_example()
