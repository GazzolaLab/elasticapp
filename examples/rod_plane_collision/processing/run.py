import numpy as np
from typing import TypeVar

from .stats import Stats, SimulationStats
from cli.progress import track_simulation

# CaseStudy = Union[RodPlaneCollisionCaseStudy, RodHalfSpaceCollisionCaseStudy]
# CaseStudyParameters = Union[
#     RodPlaneCollisionCaseStudyParameters, RodHalfSpaceCollisionCaseStudyParameters
# ]
# TODO : define interface for case-studies
CaseStudy = TypeVar("CaseStudy")
CaseStudyParameters = TypeVar("CaseStudyParameters")


def setup_stats(cs: CaseStudy, params: CaseStudyParameters) -> SimulationStats:
    _, rods, _ = cs.communicate()
    return [
        Stats(
            contact_stiffness=params.contact_stiffness,
            gravitational_acc=params.gravitational_acc,
            mass=np.array(r.get_mass()),
        )
        for r in rods
    ]


def run_simulation(cs: CaseStudy, params: CaseStudyParameters) -> SimulationStats:
    sample_interval = 1e3
    time_interval = sample_interval * params.dt
    final_time = 0.25
    n_steps = int(final_time / time_interval)

    rod_collection_stats = setup_stats(cs, params)

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
        depths, rods, plane_stats = cas.communicate()
        for stat, rod in zip(rod_collection_stats, rods):
            stat.penetration_depths.append(depths)
            plane_p, plane_v = plane_stats
            stat.plane_position.append(plane_p)
            stat.plane_velocity.append(plane_v)
            collect_rod_stats(stat, rod)

    # initial statistics
    collect_stats_from(cs)

    for _ in track_simulation(range(n_steps)):
        cs.run_for(additional_time=time_interval)
        collect_stats_from(cs)

    return rod_collection_stats
