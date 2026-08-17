# MxFP4 Matmul Performance Tuning Example

## Overview

This example uses MxFP4 matrix multiplication to introduce MxMatmul performance tuning methods based on the Ascend C `Matmul` high-level API. The example includes two scenarios (Case 1-2), both using constant tiling, replacing runtime tiling copy and computation with template-constant `MatmulApiStaticTiling` (static tiling).

**Optimization Path**:
- Case 1: Multi-core MDL constant tiling (scaleA/B transferred synchronously with A/B)
- Case 2: Multi-core MDL constant tiling (in **GM→L1 transfer**, scaleA/B transferred at multiples relative to A/B, `mxTypePara`)

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```
├── matmul_mxfp4_high_performance
│   ├── scripts
│   │   ├── gen_data.py         // Input data and ground truth generation script
│   │   └── verify_result.py    // Ground truth comparison script
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── figures                 // Illustrations
│   ├── matmul_mx.asc           // Ascend C example implementation (including 2 optimization cases)
│   ├── matmul_mx.h             // Example header file (static Tiling template and kernel implementation)
│   └── README.md               // Example documentation
```

## Example Description

### Example Function

The example implements a fixed shape 8192×8192 MxFP4 matrix multiplication (with scale quantization coefficient input).

### MxMatmul Introduction

MxMatmul (Matrix Multiply with Scale) is a **matrix multiplication with quantization scaling coefficients**, an extension of the Ascend C Matmul API in the MX (Mixed-Precision) quantization scenario. Compared to basic Matmul, the core difference of MxMatmul is the introduction of **scale input**.

#### Computation Formula

$$
C = (\text{scaleA} \otimes A) \times (\text{scaleB} \otimes B)
$$

Where $\otimes$ represents broadcast multiplication. During left/right matrix multiplication with the left/right quantization coefficient matrix, every 32 elements along the K direction share one quantization factor.

#### Parameter Description

| Input | Name | Shape | Data Type | Data Layout Type | Description |
|------|------|------|----------|--------------|------|
| A | Left matrix | [8192, 8192] | `fp4x2_e1m2_t` | `ND` | MX FP4 left matrix |
| scaleA | Left quantization coefficient matrix | [8192, 256] | `fp8_e8m0_t` | `ND` | Scaling factor matrix for A matrix, every 32 elements along A matrix K direction share one scaling factor |
| B | Right matrix | [8192, 8192] | `fp4x2_e1m2_t` | `ND` | MX FP4 right matrix |
| scaleB | Right quantization coefficient matrix | [256, 8192] | `fp8_e8m0_t` | `ND` | Scaling factor matrix for B matrix, every 32 elements along B matrix K direction share one scaling factor |
| C | Output | [8192, 8192] | `bfloat16_t` | `ND` | Computation result |

  <img src="figures/MxMatmul.png">

#### Four-Way Input Description

- In the example, `sK = ceil(K / 64) * 2`, when `K=8192`, `sK=256`
- Therefore `scaleA` shape is `[M, sK] = [8192, 256]`, `scaleB` shape is `[sK, N] = [256, 8192]`
- The ND of `scale` requires special explanation: `scaleA` is written in standard row-major `[M, sK]`; `scaleB` write order is equivalent to `[sK/2, N, 2]`, meaning first 2 Bytes continuous in K direction, then advancing along N direction. The four-way input `ND` layout is shown below:
  <img src="figures/NDformat.png">

- Four-way input transfer is shown below:

  <img src="figures/InputOfMxMatmul.png">

## Example Implementation

### Implementation Key Points

This example unifies tiling parameters to be determined at compile time in `matmul_mx.h`, passing to `Matmul` through template constant `CONSTANT_CFG`:

```cpp
constexpr static auto CONSTANT_CFG = GetMxConstantCFG<aType, bType, cType, EnableScaleCache>();
AscendC::Matmul<aType, bType, cType, cType, CONSTANT_CFG,
                    AscendC::MatmulCallBackFunc<nullptr, nullptr, nullptr>,
                    AscendC::Impl::Detail::MatmulWithScalePolicy>
    matmulObj;
REGIST_MATMUL_OBJ(pipe, GetSysWorkSpacePtr(), matmulObj, (TCubeTiling*)nullptr);
```

Description:
- Kernel side does not perform `TCubeTiling` runtime copy/computation.
- `SCENARIO_NUM` only determines the template instance: `case1 -> MatmulKernel<false>`, `case2 -> MatmulKernel<true>`.

### Case 1 and Case 2 Differences

Both scenarios use constant tiling, with identical L1 parameters: `depthA1/depthB1=4`, `stepKa/stepKb=2`, `stepM/stepN=1`, `dbL0A/dbL0B=2`.
The only difference is `mxTypePara`:

| Scenario | `mxTypePara` | Semantics |
|------|--------------|------|
| Case 1 (`SCENARIO_NUM=1`) | `CASE1_MX_TYPE_PARA = 0x01010101` | scaleA/B transferred synchronously with A/B |
| Case 2 (`SCENARIO_NUM=2`) | `CASE2_MX_TYPE_PARA = 0x01010404` | scaleA/B transferred at multiples relative to A/B in K direction |

`mxTypePara` definition:

- In MxMatmul, the ratio of Scale matrix to matrix A, B loading in L1 can be controlled by setting mxTypePara.
- **MX Scale scaling factor**: `scaleFactorKa=4` means scaleA data loading ratio in K direction is 4 times that of A matrix; `scaleFactorKb=4` means scaleB data loading ratio in K direction is 4 times that of B matrix

    - **mxTypePara**: Combined parameter, used in MxMatmul scenario, representing the ratio of scaleA/scaleB loaded into L1 size relative to A/B matrix loaded into L1 size:
      - **bit [0:6]** `scaleFactorKa`: Ratio coefficient of scaleA to A matrix K direction data loading amount, range [1, 127]
      - **bit [8:14]** `scaleFactorKb`: Ratio coefficient of scaleB to B matrix K direction data loading amount, range [1, 127]
      - **bit [16:22]** `scaleFactorM`: Ratio coefficient of scaleA to A matrix M direction data loading amount, range [1, 127]
      - **bit [24:30]** `scaleFactorN`: Ratio coefficient of scaleB to B matrix N direction data loading amount, range [1, 127]
    - Usage constraints:
      - `scaleFactorM > 1` can only be set when Ka direction is fully loaded (`baseK * stepKa * scaleFactorKa >= singleCoreK`)
      - `scaleFactorN > 1` can only be set when Kb direction is fully loaded (`baseK * stepKb * scaleFactorKb >= singleCoreK`)
      - scaleA, scaleB loading data amounts in M, N, K directions cannot exceed actual sizes
      - This parameter only takes effect in MDL mode

### Parameter Settings and Transfer Data Volume Calculation

The following statistics are for the **GM->L1** transfer path, calculated with current fixed parameters:

| Parameter | Value |
|------|----|
| `M` | `8192` |
| `N` | `8192` |
| `K` | `8192` |
| `singleCoreM` | `2048` |
| `singleCoreN` | `1024` |
| `singleCoreK` | `8192` |
| `baseM` | `256` |
| `baseN` | `256` |
| `baseK` | `256` |
| `stepKa` | `2` |
| `stepKb` | `2` |
| `scaleFactorKa (case1)` | `1` |
| `scaleFactorKb (case1)` | `1` |
| `scaleFactorKa (case2)` | `4` |
| `scaleFactorKb (case2)` | `4` |
| `Data Type` | A/B: `fp4x2` (`0.5 Byte/elem`) |
| `Data Type` | scale: `fp8` (`1 Byte/elem`) |

**Description**:

A/B base block size: `baseM * baseK * 0.5 = 256 * 256 * 0.5 = 32,768 B = 32 KB`, scaleA/scaleB base block size: `256 * (256/32) * 1 = 2,048 B = 2 KB`.

Case 1 single GM→L1 transfer volume:

- A: `stepM * stepKa = 1 * 2 = 2` base blocks, byte volume `2 * 32 = 64 KB`
- B: `stepN * stepKb = 1 * 2 = 2` base blocks, byte volume `2 * 32 = 64 KB`
- scaleA: `stepM * stepKa * scaleFactorKa = 1 * 2 * 1 = 2` base blocks, `4 KB`
- scaleB: `stepN * stepKb * scaleFactorKb = 1 * 2 * 1 = 2` base blocks, `4 KB`
- **Total: `64 + 64 + 4 + 4 = 136 KB`**

> **Note**: `dbL0A/dbL0B=2` means L1→L0 uses double buffer (while L0 computes the current portion, the next portion is already in place from L1), so **L1 needs to hold `136 × 2 = 272 KB`** data simultaneously, but each MTE2 GM→L1 transfer volume remains `136 KB`.

Case 2 single GM→L1 transfer volume:
- A/B same as Case 1: each `64 KB` (total `128 KB`)
- scaleA: `1 * 2 * 4 = 8` base blocks, `16 KB`
- scaleB: `1 * 2 * 4 = 8` base blocks, `16 KB`
- **Total MTE2 per transfer: `64 + 64 + 16 + 16 = 160 KB`**

> Similarly, due to `dbL0A/dbL0B=2`, **L1 total resident volume is `160 × 2 = 320 KB`**.

The difference between Case 1/Case 2 on the scale side is mainly reflected in "single transfer granularity and transfer count":

- Case 1: Small amount per transfer, scale transfer count approximately `16` times (`8192/512`)
- Case 2: Large amount per transfer, scale transfer count approximately `4` times (`8192/2048`)
- At this shape, both have the same total theoretical scale byte volume, but Case 2 has fewer batches and larger reuse windows, more conducive to reducing MTE2 duration.

## Performance Comparison Summary

### Ascend 950PR Chip Performance Data

| Case version | Task Duration(μs) | Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio |
|------|------------------|-----------|----------------|-----------------|---------------|-------------------|-----------------|------------------|----------------|------------------|----------------|--------------------|-------------------|
| Case 1 | 750.219 | 32 | 749.13 | 660.15 | 0.881 | 258.354 | 0.345 | 437.64 | 0.584 | 753.906 | 0.982 | 33.257 | 0.044 |
| Case 2 | 693.283 | 32 | 692.34 | 641.444 | 0.926 | 241.563 | 0.349 | 428.914 | 0.62 | 612.536 | 0.885 | 33.965 | 0.049 |

In Case 2, the Cube computation time ratio reaches `92.6%` (that is, `aic_mac_ratio` in the table).

### Case 2 Benefits (Relative to Case 1)

Both scenarios use constant tiling + template constants. Case 2 benefits relative to Case 1 mainly come from the scale multi-transfer capability brought by `mxTypePara`.

- End-to-end latency: `750.219 -> 693.283 μs`, reduced by `56.936 μs`, benefit `7.59%`.
- MTE2 absolute duration: `753.906 -> 612.536 μs`, reduced by `141.370 μs`, benefit `18.75%`.
- MTE2 ratio: `0.982 -> 0.885`, decreased by `9.7` percentage points.
- MAC ratio: `0.881 -> 0.926`, improved by `4.5` percentage points.

**Tuning Tips**:
> The key difference of MX Matmul is that `scale` transfer can be decoupled from A/B; when `aic_mte2_ratio` is high, prioritize adjusting `scale` transfer ratio through `mxTypePara` to improve L1 reuse and reduce repeated GM->L1 transfers.

### Theoretical Performance Comparison

The performance data of this example was obtained running on Ascend 950PR, which has a clock frequency of 1.65GHz. For MX-FP4 data type, it processes 16×64×16 multiply-accumulate operations per cycle. Cube theoretical computation time is:
$$
T_{\text{theory}} = \frac{M \times N \times K}{16 \times 64 \times 16 \times 1.65 \times 10^9 \times \text{core count}} = \frac{8192 \times 8192 \times 8192}{16384 \times 1.65 \times 10^9 \times 32} = 635.5 μs
$$
Case 1/Case 2 `aic_mac_time` are `660.150 μs` / `641.444 μs` respectively, relative to theoretical value `635.5 μs`:
- Case 1 error: `(660.150 - 635.5) / 635.5 = 3.88%`
- Case 2 error: `(641.444 - 635.5) / 635.5 = 0.94%`

## Build and Run

- Build and Run

Run the following steps in the root directory of this example to build and run the example:

> **Notice**: This example requires `ml_dtypes` library version `0.2.0` and `en_dtypes` library version `0.0.4`. Installation command:

  ```bash
  python3 -m pip install ml_dtypes==0.2.0 en_dtypes==0.0.4
  ```

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in this example directory.
  ```bash
  SCENARIO_NUM=2
  mkdir -p build && cd build;  # Create and enter the build directory
  cmake .. -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=npu -DCMAKE_ASC_ARCHITECTURES=dav-3510; make -j;
  python3 ../scripts/gen_data.py
  ./demo
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin
  ```

  To use NPU simulation mode, set `-DCMAKE_ASC_RUN_MODE=sim`
  ```bash
  cmake .. -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=npu -DCMAKE_ASC_ARCHITECTURES=dav-3510; make -j; # npu mode
  cmake .. -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510; make -j; # npu simulation mode
  ```

  Build option description:

  | Parameter | Values | Description |
  |------|--------|------|
  | `SCENARIO_NUM` | `1` / `2` | 1: Constant tiling + scale synchronous transfer; 2: Constant tiling + scale multi-transfer |
  | `CMAKE_ASC_RUN_MODE` | `npu` (default) / `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | Target SoC architecture (this example only supports 3510) |

  > **Notice:** Clear cmake cache before switching `CMAKE_ASC_RUN_MODE` / `CMAKE_ASC_ARCHITECTURES` / `SCENARIO_NUM`. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

  The following execution result indicates that the accuracy comparison succeeded.
  ```bash
  test pass!
  ```

## Performance Analysis

### Introduction to the msOpProf Tool

msOpProf is a single-operator performance analysis tool with two usage modes: `msopprof` and `msopprof simulator`. It helps users identify anomalies in operator memory, code, and instructions for comprehensive operator tuning. It currently supports performance data collection and automatic parsing for different run modes (on-device or simulation) and file types (executables or operator binary `.o` files).

- On-device performance collection

    On-device performance collection directly measures the operator's execution time on the Ascend AI Processor. This method is suitable for quickly locating operator performance issues in an on-device environment.

    Run msopprof on the `demo` executable for operator tuning:
    ```
    msopprof ./demo
    ```

    - Performance data description  
      After the command completes, a folder named "OPPROF_{timestamp}_XXX" will be generated in the default directory. The performance data folder structure is as follows:

      ```bash
      ├──dump                       # Raw performance data; users do not need to inspect it
      ├──ArithmeticUtilization.csv  # Cube/Vector instruction cycle proportions
      ├──L2Cache.csv                # L2 Cache hit rate; affects MTE2. Plan data transfer logic properly to increase the hit rate
      ├──Memory.csv                 # Read/write bandwidth rates of UB, L1, and main memory
      ├──MemoryL0.csv               # Read/write bandwidth rates of L0A, L0B, and L0C
      ├──MemoryUB.csv               # Read/write bandwidth rates from Vector and Scalar to UB
      ├──OpBasicInfo.csv            # Basic operator information
      ├──PipeUtilization.csv        # Durations and proportions of computation and data transfer units
      ├──ResourceConflictRatio.csv  # Proportions of UB bank groups, bank conflicts, and resource conflicts among all instructions
      └──visualize_data.bin         # MindStudio Insight presentation file
      ```

View the detailed performance analysis results:

```bash
# View Task Duration and other metrics
cat ./OPPROF_*/PipeUtilization.csv
```
