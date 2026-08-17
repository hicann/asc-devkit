# Batch Matmul Example

## Overview

This example implements batched matrix multiplication (Batch Matmul) with bias using the Tensor API programming paradigm. The input matrices A, B, and Bias all use half data type, and the output matrix C also uses half data type. Neither matrix A nor matrix B is transposed. The core computation logic is defined as follows:
```text
C[b] = A[b] * B[b] + Bias[b], b = 0, 1, ..., B - 1
```

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| <cann-filter npu-type="950">Ascend 950PR/Ascend 950DT</cann-filter> | >= CANN 9.1.0 |

> **Note:** This example depends on CANN features that have not been officially released. Use the latest CANN master package.

## Directory Structure

```text
├── batch_matmul_tensor_api
│   ├── scripts
│   │   ├── gen_data.py                 // Script for generating input data and ground truth data
│   │   └── verify_result.py            // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt                  // Build project file
│   ├── data_utils.h                    // Data read/write functions
│   ├── batch_matmul_tensor_api.asc     // Ascend C example implementation and invocation example
│   └── README.md                       // Example documentation
```

## Example Description

The data flow of this example is as follows:

1. Global Memory -> L1 Buffer: Call the Copy interface with `CopyGM2L1` `Operation` transfer capability to transfer `L1BatchSize` of matrix A, matrix B, and Bias from Global Memory to L1 Buffer.
2. L1 Buffer -> L0A Buffer/L0B Buffer: Call the Copy interface with `CopyL12L0A` and `CopyL12L0B` `Operation` transfer capability to transfer `L0BatchSize` of matrix A and matrix B from L1 Buffer to L0 Buffer.
3. L1 Buffer -> BiasTable Buffer: Call the Copy interface with `CopyL12BT` `Operation` transfer capability to transfer `L0BatchSize` of Bias from L1 Buffer to BiasTable Buffer.
4. Mmad: Loop through matrices A, B, and Bias on L0 Buffer `L0BatchSize` times, computing one batch group of A, B matrix multiplication plus Bias each time and writing to L0C Buffer.
5. L0C Buffer -> Global Memory: Call the Copy interface with `CopyL0C2GM` `Operation` transfer capability to transfer `L0BatchSize` result matrices C out to Global Memory at once.

### Batch Matmul Definition

Batch matrix multiplication (batch matmul) is an extension of standard matrix multiplication along the batch dimension. The core logic is: for a batch of data containing multiple matrices, perform standard matrix multiplication on each matrix in the batch individually, outputting the same number of result matrices.

In scenarios with Bias, if the input matrices A, B, and Bias have shapes `[B, M, K]`, `[B, K, N]`, and `[B, 1, N]` respectively, the output matrix C has shape `[B, M, N]`. For any batch `i`, take `A[i]` and `B[i]` for standard matrix multiplication, then add `Bias[i]` to produce `C[i]`.

Note that matrices from different batches do not interact with each other.

### Example Specifications

The input/output matrix specifications for this example are shown in Table 1 below.

**Table 1**  Input/Output Specifications

| Input/Output | Data Type | Shape | Format |
|----------|----------|-------|--------|
| Input Matrix A | half | [128, 32, 32] | ND |
| Input Matrix B | half | [128, 32, 32] | ND |
| Bias | half | [128, 1, 32] | ND |
| Output Matrix C | half | [128, 32, 32] | ND |

The default batch quantity transferred from Global Memory to L1 Buffer for processing each time is `L1BatchSize=32`.
The default batch quantity transferred from L1 Buffer to L0A Buffer, L0B Buffer, L0C Buffer for computation each time is `L0BatchSize=4`.

### Tensor Construction

The format of matrices A, B, C in Global Memory is `ND`, the format of matrices A, B in L1 Buffer is `NZ`, the format of matrix A in L0 Buffer is `NZ`, the format of matrix B in L0B Buffer is `ZN`, the format of matrix C in L0C Buffer is `NZ`, and the format of Bias in Global Memory, L1 Buffer, and L0 Buffer is all `ND`.

```cpp
//Construct Layout through MakeFrameLayout, build Tensor through MakeTensor.

auto gmA = MakeTensor(MakeMemPtr(a), MakeFrameLayout<NDExtLayoutPtn>(B, M, K));
auto gmB = MakeTensor(MakeMemPtr(b), MakeFrameLayout<NDExtLayoutPtn>(B, K, N));
auto gmC = MakeTensor(MakeMemPtr(c), MakeFrameLayout<NDExtLayoutPtn>(B, M, N));
auto gmBias = MakeTensor(MakeMemPtr(bias), MakeFrameLayout<NDExtLayoutPtn>(B, 1, N));

auto l1ATensor = MakeTensor(MakeMemPtr(l1ABuf), MakeFrameLayout<NZLayoutPtn, T>(L1BatchSize, M, K));
auto l1BTensor = MakeTensor(MakeMemPtr(l1BBuf), MakeFrameLayout<NZLayoutPtn, T>(L1BatchSize, K, N));
auto l1BiasTensor = MakeTensor(MakeMemPtr(l1BiasBuf), MakeFrameLayout<NDExtLayoutPtn, T>(L1BatchSize, 1, N));
auto l0ATensor = MakeTensor(MakeMemPtr(l0ABuf), MakeFrameLayout<NZLayoutPtn, T>(L0BatchSize, M, K));
auto l0BTensor = MakeTensor(MakeMemPtr(l0BBuf), MakeFrameLayout<ZNLayoutPtn, T>(L0BatchSize, K, N));
auto l0CTensor = MakeTensor(MakeMemPtr(l0CBuf), MakeFrameLayout<NZLayoutPtn>(L0BatchSize, M, N));
auto l0BiasTensor = MakeTensor(MakeMemPtr(l0BiasBuf), MakeFrameLayout<NDExtLayoutPtn>(L0BatchSize, 1, N));
```

### Matrix Batch Transfer In

The example controls the batch quantity entering L1 Buffer from Global Memory each time through `L1BatchSize`, and controls the batch quantity transferred from L1 Buffer to L0A Buffer, L0B Buffer, L0C Buffer each time through `L0BatchSize`, thereby avoiding transferring all batch data at once which would exceed on-chip memory limits.

```cpp
for (uint32_t l1batchIndex = BIndexStart; l1batchIndex < BIndexEnd; l1batchIndex += L1BatchSize) {
    uint32_t l1BatchSize = min(L1BatchSize, BIndexEnd - l1batchIndex);
    Copy(copyGM2L1Atom, l1ATensor,
         gmA.Slice(MakeCoord(l1batchIndex, MakeCoord(0, 0)), MakeShape(l1BatchSize, MakeShape(M, K))));
    Copy(copyGM2L1Atom, l1BTensor,
         gmB.Slice(MakeCoord(l1batchIndex, MakeCoord(0, 0)), MakeShape(l1BatchSize, MakeShape(K, N))));
    Copy(copyGM2L1Atom, l1BiasTensor,
         gmBias.Slice(MakeCoord(l1batchIndex, MakeCoord(0, 0)), MakeShape(l1BatchSize, MakeShape(1, N))));

    for (uint32_t l0BatchIndex = 0; l0BatchIndex < l1BatchSize; l0BatchIndex += L0BatchSize) {
        uint32_t l0BatchSize = min(L0BatchSize, l1BatchSize - l0BatchIndex);
        Copy(copyL12L0AAtom, l0ATensor,
             l1ATensor.Slice(MakeCoord(l0BatchIndex, MakeCoord(0, 0)), MakeShape(l0BatchSize, MakeShape(M, K))));
        Copy(copyL12L0BAtom, l0BTensor,
             l1BTensor.Slice(MakeCoord(l0BatchIndex, MakeCoord(0, 0)), MakeShape(l0BatchSize, MakeShape(K, N))));
        Copy(copyL12BTAtom, l0BiasTensor,
             l1BiasTensor.Slice(MakeCoord(l0BatchIndex, MakeCoord(0, 0)), MakeShape(l0BatchSize, MakeShape(1, N))));
    }
}
```

### Mmad Batch Computation

Each Batch's Mmad operation corresponds to an independent Bias. Since the hardware mad instruction does not support batch data matrix multiply-add operations, Mmad operations must be looped along the Batch dimension, with each iteration taking a single input matrix A, matrix B, and Bias as input operands for computation.

```cpp
for (uint32_t l0CBatchIndex = 0; l0CBatchIndex < l0BatchSize; l0CBatchIndex++) {
    Mmad(mmadAtom.with(MmadParams{static_cast<uint16_t>(M), static_cast<uint16_t>(N), static_cast<uint16_t>(K), 0, true}),
        l0CTensor.Slice(MakeCoord(l0CBatchIndex, MakeCoord(0, 0)), MakeShape(1, MakeShape(M, N))),
        l0ATensor.Slice(MakeCoord(l0CBatchIndex, MakeCoord(0, 0)), MakeShape(1, MakeShape(M, K))),
        l0BTensor.Slice(MakeCoord(l0CBatchIndex, MakeCoord(0, 0)), MakeShape(1, MakeShape(K, N))),
        l0BiasTensor.Slice(MakeCoord(l0CBatchIndex, MakeCoord(0, 0)), MakeShape(1, MakeShape(1, N))));
}
```

### Matrix Batch Transfer Out

After completing `L0BatchSize` quantity of matrix multiply-add computations, call the Copy interface with `CopyL0C2GM` `Operation` data transfer capability to transfer result matrices C from L0C Buffer to Global Memory.
```cpp
Copy(copyL0C2GMAtom,
     gmC.Slice(MakeCoord(l1batchIndex + l0BatchIndex, MakeCoord(0, 0)),
               MakeShape(l0BatchSize, MakeShape(M, N))),
     l0CTensor);
```

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables
  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.

  ```bash
  mkdir -p build && cd build;                                                     # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                            # Build the project, default NPU mode
  python3 ../scripts/gen_data.py                                                  # Generate test input data
  ./demo                                                                          # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin         # Verify whether the output result is correct and confirm algorithm logic is correct
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;   # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` |`dav-3510` | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
