import numpy as np
import pandas as pd
from elasticapp.CaseStudies.PyRodPlaneCollisionCaseStudy import (
    RodPlaneCollisionCaseStudy,
)
from elasticapp.Systems import CosseratRods  # noqa
from matplotlib import pyplot as plt

from cli.help import print_description
from cli.progress import track_simulation
from post_processing import Stats, animate_rods, plot_energies


def run_rod_plane_collision_case_study(
    inclination_angle: float = 0.0,
    save_to_csv: bool = False,
    save_figure: bool = False,
):
    # Parameters below are one-to-one from PRL nest
    youngs_modulus = 12e9  # Pa
    shear_modulus = 8e9  # Pa
    n_elem = 8
    base_length = 0.07574  # m
    base_radius = 1.21e-3  # m
    damping_rate = 1e-10  # lowest value allowed for energy preservation
    dt = 4e-7
    gravitational_acc = -9.80665
    contact_stiffness = 1e3
    contact_damping = 1e-10  # lowest value allowed for energy preservation
    contact_detection_time_interval = 5e-6  # Set it from 0.0 and start increasing, adjust based on energy conservation
    min_distance_from_ground = 5.0 * base_radius

    sample_interval = 1e3
    time_interval = sample_interval * dt
    final_time = 0.25
    n_steps = int(final_time / time_interval)
    n_rods = 1

    cs = RodPlaneCollisionCaseStudy(
        n_elements=n_elem,
        length=base_length,
        radius=base_radius,
        youngs_modulus=youngs_modulus,
        shear_modulus=shear_modulus,
        damping_rate=damping_rate,
        gravitational_acc=gravitational_acc,
        contact_stiffness=contact_stiffness,
        contact_damping=contact_damping,
        contact_detection_time_interval=contact_detection_time_interval,
        initial_min_distance_from_plane=min_distance_from_ground,
        angle_of_inclination=inclination_angle,
        dt=dt,
        final_sim_time=final_time,
    )

    _, rods = cs.communicate()
    rod_collection_stats = [
        Stats(
            contact_stiffness=contact_stiffness,
            gravitational_acc=gravitational_acc,
            mass=np.array(r.get_mass()),
        )
        for r in rods
    ]

    def collect_global_stats_from(cas):
        for rod_stats in rod_collection_stats:
            rod_stats.time.append(cas.current_time())
            rod_stats.step.append(cas.current_iteration())

    def collect_rod_stats(rod_stats, rod):
        rod_stats.position.append(np.copy(rod.get_position()))
        rod_stats.velocity.append(np.copy(rod.get_velocity()))
        rod_stats.translational_energy.append(rod.compute_translational_energy())
        rod_stats.rotational_energy.append(rod.compute_rotational_energy())
        rod_stats.bending_energy.append(rod.compute_bending_energy())
        rod_stats.shearing_energy.append(rod.compute_shearing_energy())

    def collect_stats_from(cas):
        collect_global_stats_from(cas)
        depths, rods = cas.communicate()
        for stat, rod in zip(rod_collection_stats, rods):
            stat.penetration_depths.append(depths)
            collect_rod_stats(stat, rod)

    # initial
    collect_stats_from(cs)

    for _ in track_simulation(range(n_steps)):
        cs.run_for(additional_time=time_interval)
        collect_stats_from(cs)

    energy_fig = plot_energies(
        rod_group_stats=rod_collection_stats, save_figure=save_figure
    )
    animation = animate_rods(
        rod_group_stats=rod_collection_stats,
        x_lim=(-base_length, base_length),
        y_lim=(-base_length, 1.5 * base_length),
        z_lim=(0.0, 1.5 * min_distance_from_ground),
        save_figure=save_figure,
        frame_skip=5,
    )

    if save_to_csv:
        for idx, rod_stats in rod_collection_stats:
            pd.DataFrame(rod_stats).to_csv(f"rod_{idx}_states.csv")

    run_on_debug_mode = True
    if run_on_debug_mode:
        fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)
        ax = fig.add_subplot(111)
        time = np.asarray(rod_collection_stats[0].time)
        centroid_height = np.mean(np.asarray(rod_collection_stats[0].position), axis=2)[
            ..., 2
        ]
        ax.plot(time, centroid_height / base_radius)
        ax.set_xlabel("Time")
        ax.set_ylabel("Centroid height / Rod radius")

    plt.show()


if __name__ == "__main__":
    print_description(RodPlaneCollisionCaseStudy)
    run_rod_plane_collision_case_study(save_figure=False)
