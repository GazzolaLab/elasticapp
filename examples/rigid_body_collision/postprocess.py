import numpy as np
from matplotlib import pyplot as plt
from matplotlib.colors import to_rgb


def plot_position(
    sphere1_stats,
    sphere2_stats,
    save_figure,
    filename="spherical_joint_test.png",
):
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

    return


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

    return


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


def plot_video(
    sphere1_stats,
    sphere2_stats,
    video_name="sphere_collision.mp4",
    fps=15,
):  # (time step, x/y/z, node)
    from matplotlib.animation import FuncAnimation  # noqa
    from tqdm import tqdm  # noqa

    time = np.asarray(sphere1_stats.time)
    step = np.asarray(sphere1_stats.step)
    position_of_sphere1 = np.asarray(sphere1_stats.position)
    position_of_sphere2 = np.asarray(sphere2_stats.position)
    _sphere1 = sphere1_stats.radius * _construct_sphere_coords()
    _sphere2 = sphere2_stats.radius * _construct_sphere_coords()

    with plt.style.context("ggplot"):
        fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)
        ax = plt.axes(projection="3d")  # fig.add_subplot(111)
        ax.set_box_aspect((1, 1, 1))
        ax.set_xlim(-0.8, 0.8)
        ax.set_ylim(-0.2, 0.2)
        ax.set_zlim(-0.2, 0.2)

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

    ax.legend()

    def update(curr_frame, spheres):
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
        pbar.update()

    n_frames = len(time)
    pbar = tqdm(total=n_frames)
    spheres = [sphere1, sphere2]
    ani = FuncAnimation(fig, update, frames=n_frames, fargs=(spheres,))

    ani.save(video_name, fps=fps, extra_args=["-vcodec", "h264", "-pix_fmt", "yuv420p"])
    pbar.close()

    return
