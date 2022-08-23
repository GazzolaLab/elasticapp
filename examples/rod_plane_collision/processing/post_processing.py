from .stats import Stats, SimulationStats
from .animate import animate_rods, plot_energies

import numpy as np
from matplotlib import pyplot as plt


def post_process_and_block(
    params,
    rod_collection_stats: SimulationStats,
    save_to_csv: bool,
    save_figure: bool,
):
    energy_fig = plot_energies(
        rod_group_stats=rod_collection_stats, save_figure=save_figure
    )
    animation = animate_rods(
        rod_group_stats=rod_collection_stats,
        x_lim=(-params.length, params.length),
        y_lim=(-params.length, 1.5 * params.length),
        z_lim=(0.0, 1.5 * params.initial_min_distance_from_plane),
        save_figure=save_figure,
        frame_skip=5,
    )

    if save_to_csv:
        import pandas as pd

        for idx, rod_stats in enumerate(rod_collection_stats):
            # FIXME : DataFrame constructor not called properly
            pd.DataFrame(rod_stats).to_csv(f"rod_{idx}_states.csv")

    run_on_debug_mode = True
    if run_on_debug_mode:
        fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)
        ax = fig.add_subplot(111)
        time = np.asarray(rod_collection_stats[0].time)
        centroid_height = np.mean(np.asarray(rod_collection_stats[0].position), axis=2)[
            ..., 2
        ]
        ax.plot(time, centroid_height / params.radius)
        ax.set_xlabel("Time")
        ax.set_ylabel("Centroid height / Rod radius")
        if save_figure:
            fig.savefig("centroid.png")

    plt.show()

    return rod_collection_stats
