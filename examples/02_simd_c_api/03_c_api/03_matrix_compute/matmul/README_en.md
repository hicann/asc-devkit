# MatMul Sample

## Overview

This sample takes the float data type as an example to demonstrate how to implement matrix multiplication (C = A × B + Bias) with the C API. Neither A nor B is transposed. The host side handles file I/O, memory management, and kernel launch, while the whole matrix multiply-accumulate runs on a single Cube Core.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|--------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure

```
├── matmul
│   ├── scripts
│   │   ├── gen_data.py             // Input and golden data generation script
│   │   └── verify_result.py        // Accuracy verification script
│   ├── CMakeLists.txt              // Build project file
│   ├── data_utils.h                // File read/write helpers
│   ├── matmul.asc                  // Main sample file (kernel implementation, host main function)
│   ├── README_en.md                // Sample description (English)
│   └── README.md                   // Sample description (Chinese)
```

## Sample Description

The computation formula is C = A × B + Bias. This sample uses [M, N, K] = [30, 70, 40], and the kernel is named `matmul_custom`. The input and output specifications are listed below.

| Parameter | Type | Data Type | Shape | Transposed |
|-----------|------|-----------|-------|------------|
| `a` | input | float | [30, 40] | no |
| `b` | input | float | [40, 70] | no |
| `bias` | input | float | [70] | - |
| `c` | output | float | [30, 70] | - |

A complete matrix multiplication involves the following data movement stages and data layouts.

| Stage | Data Movement | APIs | Data Layout |
|-------|---------------|------|-------------|
| 1 | GM → L1 | `asc_set_gm2l1_nz_para` + `asc_copy_gm2l1_nd2nz` / `asc_copy_gm2l1_dn2nz` | A and B are ND on GM and Nz on L1 |
| 2 | L1 → L0A / L0B / BT | `asc_copy_l12l0a`, `asc_copy_l12l0b`, `asc_copy_l12bt` | A is Nz on L0A, B is Zn on L0B, Bias is a 1-D tensor of shape [N] |
| 3 | Multiply-accumulate | `asc_mmad` | C is Nz on L0C, and its initial value comes from BT |
| 4 | L0C → GM | `asc_set_l0c2gm_nz2nd` + `asc_copy_l0c2gm` | C is Nz on L0C and ND on GM |

The buffer sizes on L1, L0A, L0B, and L0C are aligned to the fractal requirements. The Mmad computation includes padded invalid data; setting `m_size = M` and `n_size = N` in `asc_copy_l0c2gm` ensures that results over the padded region are not moved out. In addition, `asc_copy_gm2l1_nd2nz` and `asc_copy_gm2l1_dn2nz` pad with zeros only along the K axis, so the rows that A and B gain along the M and N axes for alignment are never written. This sample calls `asc_fill_l1` to clear them explicitly, keeping uninitialized data out of the Mmad computation.

The amount of data moved into the BiasTable must be 64-byte aligned, and the whole aligned range is read back, so the host pads the bias with zeros up to the aligned size before copying it to the device. This keeps uninitialized data from reaching the BiasTable through L1.

## Build and Run

Run the following steps in the root directory of this sample to build and run the operator.

- Set up environment variables
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **NOTE:** `${install_path}` is the CANN installation directory. If not specified, the default is `/usr/local/Ascend` for the root user and `$HOME/Ascend` for a non-root user.

- Run the sample

  ```bash
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 .. && make -j
  python3 ../scripts/gen_data.py
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin
  ```

  To use the NPU simulation mode, add `-DCMAKE_ASC_RUN_MODE=sim`:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 .. && make -j
  ```

  > **CAUTION:** Clear the cmake cache before switching the build mode, for example by running `rm CMakeCache.txt` in the build directory and re-running cmake.

- Build options

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: on NPU or NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` (default) | NPU architecture, corresponding to Ascend 950PR/Ascend 950DT |

- Expected result

  When the accuracy comparison passes, the output is:

  ```bash
  test pass!
  ```
