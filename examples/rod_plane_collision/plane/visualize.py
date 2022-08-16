import click
from elasticapp.CaseStudies.PyRodPlaneCollisionCaseStudy import (
    RodPlaneCollisionCaseStudy,
)
from elasticapp.Systems import CosseratRods  # noqa

from cli.help import print_description
from processing import run_study, replace_docstring
from processing.cli import clickify

case = RodPlaneCollisionCaseStudy


def setup(
        inclination_angle: float
):
    print_description(case)
    # Parameters below are one-to-one from PRL nest
    base_radius = 1.21e-3  # m
    case_params = case.params(
        n_elements=8,
        length=0.07574,  # m
        radius=base_radius,  # m
        youngs_modulus=12e9,  # Pa
        shear_modulus=8e9,  # Pa
        damping_rate=1e-10,  # lowest value allowed for energy preservation
        gravitational_acc=-9.80665,
        contact_stiffness=1e-1,
        contact_damping=1e-10,  # lowest value allowed for energy preservation
        contact_detection_time_interval=5e-6,
        # Set it from 0.0 and start increasing, adjust based on energy conservation
        initial_min_distance_from_plane=5.0 * base_radius,
        angle_of_inclination=inclination_angle,
        dt=4e-7,
        final_sim_time=1e6,  # Does not matter
    )
    return case(case_params), case_params


@clickify()
@replace_docstring(case.__doc__)
def run(
        inclination: float,
        csv: bool,
        fig: bool,
):
    run_study(*setup(inclination), csv, fig)


if __name__ == "__main__":
    run()
