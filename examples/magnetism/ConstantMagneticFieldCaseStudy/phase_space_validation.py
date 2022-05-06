import numpy as np
from matplotlib import pyplot as plt
from matplotlib.colors import to_rgb

from rich.console import Console
from rich.text import Text
from cli.progress import ProgressionStatus
from constant_magnetic_field_analytical_solution import MagneticBeamAnalytical
from constant_magnetic_field_case_study import run_constant_magnetic_field_case_study


def run_case_study(
    magnetization_density, magnetic_field_angle, magnetic_field_amplitude, *args
):
    final_position, final_tangents, _, _ = run_constant_magnetic_field_case_study(
        magnetization_density, magnetic_field_amplitude, magnetic_field_angle, *args
    )

    # Compute MBAL2/EI
    base_radius = 0.15  # uhoh, repeated information
    base_length = 6.0  # uhoh, repeated information
    E = 1e6  # uhoh, repeated information
    base_area = np.pi * base_radius**2

    moment_of_inertia = np.pi / 4 * base_radius**4
    MBAL2_EI = (
        magnetization_density
        * magnetic_field_amplitude
        * base_area
        * base_length**2
        / (E * moment_of_inertia)
    )

    # Get the final tip deflection
    last_dof = -1
    y_axis = 1
    deflection = final_position[..., last_dof][y_axis] / base_length

    # Get the tip angle
    tip_angle = np.arccos(
        np.dot(final_tangents[..., last_dof], np.array([1.0, 0.0, 0.0]))
    )
    return MBAL2_EI, deflection, tip_angle


def run_example(params, *args):
    """Helper to do starmap"""
    return run_case_study(*params, *args)


def compute_analytical_solution(
    magnetization_density,
    magnetic_field_angle,
    magnetic_field,
    progress,
    uuid,
):
    base_length = 6
    base_radius = 0.15
    base_area = np.pi * base_radius**2
    I = np.pi / 4 * base_radius**4
    E = 1e6

    f = magnetization_density * base_area * magnetic_field

    deflection = np.zeros((f.shape[0]))
    theta = np.zeros((f.shape[0]))
    theta_dot = np.zeros((f.shape[0]))

    for i in range(f.shape[0]):
        # For computing theta dot at x=0 we are using shooting method. Shooting method has to satify the boundary
        # condition at x=L theta_dot = 0. However, optimization methods used can converge to different optima, so
        # we start optimization using the theta_dot computed in previous iteration.
        if i == 0:
            theta_dot_initial_guess = theta_dot[i]
        else:
            theta_dot_initial_guess = theta_dot[i - 1]

        magneto = MagneticBeamAnalytical(
            F=f[i],
            EI=E * I,
            phi=magnetic_field_angle,
            L=base_length,
            ksi=np.linspace(0, 1, 100),
            theta_dot_initial_guess=theta_dot_initial_guess,
        )
        theta[i], deflection[i], theta_dot[i] = magneto()
        progress[uuid] = ProgressionStatus(current=i, total=f.shape[0])

    return f * base_length**2 / (E * I), deflection, theta


def run_analytical_solution(params, *args):
    """Helper to do starmap"""
    return compute_analytical_solution(*params, *args)


if __name__ == "__main__":
    from cli.progress import p_map

    magnetization_density = 144e3
    magnetic_field_amplitude = np.linspace(0, 42, 4) * 1e-3
    magnetic_field_amplitude_analytical = np.linspace(0, 42, 400) * 1e-3
    # magnetic_field_angle = np.deg2rad(np.array([30, 60, 90, 120, 150, 180 - 0.5]))
    magnetic_field_angle = np.deg2rad(np.array([30, 90, 120, 180 - 0.5]))

    # Run elastica simulations as a batch job
    from itertools import product

    console = Console()
    text = Text("Performing numerical simulations", style="yellow")
    console.print(text)

    results = p_map(
        run_example,
        product(
            [magnetization_density],
            magnetic_field_angle,
            magnetic_field_amplitude,
        ),
    )

    def as_matrix(func):
        return np.asarray(list(map(func, results))).reshape(
            (magnetic_field_angle.shape[0], magnetic_field_amplitude.shape[0])
        )

    MBAL2_EI = as_matrix(lambda x: x[0])
    deflection = as_matrix(lambda x: x[1])
    theta = as_matrix(lambda x: x[2])

    text = Text("Performing analytical computations", style="yellow")
    console.print(text)

    analytical_results = p_map(
        run_analytical_solution,
        product(
            [magnetization_density],
            magnetic_field_angle,
            [magnetic_field_amplitude_analytical],
        ),
    )

    def as_analytical_matrix(func):
        return np.asarray(list(map(func, analytical_results))).reshape(
            (
                magnetic_field_angle.shape[0],
                magnetic_field_amplitude_analytical.shape[0],
            )
        )

    MBAL2_EI_analytical = as_analytical_matrix(lambda x: x[0])
    deflection_analytical = as_analytical_matrix(lambda x: x[1])
    theta_analytical = as_analytical_matrix(lambda x: x[2])

    fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)

    ax = fig.add_subplot(111)
    for i in range(magnetic_field_angle.shape[0]):
        ax.plot(
            MBAL2_EI_analytical[i, :],
            deflection_analytical[i, :],
            label="phi=" + str(np.ceil(np.rad2deg(magnetic_field_angle[i]))),
        )
        ax.plot(
            MBAL2_EI[i, :],
            deflection[i, :],
            "*",
            markersize=10,
            color=to_rgb("xkcd:bluish")
            # label="phi=" + str(np.ceil(np.rad2deg(magnetic_field_angle))),
        )
    ax.set_xlabel("MBAL2/EI")
    ax.set_ylabel("delta_y/L")
    plt.tight_layout()
    fig.align_ylabels()
    fig.savefig("magnetic_beam_analytical_deflection.png", dpi=300)

    fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)

    ax = fig.add_subplot(111)
    for i in range(magnetic_field_angle.shape[0]):
        ax.plot(
            MBAL2_EI_analytical[i, :],
            theta_analytical[i, :],
            label="phi=" + str(np.ceil(np.rad2deg(magnetic_field_angle[i]))),
        )
        ax.plot(
            MBAL2_EI[i, :],
            theta[i, :],
            "*",
            markersize=10,
            color=to_rgb("xkcd:bluish")
            # label="phi=" + str(np.ceil(np.rad2deg(magnetic_field_angle))),
        )
    ax.set_xlabel("MBAL2/EI")
    ax.set_ylabel("theta (L)")
    plt.tight_layout()
    fig.align_ylabels()
    fig.savefig("magnetic_beam_analytical_theta.png", dpi=300)
