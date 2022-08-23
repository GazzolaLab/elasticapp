import numpy as np
from elasticapp.Systems import CosseratRods  # noqa
from elasticapp.CaseStudies.PyHingeJointCaseStudy import HingeJointCaseStudy
from matplotlib import pyplot as plt

from cli import track_simulation, replace_docstring, clickify
from postprocess import plot_position, animate_scenario, Stats, VisualizationParams


def run_example(
    save_to_csv: bool = False,
    save_figure: bool = False,
):
    sample_interval = 1000

    n_elem = 10
    base_length = 0.2
    base_radius = 0.007
    dt = 1e-5
    final_time = 1.0
    time_interval = sample_interval * dt
    n_steps = int(final_time / time_interval)

    params = HingeJointCaseStudy.params(
        n_elements=n_elem,
        radius=base_radius,
        length=base_length,
        dt=dt,
    )
    cs = HingeJointCaseStudy(params)

    first_rod_stats = Stats()
    second_rod_stats = Stats()
    rod_stats = (first_rod_stats, second_rod_stats)

    def collect(cas):
        for stats in rod_stats:
            stats.time.append(cas.current_time())
            stats.step.append(cas.current_iteration())

    def add_to(s, r):
        s.position.append(np.copy(r.get_position()))
        s.velocity.append(np.copy(r.get_velocity()))

    def collect_more(*rods):
        for stat, r in zip(rod_stats, rods):
            add_to(stat, r)

    def collect_stats_from(cas):
        collect(cas)
        collect_more(*cas.communicate())

    # initial
    collect_stats_from(cs)

    for _ in track_simulation(range(n_steps)):
        cs.run_for(additional_time=time_interval)
        collect_stats_from(cs)

    position_fig = plot_position(
        first_rod_stats,
        second_rod_stats,
        viz_params=VisualizationParams(save_figure, "spherical_joint_test.png"),
    )
    animation = animate_scenario(
        first_rod_stats,
        second_rod_stats,
        viz_params=VisualizationParams(save_figure=save_figure, frame_skip=5),
    )

    if save_to_csv:
        import pandas as pd

        # FIXME : DataFrame constructor not called properly
        pd.DataFrame(first_rod_stats).to_csv("hinge_joint_first.csv")
        pd.DataFrame(second_rod_stats).to_csv("hinge_joint_second.csv")

    plt.show()


@clickify()
@replace_docstring(HingeJointCaseStudy)
def run_case(
    data: bool,
    fig: bool,
):
    run_example(data, fig)


if __name__ == "__main__":
    run_case()
