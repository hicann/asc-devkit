# Ndtri Sample

## Overview

This sample demonstrates how to call the Ndtri high-level API and how to run functional and performance checks. Ndtri computes the inverse cumulative distribution function of the standard normal distribution element by element: for a probability `p`, it returns the quantile `x` satisfying $\Phi(x)=p$.

For `p = 0`, the output is negative infinity. For `p = 1`, the output is positive infinity. For `p` in `(0, 1)`, the corresponding quantile is returned. Out-of-range values, NaN, and infinity produce NaN.

## Supported Products and CANN Version

| Product | CANN version |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```text
|---- ndtri
|     |---- scripts
|     |     `---- gen_data.py         // Generates input and golden data
|     |---- CMakeLists.txt          // Build configuration
|     |---- data_utils.h            // Data file helpers
|     |---- ndtri.asc               // Ascend C API sample implementation
|     |---- run.sh                  // Functional/performance test script
|     |---- run_all.sh              // Full regression script
|     `---- README.md               // Sample documentation
```

## Sample Specification

- Function: compute the standard normal inverse CDF $\Phi^{-1}(p)$ element by element, including handling for 0, 1, out-of-range values, NaN, and infinity.
  $$dstTensor_i = \Phi^{-1}(srcTensor_i)$$
- Input: `x`, shape `[SIZE]`, data type `float`, ND format.
- Output: `y`, shape `[SIZE]`, data type `float`, ND format.
- Kernel name: `ndtri_custom`.

## Sample Description

The default input and output shapes are `x[2048]` and `y[2048]`. Use `--size` to change the data volume. The sample uses one core and moves and computes data in tiles sized for the available UB.

### Functional Verification

The sample executes CopyIn, Ndtri computation, and CopyOut, then compares the output with golden data.

- `api_mode=1`: calls the overload with `calCount` and checks that the source LocalTensor remains unchanged.
- `api_mode=2`: calls the whole-tensor overload and compares it with the specified-count overload.
- `api_mode=3`: calls the whole-tensor overload with identical input and output LocalTensor addresses to verify in-place computation.

The corresponding API UT is `tests/api/adv_api/math/ndtri/test_operator_ndtri.cpp`. The `run_all.sh` script in this directory is the sample's functional, accuracy, and performance entry point.

### Performance Verification

The sample calculates the AIV_VEC ratio and requires it to be at least 90%.

$$\text{AIV\_VEC ratio} = \frac{\text{computeTime}}{\text{total2} - \text{total1}}$$

- `total1`: data-movement-only baseline from TEST_MODE=2.
- `total2`: data movement plus 1000 compute iterations from TEST_MODE=3.
- `computeTime`: AIV_VEC time from TEST_MODE=3.

## Verification Scope

- Shapes include aligned, unaligned, and large inputs: `1`, `32`, `1023`, `2048`, and `65536`.
- Inputs cover uniform probabilities, tail values, values around the segment boundary, normal and subnormal values, 0, 1, out-of-range values, NaN, and `+/-Inf`.
- Separate-address and in-place calculations are verified.
- Performance sizes are `1k`, `4k`, `8k`, `16k`, `32k`, and `64k`, with an AIV_VEC ratio requirement of at least 90%.

## Build and Run

### Configure the Environment

Install the CANN development toolkit and configure the environment:

```bash
source ${install_path}/cann/set_env.sh
```

When the API is not installed, set the source tree path:

```bash
export ASC_DEVKIT_PATH=/path/to/asc-devkit
```

### Run the Sample

Run the following command from this directory:

```bash
bash run.sh -r <cpu|sim|npu> --size <N> --is_perf <0|1> [--api_mode <1|2|3>]
```

| Option | Values | Description |
| --- | --- | --- |
| `-r` | `cpu`, `sim`, `npu` | CPU debug, NPU simulation, or NPU execution. Performance verification supports `npu` only. |
| `--size` | Positive integer | Number of elements. |
| `--is_perf` | `0`, `1` | Functional or performance verification. |
| `--api_mode` | `1`, `2`, `3` | Specified-count overload, whole-tensor overload, or whole-tensor in-place overload. Defaults to `1`; performance mode supports `1` only. |

Examples:

```bash
bash run.sh -r cpu --size 2048 --is_perf 0
bash run.sh -r sim --size 4096 --is_perf 0
bash run.sh -r npu --size 2048 --is_perf 1
bash run.sh -r npu --size 1023 --is_perf 0 --api_mode 2
bash run.sh -r npu --size 1023 --is_perf 0 --api_mode 3
```

Run the full Ascend 950PR/950DT regression:

```bash
bash run_all.sh npu 1
```

Functional verification prints `test pass!`. Performance verification prints the measured AIV_VEC ratio when it meets the threshold.
