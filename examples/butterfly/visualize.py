import numpy as np
import pandas as pd
from matplotlib import pyplot as plt
from tqdm import tqdm

from elasticapp import Blaze  # noqa
from elasticapp.CaseStudies.PyButterflyCaseStudy import ButterflyCaseStudy


def run_example(
        save_to_csv: bool = False,
        save_figure: bool = False,
):
    sample_interval = 200

    n_elem = 2
    base_length = 3.0
    dt = 0.01 * (base_length / n_elem)
    final_time = 50.0
    time_interval = sample_interval * dt
    n_steps = int(final_time / time_interval)

    cs = ButterflyCaseStudy(
        n_elements=n_elem,
        length=base_length,
        dt=dt,
        angle_of_inclination=np.deg2rad(45.0)
    )

    stats = {'times': [], 'positions': [], 'elastic_energies': [], 'kinetic_energies': []}

    def collect(t):
        stats['times'].append(t)

    def collect_more(p, e, k):
        stats['positions'].append(np.copy(p))
        stats['elastic_energies'].append(e)
        stats['kinetic_energies'].append(k)

    def collect_stats_from(cas):
        collect(cas.current_time())
        collect_more(*cas.communicate())

    # initial
    collect_stats_from(cs)

    for step in tqdm(range(n_steps)):
        cs.run_for(additional_time=time_interval)
        # cs.step()

        # if not (step % 200):
        collect_stats_from(cs)

    # stats = pd.DataFrame(stats)

    fig = plt.figure(figsize=(5, 4), frameon=True, dpi=150)
    ax = fig.add_subplot(111)
    positions = stats["positions"].copy()

    # record first position
    first_position = positions.pop(0)
    ax.plot(first_position[2, ...], first_position[0, ...], "r--", lw=2.0)

    # final position is also separate
    last_position = positions.pop()
    ax.plot(last_position[2, ...], last_position[0, ...], "k--", lw=2.0)

    n_positions = len(positions)
    for i, pos in enumerate(positions):
        alpha = np.exp(i / n_positions - 1)
        ax.plot(pos[2, ...], pos[0, ...], "b", lw=0.6, alpha=alpha)

    if save_to_csv:
        pd.DataFrame(stats).to_csv("butterfly.csv")
    if save_figure:
        fig.savefig("butterfly.pdf")

    plt.show()


if __name__ == "__main__":
    run_example()
