from dataclasses import dataclass, field
from typing import List

import numpy as np
from matplotlib import pyplot as plt
from matplotlib.colors import to_rgb


@dataclass
class Stats:
    times: List[float] = field(default_factory=list)
    positions: List[np.ndarray] = field(default_factory=list)
    translation_e: List[float] = field(default_factory=list)
    rotation_e: List[float] = field(default_factory=list)
    shear_e: List[float] = field(default_factory=list)
    bend_e: List[float] = field(default_factory=list)


def plot_trajectory(stats: Stats, save_figure: bool = False) -> plt.Figure:
    positions = stats.positions

    with plt.style.context("ggplot"):
        fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)

        ax = fig.add_subplot(111)
        ax.set_xlabel("x")
        ax.set_ylabel("y")

        # record first position
        first_position = positions.pop(0)
        ax.plot(
            first_position[2, ...],
            first_position[0, ...],
            c=to_rgb("xkcd:reddish"),
            linestyle="--",
            lw=2.0,
        )

        # final position is also separate
        last_position = positions.pop()
        ax.plot(
            last_position[2, ...], last_position[0, ...], c="k", linestyle="--", lw=2.0
        )

        n_positions = len(positions)
        for i, pos in enumerate(positions):
            alpha = np.exp(i / n_positions - 1)
            ax.plot(
                pos[2, ...], pos[0, ...], c=to_rgb("xkcd:bluish"), lw=0.6, alpha=alpha
            )

        fig.show()

    if save_figure:
        fig.savefig("butterfly.png")

    return fig


def plot_energies(stats: Stats, save_figure: bool = False) -> plt.Figure:
    with plt.style.context("ggplot"):
        new_fig = plt.figure(figsize=(5, 4), dpi=150)

        new_ax = new_fig.add_subplot(111)

        new_ax.set_xlabel("time")
        new_ax.set_ylabel("energies")

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
        new_fig.savefig("energies.png")

    return new_fig
