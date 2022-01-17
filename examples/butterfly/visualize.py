from dataclasses import dataclass, field

import numpy as np
import pandas as pd
from elasticapp import Blaze  # noqa
from elasticapp.CaseStudies.PyButterflyCaseStudy import ButterflyCaseStudy
from matplotlib import pyplot as plt
from matplotlib.colors import to_rgb
from tqdm import tqdm


@dataclass
class Stats:
    times: list[float] = field(default_factory=list)
    positions: list[np.ndarray] = field(default_factory=list)
    translation_e: list[float] = field(default_factory=list)
    rotation_e: list[float] = field(default_factory=list)
    shear_e: list[float] = field(default_factory=list)
    bend_e: list[float] = field(default_factory=list)


def _plot_trajectory(stats: Stats, save_figure: bool):
    positions = stats.positions

    fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)

    ax = fig.add_subplot(111)

    # record first position
    first_position = positions.pop(0)
    ax.plot(first_position[2, ...], first_position[0, ...], "r--", lw=2.0)

    # final position is also separate
    last_position = positions.pop()
    ax.plot(last_position[2, ...], last_position[0, ...], "k--", lw=2.0)

    n_positions = len(positions)
    for i, pos in enumerate(positions):
        alpha = np.exp(i / n_positions - 1)
        ax.plot(pos[2, ...], pos[0, ...], "b", lw=0.6, alpha=alpha)

    fig.show()

    if save_figure:
        fig.savefig("butterfly.pdf")

    return


def _plot_energies(stats: Stats, save_figure: bool):
    new_fig = plt.figure(figsize=(5, 4), dpi=150)

    new_ax = new_fig.add_subplot(111)

    times = np.asarray(stats.times)
    te = np.asarray(stats.translation_e)
    re = np.asarray(stats.rotation_e)
    be = np.asarray(stats.bend_e)
    se = np.asarray(stats.shear_e)

    new_ax.plot(times, te, c=to_rgb("xkcd:reddish"), lw=2.0, label="Translations")
    new_ax.plot(times, re, c=to_rgb("xkcd:bluish"), lw=2.0, label="Rotation")
    # new_ax.plot(times, np.asarray(stats.translation_e) + np.asarray(stats.rotation_e), to_rgb("xkcd:reddish"), lw=2.0,
    #         label="Translations")

    new_ax.plot(times, be, c=to_rgb("xkcd:burple"), lw=2.0, label="Bend")
    new_ax.plot(times, se, c=to_rgb("xkcd:goldenrod"), lw=2.0, label="Shear")
    new_ax.plot(times, te + re + be + se, c="k", lw=2.0, label="Total energy")

    new_fig.legend()

    new_fig.show()

    if save_figure:
        new_fig.savefig("energies.pdf")

    return


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
        # cs.step()

        # if not (step % 200):
        collect_stats_from(cs)

    # _plot_trajectory(stats, save_figure=save_figure)
    _plot_energies(stats, save_figure=save_figure)

    if save_to_csv:
        pd.DataFrame(stats).to_csv("butterfly.csv")

    plt.show()


if __name__ == "__main__":
    run_example()
