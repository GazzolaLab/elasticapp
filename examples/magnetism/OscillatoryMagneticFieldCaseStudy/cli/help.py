__doc__ = """Print help message"""
__copyright__ = """Distributed under the MIT License. See LICENSE.txt for details."""

from rich.console import Console
from rich.text import Text


def print_help(case_study_cls) -> None:
    """Prints help message of a case-study"""
    console = Console()
    text = Text(style="yellow")
    text.append(case_study_cls.__doc__)
    console.print(text)
