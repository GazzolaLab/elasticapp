from typing import TypeAlias, TypeVar

from numpy.typing import NDArray
from numpy import float64

T = TypeVar("T")
Array: TypeAlias = NDArray[T]

Double: TypeAlias = float64
