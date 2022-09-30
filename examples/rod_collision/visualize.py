import numpy as np
import click

from cli import replace_docstring, clickify
from processing import run_rod_collision_case_study, RodCollisionCaseStudy


@clickify(
    click.option(
        "--inclination",
        default=45.0,
        type=float,
        help="Inclination angle between rods in degrees",
    ),
    click.option(
        "--pairs",
        default=1,
        type=int,
        help="Number of colliding rod pairs",
    ),
)
@replace_docstring(RodCollisionCaseStudy)
def run_case_study(
    inclination: float,
    pairs: int,
    data: bool,
    fig: bool,
):
    return run_rod_collision_case_study(np.deg2rad(inclination), pairs, data, fig)


if __name__ == "__main__":
    run_case_study()
