from matplotlib import pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.colors import to_rgb

import numpy as np

from rich.progress import Progress

from stats import Stats, PositiveInt


def animate_magnetic_rod(
    rod_stats: Stats,
    rod_length: float,
    video_name: str = "oscillating_magnetic_rod.mp4",
    fps: PositiveInt = 15,
    frame_skip: PositiveInt = 1,
    save_figure: bool = False,
) -> FuncAnimation:  # (time step, x/y/z, node)
    time = np.asarray(rod_stats.time)
    position_of_rod = np.asarray(rod_stats.position)

    with plt.style.context("ggplot"):
        fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)
        ax = plt.axes(projection="3d")  # fig.add_subplot(111)
        ax.set_xlim(-1.25 * rod_length, 1.25 * rod_length)
        ax.set_ylim(-1.25 * rod_length, 1.25 * rod_length)
        ax.set_zlim(-rod_length, 1.5 * rod_length)
        ax.set_box_aspect((2, 2, 2))
        ax.set_xlabel("X")
        ax.set_ylabel("Y")
        ax.set_zlabel("Z")

        (rod_line,) = ax.plot(
            [],
            [],
            [],
            lw=3.0,
            c=to_rgb("xkcd:reddish"),
            solid_capstyle="round",
            label="rod",
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
            "[magenta] Animating...", total=n_frames // frame_skip, visible=False
        )

        def update(curr_frame):
            curr_time = time[curr_frame]
            text_time.set_text(f"Time : {curr_time:.2f}")
            rod_line.set_data(
                position_of_rod[curr_frame, 0], position_of_rod[curr_frame, 1]
            )
            rod_line.set_3d_properties(position_of_rod[curr_frame, 2])
            progress.update(animation_task, advance=1, refresh=True)
            return text_time, rod_line

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
