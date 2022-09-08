from typing import Dict

import matplotlib.pyplot as plt
import numpy as np
from rich.console import Console
from rich.text import Text

from helpers import (
    simulate_dynamic_cantilever,
    setup_dynamic_cantilever_parameters,
    RecordingParameters,
)
from helpers.cli import clickify
from helpers.processing import (
    analyze_frequencies,
    density_frequency_plot,
    density_amplitude_plot,
)


def run_case(density: float, mode: int, *args) -> Dict[str, float]:
    pars = {
        "params": setup_dynamic_cantilever_parameters(density=density, mode=mode),
        "record_params": RecordingParameters(sample_interval=100),
    }
    final_time = 300.0
    stats = simulate_dynamic_cantilever(
        final_time, pars["params"], pars["record_params"], *args
    )
    frequency_results = analyze_frequencies(stats=stats, **pars)

    # Get only frequencies and amplitudes
    return {
        **{
            "density": density,
            "mode": mode,
        },
        **{
            k: frequency_results[k]
            for k in (
                "simulated_frequency",
                "theoretical_frequency",
                "simulated_amplitude",
                "theoretical_amplitude",
            )
        },
    }


def run_case_study(params, *args):
    """Helper to do starmap"""
    return run_case(*params, *args)


@clickify()
def run_phase_space(mode: int, data: bool, fig: bool):
    """Runs a density-mode phase-space for dynamic cantilever"""
    from cli.progress import p_map

    # density = 500, 1000, 2000, 3000, 4000, 5000 kg/m3
    densities = [500.0]
    densities.extend(np.arange(1000.0, 6000.0, 1000.0))

    console = Console()
    text = Text("Spanning phase space", style="yellow")
    console.print(text)

    from itertools import product

    # List of dicts
    results = p_map(run_case_study, product(densities, [mode]))

    if data:
        import pandas as pd

        pd.DataFrame(results).to_csv("results_phase_space.csv")

    def result_slice(x):
        return [r[x] for r in results]

    df_fig = density_frequency_plot(
        densities,
        result_slice("theoretical_frequency"),
        result_slice("simulated_frequency"),
    )
    da_fig = density_amplitude_plot(
        densities,
        result_slice("theoretical_amplitude"),
        result_slice("simulated_amplitude"),
    )
    plt.show()

    if fig:
        df_fig.savefig("frequency_phase_space.png")
        da_fig.savefig("amplitude_phase_space.png")


if __name__ == "__main__":
    run_phase_space()
