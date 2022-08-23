import numpy as np
import click

from cli import replace_docstring, clickify
from processing import run_rod_collision_case_study, RodCollisionCaseStudy


@clickify(
    click.option(
        "--inclination",
        default=np.deg2rad(45.0),
        type=float,
        help="Inclination angle between rods",
    )
)
@replace_docstring(RodCollisionCaseStudy)
def run_case_study(
    inclination: float,
    data: bool,
    fig: bool,
):
    return run_rod_collision_case_study(inclination, data, fig)


if __name__ == "__main__":
    run_case_study()
