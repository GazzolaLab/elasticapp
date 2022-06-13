import numpy as np
from matplotlib import pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.colors import to_rgb
from rich.progress import Progress

from .stats import Stats, PositiveInt, RodStats


def get_rod_colors():
    return to_rgb("xkcd:reddish"), to_rgb("xkcd:bluish")


def animate_collision(
    total_stats: Stats,
    video_name: str = "collision.mp4",
    fps: PositiveInt = 15,
    frame_skip: PositiveInt = 1,
    save_figure: bool = False,
) -> FuncAnimation:  # (time step, x/y/z, node)
    time = np.asarray(total_stats.time)
    step = np.asarray(total_stats.step)
    rod1_stats = total_stats.first_rod
    rod2_stats = total_stats.second_rod
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
            c=get_rod_colors()[0],
            solid_capstyle="round",
            label="rod1",
        )
        (rod2_line,) = ax.plot(
            [],
            [],
            [],
            lw=2.0,
            c=get_rod_colors()[1],
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


def plot_energies(stats: Stats, save_figure: bool = False) -> plt.Figure:
    with plt.style.context("ggplot"):
        new_fig = plt.figure(figsize=(5, 4), dpi=150)

        new_ax = new_fig.add_subplot(111)

        new_ax.set_xlabel("x")
        new_ax.set_ylabel("energies")

        times = np.asarray(stats.time)
        rod_stats = (stats.first_rod, stats.second_rod)
        linestyles = ("-", "--")
        rod_total_energies = []
        for stat, style in zip(rod_stats, linestyles):
            te = np.asarray(stat.translational_energy)
            re = np.asarray(stat.rotational_energy)
            be = np.asarray(stat.bending_energy)
            se = np.asarray(stat.shearing_energy)
            total_e = te + re + be + se
            rod_total_energies.append(total_e)

            new_ax.plot(
                times,
                te,
                c=to_rgb("xkcd:reddish"),
                lw=2.0,
                linestyle=style,
                label="Translations",
            )
            new_ax.plot(
                times,
                re,
                c=to_rgb("xkcd:bluish"),
                lw=2.0,
                linestyle=style,
                label="Rotation",
            )
            new_ax.plot(
                times,
                be,
                c=to_rgb("xkcd:burple"),
                lw=2.0,
                linestyle=style,
                label="Bend",
            )
            new_ax.plot(
                times,
                se,
                c=to_rgb("xkcd:goldenrod"),
                lw=2.0,
                linestyle=style,
                label="Shear",
            )
            new_ax.plot(
                times,
                total_e,
                c=to_rgb("xkcd:greenish"),
                lw=2.0,
                linestyle=style,
                label="Total",
            )

        # two springs in series
        k_collision = 0.5 * stats.contact_stiffness

        # at every instant there may actually be no or multiple penetrations
        def compute_pe(depths):
            return 0.5 * k_collision * np.sum(np.asarray(depths) ** 2)

        pe_collision = np.fromiter(
            map(compute_pe, stats.penetration_depth), dtype=float
        )

        new_ax.plot(
            times,
            pe_collision + rod_total_energies[0] + rod_total_energies[1],
            c="k",
            lw=2.0,
            label="Total energy",
        )

        new_fig.legend()

        new_fig.show()

    if save_figure:
        new_fig.savefig("energies.png")

    return new_fig


def _setup_momenta(r: RodStats):
    return np.sum(r.mass.reshape(1, 1, -1) * np.array(r.velocity), axis=-1)


def _setup_com_velocity(r: RodStats):
    return _setup_momenta(r) / np.sum(r.mass)


def plot_velocities(stats: Stats, save_figure: bool = False) -> plt.Figure:
    time = np.array(stats.time)
    com_velocity_rods = (
        _setup_com_velocity(stats.first_rod),
        _setup_com_velocity(stats.second_rod),
    )
    rod_colors = get_rod_colors()

    fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)
    axs = []
    axs.append(plt.subplot2grid((3, 1), (0, 0)))
    axs.append(plt.subplot2grid((3, 1), (1, 0)))
    axs.append(plt.subplot2grid((3, 1), (2, 0)))

    first_rod_index = 0
    second_rod_index = 1

    axs[0].plot(
        time[:],
        com_velocity_rods[first_rod_index][:, 0],
        lw=2,
        c=rod_colors[first_rod_index],
        label="rod_one",
    )
    axs[0].plot(
        time[:],
        com_velocity_rods[second_rod_index][:, 0],
        lw=2,
        c=rod_colors[second_rod_index],
        label="rod_two",
    )
    axs[0].set_ylabel("x com velocity")
    axs[0].legend()

    axs[1].plot(
        time[:],
        com_velocity_rods[first_rod_index][:, 1],
        c=rod_colors[first_rod_index],
        lw=2,
    )
    axs[1].plot(
        time[:],
        com_velocity_rods[second_rod_index][:, 1],
        c=rod_colors[second_rod_index],
        lw=2,
    )
    axs[1].set_ylabel("y com velocity")

    axs[2].plot(
        time[:],
        com_velocity_rods[first_rod_index][:, 2],
        c=rod_colors[first_rod_index],
        lw=2,
    )
    axs[2].plot(
        time[:],
        com_velocity_rods[second_rod_index][:, 2],
        c=rod_colors[second_rod_index],
        lw=2,
    )
    axs[2].set_ylabel("z com velocity")

    plt.tight_layout()
    fig.show()

    if save_figure:
        fig.savefig("velocities.png")

    return fig


def plot_momenta(stats: Stats, save_figure: bool = False) -> plt.Figure:
    time = np.array(stats.time)

    linear_momentum_rods = (
        _setup_momenta(stats.first_rod),
        _setup_momenta(stats.second_rod),
        _setup_momenta(stats.first_rod) + _setup_momenta(stats.second_rod),
    )
    rod_colors = get_rod_colors()

    fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)
    axs = []
    axs.append(plt.subplot2grid((3, 1), (0, 0)))
    axs.append(plt.subplot2grid((3, 1), (1, 0)))
    axs.append(plt.subplot2grid((3, 1), (2, 0)))

    first_rod_index = 0
    second_rod_index = 1
    total_index = 2

    axs[0].plot(
        time[:],
        linear_momentum_rods[first_rod_index][:, 0],
        lw=2,
        c=rod_colors[first_rod_index],
        label="rod_one",
    )
    axs[0].plot(
        time[:],
        linear_momentum_rods[second_rod_index][:, 0],
        lw=2,
        c=rod_colors[second_rod_index],
        label="rod_two",
    )
    axs[0].plot(
        time[:],
        linear_momentum_rods[total_index][:, 0],
        lw=2,
        c="k",
        label="total",
    )
    axs[0].set_ylabel("x momenta")
    axs[0].legend()

    axs[1].plot(
        time[:],
        linear_momentum_rods[first_rod_index][:, 1],
        lw=2,
        c=rod_colors[first_rod_index],
    )
    axs[1].plot(
        time[:],
        linear_momentum_rods[second_rod_index][:, 1],
        lw=2,
        c=rod_colors[second_rod_index],
    )
    axs[1].plot(
        time[:],
        linear_momentum_rods[total_index][:, 1],
        lw=2,
        c="k",
    )
    axs[1].set_ylabel("y momenta")

    axs[2].plot(
        time[:],
        linear_momentum_rods[first_rod_index][:, 2],
        c=rod_colors[first_rod_index],
        lw=2,
    )
    axs[2].plot(
        time[:],
        linear_momentum_rods[second_rod_index][:, 2],
        lw=2,
        c=rod_colors[second_rod_index],
    )
    axs[2].plot(
        time[:],
        linear_momentum_rods[total_index][:, 2],
        lw=2,
        c="k",
    )
    axs[2].set_ylabel("z momenta")

    plt.tight_layout()
    fig.show()

    if save_figure:
        fig.savefig("momenta.png")

    return fig
