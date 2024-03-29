# pieces: A simple file splitting/merging utility

`pieces` is a simple command-line utility that allows you
to split or merge any file.  Files can be split into _(and merged
from)_ consecutive halves, or interleaved halves.

It doesn't try to get any more complicated than this, i.e.
`pieces` doesn't try to do anything other than split files in
half, and merge those halves together.

The [Python implementation](./python/) (that uses only the
standard library) is the reference implementation and defines
the features that all other implementations must support.
Differences in appearance are allowed, but differences in
functionality are not.

Although the [Python implementation](./python/) defines the behavior,
it is recommended to use the [C implementation](./c/) in practice
for speed.  Feel free to examine the code for any unsafe memory
practices 😉
* This will more than likely change when a Go, Rust, or Modern
  C++ implementation exists.

## Why Write This?

Since equivalent functionality is provided by the `split` and
`cat` commands in Linux/Unix, this project is to serve more as
a sample of my coding style when it comes to projects of this
size, in multiple languages.

**Here are a few guidelines I'm forcing myself to follow**:
* A help screen using colors to make options stand out is okay
* If a combination of command line options is rejected/accepted
  by the [Python implementation](./python/), all other implementations
  must reject/accept that combination.
* For all implementations in languages X and Y, A file split using
  `pieces split` implemented X must be able to be merged back into
  the original file using `pieces merge` implemented Y.

## Requirements / How to Run

1. First, clone this repository:
   ```sh
   git clone https://github.com/thepeoplescoder/pieces
   ```
2. Afterwards, look at the README file for the implementation
   you wish to run.  From there, you will find the requirements
   and directions on how to run that specific implementation.

## Command Line Usage (caveats intentionally omitted)

```
Usage:
   pieces [help | --help | -?]
   pieces {split | --split | -s} infile  outfile1 outfile2 [--interleaved | -i]
   pieces {merge | --merge | -m} infile1 infile2  outfile  [--interleaved | -i]

      help, --help, -?              Displays this help section.
      split, --split, -s            Splits infile into outfile1 and outfile2.
      merge, --merge, -m            Merges infile1 and infile2 into outfile.
      --interleaved, -i             Split or merge interleaved.
                                    If omitted, splitting/merging is
                                    done in consecutive chunks.

   Arguments in [] are optional, arguments in {} are required.
```

## Current Features

1. This application is deliberately kept simple, with
   three subcommands: `help`, `split`, and `merge`.
2. Files can be split/merged into chunks of consecutive or
   alternating (interleaved) bytes.
3. A basic check on file lengths happens when files are being merged,
   e.g. a 7 byte file and a 3 byte file are **NOT** the result of the
   same split operation, and `pieces` will reject attempting to merge
   these files.
4. Files created by `pieces split` are guaranteed to be "mergeable"
   with `pieces merge`.  See the previous list item on checking
   file lengths.

## Current Limitations

1. Files can deliberately only be split in half, and merged from
   halves (where the first file can only be larger than the second
   file by no more than one byte).
   * This is a safeguard put in place to prevent merging the wrong
     files.
   * In order to split files into even smaller chunks, run
     `pieces split` on each piece generated by the previous
     run of `pieces split`.  Do this iteratively until the 
     desired chunk size is reached.  **This is intentional**.
2. Files **cannot** be split into thirds, fourths, fifths, etc.  See
   the previous list item.

## Project Roadmap
* Finish documentation for each existing implementation
  (including READMEs):
  * Python
  * C
* More implementations coming soon:
  * Python (using 3rd-party libraries)
  * Java
  * Kotlin
  * Groovy
  * Scala
  * Haskell
  * C++ (03 and >=11)
  * C#
  * Rust
  * Go
  * Ruby
  * PHP