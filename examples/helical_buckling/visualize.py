import numpy as np
from elasticapp import Blaze  # noqa
from elasticapp.CaseStudies.PyHelicalBucklingCaseStudy import HelicalBucklingCaseStudy

from cli.help import print_description
from cli.progress import track_simulation
from postprocess import plot_helicalbuckling_results as plot_results


def run_example(
    save_to_csv: bool = False,
    save_figure: bool = False,
):
    n_elem = 100
    base_length = 100.0
    dt = 0.001 * (base_length / n_elem)
    final_time = 1000.0
    damping_rate = 0.01

    n_steps_if_not_sampled = int(final_time / dt)
    sample_interval = n_steps_if_not_sampled // 100
    time_interval = sample_interval * dt
    n_steps = int(final_time / time_interval)

    # measure_index = (n_elem // 2) + 5
    # measure_index = 55

    cs = HelicalBucklingCaseStudy(
        n_elements=n_elem,
        dt=dt,
        damping_rate=damping_rate,
    )

    position_histories = []

    def collect_stats_from(cas):
        p, _, _, _ = cas.communicate()
        position_histories.append(np.copy(np.asarray(p)))

    for _ in track_simulation(range(n_steps)):
        cs.run_for(additional_time=time_interval)

    collect_stats_from(cs)

    if save_to_csv:
        np.savetxt(
            "final_position_histories.csv",
            np.asarray(position_histories),
            delimiter=",",
        )

    plot_results(n_elem, base_length, position_histories.pop(), save_figure=save_figure)


if __name__ == "__main__":
    print_description(HelicalBucklingCaseStudy)
    run_example()
