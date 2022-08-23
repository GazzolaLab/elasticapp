import click
from click.decorators import FC


def clickify(*args: FC):
    def dec(obj):
        obj = click.command()(obj)
        # unpack recursively from left to right
        for arg in args:
            obj = arg(obj)
        obj = click.option(
            "--data",
            is_flag=True,
            show_default=True,
            default=False,
            help="Save data in workspace",
        )(obj)
        obj = click.option(
            "--fig",
            is_flag=True,
            show_default=True,
            default=False,
            help="Save figures in workspace",
        )(obj)
        return obj

    return dec
