import itertools

from cli import track_simulation, replace_docstring, clickify

from elasticapp.Systems import (
    CosseratRods,
    MagnetizedCosseratRods,
    MagneticMillipedeCosseratRods,
    RigidBody,
)  # noqa
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
    n_elem_magnetic_rod = 10
    damping_constant_magnetic = 1.0
    length_magnetic = 1.5 / 2
    radius_magnetic = 0.15

    radius_backbone = 0.15
    damping_constant_backbone = 1.5
    density_magnetic_to_backbone = 5.0
    stiffness_magnetic_to_backbone = 1.0  # (2.0 / 3.0) ** 1

    magnetic_rod_spacing_ratio = 2.0
    number_magnetic_rods_per_backbone = 6
    number_magnetic_rods_spanwise = 4
    number_backbone_rods_for_every_magnetic_rod = 4

    mag_strength = 80e-3  # 40e-3
    mag_angular_frequency = np.deg2rad(5.0)
    # acc_gravity = -9.81 / 1000.0
    acc_gravity = -9.80665 / 10.0
    friction_coeff = 50.0
    contact_stiffness = 1e6
    # These things make the sphere stick to the ground, which is what we want
    contact_damping_normal = 1e3
    contact_damping_tangential = 1e3

    sphere_radius_to_length_ratio = 1.0
    sphere_offset_to_length_ratio = 2.0
    density_sphere_to_magnetic = 1.0

    # print(density_sphere_to_magnetic * 2e3 * (4.0 / 3.0) * np.pi * (sphere_radius_to_length_ratio * length_magnetic)**3 * acc_gravity / contact_stiffness)

    num_cycles = 5.0  # 3*5#25#0.001#10  # 4#1#8
    final_sim_time = num_cycles * 2 * np.pi / mag_angular_frequency
    dl = length_magnetic / n_elem_magnetic_rod
    dt = 0.05 * dl  # 1e-4 # 0.1 * dl # 0.01 * dl

    pyelastica_sample = False
    if pyelastica_sample:
        rendering_fps = 10
        sample_interval = int(1.0 / (rendering_fps * dt))
    else:
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
        sphere_radius_to_length_ratio=sphere_radius_to_length_ratio,
        sphere_offset_to_length_ratio=sphere_offset_to_length_ratio,
        density_sphere_to_magnetic=density_sphere_to_magnetic,
        final_sim_time=final_sim_time,
    )
    cs = MagneticMillipedesCaseStudy(params)

    def setup_stats():
        # Tuple of types of list of rods
        buffers = cs.communicate()
        # Tod
        return tuple(map(lambda x: [Stats() for _ in range(len(x))], buffers[:-1]))

    all_stats = setup_stats()

    def collect_global_stats_from(cas):
        for stats_of_rod_type in all_stats:
            for rod_stats in stats_of_rod_type:
                rod_stats.time.append(cas.current_time())
                rod_stats.step.append(cas.current_iteration())

    def collect_rod_stats(rod_stat, rod):
        rod_stat.position.append(np.copy(rod.get_position()))
        # rod_stat.velocity.append(np.copy(rod.get_acceleration()))
        # rod_stat.velocity.append(np.copy(rod.get_external_loads()))
        # rod_stat.torques.append(np.copy(rod.get_external_torques()))
        # rod_stat.int_forces.append(np.copy(rod.get_internal_loads()))
        # rod_stat.int_torques.append(np.copy(rod.get_internal_torques()))

    def collect_stats_from(cas):
        collect_global_stats_from(cas)

        ddd = cs.communicate()
        for stats_of_rod_type, rods in zip(all_stats, ddd[:-1]):
            for stat, r in zip(stats_of_rod_type, rods):
                collect_rod_stats(stat, r)
        all_stats[-1][0].velocity.append(ddd[-1])

    # initial
    collect_stats_from(cs)

    if False:
        rods = cs.communicate()
        rod = rods[2][0]

        def sa(_system):
            damping_constant = damping_constant_magnetic
            time_step = dt

            nodal_mass = np.copy(_system.get_mass())
            translational_damping_coefficient = np.exp(-damping_constant * time_step)

            # Compute the damping coefficient for exponential velocity
            element_mass = 0.5 * (nodal_mass[1:] + nodal_mass[:-1])
            element_mass[0] += 0.5 * nodal_mass[0]
            element_mass[-1] += 0.5 * nodal_mass[-1]
            rotational_damping_coefficient = np.exp(
                -damping_constant
                * time_step
                * element_mass
                * np.copy(_system.get_inv_mass_second_moment_of_inertia())
            )
            return translational_damping_coefficient, rotational_damping_coefficient

        # print(np.asarray(rod.get_magnetization()))

        # print(sa(rod))

    if True:
        for _ in track_simulation(range(n_steps)):
            cs.run_for(additional_time=time_interval)
            collect_stats_from(cs)

    f = plt.figure()
    ax = f.add_subplot(111)
    ax.plot(all_stats[-1][0].time, np.asarray(all_stats[-1][0].position)[:, -1, 0])
    # ax.plot(all_stats[-1][0].time, np.asarray(all_stats[-1][0].velocity)[:, -1, 0])
    # Is actually penetration.
    ax.plot(all_stats[-1][0].time, np.asarray(all_stats[-1][0].velocity))
    f.show()

    rods = cs.communicate()

    spheres = False
    if spheres:
        animation = animate_rods(
            rod_group_stats=list(itertools.chain(*all_stats[:-1])),
            sphere_stats=all_stats[-1],
            x_lim=(0.0, 8.0),
            y_lim=(-2.0, 2.0),
            z_lim=(0.0, 4.0),
            save_figure=fig,
            frame_skip=5,
        )
    else:
        animation = animate_rods(
            rod_group_stats=list(itertools.chain(*all_stats)),
            x_lim=(0.0, 8.0),
            y_lim=(-2.0, 2.0),
            z_lim=(0.0, 4.0),
            save_figure=fig,
            frame_skip=5,
        )

    if data:
        # for idx, rod_stats in enumerate(itertools.chain(*all_stats)):
        #     pd.DataFrame(vars(rod_stats)).to_csv(f"rod_{idx}_states.csv")

        magnetic_stats = all_stats[2]
        time = np.asarray(magnetic_stats[0].time)
        magnetic_rods_position_history = np.asarray(
            list(map(lambda x: x.position, magnetic_stats))
        )
        # magnetic_rods_loads_history = np.asarray(list(map(lambda x: x.velocity, magnetic_stats)))
        # magnetic_rods_torques_history = np.asarray(list(map(lambda x: x.torques, magnetic_stats)))
        # magnetic_rods_internal_loads_history = np.asarray(list(map(lambda x: x.int_forces, magnetic_stats)))
        # magnetic_rods_internal_torques_history = np.asarray(list(map(lambda x: x.int_torques, magnetic_stats)))
        save_dict = {
            "time": time,
            "magnetic_rods_position_history": magnetic_rods_position_history,
            # "magnetic_rods_loads_history": magnetic_rods_loads_history,
            # "magnetic_rods_torques_history": magnetic_rods_torques_history,
            # "magnetic_rods_internal_loads_history": magnetic_rods_internal_loads_history,
            # "magnetic_rods_internal_torques_history": magnetic_rods_internal_torques_history,
        }

        if False:
            backbone_rods_first_layer_stats = all_stats[0]
            backbone_rods_first_layer_position_history = np.asarray(
                list(map(lambda x: x.position, backbone_rods_first_layer_stats))
            )
            backbone_rods_first_layer_loads_history = np.asarray(
                list(map(lambda x: x.velocity, backbone_rods_first_layer_stats))
            )
            backbone_rods_first_layer_torques_history = np.asarray(
                list(map(lambda x: x.torques, backbone_rods_first_layer_stats))
            )
            backbone_rods_first_layer_internal_loads_history = np.asarray(
                list(map(lambda x: x.int_forces, backbone_rods_first_layer_stats))
            )
            backbone_rods_first_layer_internal_torques_history = np.asarray(
                list(map(lambda x: x.int_torques, backbone_rods_first_layer_stats))
            )
            save_dict.update(
                {
                    "backbone_rods_first_layer_position_history": backbone_rods_first_layer_position_history,
                    "backbone_rods_first_layer_loads_history": backbone_rods_first_layer_loads_history,
                    "backbone_rods_first_layer_torques_history": backbone_rods_first_layer_torques_history,
                    "backbone_rods_first_layer_internal_loads_history": backbone_rods_first_layer_internal_loads_history,
                    "backbone_rods_first_layer_internal_torques_history": backbone_rods_first_layer_internal_torques_history,
                }
            )

            backbone_rods_second_layer_stats = all_stats[1]
            backbone_rods_second_layer_position_history = np.asarray(
                list(map(lambda x: x.position, backbone_rods_second_layer_stats))
            )
            backbone_rods_second_layer_loads_history = np.asarray(
                list(map(lambda x: x.velocity, backbone_rods_second_layer_stats))
            )
            backbone_rods_second_layer_torques_history = np.asarray(
                list(map(lambda x: x.torques, backbone_rods_second_layer_stats))
            )
            backbone_rods_second_layer_internal_loads_history = np.asarray(
                list(map(lambda x: x.int_forces, backbone_rods_second_layer_stats))
            )
            backbone_rods_second_layer_internal_torques_history = np.asarray(
                list(map(lambda x: x.int_torques, backbone_rods_second_layer_stats))
            )
            save_dict.update(
                {
                    "backbone_rods_second_layer_position_history": backbone_rods_second_layer_position_history,
                    "backbone_rods_second_layer_loads_history": backbone_rods_second_layer_loads_history,
                    "backbone_rods_second_layer_torques_history": backbone_rods_second_layer_torques_history,
                    "backbone_rods_second_layer_internal_loads_history": backbone_rods_second_layer_internal_loads_history,
                    "backbone_rods_second_layer_internal_torques_history": backbone_rods_second_layer_internal_torques_history,
                }
            )

        np.savez("magnetic_decapot.npz", **save_dict)
        # for idx, rod_stats in enumerate(magnetic_stats):
        #     # vars(rod_stats).pop('velocity', None)
        #     # pd.DataFrame(vars(rod_stats)).to_csv(f"rod_{idx}_states.csv")
        #     np.savetxt(f"rod_{idx}_states.csv", np.asarray(rod_stats.position)[-1, ...], delimiter=',')

    plt.show()


if __name__ == "__main__":
    run_magnetic_millipede_carpet_case_study()
