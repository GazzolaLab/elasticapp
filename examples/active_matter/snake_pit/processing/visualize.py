import numpy as np
from cli import track_simulation
from elasticapp.CaseStudies.PySnakePit import (
    SnakePit,
)
from elasticapp.Systems import CosseratRods  # noqa
from matplotlib import pyplot as plt

from .animate import (
    animate_snake,
)
from .stats import Stats, RodStats


def run_snake_pit(
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

    sample_interval = 2500

    # Snake parameter
    n_elements = 2

    youngs_modulus = 1e6
    poisson_ratio = 0.5
    shear_modulus = youngs_modulus / (poisson_ratio + 1.0)
    density = 1e3
    dt = 5e-5  # 1e-5 earlier

    # youngs_modulus = 1e3
    # poisson_ratio = 0.5
    # shear_modulus = 1e5 / (poisson_ratio + 1.0)
    # density = 1e3
    # dt = 5e-5

    length = 0.35
    radius = length * 0.011
    damping = 2e-3

    # Environment
    # Was 5e-1 and 5e-2 before
    contact_stiffness = 5e-2
    contact_damping = 5e-3
    wall_distance_ratio = 2.0

    # gravitational_acc = -9.80665
    gravitational_acc = -0.1
    time_period = 2.0
    wave_length = 1.0
    activation_start_time_nd = 5.0

    # Initialization
    initialization_protocol = "random-cone"
    if initialization_protocol == "random-cylinder":
        packing_initial_vertical_span_ratio = 4.0
        packing_initial_radial_span_ratio = 1.0
    elif initialization_protocol == "random-cone":
        packing_initial_vertical_span_ratio = 1.5
        packing_initial_radial_span_ratio = 1.0
    elif initialization_protocol == "random-cube":
        packing_initial_vertical_span_ratio = 1.5
        packing_initial_radial_span_ratio = 1.0

    # Time and recording
    final_time = (12.0) * time_period
    samples_per_period = 32
    time_interval = sample_interval * dt
    n_steps = int(final_time / time_interval)

    params = SnakePit.params(
        n_elements=n_elements,
        youngs_modulus=youngs_modulus,
        shear_modulus=shear_modulus,
        density=density,
        radius=radius,
        length=length,
        damping=damping,
        contact_stiffness=contact_stiffness,
        contact_damping=contact_damping,
        gravitational_acc=gravitational_acc,
        n_snakes=n_snakes,
        time_period=time_period,
        activation_start_time_nd=activation_start_time_nd,
        wave_length=wave_length,
        packing_initial_radial_span_ratio=packing_initial_radial_span_ratio,
        packing_initial_vertical_span_ratio=packing_initial_vertical_span_ratio,
        wall_distance_ratio=wall_distance_ratio,
        initialization_protocol=initialization_protocol,
        samples_per_period=samples_per_period,
        final_sim_time=final_time,
        dt=dt,
    )
    print(params)

    cs = SnakePit(params)

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
    if True:
        initialization_volume = 0.0
        volume_occupied_by_active_matter = np.pi * (
            n_snakes * (radius * radius) * length
        )
        volume_common = (
            (packing_initial_radial_span_ratio * packing_initial_radial_span_ratio)
            * packing_initial_vertical_span_ratio
            * length
            * length
            * length
        )
        # Print packing fraction
        if initialization_protocol == "random-cylinder":
            initialization_volume = np.pi * volume_common
        elif initialization_protocol == "random-cone":
            initialization_volume = (np.pi / 3.0) * volume_common
        elif initialization_protocol == "random-cube":
            initialization_volume = 4.0 * volume_common  # side is 2R
        pf = volume_occupied_by_active_matter / initialization_volume

        print(f"Occupancy : {pf}")
        with plt.style.context("ggplot"):
            fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)
            ax = plt.axes(projection="3d")  # fig.add_subplot(111)
            a = np.array([-1.05, 1.05]) * packing_initial_radial_span_ratio * length
            ax.set_xlim(*a)
            ax.set_ylim(*a)
            ax.set_zlim(0.0, packing_initial_vertical_span_ratio * 1.05 * length)
            ax.set_box_aspect(
                (
                    2.0,
                    2.0,
                    packing_initial_vertical_span_ratio
                    / packing_initial_radial_span_ratio,
                )
            )

            ax.set_xlabel("X")
            ax.set_ylabel("Y")
            ax.set_zlabel("Z")
            # https://stackoverflow.com/a/64849390
            ax.azim = 0.0  # look from X
            ax.elev = 0.0  # xy plane matches eye

            for r in total_stats.rod_stats:
                position_of_rod = np.asarray(r.position)
                ax.plot(
                    position_of_rod[0, 0],
                    position_of_rod[0, 1],
                    position_of_rod[0, 2],
                    lw=2.0,
                    solid_capstyle="round",
                )

        plt.show()
        exit()

    for _ in track_simulation(range(n_steps)):
        cs.run_for(additional_time=time_interval)
        collect_stats_from(cs)

    animation = animate_snake(
        total_stats,
        save_figure=save_figure,
        frame_skip=1,
    )

    if save_to_csv:
        pass

    plt.show()
