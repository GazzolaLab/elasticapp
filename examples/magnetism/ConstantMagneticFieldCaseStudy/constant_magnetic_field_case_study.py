import numpy as np
from elasticapp import Blaze  # noqa
from elasticapp.CaseStudies.PyConstantMagneticFieldCaseStudy import (
    ConstantMagneticFieldCaseStudy,
)
from rich.progress import Progress


def run_constant_magnetic_field_case_study(
        magnetization_density: float,
        magnetic_field_amplitude: float,
        magnetic_field_angle: float,
        progress_ctx: Progress = None
):
    n_elements = 100
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
        comm = study.communicate()
        _, _, v = comm[first_rod]
        velocity_norms.append(np.linalg.norm(np.asarray(v)))

    # first round
    collect_stats(cs)

    if progress_ctx is None:
        progress_ctx = Progress()

    simulation_task = progress_ctx.add_task("[red]Simulating...", total=n_steps)

    for _ in range(n_steps):
        cs.run_for(additional_time=time_interval)
        collect_stats(cs)
        progress_ctx.update(simulation_task, advance=1)

    # stop and hide progress bar for this step when done
    progress_ctx.stop_task(simulation_task)
    progress_ctx.update(simulation_task, visible=False)

    # last round
    collect_stats(cs)

    comm = cs.communicate()
    positions_after_last_step, tangents_after_last_step, _ = comm[first_rod]

    return (
        np.asarray(positions_after_last_step),
        np.asarray(tangents_after_last_step),
        np.asarray(times),
        np.asarray(velocity_norms),
    )
