import click
import numpy as np
from elastica_pipelines import io
from typing import NewType, List
from dataclasses import dataclass, field
from matplotlib import pyplot as plt
from matplotlib.animation import FuncAnimation
from rich.progress import Progress

PositiveInt = NewType("PositiveInt", int)


@dataclass
class RodStats:
    position: List[np.ndarray] = field(default_factory=list)


@dataclass
class Stats:
    rod_stats: List[RodStats]
    time: List[float] = field(default_factory=list)
    step: List[PositiveInt] = field(default_factory=list)

def animate_snake(
        total_stats: Stats,
        video_name: str = "collision.mp4",
        fps: PositiveInt = 15,
        frame_skip: PositiveInt = 1,
        save_figure: bool = False,
) -> FuncAnimation:  # (time step, x/y/z, node)
    time = np.asarray(total_stats.time)
    step = np.asarray(total_stats.step)
    rod_lines = []

    with plt.style.context("ggplot"):
        fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)
        ax = plt.axes(projection="3d")  # fig.add_subplot(111)
        # ax.set_xlim(-0.2, 0.2)
        # ax.set_ylim(-0.15, 0.65)
        # ax.set_zlim(-0.4, 0.4)
        ax.set_xlim(-0.4, 0.4)
        ax.set_ylim(-0.4, 0.4)
        ax.set_zlim(0.0, 0.8)
        ax.set_box_aspect((1, 1, 1))
        ax.set_xlabel('X')
        ax.set_ylabel('Y')
        ax.set_zlabel('Z')

        for _ in range(len(total_stats.rod_stats)):
            rod_lines.append(ax.plot(
                [],
                [],
                [],
                lw=2.0,
                solid_capstyle="round",
            )[0])

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
        text_nrods = ax.text2D(
            0.2,
            0.7,
            " ",
            color="k",
            horizontalalignment="center",
            verticalalignment="center",
            transform=ax.transAxes,
        )
        fig.tight_layout()

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
            total_rods = len(total_stats.rod_stats)
            for l, r in zip(rod_lines, total_stats.rod_stats):
                position_of_rod = np.asarray(r.position)
                # Tangent is along z
                l.set_data(
                    position_of_rod[curr_frame, 0], position_of_rod[curr_frame, 1]
                )
                l.set_3d_properties(position_of_rod[curr_frame, 2])
                total_rods = total_rods - int(np.any(np.isnan(position_of_rod)))

            text_nrods.set_text(f"Rods : {total_rods:03d}")
            progress.update(animation_task, advance=1, refresh=True)
            return text_time, text_step, text_nrods, *rod_lines

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

def analyze(metadata_fn):
    series = io.series(metadata=metadata_fn, transforms=io.transforms.ToArray())

    # Count number of rods, 0 is always available
    n_rods = len(series[0].cosserat_rods())
    stats = Stats([RodStats() for _ in range(n_rods)])

    for t, snapshot in series.iterations():
        stats.time.append(t.time)
        stats.step.append(t.iterate)
        for st, rod in zip(stats.rod_stats, snapshot.cosserat_rods().values()):
            st.position.append(rod["Position"])

    animation = animate_snake(stats)
    plt.show()

@click.command()
@click.option(
    "--name",
    type=str,
    help="Path to the metadata",
)
def run_analysis(name):
    analyze(name)


if __name__ == "__main__":
    run_analysis()
