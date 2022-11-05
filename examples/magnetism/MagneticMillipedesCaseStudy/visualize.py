import itertools

from cli import track_simulation, replace_docstring, clickify

from elasticapp.Systems import CosseratRods, MagnetizedCosseratRods, MagneticMillipedeCosseratRods  # noqa
from elasticapp.CaseStudies.PyMagneticMillipedesCaseStudy import (
    MagneticMillipedesCaseStudy,
)

from matplotlib import pyplot as plt

import numpy as np

import pandas as pd

from post_processing.animate import animate_rods
from post_processing.stats import Stats


@clickify()
@replace_docstring(MagneticMillipedesCaseStudy.__doc__)
def run_magnetic_millipede_carpet_case_study(
        data: bool = False,
        fig: bool = False,
):
    n_elem_magnetic_rod = 4
    damping_constant_magnetic = 0.8
    length_magnetic = 1.0 / 2
    radius_magnetic = 0.05

    radius_backbone = 0.1
    damping_constant_backbone = 1.5
    density_magnetic_to_backbone = 5.0
    stiffness_magnetic_to_backbone = (2.0 / 3.0) ** 1

    magnetic_rod_spacing_ratio = 2.0
    number_magnetic_rods_per_backbone = 6
    number_magnetic_rods_spanwise = 4
    number_backbone_rods_for_every_magnetic_rod = 3

    mag_strength = 80e-3
    mag_angular_frequency = np.deg2rad(10.0)
    # acc_gravity = -9.81 / 1000.0
    acc_gravity = -1e0
    friction_coeff = 5.0
    contact_stiffness = 30.0
    contact_damping_normal = 1e1
    contact_damping_tangential = 1e1

    num_cycles = 5  # 3*5#25#0.001#10  # 4#1#8
    final_sim_time = num_cycles * 2 * np.pi / mag_angular_frequency
    dl = length_magnetic / n_elem_magnetic_rod
    dt = 0.02 * dl
    sample_interval = 5e2
    time_interval = sample_interval * dt
    n_steps = int(final_sim_time / time_interval)

    params = MagneticMillipedesCaseStudy.params(
        n_elements=n_elem_magnetic_rod,
        length_magnetic=length_magnetic,
        radius_magnetic=radius_magnetic,
        radius_backbone=radius_backbone,
        number_magnetic_rods_per_backbone=number_magnetic_rods_per_backbone,
        number_magnetic_rods_spanwise=number_magnetic_rods_spanwise,
        number_backbone_rods_for_every_magnetic_rod=number_backbone_rods_for_every_magnetic_rod,
        magnetic_rod_spacing_ratio=magnetic_rod_spacing_ratio,
        damping_constant_magnetic=damping_constant_magnetic,
        damping_constant_backbone=damping_constant_backbone,
        density_magnetic_to_backbone=density_magnetic_to_backbone,
        stiffness_magnetic_to_backbone=stiffness_magnetic_to_backbone,
        dt=dt,
        contact_stiffness=contact_stiffness,
        contact_damping_normal=contact_damping_normal,
        contact_damping_tangential=contact_damping_tangential,
        acc_gravity=acc_gravity,
        mag_strength=mag_strength,
        mag_angular_frequency=mag_angular_frequency,
        friction_coeff=friction_coeff,
        final_sim_time=final_sim_time
    )
    cs = MagneticMillipedesCaseStudy(params)

    def setup_stats():
        # Tuple of types of list of rods
        criss_rods, cross_rods, magnetic_rods = cs.communicate()
        # Tod
        return [Stats() for _ in range(len(criss_rods))], \
               [Stats() for _ in range(len(cross_rods))], \
               [Stats() for _ in range(len(magnetic_rods))]

    all_stats = setup_stats()

    def collect_global_stats_from(cas):
        for stats_of_rod_type in all_stats:
            for rod_stats in stats_of_rod_type:
                rod_stats.time.append(cas.current_time())
                rod_stats.step.append(cas.current_iteration())

    def collect_rod_stats(rod_stat, rod):
        rod_stat.position.append(np.copy(rod.get_position()))

    def collect_stats_from(cas):
        collect_global_stats_from(cas)
        for stats_of_rod_type, rods in zip(all_stats, cs.communicate()):
            for stat, r in zip(stats_of_rod_type, rods):
                collect_rod_stats(stat, r)

    # initial
    collect_stats_from(cs)

    if True:
        for _ in track_simulation(range(n_steps)):
            cs.run_for(additional_time=time_interval)
            collect_stats_from(cs)

    animation = animate_rods(
        rod_group_stats=list(itertools.chain(*all_stats)),
        x_lim=(0.0, 8.0),
        y_lim=(-2.0, 2.0),
        z_lim=(0.0, 4.0),
        save_figure=fig,
        frame_skip=5,
    )

    if data:
        for idx, rod_stats in enumerate(itertools.chain(*all_stats)):
            pd.DataFrame(vars(rod_stats)).to_csv(f"rod_{idx}_states.csv")

    plt.show()


if __name__ == "__main__":
    run_magnetic_millipede_carpet_case_study()
