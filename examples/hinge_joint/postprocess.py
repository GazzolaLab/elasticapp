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
    time: List[float] = field(default_factory=list)
    step: List[PositiveInt] = field(default_factory=list)
    position: List[np.ndarray] = field(default_factory=list)
    velocity: List[np.ndarray] = field(default_factory=list)


@dataclass
class VisualizationParams:
    save_figure: bool = False
    file_name: str = "hinge_joint.mp4"
    frame_skip: PositiveInt = PositiveInt(1)
    fps: PositiveInt = PositiveInt(15)


def plot_position(
    rod1_stats: Stats,
    rod2_stats: Stats,
    viz_params: VisualizationParams = VisualizationParams(
        False, "spherical_joint_test.png"
    ),
) -> plt.Figure:
    position_of_rod1 = np.asarray(rod1_stats.position)
    position_of_rod2 = np.asarray(rod2_stats.position)

    with plt.style.context("ggplot"):
        fig = plt.figure(figsize=(5, 5), frameon=True, dpi=150)
        ax = fig.add_subplot(111)

        ax.plot(
            position_of_rod1[:, 0, -1], position_of_rod1[:, 1, -1], "r-", label="rod1"
        )
        ax.plot(
            position_of_rod2[:, 0, -1],
            position_of_rod2[:, 1, -1],
            c=to_rgb("xkcd:bluish"),
            label="rod2",
        )
        ax.set_xlabel("x")
        ax.set_ylabel("y")

        fig.show()

    if viz_params.save_figure:
        fig.savefig(viz_params.file_name)

    return fig


def animate_scenario(
    rod1_stats: Stats,
    rod2_stats: Stats,
    *,
    viz_params: VisualizationParams = VisualizationParams(),
    **kwargs,
) -> FuncAnimation:  # (time step, x/y/z, node)

    time = np.asarray(rod1_stats.time)
    step = np.asarray(rod1_stats.step)
    position_of_rod1 = np.asarray(rod1_stats.position)
    position_of_rod2 = np.asarray(rod2_stats.position)

    with plt.style.context("ggplot"):
        fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)
        ax = plt.axes(projection="3d")  # fig.add_subplot(111)
        ax.set_xlim(-0.25, 0.25)
        ax.set_ylim(-0.25, 0.25)
        ax.set_zlim(0, 0.4)

        (rod1_line,) = ax.plot(
            [],
            [],
            [],
            lw=2.0,
            c=to_rgb("xkcd:reddish"),
            solid_capstyle="round",
            label="rod1",
        )
        (rod2_line,) = ax.plot(
            [],
            [],
            [],
            lw=2.0,
            c=to_rgb("xkcd:bluish"),
            solid_capstyle="round",
            label="rod2",
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

    ax.legend()

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
            rod1_line.set_data(
                position_of_rod1[curr_frame, 0], position_of_rod1[curr_frame, 1]
            )
            rod1_line.set_3d_properties(position_of_rod1[curr_frame, 2])
            rod2_line.set_data(
                position_of_rod2[curr_frame, 0], position_of_rod2[curr_frame, 1]
            )
            rod2_line.set_3d_properties(position_of_rod2[curr_frame, 2])
            progress.update(animation_task, advance=1)
            return text_time, text_step, rod1_line, rod2_line

        ani = FuncAnimation(
            fig, update, frames=range(0, n_frames, viz_params.frame_skip), blit=True
        )
        if viz_params.save_figure:
            progress.console.print(
                "[red]Rendering animation to file. It will be displayed on-screen once completed."
            )
            progress.update(animation_task, visible=True)
            kwargs.update({"fps": kwargs.get("fps", viz_params.fps)})
            ani.save(
                viz_params.file_name,
                extra_args=["-vcodec", "h264", "-pix_fmt", "yuv420p"],
                **kwargs,
            )

    return ani
