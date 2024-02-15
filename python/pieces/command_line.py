import sys

from collections import deque
from collections.abc import Iterable
from collections.abc import Callable
from typing import TypeAlias
from typing import TypeGuard

ArgumentTransformer: TypeAlias = Callable[[str], str]
ArgumentList:        TypeAlias = Iterable[str]
ArgumentChecker:     TypeAlias = Callable[[str], bool]

ArgumentGroup: TypeAlias = str | object | None
Argument:      TypeAlias = str | object | None

INVALID_ARGUMENT = object()

def is_valid_arg(arg) -> TypeGuard[str]:
    return isinstance(arg, str)

class CommandLineDeque(deque[str]):
    __last_arg: str | None

    def __init__(self, iterable: ArgumentList=None) -> None:
        super().__init__(iterable if iterable else sys.argv[1:])

    def unshift_and_transform_next_valid_arg(
            self,
            *,
            next_arg_as:          ArgumentTransformer | None=None,
            **groups_to_checkers: ArgumentChecker
            ) ->                  tuple[ArgumentGroup, Argument]:

        actual_arg = None

        for group in groups_to_checkers:
            actual_arg = self.unshift_next_valid_arg(next_arg_as=next_arg_as, is_valid_arg=groups_to_checkers[group])
            if is_valid_arg(actual_arg):
                return group, actual_arg

        return actual_arg, actual_arg

    def unshift_next_valid_arg(
            self,
            *,
            next_arg_as:  ArgumentTransformer | None=None,
            is_valid_arg: ArgumentChecker
            ) ->          Argument:

        if len(self) < 1:
            return None

        next_arg = self[0]
        if callable(next_arg_as):
            next_arg = next_arg_as(next_arg)

        return self.unshift_next_arg() if is_valid_arg(next_arg) else INVALID_ARGUMENT

    def unshift_next_arg(self) ->  str | None:
        """Pops the next arg"""
        self.__last_arg = self.popleft() if len(self) > 0 else None
        return self.__last_arg

    def unshift_next_args(self, *, count: int) -> tuple[str | None, ...]:
        return tuple(self.unshift_next_arg() for x in range(count))

    @property
    def last_arg(self) -> str | None:
        try:
            return self.__last_arg
        except AttributeError:
            return None
