from animate import animate_magnetic_rod

from cli.help import print_help
from cli.progress import track_simulation

from elasticapp import Blaze  # noqa
from elasticapp.CaseStudies.PyOscillatoryMagneticFieldCaseStudy import (
    OscillatoryMagneticFieldCaseStudy,
)

from matplotlib import pyplot as plt

import numpy as np

import pandas as pd

from stats import Stats


def run_oscillating_field_magnetic_rod_case_study(
        magnetization_angle: float = 0.0,
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
    n_elem = 50
    base_length = 1.5  # m
    base_radius = 0.15  # m
    base_area = np.pi * base_radius ** 2
    volume = base_area * base_length
    moment_of_inertia = np.pi / 4 * base_radius ** 4
    nu = 50.0
    angular_frequency = np.deg2rad(
        2.5
    )  # angular frequency of the rotating magnetic field
    magnetization_density = (
            MBAL2_EI
            * youngs_modulus
            * moment_of_inertia
            / (volume * in_plane_magnetic_field_strength * base_length)
    )
    magnetization_angle = np.deg2rad(magnetization_angle)
    dl = base_length / n_elem
    dt = 0.01 * dl
    sample_interval = 1e3
    time_interval = sample_interval * dt
    num_periods = 2
    final_time = num_periods * (2 * np.pi / angular_frequency)
    n_steps = int(final_time / time_interval)

    cs = OscillatoryMagneticFieldCaseStudy(
        n_elements=n_elem,
        length=base_length,
        radius=base_radius,
        youngs_modulus=youngs_modulus,
        shear_modulus=shear_modulus,
        damping_rate=nu,
        magnetization_density=magnetization_density,
        magnetic_field_in_plane_amplitude=in_plane_magnetic_field_strength,
        magnetic_field_out_of_plane_amplitude=(0.01 * in_plane_magnetic_field_strength),
        magnetic_field_initial_angle=0.0,
        magnetization_angle=magnetization_angle,
        magnetic_field_angular_frequency=angular_frequency,
        rampup_interval=0.01,
        dt=dt,
        final_sim_time=final_time,
    )

    cosserat_rod_stats = Stats()

    def collect_global_stats_from(cas):
        cosserat_rod_stats.time.append(cas.current_time())
        cosserat_rod_stats.step.append(cas.current_iteration())

    def collect_rod_stats(pv):
        p, v = pv
        cosserat_rod_stats.position.append(np.copy(p))
        cosserat_rod_stats.velocity.append(np.copy(v))

    def collect_stats_from(cas):
        collect_global_stats_from(cas)
        collect_rod_stats(*cas.communicate())

    # initial
    collect_stats_from(cs)

    for _ in track_simulation(range(n_steps)):
        cs.run_for(additional_time=time_interval)
        collect_stats_from(cs)

    animation = animate_magnetic_rod(
        cosserat_rod_stats,
        rod_length=base_length,
        save_figure=save_figure,
        frame_skip=5,
    )

    if save_to_csv:
        pd.DataFrame(cosserat_rod_stats).to_csv("cosserat_rod_states.csv")

    plt.show()


if __name__ == "__main__":
    print_help(OscillatoryMagneticFieldCaseStudy)
    run_oscillating_field_magnetic_rod_case_study()
