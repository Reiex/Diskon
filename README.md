# Diskon - [C++] A file formats reading/writing library

The goal of this library is to give tools for reading/writing a wide range of file formats efficiently. The efficiency is as important in compute time - the library shall be fast - as in memory usage: the ability to stream file formats instead of reading/writting the whole file from memory is a major preocupation. The tools implemented shall not overlap with tools already present in the standard library, and the library shall stay simple to include in projects and simple to use.

This is why the library is built following three rules:
- No dependency.
- No parallel computing inside the library (but the whole library is thread safe).
- No GPU use.

Tools implemented in Diskon includes, amongst others:
- Checksums and simple hash algorithms.
- Non standard data types such as 24-bit integers, 16-bit floating point numbers, etc. and conversions between these and standard data types.
- Compression algorithms such as Huffman Coding.

File formats handled by Diskon includes, for example:
- Portable Network Graphics (PNG)
- Portable AnyMap (PNM, PBM, PGM and PPM)
- Waveform Audio File Format (WAV)
- Extensible Markup Language (XML)
- Wavefront OBJ (OBJ)
