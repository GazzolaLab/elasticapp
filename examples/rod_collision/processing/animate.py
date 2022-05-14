import numpy as np
from matplotlib import pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.colors import to_rgb
from rich.progress import Progress

from .stats import Stats, PositiveInt


def animate_collision(
    rod1_stats: Stats,
    rod2_stats: Stats,
    video_name: str = "collision.mp4",
    fps: PositiveInt = 15,
    frame_skip: PositiveInt = 1,
    save_figure: bool = False,
) -> FuncAnimation:  # (time step, x/y/z, node)
    time = np.asarray(rod1_stats.time)
    step = np.asarray(rod1_stats.step)
    position_of_rod1 = np.asarray(rod1_stats.position)
    position_of_rod2 = np.asarray(rod2_stats.position)

    with plt.style.context("ggplot"):
        fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)
        ax = plt.axes(projection="3d")  # fig.add_subplot(111)
        ax.set_xlim(-0.2, 0.2)
        ax.set_ylim(-0.15, 0.65)
        ax.set_zlim(-0.4, 0.4)
        ax.set_box_aspect((1, 2, 2))

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
            "[magenta] Animating...", total=n_frames // frame_skip, visible=False
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
            progress.update(animation_task, advance=1, refresh=True)
            return text_time, text_step, rod1_line, rod2_line

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
