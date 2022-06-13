import numpy as np
import pandas as pd
from cli import track_simulation
from elasticapp.CaseStudies.PyRodCollisionCaseStudy import (
    RodCollisionCaseStudy,
)
from elasticapp.Systems import CosseratRods  # noqa
from matplotlib import pyplot as plt

from .animate import animate_collision, plot_energies, plot_velocities, plot_momenta
from .stats import Stats, RodStats


def run_rod_collision_case_study(
    angle_of_inclination: float = 0.0,
    save_to_csv: bool = False,
    save_figure: bool = False,
):
    """This is purely for debugging alone, and can be accessed if ELASTICA_DEBUG
    is set to true. This is left behind for testing purposes"""
    run_on_debug_mode = False
    if run_on_debug_mode:
        np.set_printoptions(floatmode="unique")

    sample_interval = 1e3

    dt = 5e-5
    contact_stiffness = 1e3
    contact_damping = 0.0
    final_time = 15.0
    time_interval = sample_interval * dt
    n_steps = int(final_time / time_interval)

    cs = RodCollisionCaseStudy(
        angle_of_inclination=angle_of_inclination,
        contact_stiffness=contact_stiffness,
        contact_damping=contact_damping,
        dt=dt,
    )

    def setup_stats(cas):
        if run_on_debug_mode:
            _, rods, _ = cas.communicate()
        else:
            _, rods = cas.communicate()
        keys = ("first_rod", "second_rod")
        values = (RodStats(np.copy(r.get_mass())) for r in rods)

        return Stats(contact_stiffness=contact_stiffness, **dict(zip(keys, values)))

    total_stats = setup_stats(cs)
    first_cosserat_rod_stats = total_stats.first_rod
    second_cosserat_rod_stats = total_stats.second_rod

    cosserat_rod_stats = (first_cosserat_rod_stats, second_cosserat_rod_stats)

    def collect_global_stats_from(cas):
        total_stats.time.append(cas.current_time())
        total_stats.step.append(cas.current_iteration())

    def add_to(s, rod):
        s.position.append(np.copy(rod.get_position()))
        s.velocity.append(np.copy(rod.get_velocity()))
        s.forces.append(np.copy(rod.get_external_loads()))
        s.translational_energy.append(rod.compute_translational_energy())
        s.rotational_energy.append(rod.compute_rotational_energy())
        s.bending_energy.append(rod.compute_bending_energy())
        s.shearing_energy.append(rod.compute_shearing_energy())

    # def process_depth(d):
    #     return d[0] if d else 0.0

    def collect_statistics_backend_agnostic(depths, rods):
        total_stats.penetration_depth.append(depths)
        for stat, pv in zip(cosserat_rod_stats, rods):
            add_to(stat, pv)

    if run_on_debug_mode:
        contact_ids = []
        primitive_positions = []
        primitive_axis = []
        primitive_radii = []
        primitive_lengths = []

        def collect_rod_stats(
            depths,
            rods,
            contact_pair_ids,
            primitive_pos,
            primitive_ax,
            primitive_r,
            primitive_l,
        ):
            collect_statistics_backend_agnostic(depths, rods)
            contact_ids.append(contact_pair_ids)
            primitive_positions.append(primitive_pos)
            primitive_axis.append(primitive_ax)
            primitive_radii.append(primitive_r)
            primitive_lengths.append(primitive_l)

    else:

        def collect_rod_stats(depths, rods):
            collect_statistics_backend_agnostic(depths, rods)

    def collect_stats_from(cas):
        collect_global_stats_from(cas)
        collect_rod_stats(*cas.communicate())

    # initial
    collect_stats_from(cs)

    for _ in track_simulation(range(n_steps)):
        cs.run_for(additional_time=time_interval)
        collect_stats_from(cs)

    # com velocity and momenta tell you similar things, but with momentum you can see
    # conservation better
    if False:
        com_velocity_fig = plot_velocities(stats=total_stats, save_figure=save_figure)
    else:
        momenta_fig = plot_momenta(stats=total_stats, save_figure=save_figure)
    energy_fig = plot_energies(stats=total_stats, save_figure=save_figure)
    animation = animate_collision(
        total_stats,
        save_figure=save_figure,
        frame_skip=5,
    )

    if run_on_debug_mode:

        def plot_velocity_norms():
            fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)
            ax = fig.add_subplot(111)
            time = np.asarray(total_stats.time)

            # (time, axis, dof)
            def compute_norm(p):
                return np.linalg.norm(np.linalg.norm(np.asarray(p), 2, axis=1), axis=-1)

            def n_penetrations(depths):
                # reduce list of lists to numpy array
                return np.asarray(list(map(lambda x: len(x), depths)))

            def sum_depth(depths):
                return np.asarray(list(map(lambda x: sum(x), depths)))

            ax.plot(time, compute_norm(first_cosserat_rod_stats.velocity))
            ax.plot(time, compute_norm(second_cosserat_rod_stats.velocity))
            ax.plot(time, compute_norm(first_cosserat_rod_stats.forces))
            ax.plot(time, compute_norm(second_cosserat_rod_stats.forces))
            ax.scatter(time, n_penetrations(total_stats.penetration_depth))
            ax.plot(
                time,
                sum_depth(total_stats.penetration_depth)
                / n_penetrations(total_stats.penetration_depth),
            )
            fig.show()

        plot_velocity_norms()

        contact_time = (0.08 / 0.05) + 2e-4  # roughly
        contact_time_idx = int(contact_time / time_interval)
        contact_time_idx_range = range(contact_time_idx - 2, contact_time_idx + 2)
        for idx in contact_time_idx_range:
            idd = contact_ids[idx]
            print("Contact IDs")
            print(idd)
            print("Peneteration depths")
            print(total_stats.penetration_depth[idx])
            if idd:
                print("Rods")
                print(np.asarray(first_cosserat_rod_stats.position)[idx, :, 0])
                print(np.asarray(first_cosserat_rod_stats.position)[idx, :, 1])
                # time, 10, 3
                print("Position")
                print(np.asarray(primitive_positions)[idx, 0, :])
                print(np.asarray(primitive_positions)[idx, 1, :])

                print("Axis")
                print(np.asarray(primitive_axis)[idx, 0, :])
                print(np.asarray(primitive_axis)[idx, 1, :])

                print("Radius")
                print(np.asarray(primitive_radii)[idx, 0])
                print(np.asarray(primitive_radii)[idx, 1])

                print("Length")
                print(np.asarray(primitive_lengths)[idx, 0])
                print(np.asarray(primitive_lengths)[idx, 1])

    if save_to_csv:
        pd.DataFrame(first_cosserat_rod_stats).to_csv("first_cosserat_rod_states.csv")
        pd.DataFrame(second_cosserat_rod_stats).to_csv("second_cosserat_rod_states.csv")

    plt.show()
