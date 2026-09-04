# Batch Matmul Example

## Overview

This example introduces batched matrix multiplication with float data type input where neither the left nor right matrices are transposed. The GM -> L1 path uses DataCopy (Nd2NzParams) for batch transfer, the L0C -> GM and L0C -> L1 paths use Fixpipe for batch transfer, and L1 -> L0A / L0B transfer and Mmad matrix multiplication execute in a loop for batch iterations.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.2.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.2.0 |

## Directory Structure

```text
├── batch_matmul
│   ├── scripts
│   │   ├── gen_data.py             // Script for generating input data and ground truth data
│   │   └── verify_result.py        // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt              // Build project file
│   ├── data_utils.h                // Data read/write functions
│   ├── batch_matmul.asc            // Ascend C example implementation & invocation example
│   └── README.md                   // Example documentation
```

## Example Description

The data flow of this example is as follows:

1. GM -> L1: Call DataCopy (Nd2NzParams) to transfer B pairs of A and B matrices at once and perform ND -> Nz inline conversion.
2. L1 -> L0A / L0B: Loop batch times, transferring one pair of A and B matrices for the current batch each time.
3. Mmad: Loop batch times, computing one pair of A and B matrix multiplication results each time and writing to the corresponding position in L0C.
4. L0C -> GM / L1: Call Fixpipe to transfer B C matrices out at once; Atlas A2/A3 series products additionally demonstrate the L0C -> L1 transfer path.

### Batch Mmad Definition

Batch matrix multiplication (batch mmad) is an extension of standard matrix multiplication along the batch dimension. The core logic is: for a batch of data containing multiple matrices, perform standard matrix multiplication on each matrix in the batch individually, outputting the same number of result matrices.

Simply put, given two batch matrices A and B with shapes [B, M, K] and [B, K, N] respectively (where B is the batch size, M/K/N are matrix dimensions), batch matrix multiplication takes A[i] (shape [M, K]) and B[i] (shape [K, N]) for each batch index i (i in [0, B-1]) and performs standard matrix multiplication, producing a batch result matrix C with shape [B, M, N]. For any batch i (0 <= i < B), the i-th matrix of C satisfies:
C[i]=A[i]xB[i].

Note that matrices from different batches do not interact with each other.

### Example Specifications

The input/output matrix specifications for this example are shown in Table 1 below:

<a name="table1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 1: Input/Output Specifications</span></caption>
  <tr>
    <td>Input/Output</td>
    <td>Data Type</td>
    <td>Shape</td>
    <td>Transposed</td>
  </tr>
  <tr>
    <td>Input Matrix A</td>
    <td>float</td>
    <td>[4, 30, 40]</td>
    <td>false</td>
  </tr>
  <tr>
    <td>Input Matrix B</td>
    <td>float</td>
    <td>[4, 40, 70]</td>
    <td>false</td>
  </tr>
  <tr>
    <td>Output Matrix C</td>
    <td>float</td>
    <td>[4, 30, 70]</td>
    <td>-</td>
  </tr>
</table>

### Matrix Batch Transfer In (GM -> L1)

Based on the batch Mmad definition, a total of B pairs of A and B matrices undergo matrix multiplication. The GM -> L1 path calls DataCopy (Nd2NzParams) to complete ND -> Nz inline conversion. By configuring `nd2nzA1Params.ndNum = B`, B pairs of A and B matrices are transferred at once.

```cpp
// GM -> L1, transfer A matrix
AscendC::Nd2NzParams nd2nzA1Params;
// Number of ND matrices to transfer
nd2nzA1Params.ndNum = B;
// Number of rows in ND matrix
nd2nzA1Params.nValue = m;
// Number of columns in ND matrix
nd2nzA1Params.dValue = k;
// Offset between start addresses of adjacent ND matrices in source operand, in elements
nd2nzA1Params.srcNdMatrixStride = m * k;
// Offset between start addresses of adjacent rows within the same ND matrix in source operand, in elements
nd2nzA1Params.srcDValue = k;

// After ND to NZ format conversion, one row in the source operand converts to multiple rows in the destination operand.
// This parameter represents the offset between start addresses of adjacent rows from the same source row in the destination NZ matrix, unit: C0_SIZE (32B).
// Data is aligned when transferred to L1
nd2nzA1Params.dstNzC0Stride = CeilAlign(m, cubeShape[0]);
// Offset between start addresses of adjacent Z-type matrix rows in the destination NZ matrix. Unit: C0_SIZE (32B).
nd2nzA1Params.dstNzNStride = 1;
// Offset between start addresses of adjacent NZ matrices in the destination NZ matrix, in elements
nd2nzA1Params.dstNzMatrixStride = aSizeAlignL0;
```

### L1 -> L0A / L0B Transfer and Matrix Multiplication Mmad Loop B Times

For loop B times, transferring A and B matrices for each batch from L1 -> L0A / L0B each time. The Mmad instruction computes one pair of A and B matrix multiplication results each time. Then storing them at corresponding positions in L0C. After the loop completes, the full results on L0C are transferred out to GM.

```cpp
for (int32_t batchIndex = 0; batchIndex < B; batchIndex++) {
    SplitA(a1Local[batchIndex * aSizeAlignL0], a2Local);
    SplitBTranspose(b1Local[batchIndex * bSizeAlignL0], b2Local);
    AscendC::SetFlag<AscendC::HardEvent::MTE1_M>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::MTE1_M>(EVENT_ID0);

    AscendC::MmadParams params;
    params.m = m;
    params.n = n;
    params.k = k;
    AscendC::Mmad(
        co1Local[batchIndex * CeilAlign(m, cubeShape[0]) * CeilAlign(n, cubeShape[0])], a2Local, b2Local,
        params);

    AscendC::SetFlag<AscendC::HardEvent::M_MTE1>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::M_MTE1>(EVENT_ID0);
}
```

### Matrix Batch Transfer Out

The L0C -> GM path calls the Fixpipe transfer interface. By configuring `fixpipeParams.ndNum = B`, B pairs of C matrices are transferred out at once. Note that the C matrices in L0C are aligned, while the C matrices transferred to GM have the original unaligned shape.

For Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, the example additionally demonstrates the L0C -> L1 transfer path: Fixpipe quantizes float results from L0C to half and writes to L1, printing the NZ results on L1 via DumpTensor.

```cpp
// Size of source NZ matrix in N direction.
fixpipeParams.nSize = n;
// Size of source NZ matrix in M direction.
fixpipeParams.mSize = m;
// Start address offset of adjacent Z layouts in source NZ matrix, unit: C0_Size(16*sizeof(T), where T is src data type)
fixpipeParams.srcStride = CeilAlign(m, cubeShape[0]);
// When NZ2ND feature is enabled, represents number of elements in each row of destination ND matrix, non-zero value, unit: elements
fixpipeParams.dstStride = n;
// Number of source NZ matrices, that is, number of ND matrices to transfer
fixpipeParams.ndNum = B;
// Interval between start addresses of different NZ matrices, unit: 1024B
fixpipeParams.srcNdStride = (CeilAlign(m, cubeShape[0]) * CeilAlign(n, cubeShape[0])) 
                                / (cubeShape[0] * cubeShape[0]);
// Offset between start addresses of adjacent destination ND matrices, unit: elements
```

### Avoiding Data Memory Exceeding Storage Limits

Users should ensure that the total memory occupied by data during the entire batch Mmad process does not exceed storage limits.
Users can obtain the memory sizes of L1, L0A, L0B, L0C storage spaces on the hardware platform through the PlatformAscendC class member function GetCoreMemSize.

## Build and Run

Run the following steps in the root directory of this example to build and run the operator.

- Configure environment variables  
  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.
- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DCMAKE_ASC_RUN_MODE=npu ..;make -j;    # Build the project
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic is correct
  ```
  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```
  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products/Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
