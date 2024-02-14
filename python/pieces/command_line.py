import sys

from collections import deque
from collections.abc import Iterable
from collections.abc import Callable
from collections.abc import Set
from typing import Any
from typing import TypeAlias

from .utils import these_are_all_disjoint

ArgumentTransformer: TypeAlias = Callable[[str], str]
ArgumentChoices:     TypeAlias = Iterable[str]

class CommandLineDeque(deque[str]):
    __last_arg_popped: str | None

    def __init__(self):
        super().__init__(sys.argv[1:])

    def unshift_and_transform_next_valid_arg(
            self,
            *,
            next_arg_as: ArgumentTransformer | None=None,
            **kwargs:    ArgumentChoices
            ) -> str | None:

        if not these_are_all_disjoint(*kwargs.values()):
            raise ValueError("Each option in kwargs must map to a disjoint set.")

        for key in kwargs:
            result = self.unshift_next_valid_arg(next_arg_as=next_arg_as, valid_args=kwargs[key])
            if result is not None:
                return key

        return None

    def unshift_next_valid_arg(
            self,
            *,
            next_arg_as: ArgumentTransformer | None=None,
            valid_args:  ArgumentChoices
            ) ->         str | None:

        if len(self) < 1:
            return None

        if not isinstance(valid_args, frozenset):
            valid_args = frozenset(valid_args)

        if any(not isinstance(valid_arg, str) for valid_arg in valid_args):
            raise ValueError("valid_args must contain only strings.")

        next_arg = self[0]
        if callable(next_arg_as):
            next_arg = next_arg_as(next_arg)

        return self.unshift_next_arg() if next_arg in valid_args else None

    def unshift_next_arg(
            self
            ) ->  str | None:
        """Pops the next arg"""
        self.__last_arg_popped = self.popleft() if len(self) > 0 else None
        return self.__last_arg_popped
    
    @property
    def last_arg_popped(
            self
            ) -> str | None:
        try:
            return self.__last_arg_popped
        except AttributeError:
            return None
