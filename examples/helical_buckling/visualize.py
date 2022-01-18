import numpy as np
from elasticapp import Blaze  # noqa
from elasticapp.CaseStudies.PyHelicalBucklingCaseStudy import HelicalBucklingCaseStudy

# from matplotlib import pyplot as plt
# from matplotlib.colors import to_rgb
from tqdm import tqdm


def run_example(
        save_to_csv: bool = False,
        save_figure: bool = False,
):
    sample_interval = 1000

    n_elem = 100
    base_length = 100.0
    dt = 0.001 * (base_length / n_elem)
    final_time = 40.0
    damping = 0.01
    time_interval = sample_interval * dt
    n_steps = int(final_time / time_interval)

    # measure_index = (n_elem // 2) + 5
    measure_index = 55

    cs = HelicalBucklingCaseStudy(
        n_elements=n_elem,
        dt=dt,
        damping=damping,
    )

    positions = []

    def collect_stats_from(cas):
        p, _, v, _ = cas.communicate()
        positions.append(np.copy(np.asarray(p)[..., measure_index]))

    collect_stats_from(cs)

    for step in tqdm(range(n_steps)):
        cs.run_for(additional_time=time_interval)
        collect_stats_from(cs)

    positions = np.asarray(positions)
    print(positions)


if __name__ == "__main__":
    run_example()
