from typing import List, Tuple

import numpy as np
from matplotlib import pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.colors import to_rgb
from rich.progress import Progress

from .stats import Stats, PositiveInt


def animate_rods(
        rod_group_stats: List[Stats],
        x_lim: Tuple[float, float],
        y_lim: Tuple[float, float],
        z_lim: Tuple[float, float],
        video_name: str = "rod_simulation.mp4",
        fps: PositiveInt = 15,
        frame_skip: PositiveInt = 1,
        save_figure: bool = False,
) -> FuncAnimation:  # (time step, x/y/z, node)
    time = np.asarray(rod_group_stats[0].time)
    num_rods = len(rod_group_stats)

    with plt.style.context("ggplot"):
        fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)
        ax = plt.axes(projection="3d")  # fig.add_subplot(111)
        ax.set_xlim(*x_lim)
        ax.set_ylim(*y_lim)
        ax.set_zlim(*z_lim)
        ax.set_box_aspect(
            (x_lim[1] - x_lim[0], y_lim[1] - y_lim[0], z_lim[1] - z_lim[0])
        )
        ax.set_xlabel("X")
        ax.set_ylabel("Y")
        ax.set_zlabel("Z")

        rod_line_list = []
        for i in range(num_rods):
            (rod_line,) = ax.plot(
                [],
                [],
                [],
                lw=3.0,
                c=to_rgb("xkcd:reddish"),
                solid_capstyle="round",
            )
            rod_line_list.append(rod_line)
        rod_line_list[0].set_label("rod")
        text_time = ax.text2D(
            0.2,
            0.8,
            " ",
            color="k",
            horizontalalignment="center",
            verticalalignment="center",
            transform=ax.transAxes,
        )
        fig.tight_layout()

    ax.legend()

    n_frames = len(time)
    with Progress() as progress:
        animation_task = progress.add_task(
            "[magenta] Animating...", total=n_frames // frame_skip, visible=False
        )

        def update(curr_frame):
            curr_time = time[curr_frame]
            text_time.set_text(f"Time : {curr_time:.2f}")
            for idx in range(num_rods):
                rod_line = rod_line_list[idx]
                position_of_rod = np.asarray(rod_group_stats[idx].position)
                rod_line.set_data(
                    position_of_rod[curr_frame, 0], position_of_rod[curr_frame, 1]
                )
                rod_line.set_3d_properties(position_of_rod[curr_frame, 2])
            progress.update(animation_task, advance=1, refresh=True)
            return text_time, *rod_line_list

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


def plot_energies(
        rod_group_stats: List[Stats], save_figure: bool = False
) -> plt.Figure:
    with plt.style.context("ggplot"):
        new_fig = plt.figure(figsize=(5, 4), dpi=150)

        new_ax = new_fig.add_subplot(111)

        new_ax.set_xlabel("time")
        new_ax.set_ylabel("energies")

        stats = rod_group_stats[-1]
        times = np.asarray(stats.time)
        te = np.asarray(stats.translational_energy)
        re = np.asarray(stats.rotational_energy)
        be = np.asarray(stats.bending_energy)
        se = np.asarray(stats.shearing_energy)
        # two springs in series
        k_collision = 0.5 * stats.contact_stiffness

        # at every instant there may actually be no or multiple penetrations
        def compute_pe(depths):
            return 0.5 * k_collision * np.sum(np.asarray(depths) ** 2)

        pe_collision = np.fromiter(
            map(compute_pe, stats.penetration_depths), dtype=float
        )
        # potential energy from gravity
        reference_z = 0.0
        pe_gravity = np.sum(
            stats.gravitational_acc
            * (reference_z - np.asarray(stats.position)[:, 2, :])  # (time, n_nodes)
            * stats.mass.reshape(1, -1),
            axis=-1,
        )

        new_ax.plot(times, te, c=to_rgb("xkcd:reddish"), lw=2.0, label="Translations")
        new_ax.plot(times, re, c=to_rgb("xkcd:bluish"), lw=2.0, label="Rotation")
        new_ax.plot(times, be, c=to_rgb("xkcd:burple"), lw=2.0, label="Bend")
        new_ax.plot(times, se, c=to_rgb("xkcd:goldenrod"), lw=2.0, label="Shear")
        new_ax.plot(
            times, pe_collision, c=to_rgb("xkcd:greenish"), lw=2.0, label="Collision"
        )
        new_ax.plot(
            times, pe_gravity, c=to_rgb("xkcd:pinky purple"), lw=2.0, label="Gravity"
        )
        new_ax.plot(
            times,
            te + re + be + se + pe_collision + pe_gravity,
            c="k",
            lw=2.0,
            label="Total energy",
        )

        new_fig.legend()

        new_fig.show()

    if save_figure:
        new_fig.savefig("energies.png")

    return new_fig
