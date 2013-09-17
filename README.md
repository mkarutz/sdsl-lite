SDSL - Succinct Data Structure Library
=========

What is it?
-----------

The Succinct Data Structure Library (SDSL) is a powerful and flexible C++11
library implementing many succinct data structures. Succinct data structures
can represent an object (such as a bitvector or a tree) in space close the
information-theoretic lower bound of the object while supporting operations
of the original object efficiently. The theoretical time complexity of an
operations performed on the classical data structure and the equivalent
succinct data structure are (most of the time) identical.

Why SDSL?
--------

Succinct data structures have very attractive theoretical properties. However,
in practice implementing succinct data structures is non-trivial as they are
often composed of complex operations on bitvectors. The SDSL Library provides
high quality, open source implementations of many succinct data structures
proposed in literature.

Specifically, the aim of the library is to provide basic and complex succinct
data structure which are

  * Easy and intuitive to use (like the [STL][STL], which provides classical data structures),
  * Capable of handling large inputs (yes, we support 64-bit),
  * Provide excellent performance in construction, and
  * Provide good run-time performance

![Space Visualization](extras/example-storage-visualization-smaller.png?raw=true)

The Latest Version
------------------

The latest version can be found on the SDSL github project page https://github.com/simongog/sdsl-lite .

Documentation
-------------

Requirements
------------

The SDSL library requires:

* A modern, C++11 ready compiler such as `g++` version 4.7 or higher or `clang` version 3.2 or higher.
* The [cmake]() build system.
* A 64-bit operating system. Either Mac OS X or Linux are currently supported.
* For increased performance the processor of the system should support fast bit operations available in `SSE4.2`

Installation
------------

To download and install the libary use the following commands.

```sh
git clone https://github.com/simongog/sdsl-lite.git
cd sdsl-lite
./install.sh
```

This installs the sdsl library into the `include` and `lib` directories in your
home directory. A different location prefix can be specified as a parameter of
the `install.sh` script:

```sh
./install /usr/local/
```

To remove the library from your system use the provided uninstall script:

```sh
./uninstall.sh
```

Examples
------------

Test
----

Bug Reporting
------------

While we use an extensive set of unit tests and test coverage tools you might
still find bugs in the library. We encourage you to report any problems with
the library via the [github issue tracking system](https://github.com/simongog/sdsl-lite/issues)
of the project.

Licensing
---------

The SDSL library is provided under the GNU General Public License (GPLv3). For
more information see the COPYING file in the library directory.

Lots of time was spent implementing the many features of the library. If you
use the library in an academic setting please cite the following paper:

_Simon Gog, Matthias Petri: Optimized Succinct Data Structures for Massive Data, Accepted for publication in Software, Practice and Experience_. **TODO LIBRARY PAPER**

Authors
--------

This project profits from excellent input of many coders. Timo Beller improved
the construction process during the last month and Matthias Petri contributed
new bitvectors and helped a lot in making the library more accessible. Stefan
Arnold helped us with tricky template questions. We a grateful to Kalle Karhu,
Dominik Kempa, and Shanika Kuruppu for bug reports.

Contribute
----------

Are you working on a new or improved implementation of a succinct data structure?
We encourage you to contribute your implementation to the SDSL library to make
your work accessible to the community within the existing library framework.
Feel free to contact any of the authors or create an issue on the
[issue tracking system](https://github.com/simongog/sdsl-lite/issues).


[STL]: http://www.sgi.com/tech/stl/ "Standart Template Library"
[pz]: http://pizzachili.di.unipi.it/ "Pizza&amp;Chli"
[d3js]: http://d3js.org "D3JS library"
[cmake]: http://www.cmake.org/ "CMake tool"
[MAKE]: http://www.gnu.org/software/make/ "GNU Make"
[gcc]: http://gcc.gnu.org/ "GNU Compiler Collection"
[DBLPCSTRES]: http://people.eng.unimelb.edu.au/sgog/sdsl_explore/dblp.xml.100MB_cst_sada_wt_rlmn_lcp_tree2.html "CST visualization"
[SPE]: http://people.eng.unimelb.edu.au/sgog/optimized.pdf "Preprint SP&amp;E article"
[DIVSUF]: http://code.google.com/p/libdivsufsort/ "libdivsufsort"
[LS]: http://www.sciencedirect.com/science/article/pii/S0304397507005257 "Larson &amp; Sadakane Algorithm"
[GTEST]: https://code.google.com/p/googletest/ "Google C++ Testing Framework"
