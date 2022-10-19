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
class Stats:
    n_rods: int
    series: io.temporal.Series


def animate_snake(
    total_stats: Stats,
    video_name: str = "collision.mp4",
    fps: PositiveInt = 15,
    frame_skip: PositiveInt = 1,
    save_figure: bool = False,
) -> FuncAnimation:  # (time step, x/y/z, node)
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
        ax.set_xlabel("X")
        ax.set_ylabel("Y")
        ax.set_zlabel("Z")
        ax.azim = -45.0
        ax.elev = 60.0

        for _ in range(total_stats.n_rods):
            rod_lines.append(
                ax.plot(
                    [],
                    [],
                    [],
                    lw=2.0,
                    solid_capstyle="round",
                )[0]
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

    n_frames = len(total_stats.series)
    with Progress() as progress:
        animation_task = progress.add_task(
            "[magenta] Animating...", total=n_frames // frame_skip, visible=False
        )

        def update(curr):
            t, snapshot = curr
            curr_time = t.time
            curr_step = t.iterate
            text_step.set_text(f"Step : {curr_step:04d}")
            text_time.set_text(f"Time : {curr_time:.2f}")
            total_rods = total_stats.n_rods
            for l, r in zip(rod_lines, snapshot.rods().values()):
                position_of_rod = np.asarray(r["Position"])
                # Tangent is along z
                l.set_data(position_of_rod[0], position_of_rod[1])
                l.set_3d_properties(position_of_rod[2])
                total_rods = total_rods - int(np.any(np.isnan(position_of_rod)))

            text_nrods.set_text(f"Rods : {total_rods:03d}")
            progress.update(animation_task, advance=1, refresh=True)
            return text_time, text_step, text_nrods, *rod_lines

        from itertools import islice

        ani = FuncAnimation(
            fig,
            update,
            frames=islice(total_stats.series.iterations(), 0, n_frames, frame_skip),
            blit=True,
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


def draw_large(snapshot_with_metadata, n_rods):
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
        ax.set_xlabel("X")
        ax.set_ylabel("Y")
        ax.set_zlabel("Z")
        ax.azim = -45.0
        ax.elev = 60.0

        rod_lines = []
        for _ in range(n_rods):
            rod_lines.append(
                ax.plot(
                    [],
                    [],
                    [],
                    lw=2.0,
                    solid_capstyle="round",
                )[0]
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

        t, snapshot = snapshot_with_metadata
        curr_time = t.time
        curr_step = t.iterate
        text_step.set_text(f"Step : {curr_step:04d}")
        text_time.set_text(f"Time : {curr_time:.2f}")
        total_rods = n_rods
        for l, r in zip(rod_lines, snapshot.rods().values()):
            position_of_rod = np.asarray(r["Position"])
            # Tangent is along z
            l.set_data(position_of_rod[0], position_of_rod[1])
            l.set_3d_properties(position_of_rod[2])
            total_rods = total_rods - int(np.any(np.isnan(position_of_rod)))

        text_nrods.set_text(f"Rods : {total_rods:03d}")
        fig.savefig(f"snap_{curr_step:010d}.png", format="png", dpi=250)
        # fig.close()
        plt.close("all")


def draw_large_driver(total_stats, regions, section_id):
    from itertools import islice

    for snapshot_with_metadata in islice(
        total_stats.series.iterations(), *regions[section_id]
    ):
        draw_large(snapshot_with_metadata, total_stats.n_rods)


def animate_large(
    total_stats: Stats,
    video_name: str = "collision.mp4",
    fps: PositiveInt = 15,
    frame_skip: PositiveInt = 1,
    **kwargs,
) -> int:  # (time step, x/y/z, node)

    from joblib import Parallel, delayed
    import multiprocessing

    # num_cores = multiprocessing.cpu_count()
    num_cores = 1
    n_frames = len(total_stats.series)

    def calculate_regions(section):
        work_per_section = n_frames // num_cores
        start = section * work_per_section
        stop = start + work_per_section
        skip = frame_skip
        return start, stop, skip

    regions = tuple(calculate_regions(section_id) for section_id in range(num_cores))

    Parallel(n_jobs=num_cores, backend="sequential")(
        delayed(draw_large_driver)(total_stats, regions, section_id)
        for section_id in (range(num_cores))
    )

    import os

    os.system(
        f"ffmpeg -r {fps} -y -s 3840x2160 -f image2 -pattern_type glob -i 'snap*.png' -vcodec libx264 -crf 15 -pix_fmt yuv420p -vf 'crop=trunc(iw/2)*2:trunc(ih/2)*2' {video_name}"
    )
    os.system("rm -f snap*.png")

    return 1


def analyze(metadata_fn: str, fps: PositiveInt):
    series = io.series(metadata=metadata_fn, transforms=io.transforms.ToArray())

    # Count number of rods, 0 is always available
    n_rods = len(series[0].rods())
    stats = Stats(n_rods, series)

    if n_rods < 128:
        # Funcanimation leaks memory and closes before the files can be written down.
        ani = animate_snake(stats, frame_skip=5, fps=fps, save_figure=False)
    else:
        ani = animate_large(stats, frame_skip=1, fps=fps, save_figure=False)
    plt.show()


@click.command()
@click.option(
    "--name",
    type=str,
    help="Path to the metadata",
)
@click.option(
    "--fps",
    default=15,
    type=int,
    help="Frames per second",
)
def run_analysis(name, fps):
    analyze(name, fps)


if __name__ == "__main__":
    run_analysis()
