import click

from cli import replace_docstring, clickify
from processing.visualize import run_snake_pit, SnakePit


@clickify(
    click.option(
        "--snakes",
        default=1,
        type=int,
        help="Number of snakes",
    ),
)
@replace_docstring(SnakePit)
def run_case_study(
    snakes: int,
    data: bool,
    fig: bool,
):
    return run_snake_pit(snakes, data, fig)


if __name__ == "__main__":
    run_case_study()
