import numpy as np
from cli import track_simulation
from elasticapp.CaseStudies.PySnakeOnPlane import (
    SnakeOnPlane,
)
from elasticapp.Systems import CosseratRods  # noqa
from matplotlib import pyplot as plt

from .animate import animate_snake, plot_com_trajectory
from .stats import Stats, RodStats


def run_snake_on_plane(
    n_snakes: int = 1,
    save_to_csv: bool = False,
    save_figure: bool = False,
):
    """Run rod collison study"""

    """This varaible is purely for debugging alone, and can be accessed if ELASTICA_DEBUG
    is set to true. This is left behind for testing purposes"""
    run_on_debug_mode = False
    if run_on_debug_mode:
        np.set_printoptions(floatmode="unique")

    sample_interval = 1e2

    n_elements = 20
    youngs_modulus = 1e6
    poisson_ratio = 0.5
    shear_modulus = youngs_modulus / (poisson_ratio + 1.0)
    damping_rate = 0.01  # Keeping it low acheives convergence
    density = 1e3
    length = 0.35
    radius = length * 0.011

    dt = 1e-4
    # These numbers seem very low.
    contact_stiffness = 1e0
    contact_damping = 1e-2

    gravitational_acc = -9.80665  # Causes initial bounce!
    gravitational_acc = -1.00
    time_period = 2.0
    wave_length = 1.0

    final_time = (3.0) * time_period
    samples_per_period = 32
    time_interval = time_period / samples_per_period
    n_steps = int(final_time / time_interval)

    params = SnakeOnPlane.params(
        n_elements=n_elements,
        youngs_modulus=youngs_modulus,
        shear_modulus=shear_modulus,
        density=density,
        damping_rate=damping_rate,
        radius=radius,
        length=length,
        contact_stiffness=contact_stiffness,
        contact_damping=contact_damping,
        gravitational_acc=gravitational_acc,
        n_snakes=n_snakes,
        time_period=time_period,
        wave_length=wave_length,
        dt=dt,
    )

    cs = SnakeOnPlane(params)

    def setup_stats(cas):
        if run_on_debug_mode:
            _, rods, _ = cas.communicate()
        else:
            _, rods = cas.communicate()
        rod_stats = [RodStats(np.copy(r.get_mass())) for r in rods]

        return Stats(contact_stiffness=contact_stiffness, rod_stats=rod_stats)

    total_stats = setup_stats(cs)

    def collect_global_stats_from(cas):
        total_stats.time.append(cas.current_time())
        total_stats.step.append(cas.current_iteration())

    def add_to(s, rod):
        s.position.append(np.copy(rod.get_position()))
        s.director.append(np.copy(rod.get_director()))
        s.velocity.append(np.copy(rod.get_velocity()))
        s.forces.append(np.copy(rod.get_external_loads()))
        s.torques.append(np.copy(rod.get_external_torques()))

    # def process_depth(d):
    #     return d[0] if d else 0.0

    def collect_statistics_backend_agnostic(depths, rods):
        total_stats.penetration_depth.append(depths)
        for stat, pv in zip(total_stats.rod_stats, rods):
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

    traj = plot_com_trajectory(total_stats, save_figure=save_figure, frame_skip=1)
    animation = animate_snake(
        total_stats,
        save_figure=save_figure,
        frame_skip=5,
    )

    if save_to_csv:
        time = np.asarray(total_stats.time)
        position_history = np.asarray(
            list(map(lambda x: x.position, total_stats.rod_stats))
        )
        save_dict = {
            "time": time,
            "position_history": position_history,
        }
        np.savez("snake_dynamics.npz", **save_dict)

    plt.show()
