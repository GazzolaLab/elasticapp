from dataclasses import dataclass, field
from typing import List, NewType
from matplotlib.animation import FuncAnimation
from rich.progress import Progress

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


PositiveInt = NewType("PositiveInt", int)


def animate_trajectory(
    total_stats: Stats,
    video_name: str = "collision.mp4",
    fps: PositiveInt = 15,
    frame_skip: PositiveInt = 1,
    save_figure: bool = False,
) -> FuncAnimation:  # (time step, x/y/z, node)
    rod_lines = []
    position = total_stats.positions

    with plt.style.context("ggplot"):
        fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)
        ax = fig.add_subplot(111)
        ax.set_xlim(-0.05, 3.05)
        ax.set_ylim(-1.0, 1.0)
        ax.set_xlabel("X")
        ax.set_ylabel("Y")
        ax.set_aspect("equal")

        rod_lines.append(
            ax.plot(
                [],
                [],
                lw=2.0,
                solid_capstyle="round",
            )[0]
        )
        fig.tight_layout()

    n_frames = len(position)
    with Progress() as progress:
        animation_task = progress.add_task(
            "[magenta] Animating...", total=n_frames // frame_skip, visible=False
        )

        def update(curr_frame):
            position_of_rod = np.asarray(position[curr_frame])
            # Tangent is along z
            l = rod_lines[0]
            l.set_data(position_of_rod[2, ...], position_of_rod[0, ...])

            progress.update(animation_task, advance=1, refresh=True)
            return (*rod_lines,)

        ani = FuncAnimation(
            fig, update, frames=range(0, n_frames, frame_skip), blit=True
        )
        if save_figure:
            progress.console.print(
                "[red]Rendering animation to file. It will be displayed on-screen once completed."
            )
            progress.update(animation_task, visible=True)
            ani.save(
                video_name,
                fps=fps,
                extra_args=["-vcodec", "h264", "-pix_fmt", "yuv420p"],
            )

    return ani
