import numpy as np
from elasticapp.CaseStudies.PyMovingWallStaticRodStudy import MovingWallStaticRodStudy
from elasticapp.Systems import CosseratRods  # noqa
from cli import track_simulation
from matplotlib import pyplot as plt

from dataclasses import dataclass, field
from typing import NewType, List

PositiveInt = NewType("PositiveInt", int)


@dataclass
class RodStats:
    mass: np.ndarray
    position: List[np.ndarray] = field(default_factory=list)
    velocity: List[np.ndarray] = field(default_factory=list)
    forces: List[np.ndarray] = field(default_factory=list)
    translational_energy: List[float] = field(default_factory=list)
    rotational_energy: List[float] = field(default_factory=list)
    shearing_energy: List[float] = field(default_factory=list)
    bending_energy: List[float] = field(default_factory=list)


@dataclass
class Stats:
    first_rod: RodStats
    time: List[float] = field(default_factory=list)
    step: List[PositiveInt] = field(default_factory=list)
    penetration_depth: List[float] = field(default_factory=list)


sample_interval = 1e2
final_sim_time = 10.0
dt = 1e-5
time_interval = sample_interval * dt
n_elements = 8
damping = 85.0  # 50.0
youngs_modulus = 12e6
shear_modulus = 12e6 / 1.5
gravitational_acc = -981.0
radius = 1.21
packing_initial_vertical_span = 75.74 * 1.51
packing_initial_radial_span = 10.0
# length = 2.0 * np.sqrt(2.0) * (2.0 * packing_initial_radial_span - radius)
length = 75.74
contact_stiffness = 1e6
contact_damping_normal = 1e1
contact_damping_tangential = 1e1

params = MovingWallStaticRodStudy.params(
    n_elements=n_elements,
    damping=damping,
    youngs_modulus=youngs_modulus,
    shear_modulus=shear_modulus,
    gravitational_acc=gravitational_acc,
    radius=radius,
    length=length,
    packing_initial_vertical_span=packing_initial_vertical_span,
    packing_initial_radial_span=packing_initial_radial_span,
    dt=dt,
    contact_stiffness=contact_stiffness,
    contact_damping_normal=contact_damping_normal,
    contact_damping_tangential=contact_damping_tangential,
    final_sim_time=final_sim_time
)
cs = MovingWallStaticRodStudy(params)


def setup_stats(cas):
    rods, _, _ = cas.communicate()
    return Stats(RodStats(np.copy(rods[0].get_mass())))


total_stats = setup_stats(cs)
cosserat_rod_stats = (total_stats.first_rod,)


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


def collect_statistics_backend_agnostic(depths, rods):
    total_stats.penetration_depth.append(depths)
    for stat, pv in zip(cosserat_rod_stats, rods):
        add_to(stat, pv)


contact_ids = []


def collect_rod_stats(
        rods,
        depths,
        contact_pair_ids,
):
    collect_statistics_backend_agnostic(depths, rods)
    contact_ids.append(contact_pair_ids)


def collect_stats_from(cas):
    collect_global_stats_from(cas)
    collect_rod_stats(*cas.communicate())


n_steps = int(final_sim_time / time_interval)

collect_stats_from(cs)

for _ in track_simulation(range(n_steps)):
    cs.run_for(additional_time=time_interval)
    collect_stats_from(cs)

fig = plt.figure(figsize=(5, 5))
ax = fig.add_subplot()
ax.plot(total_stats.time, np.linalg.norm(np.average(total_stats.first_rod.velocity, axis=-1), axis=-1))
fig.show()

p_fig = plt.figure(figsize=(5, 5))
p_ax = p_fig.add_subplot()
p_ax.plot(total_stats.time, np.linalg.norm(np.average(total_stats.first_rod.position, axis=-1), axis=-1))
p_fig.show()


def n_penetrations(depths):
    # reduce list of lists to numpy array
    return np.asarray(list(map(lambda x: len(x), depths)))


d_fig = plt.figure(figsize=(5, 5))
d_ax = d_fig.add_subplot()
d_ax.scatter(total_stats.time, n_penetrations(total_stats.penetration_depth))
d_fig.show()

plt.show()
