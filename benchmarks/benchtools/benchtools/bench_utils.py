import subprocess
import xml.etree.ElementTree as ET
from collections import defaultdict
from dataclasses import dataclass, field
from functools import partial
from typing import NewType, List, Dict
from tqdm import tqdm


@dataclass
class RunResults:
    n: list[int] = field(default_factory=list)
    mean: list[float] = field(default_factory=list)
    stddev: list[float] = field(default_factory=list)
    name: str = ""


PositiveInt = NewType("PositiveInt", int)
BenchResults = List[RunResults]
Elements = List[PositiveInt]


class BenchmarkApp:
    # intenral
    Results = Dict[str, RunResults]

    def __init__(self, application_name: str):
        self.app_name = application_name

    def run_app(self, n_value: PositiveInt) -> bytes:
        result = subprocess.run(
            [self.app_name, "-r", "xml", "--n_values", str(n_value)], capture_output=True, text=True
        )
        result.check_returncode()
        return result.stdout

    def collect_data_from_single_run(self, n_value: PositiveInt, results: Results):
        # updates results in place
        root = ET.fromstring(self.run_app(n_value))

        for item in root.findall('./Group/TestCase/BenchmarkResults'):
            name = item.get('name')
            results[name].n.append(n_value)

            for child in item:
                if child.tag == 'mean':
                    results[name].mean.append(float(child.get('value')))
                if child.tag == 'standardDeviation':
                    results[name].stddev.append(float(child.get('value')))

    def __call__(self, n_values: Elements) -> BenchResults:
        results = defaultdict(RunResults)
        collect_data_from = partial(self.collect_data_from_single_run, results=results)

        for val in tqdm(n_values):
            collect_data_from(val)

        for k, v in results.items():
            v.name = k

        return list(results.values())

    def run(self, *args, **kwargs):
        return self.__call__(*args, **kwargs)


def export_as_table(results: BenchResults, tablefmt: str = "github"):
    from tabulate import tabulate
    from functools import reduce

    def fmt(x, y):
        return x + [[y.name] + y.mean]

    def header(x):
        return ["Kernel \ n"] + x.n

    return tabulate(
        reduce(fmt, results, []), headers=header(results[0]), tablefmt=tablefmt
    )


def plot_times(results):
    from matplotlib import pyplot as plt
    import numpy as np

    with plt.style.context('ggplot'):
        fig = plt.figure(figsize=(5, 4), dpi=150)
        ax = fig.add_subplot(111)
        ax.set_xscale("log", nonpositive='clip')
        ax.set_yscale("log", nonpositive='clip')

        temp = 0.0

        for result in results:
            ax.errorbar(
                result.n,
                result.mean,
                yerr=result.stddev,
                # c=to_rgb("xkcd:reddish"),
                label=result.name,
                lw=2,
                elinewidth=1,
                capsize=1
            )
            temp += np.asarray(result.mean) / np.asarray(result.n)

        scaling_factor = temp / len(results)
        ax.plot(result.n, scaling_factor * result.n, "k--", lw=1)

        ax.legend(bbox_to_anchor=(0, 1, 1, 0), loc="lower left", mode="expand", ncol=2)
        ax.set_xlabel("n")
        ax.set_ylabel("time")

    return fig


def plot_speedup(results):
    from matplotlib import pyplot as plt
    import numpy as np

    with plt.style.context('ggplot'):
        fig = plt.figure(figsize=(5, 4), dpi=150)
        ax = fig.add_subplot(111)

        n_samples = len(results[0].n)
        n_kernels = len(results)

        means = np.zeros((n_kernels, n_samples))
        stddevs = np.zeros_like(means)

        for idx, result in enumerate(results):
            means[idx] = np.asarray(result.mean)
            stddevs[idx] = np.asarray(result.stddev)

        means /= np.amax(means, axis=0)
        stddevs /= np.amax(means, axis=0)

        ind = np.arange(n_samples)
        width = 0.8 / (n_kernels)

        for idx, result in enumerate(results):
            ax.bar(
                ind + idx * width,
                1.0 / means[idx],
                width,
                label=result.name,
                align='center'
            )
        ax.set_xticks(ind + (n_kernels - 1) * width / 2, results[0].n)
        ax.legend(bbox_to_anchor=(0, 1, 1, 0), loc="lower left", mode="expand", ncol=2)
        ax.set_xlabel("n")
        ax.set_ylabel("speedup")

    return fig
