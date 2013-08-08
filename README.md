### A deterministic pseudorandom symbolic perturbation scheme for arbitrary polynomial predicates

Latex source and notes for a paper on symbolic perturbation of geometric predicates.
See the latex document for details.  For code, see the `exact` directory of
https://github.com/otherlab/core, possibly in the `exact` branch.

#### Paper

To build the paper, install and run scons or do

    pdflatex perturb
    bibtex perturb
    pdflatex perturb

#### Results

To rerun the examples and benchmarks, first install and build `other/core` following the instructions
at https://github.com/otherlab/core.  You may need to check out the `exact` branch to get symbolic
perturbation functionality.  Make sure the `config.py` file in `core` or `other` contains the lines

    import os
    PREFIX = os.environ['OTHER']+'/install/$type'

and build the C++ helper routines used by the benchmarks via

    cd helper
    $OTHER/core/build/setup # Ignore the __init__.py warning
    scons

Example visualization and benchmarks are then available through the `example` script.  To reproduce
the figures in the paper, run one of

    ./examples delaunay --count 2000 --plot 1
    ./examples delaunay --count 1000000
    ./examples circles --plot 1 --count 1000
    ./examples --mode circles --count 1000 --min-count 10

#### Contact

For comments, questions, or issues with the above directions, please contact Geoffrey Irving at <irving@naml.us>.
We plan to set up a mailing list for discussions about `other/core`, but have not yet done so.
