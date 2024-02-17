HELP_ARGS        = ("help", "--help", "-?")
SPLIT_ARGS       = ("split", "--split", "-s")
MERGE_ARGS       = ("merge", "--merge", "-m")
INTERLEAVED_ARGS = ("--interleaved", "-i")

def is_help_arg(s: str) -> bool:
    return s in HELP_ARGS

def is_split_arg(s: str) -> bool:
    return s in SPLIT_ARGS

def is_merge_arg(s: str) -> bool:
    return s in MERGE_ARGS

def is_interleaved_arg(s: str) -> bool:
    return s in INTERLEAVED_ARGS