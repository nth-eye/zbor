# utl

Header-only utility library (`utl::` namespace stands for __UTiL__) with small, simple and common functions, which nevertheless weren't present in C++17 standard library. All functions that can be `constexpr` are. Not intended to become anything serious, but you may find some parts really useful, for example `"float.h"` has IEE 754 half-precision floating point conversion functions. I use this library in many projects for embedded systems and in my other libraries, e.g. [zbor][1].

## Dependencies 

Basic requirements are `cstdint` and `cstddef`. Additional are below:

| Header        | Dependency                        |
| ------------- | --------------------------------- |
| "physics.h"   | `<cmath>`                         |
| "str.h"       | `<string_view>`                   |
| "log.h"       | "str.h" + `<cctype>` + `<cstdio>` |
| "time.h"      | "str.h" + `<ctime>`               |

## TODO

- [ ] standardize API for containers (svector, ring, etc)
- [ ] make ring.h constexpr
- [x] fix split in str.h
- [ ] tests
    - [x] base
    - [x] bit
    - [ ] float
    - [ ] log
    - [ ] math
    - [ ] physics
    - [ ] ring
    - [ ] storage
    - [x] str
    - [ ] svector
    - [ ] time
- [ ] readme
- [ ] maybe restrict / remove bit offset in `log_bits()` ?
- [ ] maybe move here finite field arithmetic ?

[1]: https://github.com/nth-eye/zbor