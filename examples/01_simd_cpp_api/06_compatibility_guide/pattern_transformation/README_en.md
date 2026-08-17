# pattern_transformation Compatibility Example

## Overview

This example is based on the basic mmad example and demonstrates the fractal transformation logic for the L1 Buffer -> L0A Buffer path, isolating different hardware implementations through the compile-time macro `__NPU_ARCH__`.

- In Atlas A2/A3 Series Products, the data layout of L0A Buffer is Zz fractal
- In Ascend 950PR/Ascend 950DT Series Products, the data layout of L0A Buffer is Nz fractal

The fractal transformation involves compatibility adaptation for the following two typical scenarios:

1. L0A is not reused. A single mmad computation is performed on an entire A matrix.
2. L0A is reused. The M axis is split, the A matrix is divided into two sub-matrices, and two mmad computations are performed.

**For Scenario 1**, the logic of the L1>L0A transfer process needs to be modified, changing from Nz->Zz fractal transformation to Nz->Nz. For details, refer to [Example Implementation](#example-implementation).  
**For Scenario 2**, on Atlas A2/A3 Series Products, since the Zz fractal is inherently continuous along the M axis, the L1->L0A transfer logic is the same as Scenario 1. For Ascend 950PR/Ascend 950DT Series Products, splitting the M axis causes matrix discontinuity, requiring splitting into two Nz matrices. For details, refer to [Example Implementation](#example-implementation).

## Supported Products and CANN Versions

| Product | CANN Version |
| ------ | ------------- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```text
├── pattern_transformation
│   ├── scripts
│   │   ├── gen_data.py                    // Input data and ground truth generation script
│   │   └── verify_result.py               // Verification script for comparing output data with ground truth
│   ├── CMakeLists.txt                     // Build project file
│   ├── data_utils.h                       // Data read/write functions
│   ├── figures                            // Image resources directory for README
│   ├── pattern_transformation.asc         // AscendC operator implementation & invocation example
│   └── README.md                          // Example documentation
```

## Example Specifications

<table>
<caption>Example Specification Table</caption>
<tr><td rowspan="1" align="center">Category</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td rowspan="3" align="center">Example Input</td></tr>
<tr><td align="center">x</td><td align="center">[64, 64]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[64, 64]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[64, 64]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">pattern_transformation</td></tr>
</table>

## Example Implementation

***Figure 1 L0A Non-Reuse Scenario Overview***

![L0A Non-Reuse Scenario](./figures/普通兼容场景.png)

Figure 1 shows the L1→L0A data transfer differences for the two chip types in the L0A non-reuse scenario:
- Atlas A2/A3 Series: L0A uses Zz fractal, requiring Nz→Zz fractal transformation through LoadData (2D matrix transfer).
- Ascend 950PR/Ascend 950DT: L0A uses Nz fractal, which is the same as L1, requiring no fractal transformation, and LoadData (2D matrix transfer) parameters are more concise.

***Figure 2 L0A Reuse Scenario Overview***

![L0A Split Compatibility Scenario](./figures/L0A切分兼容场景.png)

Figure 2 shows the adaptation logic for the two chip types in the L0A reuse (M axis split) scenario:
- Atlas A2/A3 Series: Under Zz fractal, A1 and A2 remain continuous in L0A after M axis splitting, requiring only adjustment of the second Mmad offset.
- Ascend 950PR/Ascend 950DT: Under Nz fractal, A1 and A2 are not continuous after M axis splitting. Two LoadData (2D matrix transfer) instructions are needed to separately transfer the upper and lower Nz sub-matrices, without changing the Mmad computation logic or pipeline arrangement.

### GM->L1

The GM to L1 transfer process **requires no compatibility adaptation**. The DataCopy interface is used to load the ND matrix from GM into L1. The relevant code snippet is as follows:

```cpp
__aicore__ inline void CopyIn(AscendC::LocalTensor<T> a1Local, AscendC::LocalTensor<U> b1Local)
{
    ...
    AscendC::DataCopy(a1Local, aGM, nd2nzA1Params);
    ...
    AscendC::DataCopy(b1Local, bGM, nd2nzB1Params);
}
```

DataCopy transfers the ND format matrices x[M, K] and y[K, N] from GM into the L1 Buffer, storing them in Nz fractal format for subsequent Cube operations.

### L1->L0A

In Atlas A2/A3 Series Products, the L0A data layout is Zz fractal, while in Ascend 950PR/Ascend 950DT it changes to Nz fractal. The following sections describe the compatibility differences by scenario. The compile-time macro `SCENARIO_NUM` distinguishes scenarios: `SCENARIO_NUM=1` is the L0A non-reuse scenario, and `SCENARIO_NUM=2` is the L0A reuse scenario (M axis split).

#### L0A Non-Reuse Scenario

In Atlas A2/A3 Series Products, L1->L0A requires transformation from Nz fractal to Zz fractal, implemented through the LoadData (2D matrix transfer) interface. The relevant code is as follows:

```cpp
...
for (uint32_t i = 0; i < mBlocks; ++i) {
    constexpr uint32_t mBlocks = M / CUBE_BLOCK;
    constexpr uint32_t kBlocks = K * sizeof(T) / C0_SIZE;
    int srcOffset = 0;
    int dstOffset = 0;
    for (uint32_t i = 0; i < mBlocks; ++i) {
        AscendC::LoadData2DParams loadDataParams;
        loadDataParams.repeatTimes = kBlocks;
        loadDataParams.srcStride = mBlocks;
        loadDataParams.ifTranspose = false;
        AscendC::LoadData(a2[dstOffset], a1[srcOffset], loadDataParams);
        srcOffset += CUBE_BLOCK * CUBE_BLOCK;
        dstOffset += K * CUBE_BLOCK;
    }
}
```

In Ascend 950PR/Ascend 950DT, L1->L0A does not require fractal transformation and maintains the Nz fractal. It is implemented through the LoadData (2D matrix transfer) interface. The relevant code is as follows:

```cpp
constexpr uint32_t mBlocks = M / CUBE_BLOCK;
constexpr uint32_t kBlocks = K / CUBE_BLOCK;
int srcOffset = 0;
int dstOffset = 0;
for (uint32_t i = 0; i < kBlocks; ++i) {
    AscendC::LoadData2DParams loadDataParams;
    loadDataParams.repeatTimes = mBlocks;
    loadDataParams.srcStride = 1;
    loadDataParams.ifTranspose = false;
    AscendC::LoadData(a2[dstOffset], a1[srcOffset], loadDataParams);
    srcOffset += CUBE_BLOCK * CUBE_BLOCK * mBlocks;
    dstOffset += CUBE_BLOCK * CUBE_BLOCK * mBlocks;
}
```


#### L0A Reuse Scenario

Before analysis, let us introduce the mathematical background. For matrix multiplication C = A × B, when A can be split into two blocks:

$$
A = \begin{bmatrix} A_1 \\ A_2 \end{bmatrix}, \quad
A \cdot B = \begin{bmatrix} A_1 \\ A_2 \end{bmatrix} B
= \begin{bmatrix} A_1 B \\ A_2 B \end{bmatrix}
= \begin{bmatrix} C_1 \\ C_2 \end{bmatrix} = C
$$

That is, A can be split by rows into A1 and A2, each multiplied with B to produce C1 and C2, which are then concatenated to form C.
In Atlas A2/A3 Series Products, since L0A uses Zz fractal, A1 and A2 remain continuous after M axis splitting. The second mmad computation only requires modifying the A matrix offset.
In Ascend 950PR/Ascend 950DT, since L0A uses Nz fractal, A1 and A2 are not continuous after M axis splitting. To avoid changing the mmad computation logic and subsequent pipeline arrangement, the LoadData (2D matrix transfer) instruction is used to split the entire Nz matrix into two sub-Nz matrices. The relevant code is as follows:

```cpp
// Transfer the upper Nz fractal
...
for (uint32_t i = 0; i < kBlocks; ++i) {
    ...
    AscendC::LoadData(a2[dstOffset], a1[srcOffset], loadDataParams);
    ...
}
// Transfer the lower Nz fractal
srcOffset = CUBE_BLOCK * CUBE_BLOCK * mBlocks / 2;  // Start from the lower half
for (uint32_t i = 0; i < kBlocks; ++i) {
    ...
    AscendC::LoadData(a2[dstOffset], a1[srcOffset], loadDataParams);
    ...
}
```


### L1->L0B

The L1 to L0B transfer uses LoadData (2D matrix transfer) for Nz format transfer on all products and **requires no compatibility adaptation**. The relevant code is as follows:

```cpp
...
for (uint32_t i = 0; i < kBlocks; ++i) {
    AscendC::LoadData(b2[i * dstStride], b1[i * srcStride], loadDataParams);
}
```

### Compute

The Compute stage uses the Mmad interface to complete matrix multiplication. The Mmad interface is common across both architectures and **requires no compatibility adaptation**.

- `SCENARIO_NUM=1` (L0A non-reuse): Call Mmad once to complete the full matrix multiplication C=A×B.

```cpp
AscendC::MmadParams mmadParams;
mmadParams.m = M;
mmadParams.n = N;
mmadParams.k = K;
AscendC::Mmad(co1Local, a2, b2, mmadParams);
```

- `SCENARIO_NUM=2` (L0A reuse): Split the A matrix by the M axis into upper and lower halves, computing C1=A1×B and C2=A2×B respectively.

```cpp
mmadParams.m = M / 2;  // Split the M axis into upper and lower halves; compute half of M each time
mmadParams.n = N;
mmadParams.k = K;
mmadParams.cmatrixInitVal = true;
mmadParams.isBias = false;
AscendC::Mmad(co1Local, a2, b2, mmadParams);
AscendC::Mmad(co1Local[M * N / 2], a2[M * K / 2], b2, mmadParams);  // Lower half A2[32, 64]
```

### L0C->GM

The L0C to GM transfer uses the AscendC::FixpipeParamsV220 interface to write the float results from L0C back to GM. This interface is common across both architectures and **requires no compatibility adaptation**.

- `SCENARIO_NUM=1` (L0A non-reuse): Transfer the complete result matrix in a single Fixpipe call.

```cpp
AscendC::FixpipeParamsV220 fixpipeParams;
fixpipeParams.mSize = M;
fixpipeParams.srcStride = M;
AscendC::Fixpipe(cGM, co1Local, fixpipeParams);
```

- `SCENARIO_NUM=2` (L0A reuse): Transfer the result matrix in upper and lower halves along the M axis.

```cpp
AscendC::FixpipeParamsV220 fixpipeParams;
fixpipeParams.mSize = M / 2;
fixpipeParams.srcStride = M / 2;
AscendC::Fixpipe(cGM, co1Local, fixpipeParams);
AscendC::Fixpipe(cGM[M * N / 2], co1Local[M * N / 2], fixpipeParams);
```

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in this example directory.
  ```bash
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=1 .. && make -j       # Build (default NPU mode)
  python3 ../scripts/gen_data.py                                # Generate test input data
  ./demo                                                        # Run the compiled executable
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin  # Verify results
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter. Examples:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=1 .. && make -j   # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=1 .. && make -j   # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2/A3 Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`, `2` | Scenario number: 1 for L0A non-reuse scenario, 2 for L0A reuse scenario (M axis split) |

- Execution results

  The following execution result indicates that the accuracy comparison succeeded:

  ```bash
  test pass!
  ```
