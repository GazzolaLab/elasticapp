from dataclasses import dataclass, field
from typing import NewType, List

import numpy as np
from matplotlib import pyplot as plt
from matplotlib.animation import FuncAnimation  # noqa
from matplotlib.colors import to_rgb
from rich.progress import Progress

PositiveInt = NewType("PositiveInt", int)


@dataclass
class SphereStats:
    radius: float
    position: List[np.ndarray] = field(default_factory=list)
    velocity: List[np.ndarray] = field(default_factory=list)


@dataclass
class PlaneStats:
    position: List[np.ndarray] = field(default_factory=list)
    velocity: List[np.ndarray] = field(default_factory=list)


@dataclass
class Stats:
    sphere: SphereStats
    plane: PlaneStats
    contact_stiffness: float
    gravitational_acc: float
    time: List[float] = field(default_factory=list)
    step: List[PositiveInt] = field(default_factory=list)
    penetration_depth: List[float] = field(default_factory=list)


def plot_position(
    stats: Stats,
    save_figure: bool = False,
    filename: str = "imposed_primitives.png",
) -> plt.Figure:
    position_of_sphere = np.asarray(stats.sphere.position)
    position_of_plane = np.asarray(stats.plane.position)
    time = np.asarray(stats.time)

    with plt.style.context("ggplot"):
        fig = plt.figure(figsize=(5, 4), dpi=150)
        ax = fig.add_subplot(111)

        z_axis = 2
        ax.plot(
            time,
            position_of_sphere[:, z_axis, -1],
            c=to_rgb("xkcd:reddish"),
            marker="o",
            label="sphere",
        )
        ax.plot(
            time,
            position_of_plane[:, z_axis],
            c=to_rgb("xkcd:bluish"),
            marker="o",
            label="plane",
        )
        ax.set_xlabel("Time")
        ax.set_ylabel("Centroid height")
        ax.legend()

        fig.show()

    if save_figure:
        fig.savefig(filename)

    return fig


def plot_energies(
    stats: Stats, save_figure: bool = False, filename: str = "energies.png"
):
    with plt.style.context("ggplot"):
        new_fig = plt.figure(figsize=(5, 4), dpi=150)

        new_ax = new_fig.add_subplot(111)

        new_ax.set_xlabel("times")
        new_ax.set_ylabel("energies")

        time = np.asarray(stats.time)
        k_spring = stats.contact_stiffness
        # two springs in series
        k_collision = 0.5 * k_spring
        sphere_idx = 0

        def compute_mass(r):
            density = 1.1e3
            return density * (4.0 / 3.0) * np.pi * (r * r * r)

        # last axis is just a scalar
        ke_sphere = (
            0.5
            * np.sum(np.asarray(stats.sphere.velocity)[..., sphere_idx] ** 2, axis=1)
            * compute_mass(stats.sphere.radius)
        )
        reference_z = 0.0
        pe_sphere = (
            stats.gravitational_acc
            * (reference_z - np.asarray(stats.sphere.position)[:, 2, sphere_idx])
            * compute_mass(stats.sphere.radius)
        )
        # 1/2 * k * x**2, k is hardcoded for now
        pe_collision = 0.5 * k_collision * np.asarray(stats.penetration_depth) ** 2

        new_ax.plot(
            time, ke_sphere, c=to_rgb("xkcd:reddish"), lw=2.0, label="KE Sphere"
        )
        new_ax.plot(time, pe_sphere, c=to_rgb("xkcd:burple"), lw=2.0, label="PE Sphere")
        new_ax.plot(
            time, pe_collision, c=to_rgb("xkcd:goldenrod"), lw=2.0, label="PE Collision"
        )
        new_ax.plot(
            time,
            ke_sphere + pe_sphere + pe_collision,
            c="k",
            lw=2.0,
            label="Total energy",
        )

        new_fig.legend()

        new_fig.show()

    if save_figure:
        new_fig.savefig(filename)

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


def _construct_plane_coords(N=50):
    # create x,y
    xx, yy = np.meshgrid(np.linspace(-10, 10, N), np.linspace(-10, 10, N))
    return np.stack((xx, yy, 0.0 * xx))


def _reconstruct_plane_coords(positions, plane_coords=None):
    if plane_coords is None:
        plane_coords = _construct_plane_coords()

    return positions.reshape(-1, 1, 1) + plane_coords


def animate_scenario(
    stats: Stats,
    video_name: str = "imposed_primitive_collision.mp4",
    fps: PositiveInt = 15,
    frame_skip: PositiveInt = 1,
    save_figure: bool = False,
) -> FuncAnimation:  # (time step, x/y/z, node)

    time = np.asarray(stats.time)
    step = np.asarray(stats.step)
    position_of_sphere = np.asarray(stats.sphere.position)
    position_of_plane = np.asarray(stats.plane.position)
    _sphere = stats.sphere.radius * _construct_sphere_coords()
    _plane = 0.05 * _construct_plane_coords(N=5)

    with plt.style.context("ggplot"):
        fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)
        ax = plt.axes(projection="3d")  # fig.add_subplot(111)
        ax.set_xlim(-0.5, 0.5)
        ax.set_ylim(-0.5, 0.5)
        ax.set_zlim(-0.1, 1.1)
        ax.set_box_aspect((5, 5, 6))

        plane = ax.plot_surface(
            *_plane,
            color=to_rgb("xkcd:bluish"),
        )
        sphere = ax.plot_surface(
            *_sphere,
            color=to_rgb("xkcd:reddish"),
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

    artists = [plane, sphere]
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
            artists[0].remove()
            artists[0] = ax.plot_surface(
                *_reconstruct_plane_coords(position_of_plane[curr_frame], _plane),
                color=to_rgb("xkcd:bluish"),
            )
            artists[1].remove()
            artists[1] = ax.plot_surface(
                *_reconstruct_sphere_coords(position_of_sphere[curr_frame], _sphere),
                color=to_rgb("xkcd:reddish"),
            )
            progress.update(animation_task, advance=1)
            return artists

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
