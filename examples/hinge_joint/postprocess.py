import numpy as np
from matplotlib import pyplot as plt
from matplotlib.colors import to_rgb


def plot_position(
        rod1_stats,
        rod2_stats,
        save_figure,
        filename="spherical_joint_test.png",
):
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

        fig.show()

    if save_figure:
        fig.savefig(filename)

    return


def plot_video(
        rod1_stats,
        rod2_stats,
        video_name="hinge_joint.mp4",
        fps=15,
):  # (time step, x/y/z, node)
    from matplotlib.animation import FuncAnimation  # noqa
    from tqdm import tqdm  # noqa

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

    ax.legend()

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
        pbar.update()

    n_frames = len(time)
    pbar = tqdm(total=n_frames)
    ani = FuncAnimation(fig, update, frames=n_frames)

    ani.save(video_name, fps=fps, extra_args=["-vcodec", "h264", "-pix_fmt", "yuv420p"])
    pbar.close()

    return
