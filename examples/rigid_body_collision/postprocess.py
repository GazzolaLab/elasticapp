from dataclasses import dataclass, field
from typing import NewType, List

import numpy as np
from matplotlib import pyplot as plt
from matplotlib.animation import FuncAnimation  # noqa
from matplotlib.colors import to_rgb
from rich.progress import Progress

PositiveInt = NewType("PositiveInt", int)


@dataclass
class Stats:
    radius: float
    time: List[float] = field(default_factory=list)
    step: List[PositiveInt] = field(default_factory=list)
    position: List[np.ndarray] = field(default_factory=list)
    velocity: List[np.ndarray] = field(default_factory=list)
    penetration_depth: List[float] = field(default_factory=list)


def plot_position(
    sphere1_stats: Stats,
    sphere2_stats: Stats,
    save_figure: bool = False,
    filename: str = "sphere_collision.png",
) -> plt.Figure:
    position_of_sphere1 = np.asarray(sphere1_stats.position)
    position_of_sphere2 = np.asarray(sphere2_stats.position)

    with plt.style.context("ggplot"):
        fig = plt.figure(figsize=(5, 5), frameon=True, dpi=150)
        ax = fig.add_subplot(111)

        ax.plot(
            position_of_sphere1[:, 0, -1],
            position_of_sphere1[:, 1, -1],
            c=to_rgb("xkcd:reddish"),
            marker="o",
            label="sphere1",
        )
        ax.plot(
            position_of_sphere2[:, 0, -1],
            position_of_sphere2[:, 1, -1],
            c=to_rgb("xkcd:bluish"),
            marker="o",
            label="sphere2",
        )
        ax.set_xlabel("x")
        ax.set_ylabel("y")

        fig.show()

    if save_figure:
        fig.savefig(filename)

    return fig


def plot_energies(sphere1_stats, sphere2_stats, save_figure: bool):
    with plt.style.context("ggplot"):
        new_fig = plt.figure(figsize=(5, 4), dpi=150)

        new_ax = new_fig.add_subplot(111)

        new_ax.set_xlabel("times")
        new_ax.set_ylabel("energies")

        time = np.asarray(sphere1_stats.time)
        k_spring = 200
        # two springs in series
        k_collision = 0.5 * k_spring
        x_axis = 0
        sphere_idx = 0

        def compute_mass(r):
            density = 1.1e3
            return density * (4.0 / 3.0) * np.pi * (r * r * r)

        # last axis is just a scalar
        ke1 = (
            0.5
            * np.sum(np.asarray(sphere1_stats.velocity)[..., sphere_idx] ** 2, axis=1)
            * compute_mass(sphere1_stats.radius)
        )
        ke2 = (
            0.5
            * np.sum(np.asarray(sphere2_stats.velocity)[..., sphere_idx] ** 2, axis=1)
            * compute_mass(sphere2_stats.radius)
        )
        # 1/2 * k * x**2, k is hardcoded for now
        pe_collision = (
            0.5 * k_collision * np.asarray(sphere2_stats.penetration_depth) ** 2
        )

        new_ax.plot(time, ke1, c=to_rgb("xkcd:reddish"), lw=2.0, label="KE Sphere 1")
        new_ax.plot(time, ke2, c=to_rgb("xkcd:bluish"), lw=2.0, label="KE Sphere 2")
        new_ax.plot(
            time, pe_collision, c=to_rgb("xkcd:goldenrod"), lw=2.0, label="PE Collision"
        )
        new_ax.plot(time, ke1 + ke2 + pe_collision, c="k", lw=2.0, label="Total energy")

        new_fig.legend()

        new_fig.show()

    if save_figure:
        new_fig.savefig("energies.pdf")

    return new_fig


def _construct_sphere_coords(N=50):
    u = np.linspace(0, 2 * np.pi, N)
    v = np.linspace(0, np.pi, N)
    x = np.outer(np.cos(u), np.sin(v))
    y = np.outer(np.sin(u), np.sin(v))
    z = np.outer(np.ones(np.size(u)), np.cos(v))
    return np.stack((x, y, z))


def _reconstruct_sphere_coords(positions, sphere_coords=None):
    if sphere_coords is None:
        sphere_coords = _construct_sphere_coords()

    return positions.reshape(-1, 1, 1) + sphere_coords


def animate_scenario(
    sphere1_stats: Stats,
    sphere2_stats: Stats,
    video_name: str = "sphere_collision.mp4",
    fps: PositiveInt = 15,
    frame_skip: PositiveInt = 1,
    save_figure: bool = False,
) -> FuncAnimation:  # (time step, x/y/z, node)

    time = np.asarray(sphere1_stats.time)
    step = np.asarray(sphere1_stats.step)
    position_of_sphere1 = np.asarray(sphere1_stats.position)
    position_of_sphere2 = np.asarray(sphere2_stats.position)
    _sphere1 = sphere1_stats.radius * _construct_sphere_coords()
    _sphere2 = sphere2_stats.radius * _construct_sphere_coords()

    with plt.style.context("ggplot"):
        fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)
        ax = plt.axes(projection="3d")  # fig.add_subplot(111)
        ax.set_xlim(-0.8, 1.2)
        ax.set_ylim(-0.2, 0.2)
        ax.set_zlim(-0.2, 0.2)
        ax.set_box_aspect((5, 1, 1))

        sphere1 = ax.plot_surface(
            *_sphere1,
            color=to_rgb("xkcd:reddish"),
        )
        sphere2 = ax.plot_surface(
            *_sphere2,
            color=to_rgb("xkcd:bluish"),
        )
        text_step = ax.text2D(
            0.2,
            0.9,
            " ",
            color="k",
            horizontalalignment="center",
            verticalalignment="center",
            transform=ax.transAxes,
        )
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

    spheres = [sphere1, sphere2]
    n_frames = len(time)

    with Progress() as progress:
        animation_task = progress.add_task(
            "[magenta] Animating...", total=n_frames, visible=False
        )

        def update(curr_frame):
            curr_time = time[curr_frame]
            curr_step = step[curr_frame]
            text_step.set_text(f"Step : {curr_step:04d}")
            text_time.set_text(f"Time : {curr_time:.2f}")
            spheres[0].remove()
            spheres[0] = ax.plot_surface(
                *_reconstruct_sphere_coords(position_of_sphere1[curr_frame], _sphere1),
                color=to_rgb("xkcd:reddish"),
            )
            spheres[1].remove()
            spheres[1] = ax.plot_surface(
                *_reconstruct_sphere_coords(position_of_sphere2[curr_frame], _sphere2),
                color=to_rgb("xkcd:bluish"),
            )
            progress.update(animation_task, advance=1)
            return spheres

        ani = FuncAnimation(fig, update, frames=range(0, n_frames, frame_skip))
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
