import click
from .analytical import ModeRange


def clickify(*args):
    from cli import clickify

    def dec(obj):
        opts = (
            click.option(
                "--mode",
                default=0,
                help="Integral frequency mode",
                type=click.IntRange(ModeRange.min, ModeRange.max - 1),
            ),
        )
        obj = clickify(*opts, *args)(obj)

        return obj

    return dec
