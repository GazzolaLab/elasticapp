__doc__ = """Rod collision study processing helpers"""
__copyright__ = """Distributed under the MIT License. See LICENSE.txt for details."""

from .animate import animate_rods, plot_energies
from .stats import Stats
from .post_processing import post_process_and_block
from .run import run_simulation, CaseStudy, CaseStudyParameters


def run_study(
    cs: CaseStudy,
    params: CaseStudyParameters,
    save_to_csv: bool,
    save_figure: bool,
):
    stats = run_simulation(cs, params)
    return post_process_and_block(params, stats, save_to_csv, save_figure)
