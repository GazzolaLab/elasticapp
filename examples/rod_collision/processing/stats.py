from dataclasses import dataclass, field
from typing import NewType, List

import numpy as np

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
    contact_stiffness: float
    first_rod: RodStats
    second_rod: RodStats
    time: List[float] = field(default_factory=list)
    step: List[PositiveInt] = field(default_factory=list)
    penetration_depth: List[float] = field(default_factory=list)
