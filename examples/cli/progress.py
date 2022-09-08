__doc__ = """Nested progress bars with multiprocessing"""
__copyright__ = """Distributed under the MIT License. See LICENSE.txt for details."""

import multiprocessing as mp
from dataclasses import dataclass
from typing import Callable, Iterable, List, Optional, Sequence, Union, Any, Dict

from rich.console import Console, Group
from rich.live import Live
from rich.panel import Panel
from rich.progress import (
    Progress,
    TextColumn,
    BarColumn,
    TaskProgressColumn,
    TimeElapsedColumn,
    TimeRemainingColumn,
    ProgressType,
    ProgressColumn,  # noqa
    TaskID,
)
from rich.style import StyleType


def _default_columns(
    style: StyleType = "bar.back",
    complete_style: StyleType = "bar.complete",
    finished_style: StyleType = "bar.finished",
    pulse_style: StyleType = "bar.pulse",
    show_speed: bool = True,
):
    """Default columns of a simulator"""
    columns: List[ProgressColumn] = [TextColumn("[bold blue]{task.description}")]
    columns.extend(
        (
            BarColumn(
                style=style,
                complete_style=complete_style,
                finished_style=finished_style,
                pulse_style=pulse_style,
            ),
            TaskProgressColumn(show_speed=show_speed),
            TimeElapsedColumn(),
            TimeRemainingColumn(),
        )
    )
    return columns


def track_simulation(
    sequence: Union[Sequence[ProgressType], Iterable[ProgressType]],
    description: str = "Simulating...",
    total: Optional[float] = None,
    auto_refresh: bool = True,
    console: Optional[Console] = None,
    transient: bool = False,
    get_time: Optional[Callable[[], float]] = None,
    refresh_per_second: float = 10,
    style: StyleType = "bar.back",
    complete_style: StyleType = "bar.complete",
    finished_style: StyleType = "bar.finished",
    pulse_style: StyleType = "bar.pulse",
    update_period: float = 0.1,
    disable: bool = False,
    show_speed: bool = True,
) -> Iterable[ProgressType]:
    """Track progress by iterating over a sequence.

    Args:
        sequence (Iterable[ProgressType]): A sequence (must support "len") you wish to iterate over.
        description (str, optional): Description of task show next to progress bar. Defaults to "Working".
        total: (float, optional): Total number of steps. Default is len(sequence).
        auto_refresh (bool, optional): Automatic refresh, disable to force a refresh after each iteration. Default is True.
        transient: (bool, optional): Clear the progress on exit. Defaults to False.
        console (Console, optional): Console to write to. Default creates internal Console instance.
        refresh_per_second (float): Number of times per second to refresh the progress information. Defaults to 10.
        style (StyleType, optional): Style for the bar background. Defaults to "bar.back".
        complete_style (StyleType, optional): Style for the completed bar. Defaults to "bar.complete".
        finished_style (StyleType, optional): Style for a finished bar. Defaults to "bar.done".
        pulse_style (StyleType, optional): Style for pulsing bars. Defaults to "bar.pulse".
        update_period (float, optional): Minimum time (in seconds) between calls to update(). Defaults to 0.1.
        disable (bool, optional): Disable display of progress.
        show_speed (bool, optional): Show speed if total isn't known. Defaults to True.
    Returns:
        Iterable[ProgressType]: An iterable of the values in the sequence.

    """
    columns = _default_columns(
        style=style,
        complete_style=complete_style,
        finished_style=finished_style,
        pulse_style=pulse_style,
        show_speed=show_speed,
    )
    progress = Progress(
        *columns,
        auto_refresh=auto_refresh,
        console=console,
        transient=transient,
        get_time=get_time,
        refresh_per_second=refresh_per_second or 10,
        disable=disable,
    )

    with progress:
        yield from progress.track(
            sequence, total=total, description=description, update_period=update_period
        )


@dataclass
class ProgressionStatus:
    """Tracks status of progression of a simulation"""

    current: int
    total: int


def p_map(
    func: Callable[[Any, Dict[TaskID, ProgressionStatus], TaskID], Any],
    *iterables: Union[Sequence[Any], Iterable[Any]],
    max_workers: int = mp.cpu_count(),
) -> List[Any]:
    """Performs a parallel ordered map with nested progress bars."""
    from concurrent.futures import ProcessPoolExecutor

    simulation_progress = Progress(
        *_default_columns(),
    )

    phase_space_progress = Progress(
        TextColumn("[bold green]{task.description}"),
        BarColumn(),
        TaskProgressColumn(),
        TimeElapsedColumn(),
    )

    progress_group = Group(
        phase_space_progress,
        Panel(Group(simulation_progress)),
    )

    # with mp.Pool(mp.cpu_count()) as pool:
    #     results = pool.map(simulate_axial_stretching_with, elements)
    with Live(progress_group):
        futures = []
        with mp.Manager() as manager:
            _progress = manager.dict()
            overall_progress_task = phase_space_progress.add_task("Overall progress")

            with ProcessPoolExecutor(max_workers=max_workers) as executor:
                for id, element in enumerate(*iterables):
                    task_id = simulation_progress.add_task(
                        f"[blue] Simulation no. {id + 1}", visible=False
                    )
                    futures.append(executor.submit(func, element, _progress, task_id))

                # monitor the progress:
                while (n_finished := sum([future.done() for future in futures])) < len(
                    futures
                ):
                    phase_space_progress.update(
                        overall_progress_task, completed=n_finished, total=len(futures)
                    )
                    for task_id, update_data in _progress.items():
                        current = update_data.current
                        total = update_data.total
                        # update the progress bar for this task:
                        simulation_progress.update(
                            task_id,
                            completed=current,
                            total=total,
                            visible=current < total,
                        )

                results = [future.result() for future in futures]

    return results
