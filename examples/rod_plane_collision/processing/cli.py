import click


def clickify():
    def dec(obj):
        obj = click.command()(obj)
        obj = click.option('--inclination', default=0.0, help='Inclination angle')(obj)
        obj = click.option('--csv', is_flag=True, show_default=True, default=False,
                           help='Save csv data in workspace')(obj)
        obj = click.option('--fig', is_flag=True, show_default=True, default=False,
                           help='Save figures in workspace')(obj)
        return obj

    return dec
