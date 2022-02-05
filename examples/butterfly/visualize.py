import numpy as np
import pandas as pd
from elasticapp import Blaze  # noqa
from elasticapp.CaseStudies.PyButterflyCaseStudy import ButterflyCaseStudy
from matplotlib import pyplot as plt
from tqdm import tqdm
from postprocess import Stats, plot_trajectory, plot_energies


def run_example(
    save_to_csv: bool = False,
    save_figure: bool = False,
):
    sample_interval = 200

    n_elem = 2
    base_length = 3.0
    dt = 0.01 * (base_length / n_elem)
    final_time = 50.0
    time_interval = sample_interval * dt
    n_steps = int(final_time / time_interval)

    cs = ButterflyCaseStudy(
        n_elements=n_elem,
        length=base_length,
        dt=dt,
        angle_of_inclination=np.deg2rad(45.0),
    )

    stats = Stats()

    def collect(t):
        stats.times.append(t)

    def collect_more(p, be, se, te, re):
        stats.positions.append(np.copy(p))
        stats.bend_e.append(be)
        stats.shear_e.append(se)
        stats.translation_e.append(te)
        stats.rotation_e.append(re)

    def collect_stats_from(cas):
        collect(cas.current_time())
        collect_more(*cas.communicate())

    # initial
    collect_stats_from(cs)

    for step in tqdm(range(n_steps)):
        cs.run_for(additional_time=time_interval)

        collect_stats_from(cs)

    plot_trajectory(stats, save_figure=save_figure)
    plot_energies(stats, save_figure=save_figure)

    if save_to_csv:
        pd.DataFrame(stats).to_csv("butterfly.csv")

    plt.show()


if __name__ == "__main__":
    run_example()
