# hadddraw

Program to add and draw histograms in multiple ROOT files.
Hopefully useful in quickly drawing many histograms generated in split jobs.


## Usage

Available in the help message, `hadddraw -h`.


## Build Procedure

```
source setup.sh
cmake-this
make-this
```

Any ROOT version should work.
The E1039 online version is selected by default in `setup.sh`.
The version used in the build is added to the run-time search path (`rpath`) of the executable.
Therefore, when you execute the program, the ROOT version must be of course accessible but need not be sourced via `thisroot.sh`.


The executable is installed to `~/bin/`.


## Contact

Kenichi Naknao <knakano0524@gmail.com>
