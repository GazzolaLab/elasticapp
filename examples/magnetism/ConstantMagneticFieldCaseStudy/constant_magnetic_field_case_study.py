from typing import Dict, Optional

import numpy as np
from elasticapp.Systems import MagnetizedCosseratRods  # noqa
from elasticapp.CaseStudies.PyConstantMagneticFieldCaseStudy import (
    ConstantMagneticFieldCaseStudy,
)

from cli.progress import ProgressionStatus, TaskID


def run_constant_magnetic_field_case_study(
    magnetization_density: float,
    magnetic_field_amplitude: float,
    magnetic_field_angle: float,
    progress: Optional[Dict[TaskID, ProgressionStatus]] = None,
    uuid: Optional[TaskID] = None,
):
    n_elements = 40
    length = 6.0
    dt = 0.01 * (length / n_elements)
    # dt = 0.1
    final_time = 1000.0
    rampup_interval = 500.0
    sample_interval = 100
    time_interval = sample_interval * dt
    n_steps = int(final_time / time_interval)

    cs = ConstantMagneticFieldCaseStudy(
        n_elements=n_elements,
        length=length,
        radius=0.15,
        magnetization_density=magnetization_density,
        magnetic_field_amplitude=magnetic_field_amplitude,
        magnetic_field_angle=magnetic_field_angle,
        rampup_interval=rampup_interval,
        dt=dt,
    )

    times = []
    velocity_norms = []
    first_rod = 0

    def collect_stats(study):
        times.append(study.current_time())
        rods = study.communicate()
        velocity_norms.append(
            np.linalg.norm(np.asarray(rods[first_rod].get_velocity()))
        )

    # first round
    collect_stats(cs)
    if progress is not None:

        def update_progress(step):
            progress[uuid] = ProgressionStatus(current=step + 1, total=n_steps)

    else:

        def update_progress(step):
            pass

    for i_step in range(n_steps):
        cs.run_for(additional_time=time_interval)
        collect_stats(cs)
        # state
        update_progress(i_step)

    # last round
    collect_stats(cs)

    rods = cs.communicate()
    positions_after_last_step = np.copy(rods[first_rod].get_position())
    tangents_after_last_step = np.copy(rods[first_rod].get_tangent())

    return (
        positions_after_last_step,
        tangents_after_last_step,
        np.asarray(times),
        np.asarray(velocity_norms),
    )
