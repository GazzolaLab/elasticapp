from typing import Dict, Optional

import numpy as np
from cli.progress import ProgressionStatus, TaskID
from elasticapp.CaseStudies.PyDynamicCantileverCaseStudy import (
    DynamicCantileverCaseStudy,
)
from elasticapp.Systems import CosseratRods  # noqa

from .analytical import setup_analytical
from .data import RecordingParameters, CaseStudyParameters, Stats


def setup_dynamic_cantilever_simulator(
    params: CaseStudyParameters,
) -> DynamicCantileverCaseStudy:
    cs = DynamicCantileverCaseStudy(params)
    analytical_cantilever = setup_analytical(params)

    cantilever_rod = cs.communicate()

    initial_velocity = analytical_cantilever.get_initial_velocity_profile(
        np.copy(cantilever_rod.position_collection)[0, :]
    )
    v = np.asarray(cantilever_rod.loc[cantilever_rod.Velocity])
    v[2, :] = initial_velocity
    # print(initial_velocity)
    # print(np.copy(cantilever_rod.get_velocity())[2, :])
    # raise RuntimeError("The velocities are too high, investigate")

    return cs


def simulate_dynamic_cantilever(
    final_time: float,
    params: CaseStudyParameters,
    record_params: RecordingParameters,
    progress: Optional[Dict[TaskID, ProgressionStatus]] = None,
    uuid: Optional[TaskID] = None,
) -> Stats:
    cs = setup_dynamic_cantilever_simulator(params)
    stats = Stats()

    def collect_statistics(cas):
        rod = cas.communicate()
        stats.time.append(cas.current_time())
        stats.position.append(np.copy(rod.get_position()))
        stats.tip_deflection.append(np.asarray(rod.get_position())[2, -1])

    collect_statistics(cs)

    time_interval = record_params.sample_interval * params.dt
    n_steps = int(final_time / time_interval)

    if progress is not None:

        def update_progress(step):
            progress[uuid] = ProgressionStatus(current=step + 1, total=n_steps)

    else:

        def update_progress(step):
            pass

    for i_step in range(n_steps):
        cs.run_for(additional_time=time_interval)
        collect_statistics(cs)
        update_progress(i_step)

    return stats
