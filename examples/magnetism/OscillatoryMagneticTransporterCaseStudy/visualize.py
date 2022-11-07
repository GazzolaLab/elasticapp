from cli import track_simulation, replace_docstring, clickify

from elasticapp.Systems import CosseratRods, MagnetizedCosseratRods, RigidBody  # noqa
from elasticapp.CaseStudies.PyOscillatoryMagneticTransporterCaseStudy import (
    OscillatoryMagneticTransporterCaseStudy,
)

from matplotlib import pyplot as plt

import numpy as np

import pandas as pd
from typing import List

from post_processing.animate import animate_rods
from post_processing.stats import Stats


def plot_sphere_trajectories(sphere_stats: List[Stats], save_figure: bool = False):
    time = np.asarray(sphere_stats[0].time)
    with plt.style.context("ggplot"):
        fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)
        ax = plt.axes(projection="3d")  # fig.add_subplot(111)
        # ax = fig.add_subplot(111)

        ax.set_xlabel("X")
        ax.set_ylabel("Y")
        ax.set_zlabel("Z")

        for sphere_stat in sphere_stats:
            # (n_times, 3, 1)
            p = np.squeeze(np.asarray(sphere_stat.position), axis=-1)
            ax.plot(
                p[:, 0],
                p[:, 1],
                p[:, 2],
                lw=2.0,
                solid_capstyle="round",
            )

    if save_figure:
        fig.savefig("trajectories.png")

    return fig


@clickify()
@replace_docstring(OscillatoryMagneticTransporterCaseStudy.__doc__)
def run_oscillatory_magnetic_transporter_case_study(
        data: bool = False,
        fig: bool = False,
):
    # Parameters below are one-to-one from Gu2020 (see case help message)
    youngs_modulus = 1.85e5  # Pa
    shear_modulus = 6.16e4  # Pa
    in_plane_magnetic_field_strength = 40e-3  # 80mT
    # MBAL2_EI is a non-dimensional number from Wang 2019
    Gu_reference_magnetization = 3.82e-5
    Gu_reference_I = np.pi / 4 * (0.4e-3) ** 4
    Gu_reference_length = 4e-3
    MBAL2_EI = (
            Gu_reference_magnetization
            * in_plane_magnetic_field_strength
            * Gu_reference_length
            / (youngs_modulus * Gu_reference_I)
    )  # Magnetization magnitude * B * Length/(EI)

    # setting up test params (slightly different for higher time step)

    n_elem = 4
    base_length = 1.5  # m
    base_radius = 0.15  # m
    base_area = np.pi * base_radius ** 2
    volume = base_area * base_length
    moment_of_inertia = np.pi / 4 * base_radius ** 4
    damping_rate = 0.05

    n_rods_along_x = 8
    n_rods_along_y = 4
    n_rods = n_rods_along_x * n_rods_along_y

    spacing_between_rods = 0.4 * base_length  # following Gu2020
    carpet_length_along_x = spacing_between_rods * (n_rods_along_x - 1)
    carpet_length_along_y = spacing_between_rods * (n_rods_along_y - 1)

    spatial_magnetization_wavelength_along_x = 100.0 * carpet_length_along_x
    spatial_magnetization_wavelength_along_y = 100.0 * carpet_length_along_y
    spatial_magnetization_phase_diff = 0 * np.pi
    angular_frequency = np.deg2rad(
        4.0
    )  # angular frequency of the rotating magnetic field
    magnetization_density = (
            MBAL2_EI
            * youngs_modulus
            * moment_of_inertia
            / (volume * in_plane_magnetic_field_strength * base_length)
    )

    acc_gravity = 0.0  # -9.81 / 10.0
    contact_stiffness = 0.8
    contact_damping_normal = 0.25 * contact_stiffness
    contact_damping_tangential = 0.25 * contact_stiffness
    friction_coeff = 0.2
    sphere_radius_to_rod_radius = 2.0
    sphere_density_to_rod_density = 1.2

    # Top layer
    top_layer_nondimensional_width = 2.2

    dl = base_length / n_elem
    dt = 0.02 * dl
    sample_interval = 5e2
    time_interval = sample_interval * dt
    num_periods = 160.0
    final_time = num_periods * (2 * np.pi / angular_frequency)
    n_steps = int(final_time / time_interval)

    clearance = spacing_between_rods - 2 * base_radius * (1.0 + sphere_radius_to_rod_radius)
    print("Clearance : ", clearance)

    params = OscillatoryMagneticTransporterCaseStudy.params(
        n_rods_along_x=n_rods_along_x,
        n_rods_along_y=n_rods_along_y,
        n_elements=n_elem,
        length=base_length,
        radius=base_radius,
        youngs_modulus=youngs_modulus,
        shear_modulus=shear_modulus,
        damping_rate=damping_rate,
        contact_stiffness=contact_stiffness,
        contact_damping_normal=contact_damping_normal,
        contact_damping_tangential=contact_damping_tangential,
        friction_coeff=friction_coeff,
        magnetization_density=magnetization_density,
        carpet_spacing=spacing_between_rods,
        carpet_magnetization_spatial_wavelength_along_x=spatial_magnetization_wavelength_along_x,
        carpet_magnetization_spatial_wavelength_along_y=spatial_magnetization_wavelength_along_y,
        carpet_magnetization_spatial_phase_difference=spatial_magnetization_phase_diff,
        magnetic_field_in_plane_amplitude=in_plane_magnetic_field_strength,
        magnetic_field_out_of_plane_amplitude=(0.01 * in_plane_magnetic_field_strength),
        magnetic_field_angular_frequency=angular_frequency,
        rampup_interval=0.01,
        acc_gravity=acc_gravity,
        sphere_radius_to_rod_radius=sphere_radius_to_rod_radius,
        sphere_density_to_rod_density=sphere_density_to_rod_density,
        top_layer_nondimensional_width=top_layer_nondimensional_width,
        dt=dt,
        final_sim_time=final_time,
    )
    cs = OscillatoryMagneticTransporterCaseStudy(params)

    def setup_stats():
        # Tuple of types of list of rods
        magnetic_rods, spheres = cs.communicate()
        # Tod
        return [Stats() for _ in range(len(magnetic_rods))], [Stats() for _ in range(len(spheres))]

    carpet_stats = setup_stats()

    def collect_global_stats_from(cas):
        for stats_of_rod_type in carpet_stats:
            for rod_stats in stats_of_rod_type:
                rod_stats.time.append(cas.current_time())
                rod_stats.step.append(cas.current_iteration())

    def collect_rod_stats(rod_stat, rod):
        rod_stat.position.append(np.copy(rod.get_position()))

    def collect_stats_from(cas):
        collect_global_stats_from(cas)
        for stats_of_rod_type, rods in zip(carpet_stats, cs.communicate()):
            for stat, r in zip(stats_of_rod_type, rods):
                collect_rod_stats(stat, r)

    # initial
    collect_stats_from(cs)

    if True:
        for _ in track_simulation(range(n_steps)):
            cs.run_for(additional_time=time_interval)
            collect_stats_from(cs)

    if True:
        traj = plot_sphere_trajectories(carpet_stats[1], save_figure=fig)
        animation = animate_rods(
            rod_group_stats=carpet_stats[0],
            sphere_stats=carpet_stats[1],
            x_lim=(-base_length, carpet_length_along_x + base_length),
            y_lim=(-base_length, carpet_length_along_y + base_length),
            z_lim=(-0.5 * base_length, 1.5 * base_length),
            save_figure=fig,
            frame_skip=1,
        )
    else:
        print(carpet_stats[1][0].position)

    if data:
        # Spheres first
        stats_of_rod_type = carpet_stats[1]
        for idx, rod_stats in enumerate(stats_of_rod_type):
            # vars(rod_stats).pop('velocity', None)
            # pd.DataFrame(vars(rod_stats)).to_csv(f"rod_{idx}_states.csv")
            np.savetxt(f"sphere_{idx}_states.csv", np.squeeze(np.asarray(rod_stats.position)), delimiter=',')

    plt.show()


if __name__ == "__main__":
    run_oscillatory_magnetic_transporter_case_study()
