Flye-win32
==============

Building Requirements
---------------------

* MSYS2 UCRT64 environment
* Python 2.7 or 3.5+
* C++ compiler with C++11 support
* GNU make
* Git
* Core OS development headers (zlib, dlfcn, ...)

Building with MSYS2
-------------------

Get inside your `MSYS2 UCRT64` environment and install the required packages
```sh
# install GCC toolchain with basic libraries including zlib
pacman -Sy mingw-w64-ucrt-x86_64-toolchain

# install the rest packages
pacman -Sy mingw-w64-ucrt-x86_64-python3
pacman -Sy make git mingw-w64-ucrt-x86_64-dlfcn
```

Build Flye-win32 from source
```sh
git clone https://github.com/NewComer00/Flye-win32 --depth 1
cd Flye-win32
make
```

Running with MSYS2
------------------

Then, Flye will be available as
```sh
python bin/flye
```

Optionally, run some tests to ensure that installation was successful
```sh
python flye/tests/test_toy.py
```

Running on Native Windows
-------------------------

**IMPORTANT**: Please ensure `python.exe` and `bash.exe` is in the *PATH*. The former is available on [www.python.org](https://www.python.org), and the latter could be found in `MSYS2`, `Cygwin`, `Git for Windows`, `MSYS` or even `WSL/WSL2`.

Run Flye on `PowerShell` or `CMD`
```powershell
python bin\flye
```

Run the test
```powershell
python flye\tests\test_toy.py
```

<details>
<summary>Show the log of test script on PowerShell</summary>

```powershell
PS D:\msys64\home\AI\Flye-win32> python flye/tests/test_toy.py
Running toy test:

[2024-02-18 17:51:22] INFO: Starting Flye 2.9.3-b1797
[2024-02-18 17:51:22] INFO: >>>STAGE: configure
[2024-02-18 17:51:22] INFO: Configuring run
[2024-02-18 17:51:22] INFO: Total read length: 8397200
[2024-02-18 17:51:22] INFO: Input genome size: 500000
[2024-02-18 17:51:22] INFO: Estimated coverage: 16
[2024-02-18 17:51:22] INFO: Reads N50/N90: 2700 / 1382
[2024-02-18 17:51:22] INFO: Selected minimum overlap: 1000
[2024-02-18 17:51:22] INFO: >>>STAGE: assembly
[2024-02-18 17:51:22] INFO: Assembling disjointigs
[2024-02-18 17:51:22] INFO: Reading sequences
[2024-02-18 17:51:22] INFO: Building minimizer index
[2024-02-18 17:51:22] INFO: Pre-calculating index storage
0% 10% 20% 30% 40% 50% 60% 70% 80% 90% 100%
[2024-02-18 17:51:22] INFO: Filling index
0% 10% 20% 30% 40% 50% 60% 70% 80% 90% 100%
[2024-02-18 17:51:22] INFO: Extending reads
[2024-02-18 17:51:25] INFO: Overlap-based coverage: 15
[2024-02-18 17:51:25] INFO: Median overlap divergence: 0.0138191
0% 10% 20% 50% 90% 100%
[2024-02-18 17:51:25] INFO: Assembled 6 disjointigs
[2024-02-18 17:51:25] INFO: Generating sequence
0% 10% 20% 30% 40% 50% 60% 70% 80% 90% 100%
[2024-02-18 17:51:25] INFO: Filtering contained disjointigs
0% 10% 30% 50% 60% 80% 100%
[2024-02-18 17:51:25] INFO: Contained seqs: 1
[2024-02-18 17:51:25] INFO: >>>STAGE: consensus
[2024-02-18 17:51:25] INFO: Running Minimap2
[2024-02-18 17:51:30] INFO: Computing consensus
[2024-02-18 17:51:34] INFO: Alignment error rate: 0.018129
[2024-02-18 17:51:34] INFO: >>>STAGE: repeat
[2024-02-18 17:51:34] INFO: Building and resolving repeat graph
[2024-02-18 17:51:34] INFO: Parsing disjointigs
[2024-02-18 17:51:34] INFO: Building repeat graph
0% 20% 40% 60% 80% 100%
[2024-02-18 17:51:34] INFO: Median overlap divergence: 0.00338524
[2024-02-18 17:51:34] INFO: Parsing reads
[2024-02-18 17:51:34] INFO: Aligning reads to the graph
0% 10% 20% 30% 40% 50% 60% 70% 80% 90% 100%
[2024-02-18 17:51:34] INFO: Aligned read sequence: 8305781 / 8396200 (0.989231)
[2024-02-18 17:51:34] INFO: Median overlap divergence: 0.00694442
[2024-02-18 17:51:34] INFO: Mean edge coverage: 19
[2024-02-18 17:51:34] INFO: Simplifying the graph
[2024-02-18 17:51:34] INFO: >>>STAGE: contigger
[2024-02-18 17:51:34] INFO: Generating contigs
[2024-02-18 17:51:34] INFO: Reading sequences
[2024-02-18 17:51:34] INFO: Generated 4 contigs
[2024-02-18 17:51:34] INFO: Added 0 scaffold connections
[2024-02-18 17:51:34] INFO: >>>STAGE: polishing
[2024-02-18 17:51:34] INFO: Polishing genome (1/1)
[2024-02-18 17:51:34] INFO: Running minimap2
[2024-02-18 17:51:35] INFO: Separating alignment into bubbles
[2024-02-18 17:51:40] INFO: Alignment error rate: 0.009640
[2024-02-18 17:51:40] INFO: Correcting bubbles
0% 10% 20% 30% 40% 50% 60% 70% 80% 90% 100%
[2024-02-18 17:51:44] INFO: >>>STAGE: finalize
[2024-02-18 17:51:44] INFO: Assembly statistics:

        Total length:   417315
        Fragments:      4
        Fragments N50:  225779
        Largest frg:    225779
        Scaffolds:      0
        Mean coverage:  20

[2024-02-18 17:51:44] INFO: Final assembly: D:\msys64\home\AI\Flye-win32\flye_toy_test\assembly.fasta

TEST SUCCESSFUL
PS D:\msys64\home\AI\Flye-win32>
```

</details>


Flye assembler
==============

[![BioConda Install](https://img.shields.io/conda/dn/bioconda/flye.svg?style=flag&label=BioConda%20install)](https://anaconda.org/bioconda/flye)

### Version: 2.9.3

Flye is a de novo assembler for single-molecule sequencing reads,
such as those produced by PacBio and Oxford Nanopore Technologies.
It is designed for a wide range of datasets, from small bacterial projects
to large mammalian-scale assemblies. The package represents a complete
pipeline: it takes raw PacBio / ONT reads as input and outputs polished contigs.
Flye also has a special mode for metagenome assembly.

Currently, Flye will produce collapsed assemblies of diploid genomes, 
represented by a single mosaic haplotype. To recover two phased haplotypes
consider applying [HapDup](https://github.com/fenderglass/hapdup) after the assembly.

Manuals
-------

- [Installation instructions](docs/INSTALL.md)
- [Usage](docs/USAGE.md)
- [FAQ](docs/FAQ.md)

Latest updates
--------------

### Flye 2.9.3 release (28 November 2023)
* Disjointig step speedup for `--nano-hq` mode
* Improved `--keep-haplotypes` mode preserves more heterozygous SVs
* A few bug fixes


### Flye 2.9.2 release (18 March 2023)
* Update to minimap 2.24 + using HiFi and Kit14 parameters for faster alignment
* Fixed a few small bugs and corner cases
* Polisher now outputs read depth for each base of consensus (can be used as confidence measure)

### Flye 2.9.1 release (07 Aug 2022)
* New option --no-alt-contigs to remove all non-primary contigs from the assembly
* Fixed crash on MacOS with Python 3.8+
* Fixed rare artificially introduced mismatches while polishing
* Fixed slow simplification of very tangled graphs
* Various other small fixes

### Flye 2.9 release (20 Aug 2021)
* Better assembly of very short sequences (e.g. plasmids or viruses). They were often missed in previous versions.
* New --nano-hq mode for ONT Guppy5+ (SUP mode) and Q20 reads (3-5% error rate)
* Optimized default parameters for HiFi (HPC error threshold 0.01 -> 0.001; increased min overlap)
* Polishing improvements: reduced number of possible clusters of errors
* Improvements in repeat detection algorithm to further limit a chance of (otherwise infrequent) misassemblies
* Scaffolding is no longer performed by default (could be enabled with --scaffold)
* Bam file input for the standalone polisher (same interface as for FASTA/Q)
* Automatically selected minimum overlap up to 10k (was 5k)
* Discontinued --plasmid option due to the improvements in short sequences assembly
* --trestle and --subassemblies modes are now deprecated, and will be removed in the future versions
* New --extra-params option to modify config-level parameters
* Contig paths output in Gfa + number of reads supporting each link (RC tag)
* Update to minimap 2.18
* Several rare bug fixes/other improvements


Repeat graph
------------

Flye is using a repeat graph as the core data structure. 
Compared to de Bruijn graphs (which require exact k-mer matches),
repeat graphs are built using approximate sequence matches, and
can tolerate the higher noise of SMS reads.

The edges of a repeat graph represent the genomic sequence, and nodes define
the junctions. Each edge is classified as unique or repetitive.
The genome traverses the graph (in an unknown way), so each unique
edge appears exactly once in this traversal. Repeat graphs reveal the
repeat structure of the genome, which helps to reconstruct an optimal assembly.


<p align="center">
  <img src="docs/graph_example.png" alt="Graph example"/>
</p>

Above is an example of the repeat graph of a bacterial assembly.
Each edge is labeled with its id, length, and coverage. Repetitive edges are shown
in color, and unique edges are black. Note that each edge is represented in 
two copies: forward and reverse complement (marked with +/- signs), 
therefore the entire genome is represented in two copies. This is necessary
because the orientation of input reads is unknown.

In this example, there are two unresolved repeats: (i) a red repeat of 
multiplicity two and length 35k and (ii) a green repeat cluster of multiplicity
three and length 34k - 36k. As the repeats remained unresolved, there are no reads
in the dataset that cover those repeats in full. Five unique edges 
will correspond to five contigs in the final assembly.

Repeat graphs produced by Flye could be visualized using
[AGB](https://github.com/almiheenko/AGB) or [Bandage](https://github.com/rrwick/Bandage).


Flye benchmarks
---------------

| Genome                   | Data           | Asm.Size  | NG50     | CPU time  | RAM    |
|--------------------------|----------------|-----------|----------|-----------|--------|
| [E.coli][ecoli]          | PB 50x         | 4.6 Mb    | 4.6 Mb   | 2 h       | 2 Gb   |
| [C.elegans][ce]          | PB 40x         | 107 Mb    | 2.7 Mb   | 100 h     | 31 Gb  |
| [A.thaliana][at]         | PB 75x         | 120 Mb    | 8.7 Mb   | 100 h     | 59 Gb  |
| [D.melanogaster][dm-ont] | ONT 30x        | 136 Mb    | 13.8 Mb  | 130 h     | 33 Gb  |
| [D.melanogaster][dm-pb]  | PB 120x        | 141 Mb    | 11.5 Mb  | 150 h     | 70 Gb  |
| [Human NA12878][na12878] | ONT 35x (rel6) | 2.8 Gb    | 30.3 Mb  | 3100 h    | 394 Gb |
| [Human CHM13 ONT][t2t]   | ONT 120x (rel5)| 2.9 Gb    | 69.5 Mb  | 4000 h    | 450 Gb |
| [Human CHM13 HiFi][t2t]  | PB HiFi 30x    | 3.0 Gb    | 34.8 Mb  | 780 h     | 141 Gb |
| [Human HG002][hg002]     | PB ONT  110x   | 2.9 Gb    | 46.9 Mb  | 4000 h    | 409 Gb |
| [Human CHM1][chm1]       | PB 100x        | 2.8 Gb    | 18.6 Mb  | 2700 h    | 444 Gb |
| [Cliveome Q20][cliveome] | ONT 35x        | 3.0 Gb    | 46.5 Mb  | 2000 h    | 257 Gb |
| [HMP mock][hmp]          | PB meta 7 Gb   | 68 Mb     | N/A      | 60 h      | 72 Gb  |
| [Zymo Even][zymo]        | ONT meta 14 Gb | 65 Mb     | N/A      | 60 h      | 129 Gb |
| [Zymo Log][zymo]         | ONT meta 16 Gb | 29 Mb     | N/A      | 100 h     | 76 Gb  |
| [Sheep gut][sheep]       | HiFi meta 255G | 4.2 Gb    | N/A      | 3500 h    | 662 Gb |

[na12878]: https://github.com/nanopore-wgs-consortium/NA12878/blob/master/Genome.md
[ce]: https://github.com/PacificBiosciences/DevNet/wiki/C.-elegans-data-set
[at]: https://downloads.pacbcloud.com/public/SequelData/ArabidopsisDemoData/
[dm-pb]: https://github.com/PacificBiosciences/DevNet/wiki/Drosophila-sequence-and-assembly
[dm-ont]: https://www.ebi.ac.uk/ena/data/view/SRR6702603
[hg002]: https://github.com/human-pangenomics/HG002_Data_Freeze_v1.0
[ecoli]: https://github.com/PacificBiosciences/DevNet/wiki/E.-coli-Bacterial-Assembly
[hmp]: https://github.com/PacificBiosciences/DevNet/wiki/Human_Microbiome_Project_MockB_Shotgun 
[chm1]: https://trace.ncbi.nlm.nih.gov/Traces/sra/?study=SRP044331
[t2t]: https://github.com/nanopore-wgs-consortium/CHM13
[zymo]: https://github.com/LomanLab/mockcommunity
[cliveome]: https://labs.epi2me.io/cliveome_2010.05/
[sheep]: https://www.ncbi.nlm.nih.gov/bioproject/?term=PRJNA595610

The assemblies generated using Flye 2.9 could be downloaded from [Zenodo](https://doi.org/10.5281/zenodo.5228989).
All datasets were run with default parameters for the corresponding read type
with the following exceptions: CHM13 T2T, CHM1 and HG002 were run with `--asm-coverage 50`.

Note that this version of the table reflects contigs NG50, while the previous versions
were referring to scaffold NG50.


Third-party
-----------

Flye package includes some third-party software:

* [libcuckoo](http://github.com/efficient/libcuckoo)
* [intervaltree](https://github.com/ekg/intervaltree)
* [lemon](http://lemon.cs.elte.hu/trac/lemon)
* [minimap2](https://github.com/lh3/minimap2)
* [samtools](https://github.com/samtools/samtools)


License
-------

Flye is distributed under a BSD license. See the [LICENSE file](LICENSE) for details.


Credits
-------

Flye is developed in [Pavel Pevzner's lab at UCSD](http://cseweb.ucsd.edu/~ppevzner/)

Main code contributors:

* metaFlye: Mikhail Kolmogorov
* Repeat graph and current package maintaining: Mikhail Kolmogorov
* Trestle module and original polisher code: Jeffrey Yuan
* Original contig extension code: Yu Lin
* Short plasmids recovery module: Evgeny Polevikov


Publications
------------

Mikhail Kolmogorov, Derek M. Bickhart, Bahar Behsaz, Alexey Gurevich, Mikhail Rayko, Sung Bong
Shin, Kristen Kuhn, Jeffrey Yuan, Evgeny Polevikov, Timothy P. L. Smith and Pavel A. Pevzner
"metaFlye: scalable long-read metagenome assembly using repeat graphs", Nature Methods, 2020
[doi:s41592-020-00971-x](https://doi.org/10.1038/s41592-020-00971-x)

Mikhail Kolmogorov, Jeffrey Yuan, Yu Lin and Pavel Pevzner, 
"Assembly of Long Error-Prone Reads Using Repeat Graphs", Nature Biotechnology, 2019
[doi:10.1038/s41587-019-0072-8](https://doi.org/10.1038/s41587-019-0072-8)

Yu Lin, Jeffrey Yuan, Mikhail Kolmogorov, Max W Shen, Mark Chaisson and Pavel Pevzner, 
"Assembly of Long Error-Prone Reads Using de Bruijn Graphs", PNAS, 2016
[doi:10.1073/pnas.1604560113](https://www.doi.org/10.1073/pnas.1604560113)

**How to cite**: the 2020 paper is the most relevant to metagenome assembly. For single genome assembly, use the 2019 paper as reference. The 2016 paper describes solid k-mer indexing and polishing approaches that are used as core algorithms in the current pipeline.

How to get help
---------------
A preferred way report any problems or ask questions about Flye is the 
[issue tracker](https://github.com/fenderglass/Flye/issues). 
Before posting an issue/question, consider to look through the FAQ
and existing issues (opened and closed) - it is possible that your question
has already been answered.

If you are reporting a problem, please include the `flye.log` file and provide
details about your dataset.

In case you prefer personal communication, please contact Mikhail at mikolmogorov@gmail.com.
