__doc__ = """Print help message"""
__copyright__ = """Distributed under the MIT License. See LICENSE.txt for details."""

from typing import Optional

from rich.console import Console
from rich.text import Text


def print_description(
    case_study_cls, additional_description: Optional[str] = None
) -> None:
    """Prints help message of a case-study"""
    console = Console()
    text = Text(style="yellow")
    text.append(case_study_cls.__doc__)
    if additional_description:
        text.append(additional_description)
    console.print(text)
