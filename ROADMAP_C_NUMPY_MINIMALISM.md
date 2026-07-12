# ROADMAP — Minimal NumPy-style Array + DSP Library in C

> Goal: a small, teachable C library that captures the *essential* ideas of
> NumPy (N-dimensional strided arrays) plus a basic DSP layer (FIR/IIR/FFT/Bode),
> without trying to reproduce all of NumPy.
>
> This file is a living document. Check items off, add notes, revise the plan.
> Status legend: `[ ]` todo · `[~]` in progress · `[x]` done · `[?]` undecided

---

## 0. Guiding principles

- **One core type.** Everything is an `NArray` (strided, N-dim). Do not maintain a
  separate `NArray1D` — a 1-D array is just `ndim == 1`. (Current `NArray1D` is
  redundant; plan its removal or make it a thin wrapper.)
- **Strided memory model.** Shape + strides + a single flat buffer is the whole
  trick behind NumPy. Reshape, transpose, and slicing are *just stride/shape
  edits over shared data* — no copying. Protect this invariant above all else.
- **View vs. copy is explicit.** `own_data` already exists; make every operation
  clearly document whether it returns a view (shares `data`) or a copy (owns new
  `data`). This is the #1 correctness concept.
- **Typeless storage, typed dispatch.** Keep `void* data` + `DataType type`;
  dispatch on `type` at the leaves (you already do this in `print_scalar`).
- **Fail loud, fail early.** Return a status; validate shapes, bounds, overflow.

---

## 1. Current state (baseline — 2026-07)

- [x] `NArray` struct: data, type, ndim, item_size, shape[], strides[], total_items, own_data
- [x] C-contiguous stride computation
- [x] create / free / offset / get_item / set_item
- [x] recursive pretty-printer + info dump
- [x] gnuplot wrapper (title, axis, timeseries plot)
- [x] `NArray1D` (redundant — schedule for removal)
- [ ] empty stubs: `lsp_filter.c`, `rtp_real.c`

### Cleanups to do first (small, high value)
- [ ] Remove/merge `NArray1D` into `NArray`
- [ ] Declare `narray_print_data` / `datatype_name` in the header (or hide `static`)
- [ ] Fix overflow check in `narray1d_create` (logic bug) — or delete with the type
- [ ] `narray_offset` should set `*out_offset = 0` itself, not trust the caller
- [ ] Introduce `typedef enum { NA_OK, NA_ERR_NULL, NA_ERR_SHAPE, ... } NArrayStatus;`
      (optional but recommended over bare `bool`)
- [ ] Add `bool` flag or helper `narray_is_contiguous()` (needed once views exist)

---

## 2. Core array layer (the "NumPy" part)

### 2.1 Construction & memory
- [ ] `narray_create` (have it) — zero-filled, owns data
- [ ] `narray_empty` / `narray_zeros` / `narray_ones` / `narray_full`
- [ ] `narray_from_data(ptr, shape, ...)` — wrap existing buffer as a **view** (own_data=false)
- [ ] `narray_copy(dst, src)` — deep copy, always contiguous
- [ ] `narray_arange` / `narray_linspace` (very handy for DSP time axes)
- [ ] Reference-counting **[?]** — decide: do views need refcounts on the buffer,
      or do you enforce "view must not outlive owner" by discipline? (Start with
      discipline; document it. Refcount is the "advanced" upgrade.)

### 2.2 Indexing & iteration
- [x] N-dim element get/set via index array
- [ ] **Flat iterator** (`NArray.flat` equivalent) — walk elements in C order even
      when strides are non-contiguous (needed after transpose/slice). This is the
      C analog of NumPy's `flatiter`. Implement as a small struct holding a
      coordinate counter + current `dataptr`.
- [ ] `narray_at(arr, indices)` returning `void*` (already have `_at` for 1D)

### 2.2b Contiguous vs Non-Contiguous (concept)
The array is one flat buffer (`data`) + `shape[]` + `strides[]`. "Contiguous"
describes whether the strides still match a tightly packed row-major layout.

- **C-contiguous** (what `narray_create` always produces): `strides[ndim-1] ==
  item_size` and `strides[d] == strides[d+1] * shape[d+1]`. Memory has no gaps and
  walking the buffer straight through == logical C order.
- **Non-contiguous** appears only once **views** exist — the buffer is shared and
  only shape/strides change: `transpose` swaps shape+strides, `slice` introduces
  gaps/offset, reverse (`a[::-1]`) uses a negative stride. Walking the buffer
  straight through no longer matches logical order.
- Consequences:
  - Every op must step via `strides[]`, never assume `data[i]` is element `i`
    (the recursive printer already does this correctly).
  - `reshape` is a free **view** only when contiguous; otherwise it must **copy**.
  - Contiguous data enables `memcpy`/tight-loop fast paths; non-contiguous needs
    the general strided walk (see the flat iterator, §2.2).
- Prereqs already tracked: `narray_is_contiguous()` (§1) and a data `offset`
  field (Design Note A, §6).

### 2.3 Shape manipulation (your item #2)
- [ ] `narray_reshape` — **view** when contiguous (just rewrite shape+strides);
      copy when not. Validate `product(new_shape) == total_items`.
- [ ] `narray_ravel` / `narray_flatten` (view vs copy variants)
- [ ] `narray_transpose` / axis permutation — **view**, only swaps shape+strides
- [ ] `narray_squeeze` / `narray_expand_dims`
- [ ] `narray_slice` — view with adjusted shape/strides/offset (needs a data
      *offset* field OR a separate `base` pointer; see Design Note A)
- [ ] `narray_concatenate` along an axis (rows/cols) — always a copy
- [ ] `narray_stack` / `narray_split`

### 2.4 Element-wise & reduction ops (your item #1: multi-channel operations)
- [ ] Unary: `negate`, `abs`, `sqrt`, `exp`, `log`, ...
- [ ] Binary: `add`, `sub`, `mul`, `div` (element-wise, matching shapes)
- [ ] Scalar broadcasting (array `op` scalar) — the *easy* half of broadcasting
- [ ] Full **broadcasting** (shape-(3,1) with shape-(1,4)) **[?]** — powerful but
      complex; consider deferring. Multi-channel DSP often only needs "per-column"
      or "per-row" ops, which scalar/axis ops cover.
- [ ] Reductions: `sum`, `mean`, `min`, `max`, `std` — with optional `axis`
- [ ] `matmul` / dot **[?]** — needed for some DSP (e.g. filterbanks) but big

### 2.5 Data types
- [x] uint8, int, float, double
- [ ] **complex float / complex double** — *mandatory* for FFT / Bode / transfer
      functions. Add `CFLOAT_TYPE`, `CDOUBLE_TYPE` using C99 `<complex.h>`.
- [ ] `narray_astype` (cast between dtypes, always a copy)

---

## 3. DSP layer (your item #3) — build on the core, in this order

- [ ] **Signal generators**: sine, cosine, impulse, step, chirp, noise
- [ ] **Convolution** (`conv`, `conv_same`, `conv_full`) — the workhorse
- [ ] **FIR filter**: apply given tap coefficients (this is just convolution)
- [ ] **IIR filter**: direct-form II transposed (`b[]`, `a[]` coefficients)
- [ ] **Windowing**: Hann, Hamming, Blackman
- [ ] **FFT / IFFT** (radix-2 Cooley–Tukey to start; needs complex dtype)
- [ ] **DFT** (naive, for validating the FFT and for non-power-of-2)
- [ ] **Spectrum**: magnitude, phase, power spectral density
- [ ] **Bode plot**: evaluate H(e^jw) from `b[]`,`a[]` over a frequency grid →
      magnitude (dB) + phase, then feed the gnuplot wrapper (log x-axis)
- [ ] **Filter design [?]**: Butterworth/bilinear-transform — advanced, optional
- [ ] Multi-channel: run any 1-D DSP op along a chosen axis of an N-dim array

---

## 4. Infrastructure / quality

- [ ] Unit tests (a tiny assert-based harness or Unity/greatest). Test views,
      strides, reshape-no-copy, filter impulse response, FFT vs DFT agreement.
- [ ] Run under **AddressSanitizer / UBSan** (`-fsanitize=address,undefined`) and
      **Valgrind** — you will find leaks/aliasing bugs in the view code.
- [ ] Consistent error handling (the `NArrayStatus` enum above)
- [ ] Doc comments on every public function stating **view vs copy** and ownership
- [ ] `narray_equal` / `narray_allclose` (float tolerance) for tests
- [ ] CI-style build script that treats warnings as errors (`-Wall -Wextra -Werror`)

---

## 5. Advanced C features you will likely need (learning targets)

These map to your broader roadmap.txt goals (serious C, memory, performance).

- **Flexible array members / single-allocation structs** — allocate the header
  and its buffer in one `malloc` to improve locality (advanced alternative to the
  current two-pointer design).
- **Function pointers / dispatch tables** — a per-`DataType` table of
  `add/mul/print` function pointers avoids giant `switch` statements as ops grow.
- **`_Generic` (C11)** — type-generic macros so callers write `narray_set(a, i, 3.0)`
  and the right typed path is chosen at compile time. This is the closest C gets
  to NumPy's ergonomic typing.
- **`restrict` pointers** — tell the compiler buffers don't alias; big win in
  tight DSP loops (convolution/FFT).
- **`<complex.h>` (C99)** — complex arithmetic for FFT/Bode.
- **Alignment (`_Alignas`, `aligned_alloc`)** — needed for SIMD-friendly buffers.
- **SIMD [?]** — intrinsics (SSE/AVX/NEON) or just `-O3 -march=native` auto-vec.
  Advanced; do only after correctness + tests exist.
- **`const` correctness & `assert`** — cheap, catches misuse early.
- **Opaque pointers / API boundary** — hide struct internals behind the header if
  you ever want a stable ABI (relevant to your later LLVM/embedded projects).
- **Threads [?]** — `pthreads`/C11 `<threads.h>` for multi-channel parallelism.
  Last, and only if profiling justifies it.

---

## 6. What a "minimal NumPy" needs that is easy to forget

Cross-checking against real NumPy, these are commonly-missed essentials:

- [ ] **Non-contiguous views** (from slice/transpose) and code that *respects
      strides everywhere* — not just contiguous fast-paths. Your printer already
      uses strides (good); make sure every future op does too.
- [ ] **A data offset** (or `base` pointer) so a slice can start mid-buffer.
      *Design Note A: pick one — (a) add `size_t offset` to `NArray`, or (b) let
      `data` point into the middle of an owner's buffer and track the owner
      separately. (a) is simpler to reason about.*
- [ ] **Ownership tracking for views** — who frees the buffer? (refcount vs.
      discipline; see 2.1).
- [ ] **Endianness / dtype metadata** — only matters if you ever serialize
      (e.g. read/write `.npy`). Optional.
- [ ] **Empty / zero-size arrays** — NumPy allows a 0-length dimension; you
      currently reject `shape[d]==0`. Decide if you care (probably fine to reject).
- [ ] **Negative / reverse strides** — NumPy uses them for `a[::-1]`. Advanced;
      only if you implement full slicing.
- [ ] **Scalar (0-dim) arrays** — `ndim==0`. Edge case; optional.
- [ ] **`copy` semantics on assignment** — document that `set_item` writes through
      views to shared data (can surprise users).
- [ ] **Type promotion rules** — `int + double -> double`. Decide a simple policy
      (or forbid mixed-type ops and require explicit `astype`).

---

## 7. Suggested build order (dependency-aware)

1. Cleanups in §1 (remove NArray1D, fix bugs, error enum)
2. `offset`/`base` field + `narray_from_data` view constructor + `is_contiguous`
3. `reshape` / `transpose` / `flatten` (exercise the view model)
4. `flat` iterator (stride-correct iteration)
5. element-wise + scalar ops + reductions
6. `slice` + `concatenate` (finish shape manipulation)
7. complex dtype
8. DSP: generators → convolution → FIR → IIR → windows → FFT → spectrum → Bode
9. tests + sanitizers throughout (not at the end)
10. advanced C (function tables, `_Generic`, `restrict`, SIMD) as optimization

---

## Notes / open questions (append freely)

- [?] Refcounting for views, or ownership-by-discipline?
- [?] Full broadcasting, or scalar + axis ops only?
- [?] Keep `NArray1D` as a convenience wrapper, or delete entirely?
- [?] `matmul` in scope, or out?
