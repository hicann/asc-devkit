# Mmad MX Example

## Overview

This example demonstrates MX matrix multiplication with quantization by using the C API. Matrices A and B use FP4 or FP8 data types, while ScaleA and ScaleB use `fp8_e8m0_t`. A and ScaleA are not transposed; B and ScaleB are transposed.

This example also describes K-axis transfer alignment and zero-padding for the FP4 and FP8 scenarios. In the FP4 scenarios, 32-byte transfer alignment of the packed data covers the range required by the computation, so no additional clearing operation is needed. In the FP8 scenario, `asc_fill_l1` must explicitly clear the padded tail not covered by the transfer. For details, see [K-Axis Alignment and FP8 Tail Clearing](#k-axis-alignment-and-fp8-tail-clearing).

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|--------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```text
├── mmad_mx
│   ├── figures
│   │   └── mmad-mx.png             // MX matrix multiplication diagram
│   ├── scripts
│   │   ├── gen_data.py             // Input and golden data generator
│   │   └── verify_result.py        // Result verification script
│   ├── CMakeLists.txt               // Build configuration
│   ├── data_utils.h                 // File I/O utilities
│   ├── mmad_mx.asc                 // C API example implementation
│   ├── README_en.md                 // English documentation
│   └── README.md                    // Chinese documentation
```

## Description

MX matrix multiplication uses the following formula:

$$ C = (ScaleA \otimes A) \times (ScaleB \otimes B) + Bias $$

The shapes of A, ScaleA, B, ScaleB, Bias, and C are [M, K], [M, ScaleK], [K, N], [ScaleK, N], [N], and [M, N], where `ScaleK = CeilAlign(CeilDiv(K, 32), 2)`. This example uses [M, N, K] = [40, 50, 70].

The complete pipeline consists of the following stages:

1. `asc_copy_gm2l1_nd2nz` moves A, B, and Bias from GM to L1, and `asc_copy_gm2l1_dn2nz` moves ScaleA and ScaleB;
2. Scenario 3 uses `asc_fill_l1` to clear the additional K-axis padding for FP8 data;
3. `asc_copy_l12l0a` and `asc_copy_l12l0a_mx` move A and ScaleA to L0A and L0AMX;
4. `asc_copy_l12l0b` and `asc_copy_l12l0b_mx` move B and ScaleB to L0B and L0BMX;
5. Scenario 2 uses `asc_copy_l12bt` to move Bias to the BiasTable Buffer;
6. `asc_mmad_mx` performs MX matrix multiplication;
7. `asc_copy_l0c2gm` moves the result from L0C to GM and converts it to ND layout.

The layouts in each storage unit are as follows:

| Storage Unit | Layout |
|--------------|--------|
| GM | A, B, and C use ND layout |
| L1 | A and B use Nz layout |
| L0A / L0AMX | A uses Nz; ScaleA uses Zz |
| L0B / L0BMX | B uses Zn; ScaleB uses Nn |
| BiasTable Buffer | Bias is a one-dimensional tensor with shape [N] |
| L0C | C uses Nz layout |

The `SCENARIO_NUM` build option selects one of the following scenarios:

| `SCENARIO_NUM` | A Type | ScaleA Type | B Type | ScaleB Type | Bias Type | C Type | Bias and Initial C Description |
|----------------|--------|-------------|--------|-------------|-----------|--------|--------------------------------|
| 1 | `fp4x2_e1m2_t` | `fp8_e8m0_t` | `fp4x2_e2m1_t` | `fp8_e8m0_t` | `float` | `float` | No Bias. C is initialized to zero. |
| 2 | `fp4x2_e2m1_t` | `fp8_e8m0_t` | `fp4x2_e1m2_t` | `fp8_e8m0_t` | `float` | `float` | Bias is not passed to the matrix instruction. C is initialized from offset 0 of C2 (BiasTable Buffer). |
| 3 | `fp8_e4m3fn_t` | `fp8_e8m0_t` | `fp8_e5m2_t` | `fp8_e8m0_t` | `float` | `float` | No Bias. The second call accumulates from the existing CO1 value. |

### Scenario Details

The `SCENARIO_NUM` build option selects the scenario. All scenarios use the same matrix multiplication dimensions: [M, N, K] = [40, 50, 70].

**Scenario 1: C Initialized to Zero**

- A: [40, 70], `fp4x2_e1m2_t`, ND;
- ScaleA: [40, 4], `fp8_e8m0_t`, ND;
- Transposed B input: [50, 70], `fp4x2_e2m1_t`, ND;
- Transposed ScaleB input: [50, 4], `fp8_e8m0_t`, ND;
- `asc_mmad_mx` is called with `c_matrix_init_val = true`.

**Scenario 2: C Initialized from the BiasTable Buffer**

- A: [40, 70], `fp4x2_e2m1_t`, ND;
- ScaleA: [40, 4], `fp8_e8m0_t`, ND;
- Transposed B input: [50, 70], `fp4x2_e1m2_t`, ND;
- Transposed ScaleB input: [50, 4], `fp8_e8m0_t`, ND;
- Bias: [50], `float`;
- `asc_mmad_mx` is called with `c_matrix_init_val = false` and `c_matrix_source = true`.

**Scenario 3: Accumulation from L0C**

- A: [40, 70], `fp8_e4m3fn_t`, ND;
- ScaleA: [40, 4], `fp8_e8m0_t`, ND;
- Transposed B input: [50, 70], `fp8_e5m2_t`, ND;
- Transposed ScaleB input: [50, 4], `fp8_e8m0_t`, ND;
- The first `asc_mmad_mx` call initializes C to zero. The second call uses `c_matrix_init_val = false` and `c_matrix_source = false` to accumulate into L0C.

### Matrix Multiplication (MmadMx)

`asc_mmad_mx` automatically applies the broadcast multiplication between the left and right matrices and their corresponding Scale matrices. Every 32 logical elements along the K axis share one scale factor. The following figure uses FP4 matrices A and B to illustrate the fractal layouts of A, ScaleA, B, and ScaleB and the MX quantization process.

<a id="k-axis-alignment-and-fp8-tail-clearing"></a>

#### K-Axis Alignment and FP8 Tail Clearing

MX matrix multiplication requires the K-axis length used for computation to be aligned to 64 logical elements. Therefore, the K argument passed to `asc_mmad_mx` must be `ALIGN_K = CeilAlign(K, 64)` instead of the original K. In this example, the original `K = 70`, while the actual argument is `ALIGN_K = 128`. The L1 and L0A/L0B buffers are also allocated using `ALIGN_K`. Every padded element beyond the original data must be zero; otherwise, invalid data participates in the matrix multiplication and changes the result.

Scale values are grouped by every 32 logical elements along the K axis, and the number of groups must be aligned to 2. This example has `CeilDiv(70, 32) = 3` valid scale groups and therefore uses `ScaleK = CeilAlign(3, 2) = 4`.

FP4 and FP8 use different storage widths along the K axis, so the padding ranges produced by the GM-to-L1 transfer are also different:

| Item | FP4 Scenarios (1 and 2) | FP8 Scenario (3) |
|------|-------------------------|------------------|
| Per-row K-axis storage in GM | 70 logical FP4 elements packed into `PACKED_K = 35` bytes | 70 FP8 elements, or 70 bytes |
| Range after the 32-byte-aligned `asc_copy_gm2l1_nd2nz` transfer | 64 bytes, corresponding to 128 logical FP4 elements | 96 bytes, corresponding to 96 logical FP8 elements |
| K-axis length used by `asc_mmad_mx` | 128 | 128 |
| Additional clearing required | No | Yes. Clear the K-axis range `[96, 128)`. |

In Scenario 3, after the GM-to-L1 transfer moves the `K = 70` data and applies its 32-byte-granularity padding, the initialized K-axis range in L1 ends at the 96-element boundary, while `asc_mmad_mx` computes 128 elements. The example calls `asc_fill_l1` for A and B. Starting at K-axis offset 96, it writes one 32-byte zero block for every aligned row and clears the `[96, 128)` range.

The destination of `asc_fill_l1` is addressed as `uint16_t`, and its start offset is calculated as `AlignHeight * (CeilAlign(PACKED_K, 32) / 2)`. In the FP8 scenario, A and B use `M_ALIGN = 48` and `N_ALIGN = 64` as their respective `AlignHeight` values, so their start offsets are 2304 and 3072 `uint16_t` elements. The two operations set `repeat` to 48 and 64, respectively, with `blk_num = 1` and `dst_gap = 0`, continuously covering one 32-byte block for every aligned row. After clearing, `asc_sync_pipe(PIPE_MTE1)` ensures that the L1 writes finish before the L1-to-L0A/L0B transfers begin.

When changing K in this example, recalculate `PACKED_K`, `ScaleK`, and `ALIGN_K` together, and handle any remaining padding according to the K-axis range actually covered by the GM-to-L1 transfer.

<p align="center">
  <img src="figures/mmad-mx.png" width="1000">
</p>

<p align="center">Figure 1: MX matrix multiplication with quantization</p>

## Build and Run

- Configure the environment:

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

- Install the data generation dependencies:

  ```bash
  python3 -m pip install ml_dtypes==0.2.0 en_dtypes==0.0.4
  ```

- Build and run the example from this directory:

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM
  make -j
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin
  ```

  Add `-DCMAKE_ASC_RUN_MODE=sim` to the CMake command for NPU simulation. Clear the CMake cache before changing the run mode.

| Option | Values | Description |
|--------|--------|-------------|
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture |
| `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | NPU execution or simulation |
| `SCENARIO_NUM` | `1` (default), `2`, `3` | Scenario number |

Successful verification prints:

```text
test pass!
```
