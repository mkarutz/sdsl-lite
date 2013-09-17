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
  * Faithful to the original theoretical results.
  * Capable of handling large inputs (yes, we support 64-bit),
  * Provide efficient construction of all implemented succinct data structures, while at the same time enable good run-time performance.

<img align="right" src="extras/example-storage-visualization-smaller.png?raw=true" />

In addition we provide additional functionality which can help you use succinct
data structure to their full potential.

  * Each data structure can easily be serialized and loaded to/from disk.
  * We provide functionality which helps you analyze the storage requirements of any
  SDSL based data structure.
  * All implemented data structures are composable. For example, a compressed
  suffix tree can be composed of a variety different, smaller succinct data structures.
  We provide an intiuitive template interface to declare the succinct data structure
  you want to use.
  * We maintain an extensive collection of examples which help you use the different
  features provided by the library.
  * All data structures are tested for correctness using a unit-testing framework.
  * We provide a large collection of supporting documentation constisting of examples,
    cheat sheets, tutorial slides and walkthroughs.

List of Implemented Data Structures
--------

 * Bitvectors
   * An uncompressed mutual bitvector (`bit_vector`)
   * An uncompressed immutable bitvector ([bit_vector_il](./include/sdsl/bit_vector_il.hpp))
   * A ![H_0](http://latex.codecogs.com/gif.latex?H_0)-compressed
     immutable bitvector ([rrr_vector<>](./include/sdsl/rrr_vector.hpp))
   * A bitvector for sparse populated arrays ([sd_vector<>](./include/sdsl/sd_vector.hpp))
 * Rank Support (RS) and Select Support (SS)
   * Several rank and select implementations with different time-space
     trade-offs for the uncompressed bitvectors
     ([rank_support_v](./include/sdsl/rank_support_v.hpp),
      [rank_support_v5](./include/sdsl/ran_support_v5.hpp),
      [select_support_mcl](./include/sdsl/select_support_mcl.hpp), ...)
   * Rank and select for compressed bitvectors
     ([rank_support_rrr](./include/sdsl/rrr_vector.hpp),
      [rank_support_sd](./include/sdsl/sd_vector.hpp), ...)
 * Variable-length Coders
   * Elias- ![\delta](http://latex.codecogs.com/gif.latex?%5Cdelta) coder
     ([coder::elias_delta](./include/sdsl/coder_elias_delta.hpp))
   * Fibonacci-coder ([coder::fibonacci](./include/sdsl/coder_fibonacci.hpp))
 * Integer Vectors
   * Mutable vectors for (compile-time) fixed `w`-bit integers ([int_vector<w>](./include/sdsl/int_vector.hpp))
   * Mutable vector for (run-time) fixed `w`-bit integers ([int_vector<0>](./include/sdsl/int_vector.hpp),
     `w` passed to the constructor)
   * Immutable compressed integer vectors using a variable-length coder `coder`
     ([enc_vector<coder>](./include/sdsl/enc_vector.hpp),
     [vlc_vector<coder>](./include/sdsl/vlc_vector.hpp))
 * Wavelet Trees (WT) (all immutable)
   * Balanced wavelet tree for a byte-alphabet ([wt](./include/sdsl/wt.hpp))
   * Balanced wavelet tree for a integer-alphabet ([wt_int](./include/sdsl/wt_int.hpp))
   * Huffman-shaped wavelet tree for a byte-alphabet ([wt_huff](./include/sdsl/wt_huff.hpp))
   * Run-length compressed wavelet trees for a byte-alphabet
     ([wt_rlmn](./include/sdsl/wt_rlmn.hpp),
      [wt_rlg](./include/sdsl/wt_rlg.hpp),
      [wt_rlg8](./include/sdsl/wt_rlg8.hpp))
 * Compressed Suffix Arrays (CSA) (all immutable)
   * [csa_bitcompressed](./include/sdsl/csa_bitcompressed.hpp) is based on the bitcompressed SA and inverse SA.
   * [csa_wt](./include/sdsl/csa_wt.hpp) is based on a WT of the BWT.
   * [csa_sada](./include/sdsl/csa_sada.hpp) is based on the compressed
      ![\Psi](http://latex.codecogs.com/gif.latex?%5CPsi)-function
 * Balanced Parentheses Support (BPS) (all immutable)
   * A range-min-max-tree implementation ([bp_support_sada](./include/sdsl/bp_support_sada.hpp))
     to support operations `find_open`, `find_close`, `enclose`, `double_enclose`,...
   * Hierarchical solutions with pioneer parentheses
     ([bp_support_g](./include/sdsl/bp_support_g.hpp),
      [bp_support_gg](./include/sdsl/bp_support_gg.hpp))
 * Longest Common Prefix (LCP) Arrays (all immutable)
   * [lcp_bitcompressed](./include/sdsl/lcp_bitcompressed.hpp) is a bitcompressed version
   * [lcp_byte](./include/sdsl/lcp_byte.hpp) encodes small values with one
     byte and large values with two words
   * [lcp_dac](./include/sdsl/lcp_dac.hpp) used direct accessible codes
   * [lcp_wt](./include/sdsl/lcp_wt.hpp) stores small values in a WT and
     large value in on word.
   * [lcp_vlc](./include/sdsl/lcp_vlc.hpp) stores the values in a `vlc_vector`.
   * [lcp_support_sada](./include/sdsl/lcp_support_sada.hpp) uses a bitvector of 2n bits, a
     select structure supporting it, and the corresponding CSA.
   * [lcp_support_tree](./include/sdsl/lcp_support_tree.hpp) uses the topology of the
     corresponding CST.
   * [lcp_support_tree2](./include/sdsl/lcp_support_tree2.hpp) uses the corresponding CSA
   and CST.
 * Compressed Suffix Trees (CST) (all immutable)
   * [cst_sada](./include/sdsl/cst_sada.hpp) provides very fast navigation operations; worst case
     space |CSA|+|LCP|+4n+o(n)
   * [cst_sct3](./include/sdsl/cst_sct3.hpp) representing nodes as intervals in the suffix array;
     worst case space |CSA|+|LCP|+3n+o(n)
 * Range Minimum/Maximum Query (RMQ) Structures (all immutable)
   * Self-contained RMQ structure using 2n+o(n) bits or 4n+o(n) bits
     ([rmq_succinct_sct](./include/sdsl/rmq_succinct_sct.hpp),
      [rmq_succinct_sada](./include/sdsl/rmq_succinct_sada.hpp))
   * Non-succinct support structure for RMQ ([rmq_support_sparse_table](./include/sdsl/rmq_support_sparse_table.hpp))


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
