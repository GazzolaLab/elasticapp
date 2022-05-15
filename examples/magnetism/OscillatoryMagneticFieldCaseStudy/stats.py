from dataclasses import dataclass, field

import numpy as np

from typing import NewType, List

PositiveInt = NewType("PositiveInt", int)


@dataclass
class Stats:
    time: List[float] = field(default_factory=list)
    step: List[PositiveInt] = field(default_factory=list)
    position: List[np.ndarray] = field(default_factory=list)
    velocity: List[np.ndarray] = field(default_factory=list)
