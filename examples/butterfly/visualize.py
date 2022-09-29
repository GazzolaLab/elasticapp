import numpy as np
from elasticapp.Systems import CosseratRods  # noqa
from elasticapp.CaseStudies.PyButterflyCaseStudy import ButterflyCaseStudy
from matplotlib import pyplot as plt

from cli import track_simulation, replace_docstring, clickify
from postprocess import Stats, plot_trajectory, plot_energies
import click


@clickify(
    click.option("--inclination", default=45.0, type=float, help="Inclination angle in degrees")
)
@replace_docstring(ButterflyCaseStudy.__doc__)
def run_example(
    inclination: float,
    data: bool,
    fig: bool,
):
    np.set_printoptions(floatmode="unique")
    sample_interval = 20

    n_elem = 2
    base_length = 3.0
    dt = 0.01 * (base_length / n_elem)
    final_time = 50.0
    time_interval = sample_interval * dt
    n_steps = int(final_time / time_interval)

    params = ButterflyCaseStudy.params(
        n_elements=n_elem,
        angle_of_inclination=np.deg2rad(inclination),
        length=base_length,
        dt=dt,
    )
    cs = ButterflyCaseStudy(params)

    stats = Stats()

    def collect(t):
        stats.times.append(t)

    def collect_more(r: CosseratRods.CosseratRod):
        stats.positions.append(np.copy(r.get_position()))
        stats.bend_e.append(r.compute_bending_energy())
        stats.shear_e.append(r.compute_shearing_energy())
        stats.translation_e.append(r.compute_translational_energy())
        stats.rotation_e.append(r.compute_rotational_energy())

    def collect_stats_from(cas):
        collect(cas.current_time())
        collect_more(cas.communicate())

    # initial
    collect_stats_from(cs)

    for _ in track_simulation(range(n_steps)):
        cs.run_for(additional_time=time_interval)

        collect_stats_from(cs)

    plot_trajectory(stats, save_figure=fig)
    plot_energies(stats, save_figure=fig)

    if data:
        import pandas as pd

        # FIXME : DataFrame constructor not called properly
        pd.DataFrame(stats).to_csv("butterfly.csv")

    plt.show()


if __name__ == "__main__":
    run_example()
