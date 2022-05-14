import numpy as np

from cli import print_description
from processing import run_rod_collision_case_study, RodCollisionCaseStudy

if __name__ == "__main__":
    print_description(RodCollisionCaseStudy)
    angle_of_inclination = np.deg2rad(90.0)
    run_rod_collision_case_study(angle_of_inclination)
