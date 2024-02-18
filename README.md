# pieces: A simple file splitting/merging utility

`pieces` is a simple command-line utility that allows you
to split or merge any file.  Files can be split into _(and merged
from)_ consecutive halves, or interleaved halves.

It doesn't try to get any more complicated than this, i.e.
`pieces` doesn't try to do anything other than split files in
half, and merge those halves together.

Although the C implementation is the reference implementation, a
Python implementation (using only the standard library) exists in
this repository as well.

## Project Roadmap
Since equivalent functionality is provided by the `split` and
`cat` commands in Linux/Unix, this project is to serve more as
a sample of my coding style when it comes to projects of this
size, in multiple languages.

#### More implementations coming soon:
  * Java
  * Kotlin
  * Haskell
  * C++ (03 and >=11)
  * C#
  * Rust
  * Go
  * Ruby
  * PHP