import numpy as np
from matplotlib import pyplot as plt
from matplotlib.colors import to_rgb
from rich.console import Console
from rich.progress import Progress
from rich.text import Text

from constant_magnetic_field_case_study import (
    run_constant_magnetic_field_case_study,
    ConstantMagneticFieldCaseStudy,
)


def run_example(
    save_to_csv: bool = False,
    save_figure: bool = False,
):
    # setting up magnetic properties
    magnetization_density = 1e5
    magnetic_field_angle = 2 * np.pi / 3
    magnetic_field_amplitude = 1e-2

    with Progress() as progress:
        position, _, times, velocity_norms = run_constant_magnetic_field_case_study(
            magnetization_density=magnetization_density,
            magnetic_field_amplitude=magnetic_field_amplitude,
            magnetic_field_angle=magnetic_field_angle,
            progress_ctx=progress,
        )

    with plt.style.context("ggplot"):
        fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)

        ax = fig.add_subplot(111)
        ax.set_xlabel("x")
        ax.set_ylabel("y")

        ax.plot(
            position[0, ...],
            position[1, ...],
            c=to_rgb("xkcd:reddish"),
            linestyle="-",
            lw=2.0,
        )
        fig.show()

    with plt.style.context("ggplot"):
        norm_fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)

        ax = norm_fig.add_subplot(111)
        ax.set_xlabel("Time")
        ax.set_ylabel("Velocity norms")

        ax.semilogy(
            times,
            velocity_norms,
            c=to_rgb("xkcd:bluish"),
            marker="o",
            lw=2.0,
        )
        norm_fig.show()

    if save_figure:
        fig.savefig("magnetic_beam_profile.png")
        norm_fig.savefig("magnetic_beam_velocity_norms.png")

    if save_to_csv:
        np.savetxt("magnetic_beam_profile.csv", position, delimiter=",")
        np.savetxt(
            "magnetic_beam_velocity_norms.csv",
            np.c_[times, velocity_norms],
            delimiter=",",
        )

    plt.show()


if __name__ == "__main__":
    console = Console()
    text = Text(style="yellow")
    text.append(ConstantMagneticFieldCaseStudy.__doc__)
    console.print(text)

    run_example()
