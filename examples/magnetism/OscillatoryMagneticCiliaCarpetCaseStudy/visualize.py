from cli.help import print_description
from cli.progress import track_simulation

from elasticapp import Arrays  # noqa
from elasticapp.CaseStudies.PyOscillatoryMagneticCiliaCarpetCaseStudy import (
    OscillatoryMagneticCiliaCarpetCaseStudy,
)

from matplotlib import pyplot as plt

import numpy as np

import pandas as pd

from post_processing.animate import animate_rods
from post_processing.stats import Stats


def run_oscillatory_magnetic_cilia_carpet_case_study(
    save_to_csv: bool = False,
    save_figure: bool = False,
):
    # Parameters below are one-to-one from Gu2020 (see case help message)
    youngs_modulus = 1.85e5  # Pa
    shear_modulus = 6.16e4  # Pa
    in_plane_magnetic_field_strength = 80e-3  # 80mT
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
    n_rods_along_x = 8
    n_rods_along_y = 8
    n_rods = n_rods_along_x * n_rods_along_y
    n_elem = 20
    base_length = 1.5  # m
    base_radius = 0.15  # m
    base_area = np.pi * base_radius ** 2
    volume = base_area * base_length
    moment_of_inertia = np.pi / 4 * base_radius ** 4
    damping_rate = 1.0
    spacing_between_rods = base_length  # following Gu2020
    carpet_length_along_x = spacing_between_rods * (n_rods_along_x - 1)
    carpet_length_along_y = spacing_between_rods * (n_rods_along_y - 1)
    spatial_magnetization_wavelength_along_x = carpet_length_along_x
    spatial_magnetization_wavelength_along_y = spatial_magnetization_wavelength_along_x
    spatial_magnetization_phase_diff = 0 * np.pi
    angular_frequency = np.deg2rad(
        5.0
    )  # angular frequency of the rotating magnetic field
    magnetization_density = (
        MBAL2_EI
        * youngs_modulus
        * moment_of_inertia
        / (volume * in_plane_magnetic_field_strength * base_length)
    )
    dl = base_length / n_elem
    dt = 0.01 * dl
    sample_interval = 1e3
    time_interval = sample_interval * dt
    num_periods = 3
    final_time = num_periods * (2 * np.pi / angular_frequency)
    n_steps = int(final_time / time_interval)

    cs = OscillatoryMagneticCiliaCarpetCaseStudy(
        n_rods_along_x=n_rods_along_x,
        n_rods_along_y=n_rods_along_y,
        n_elements=n_elem,
        length=base_length,
        radius=base_radius,
        youngs_modulus=youngs_modulus,
        shear_modulus=shear_modulus,
        damping_rate=damping_rate,
        magnetization_density=magnetization_density,
        carpet_spacing=spacing_between_rods,
        carpet_magnetization_spatial_wavelength_along_x=spatial_magnetization_wavelength_along_x,
        carpet_magnetization_spatial_wavelength_along_y=spatial_magnetization_wavelength_along_y,
        carpet_magnetization_spatial_phase_difference=spatial_magnetization_phase_diff,
        magnetic_field_in_plane_amplitude=in_plane_magnetic_field_strength,
        magnetic_field_out_of_plane_amplitude=(0.01 * in_plane_magnetic_field_strength),
        magnetic_field_angular_frequency=angular_frequency,
        rampup_interval=0.01,
        dt=dt,
        final_sim_time=final_time,
    )

    carpet_stats = [Stats() for _ in range(n_rods)]

    def collect_global_stats_from(cas):
        for rod_stats in carpet_stats:
            rod_stats.time.append(cas.current_time())
            rod_stats.step.append(cas.current_iteration())

    def collect_rod_stats(rod_stats, p, v):
        rod_stats.position.append(np.copy(p))
        rod_stats.velocity.append(np.copy(v))

    def collect_stats_from(cas):
        collect_global_stats_from(cas)
        for idx in range(n_rods):
            collect_rod_stats(carpet_stats[idx], *(cas.communicate()[idx]))

    # initial
    collect_stats_from(cs)

    for _ in track_simulation(range(n_steps)):
        cs.run_for(additional_time=time_interval)
        collect_stats_from(cs)

    animation = animate_rods(
        rod_group_stats=carpet_stats,
        x_lim=(-base_length, carpet_length_along_x + base_length),
        y_lim=(-base_length, carpet_length_along_y + base_length),
        z_lim=(-0.5 * base_length, 1.5 * base_length),
        save_figure=save_figure,
        frame_skip=5,
    )

    if save_to_csv:
        for idx, rod_stats in carpet_stats:
            pd.DataFrame(rod_stats).to_csv(f"rod_{idx}_states.csv")

    plt.show()


if __name__ == "__main__":
    print_description(OscillatoryMagneticCiliaCarpetCaseStudy)
    run_oscillatory_magnetic_cilia_carpet_case_study()
