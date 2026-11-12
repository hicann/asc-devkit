# Register Load and Store Sample

## Overview

This sample demonstrates transfers between UB and registers using experimental Tensor APIs. Scenarios 1 and 2 use `unalign_load` and `unalign_store` for unaligned copying and reduction. Scenario 3 uses `load<bool>` and `store` for aligned mask register transfers. GM/UB transfers use `copy`, and pipeline synchronization uses `asc::te::mutex`.

Select a scenario with `SCENARIO_NUM`:

| Scenario | Input | Output | Operation |
| --- | --- | --- | --- |
| 1 | `float [1024]` | `float [1024]` | Copy two registers (128 elements) starting at element 1. All other output elements remain zero. |
| 2 | `uint16_t [14, 255]` | `uint16_t [14, 16]` | Four cores process up to four rows each; the last core processes two rows. Reduce every 16 elements by maximum, with 15 valid elements in the last datablock. |
| 3 | `uint32_t [16]` | `uint32_t [32]` | Load masks directly or with upsampling or downsampling, and store them directly or in packed form. |

## Supported Products and CANN Versions

| Product | CANN Version |
| --- | --- |
| Ascend 950PR&950DT products | >= CANN 9.3.0 |

The CANN installation must provide `unalign_load` and `unalign_store` for scenarios 1 and 2, and the three `load<bool>` modes and `store` support for `reg_tensor<bool>` for scenario 3. CMake uses the installed headers by default.

## Directory Structure

```text
├── CMakeLists.txt                 // Build configuration and CTest entry
├── data_utils.h                   // Local sample utility header
├── README.md                      // Chinese documentation
├── README_en.md                   // English documentation
└── reg_load_store.asc             // Kernels, input generation, and verification
```

## Implementation

### Scenario 1: Unaligned Copy

1. Copy the input from GM to UB and zero the output UB using `fill/store`.
2. Create source and destination Tensor views beginning at element 1 using `slice` in the kernel. Calculate `repeat_time = ceil(copy_elements / elements_per_register)` and pass it to the VF. On `dav-3510`, each `float` register holds 64 elements, so the loop runs twice.
3. In the VF, create `unalign_load` and `unalign_store` sessions and call `loader.prepare(src_stream)` once before the loop.
4. In each iteration, use `repeat * elements_per_register` as the relative element coordinate. Load one full register with `loader.load(src_stream, coord)`, then store it consecutively with `storer.store(dst_stream, coord, src, elements_per_register)`.
5. Commit pending stores with `storer.flush(dst_stream, repeat_time * elements_per_register)` and copy the output back to GM. Output indices 1 through 128 match the input; all other elements are zero.

```cpp
asc::te::experimental::unalign_load loader;
asc::te::experimental::unalign_store storer;
loader.prepare(src_stream);
for (uint32_t repeat = 0; repeat < repeat_time; ++repeat) {
    auto coord = asc::te::make_coord(repeat * elements_per_register);
    auto src = loader.load(src_stream, coord);
    storer.store(dst_stream, coord, src, elements_per_register);
}
storer.flush(dst_stream, static_cast<int32_t>(repeat_time * elements_per_register));
```

### Scenario 2: Multicore Reduction with Unaligned Addresses and Lengths

1. Use `block_idx` to compute the first row and actual row count. Copy each core's input into a contiguous, one-dimensional UB buffer.
2. Keep the 255-element rows packed without inter-row padding. Subsequent UB rows and subsequent cores' GM input addresses are not 32-byte aligned.
3. Create source and destination row views in the kernel, calculate `repeat_time = ceil(input_columns / elements_per_register)`, and pass it to the VF. Call `loader.prepare(src_row)` once before the loop, then read two 256-byte registers with `loader.load(src_row, make_coord(repeat * elements_per_register))`. Reserve an additional register of source UB space so a full-register tail load cannot access the output buffer.
4. Mask the last invalid element using `update_mask<uint16_t>` and call `reduce_max<reduce_scope::datablock>` for the maximum of every 32-byte datablock. The data type is deduced from the input.
5. Store eight results at destination row coordinates `repeat * results_per_register` with `storer.store`. Call `storer.flush(dst_row, repeat_time * results_per_register)` and copy results back to GM.

**Interface usage:** Scenario 1 calls `prepare` once before the loop for the source view, while scenario 2 calls `prepare` once before the loop for each row. Both scenarios advance the source coordinate by one register per iteration; this sequential loading pattern should not be generalized to arbitrary address jumps with the same prepared state. Loads read a full register and take no `count` argument. Scenario 2 uses `with_mask(mask)` to select valid elements for reduction. Stores write `count` elements without using the Reg Tensor mask. Call `flush` once after each contiguous store stream, passing the first destination view and the end-of-stream element offset relative to that view. Both scenarios construct views using `slice` in the kernel rather than inside the VF.

### Scenario 3: Aligned Mask Register Transfers

Copy 64 bytes from GM to UB. Load mask bits from the `uint32_t` source tensor, then store the `reg` member of each `reg_tensor<bool>` without reading its `mask` member. The offsets below are zero-based `uint32_t` element offsets.

| Output Offset | Size | Operation |
| --- | --- | --- |
| 0 | 32 bytes | Load from the start of the input and store unchanged. |
| 8 | 32 bytes | Load 16 bytes from input offset 4, repeat each bit twice, and store. |
| 16 | 32 bytes | Load from the start of the input, retain the even-indexed bits of all 64 bytes, and store. |
| 24 | 16 bytes | Pack and store the even-indexed bits of the direct load. |
| 28 | 16 bytes | Pack the upsampled mask to recover the 16 bytes at input offset 4. |

Direct and downsample loads and direct stores use 32-byte-aligned addresses. Upsample loads and packed stores use 16-byte-aligned addresses.

### Verification

The host generates deterministic input and computes golden results without Python or external files. Scenario 1 checks all 1024 outputs, including zeros outside the copied range. Scenario 2 checks all 224 results, including each row's tail datablock and the last core. Scenario 3 computes the expected result bit by bit and checks all 32 output words. ACL errors or mismatches cause a nonzero exit status.

## Build and Run

Configure the environment using the CANN installation directory:

```bash
source ${install_path}/set_env.sh

```

Replace `${install_path}` with the CANN installation directory. The sample uses `find_package(ASC REQUIRED)` to obtain Tensor/C API headers, the compiler, ACL runtime, and simulator from CANN.

Run these commands from the sample directory. NPU mode and scenario 1 are the defaults:

```bash
mkdir -p build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;
./demo
```

For scenario 2, reconfigure and rebuild from the build directory. To run scenario 3, pass `-DSCENARIO_NUM=3` instead:

```bash
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=2 ..;make -j;
./demo
```

For NPU simulation, add `-DCMAKE_ASC_RUN_MODE=sim`:

```bash
cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=1 ..;make -j;
./demo
```

Use `-DSCENARIO_NUM=2` or `-DSCENARIO_NUM=3` to simulate the corresponding scenario. Input and golden results are generated by the host program; no data generation script is needed.

**Note:** Before switching run modes, remove the CMake cache with `rm CMakeCache.txt` in the build directory and reconfigure. Scenario selection is cached; explicitly pass `-DSCENARIO_NUM=1` to switch back to scenario 1.

After updating CANN package headers, run `make clean` followed by `make -j`, or use a new build directory to avoid reusing stale compilation artifacts.

| Option | Values | Description |
| --- | --- | --- |
| `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | NPU execution or simulation |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | Ascend 950PR/Ascend 950DT |
| `SCENARIO_NUM` | `1` (default), `2`, `3` | Unaligned copy, datablock maximum reduction, or mask register transfers |

Successful verification prints the corresponding line:

```text
scenario 1: test pass!
scenario 2: test pass!
scenario 3: test pass!
```
