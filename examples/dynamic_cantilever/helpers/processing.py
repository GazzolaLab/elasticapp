import matplotlib.pyplot as plt
import numpy as np
from matplotlib.colors import to_rgb

from .analytical import setup_analytical
from .data import Stats, CaseStudyParameters, RecordingParameters


def analyze_frequencies(
    stats: Stats, params: CaseStudyParameters, record_params: RecordingParameters
):
    from scipy.fft import fft, fftfreq
    from scipy.signal import find_peaks

    # FFT
    amplitudes = np.abs(fft(stats.tip_deflection))
    fft_length = len(amplitudes)
    amplitudes = amplitudes * 2 / fft_length
    omegas = (
        fftfreq(fft_length, params.dt * record_params.sample_interval) * 2 * np.pi
    )  # [rad/s]

    try:
        peaks, _ = find_peaks(amplitudes)
        peak = peaks[np.argmax(amplitudes[peaks])]

        simulated_frequency = omegas[peak]
        analytical_cantilever = setup_analytical(params)
        theoretical_frequency = analytical_cantilever.get_omega()

        simulated_amplitude = max(stats.tip_deflection)
        theoretical_amplitude = analytical_cantilever.get_amplitude()

        return {
            "omegas": omegas,
            "amplitudes": amplitudes,
            "peak": peak,
            "simulated_frequency": simulated_frequency,
            "theoretical_frequency": theoretical_frequency,
            "simulated_amplitude": simulated_amplitude,
            "theoretical_amplitude": theoretical_amplitude,
        }

    except RuntimeError:
        print("No peaks detected: change input parameters.")


# improve
def report_frequencies(post_processed_results):
    for k in [
        "simulated_frequency",
        "theoretical_frequency",
        "simulated_amplitude",
        "theoretical_amplitude",
    ]:
        print(f"{k} : {post_processed_results[k]}")


# Plotting frequency and amplitudes against densities
def density_frequency_plot(
    densities,
    theory_frequency,
    sim_frequency,
) -> plt.Figure:
    with plt.style.context("ggplot"):
        fig = plt.figure(figsize=(5, 4), dpi=150)
        ax = fig.add_subplot(111)
        ax.plot(
            densities,
            theory_frequency,
            color="k",
            marker="o",
            ms=8,
            lw=2,
            label="theoretical_frequency",
        )
        ax.plot(
            densities,
            sim_frequency,
            c=to_rgb("xkcd:reddish"),
            marker="o",
            ms=8,
            lw=2,
            label="simulated_frequency",
        )
        ax.set_xlabel(r"Density [$kg/m^3$]")
        ax.set_ylabel(r"Frequency [$rad/s$]")
        ax.legend()

        fig.show()

    return fig


def density_amplitude_plot(
    densities,
    theory_amplitude,
    sim_amplitude,
) -> plt.Figure:
    with plt.style.context("ggplot"):
        fig = plt.figure(figsize=(5, 4), dpi=150)
        ax = fig.add_subplot(111)
        ax.plot(
            densities,
            theory_amplitude,
            color="k",
            marker="o",
            ms=8,
            lw=2,
            label="theoretical_amplitude",
        )
        ax.plot(
            densities,
            sim_amplitude,
            c=to_rgb("xkcd:reddish"),
            marker="o",
            ms=8,
            lw=2,
            label="simulated_amplitude",
        )
        ax.set_xlabel(r"Density [$kg/m^3$]")
        ax.set_ylabel(r"Amplitude [$m$]")
        ax.legend()

        fig.show()

    return fig


def plot_tip_displacement(params: CaseStudyParameters, stats: Stats) -> plt.Figure:
    with plt.style.context("ggplot"):
        fig = plt.figure(figsize=(5, 4), dpi=150)
        ax = fig.add_subplot(111)

        time = np.array(stats.time)
        ax.plot(
            time,
            stats.tip_deflection,
            c=to_rgb("xkcd:reddish"),
            lw=2.0,
            label="Cosserat rod model",
        )

        analytical_cantilever_soln = setup_analytical(params)
        positions = analytical_cantilever_soln.get_time_dependent_positions(1, time)

        ax.plot(time, positions, c="k", lw=2.0, label="Euler-Bernoulli beam theory")
        ax.legend()

        ax.set_xlabel("Time [s]")
        ax.set_ylabel("Displacement [m]")
        fig.show()

    return fig


def bode_plot(params: CaseStudyParameters, analyze_frequency_results) -> plt.Figure:
    omegas = analyze_frequency_results["omegas"]
    amplitudes = analyze_frequency_results["amplitudes"]
    peak = analyze_frequency_results["peak"]

    with plt.style.context("ggplot"):
        fig = plt.figure(figsize=(5, 4), dpi=150)
        fft_ax = fig.add_subplot(111)
        fft_ax.plot(omegas, amplitudes, lw=2, c=to_rgb("xkcd:bluish"))

        analytical_cantilever_soln = setup_analytical(params)
        fft_ax.set_xlim([0, analytical_cantilever_soln.get_omega() * 2])

        fft_ax.set_xlabel("Frequency [rad/s]")
        fft_ax.set_ylabel("Amplitude [m]")

        fft_ax.plot(omegas[peak], amplitudes[peak], "*", color="red", ms=8)
        fig.show()

    return fig
