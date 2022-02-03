from matplotlib import pyplot as plt
from matplotlib.colors import to_rgb
import numpy as np


def plot_beam(n_elem, numerical_positions, analytical, save_figure):
    with plt.style.context("ggplot"):
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


def plot_velocity(times, velocity_norms):
    with plt.style.context("ggplot"):
        fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)
        ax = fig.add_subplot(111)
        ax.semilogy(times, velocity_norms, c=to_rgb("xkcd:reddish"), lw=2.0)

        ax.set_xlabel("t")
        ax.set_ylabel("|v|")

        fig.show()

    return
