from collections.abc import Iterable
from typing import Any

def these_are_all_disjoint(*iterables: Iterable[Any]) -> bool:
    sets = tuple(set(iterable) for iterable in iterables)
    union_of_all_sets = set().union(*sets)
    return len(union_of_all_sets) == sum(len(set_) for set_ in sets)
