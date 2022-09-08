import click
from cli import replace_docstring, p_map
from helpers.cli import clickify
from helpers import (
    simulate_dynamic_cantilever,
    setup_dynamic_cantilever_parameters,
    DynamicCantileverCaseStudy,
    RecordingParameters,
    ModeRange,
)
from helpers.processing import analyze_frequencies, plot_tip_displacement, bode_plot

import matplotlib.pyplot as plt


def run_case_study(params, *args):
    """Helper to do starmap"""
    return simulate_dynamic_cantilever(*params, *args)


@clickify(
    click.option(
        "--time",
        default=100.0,
        help="Final time (in s)",
        type=click.FloatRange(0.0, max_open=True),
    )
)
@replace_docstring(DynamicCantileverCaseStudy)
def run_and_visualize(
    mode: int,
    time: float,
    data: bool,
    fig: bool,
):
    pars = {
        "params": setup_dynamic_cantilever_parameters(density=500.0, mode=mode),
        "record_params": RecordingParameters(sample_interval=100),
    }
    tstats = p_map(
        run_case_study,
        [[time, *pars.values()]],
        max_workers=1,
    )
    # There is only one case
    stats = tstats[0]

    frequency_results = analyze_frequencies(stats=stats, **pars)
    if data:
        import pandas as pd

        pd.DataFrame(frequency_results).to_csv("frequency_results.csv")

    tip_disp_fig = plot_tip_displacement(pars["params"], stats)
    bode_plot_fig = bode_plot(pars["params"], frequency_results)

    if fig:
        tip_disp_fig.savefig("tip_displacement.png")
        bode_plot_fig.savefig("bode_plot.png")

    plt.show()


if __name__ == "__main__":
    run_and_visualize()
