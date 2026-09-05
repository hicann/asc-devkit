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

1. Global Memory -> L1 Buffer: Call the copy interface with `copy_gm_to_l1` `Operation` transfer capability to transfer `L1BatchSize` of matrix A, matrix B, and Bias from Global Memory to L1 Buffer.
2. L1 Buffer -> L0A Buffer/L0B Buffer: Call the copy interface with `copy_l1_to_l0a` and `copy_l1_to_l0b` `Operation` transfer capability to transfer `L0BatchSize` of matrix A and matrix B from L1 Buffer to L0 Buffer.
3. L1 Buffer -> BiasTable Buffer: Call the copy interface with `copy_l1_to_biastable` `Operation` transfer capability to transfer `L0BatchSize` of Bias from L1 Buffer to BiasTable Buffer.
4. mmad: Loop through matrices A, B, and Bias on L0 Buffer `L0BatchSize` times, computing one batch group of A, B matrix multiplication plus Bias each time and writing to L0C Buffer.
5. L0C Buffer -> Global Memory: Call the copy interface with `copy_l0c_to_gm` `Operation` transfer capability to transfer `L0BatchSize` result matrices C out to Global Memory at once.

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
//Construct Layout through make_frame_layout, build Tensor through make_tensor.

auto gmA = make_tensor(make_mem_ptr(a), make_frame_layout<nd_ext_layout_ptn>(B, M, K));
auto gmB = make_tensor(make_mem_ptr(b), make_frame_layout<nd_ext_layout_ptn>(B, K, N));
auto gmC = make_tensor(make_mem_ptr(c), make_frame_layout<nd_ext_layout_ptn>(B, M, N));
auto gmBias = make_tensor(make_mem_ptr(bias), make_frame_layout<nd_ext_layout_ptn>(B, 1, N));

auto l1ATensor = make_tensor(make_mem_ptr(l1ABuf), make_frame_layout<nz_layout_ptn, T>(L1BatchSize, M, K));
auto l1BTensor = make_tensor(make_mem_ptr(l1BBuf), make_frame_layout<nz_layout_ptn, T>(L1BatchSize, K, N));
auto l1BiasTensor = make_tensor(make_mem_ptr(l1BiasBuf), make_frame_layout<nd_ext_layout_ptn, T>(L1BatchSize, 1, N));
auto l0ATensor = make_tensor(make_mem_ptr(l0ABuf), make_frame_layout<nz_layout_ptn, T>(L0BatchSize, M, K));
auto l0BTensor = make_tensor(make_mem_ptr(l0BBuf), make_frame_layout<zn_layout_ptn, T>(L0BatchSize, K, N));
auto l0CTensor = make_tensor(make_mem_ptr(l0CBuf), make_frame_layout<nz_layout_ptn>(L0BatchSize, M, N));
auto l0BiasTensor = make_tensor(make_mem_ptr(l0BiasBuf), make_frame_layout<nd_ext_layout_ptn>(L0BatchSize, 1, N));
```

### Matrix Batch Transfer In

The example controls the batch quantity entering L1 Buffer from Global Memory each time through `L1BatchSize`, and controls the batch quantity transferred from L1 Buffer to L0A Buffer, L0B Buffer, L0C Buffer each time through `L0BatchSize`, thereby avoiding transferring all batch data at once which would exceed on-chip memory limits.

```cpp
for (uint32_t l1batchIndex = BIndexStart; l1batchIndex < BIndexEnd; l1batchIndex += L1BatchSize) {
    uint32_t l1BatchSize = min(L1BatchSize, BIndexEnd - l1batchIndex);
    copy(copyGM2L1Atom, l1ATensor,
         gmA.slice(make_coord(l1batchIndex, make_coord(0, 0)), make_shape(l1BatchSize, make_shape(M, K))));
    copy(copyGM2L1Atom, l1BTensor,
         gmB.slice(make_coord(l1batchIndex, make_coord(0, 0)), make_shape(l1BatchSize, make_shape(K, N))));
    copy(copyGM2L1Atom, l1BiasTensor,
         gmBias.slice(make_coord(l1batchIndex, make_coord(0, 0)), make_shape(l1BatchSize, make_shape(1, N))));

    for (uint32_t l0BatchIndex = 0; l0BatchIndex < l1BatchSize; l0BatchIndex += L0BatchSize) {
        uint32_t l0BatchSize = min(L0BatchSize, l1BatchSize - l0BatchIndex);
        copy(copyL12L0AAtom, l0ATensor,
             l1ATensor.slice(make_coord(l0BatchIndex, make_coord(0, 0)), make_shape(l0BatchSize, make_shape(M, K))));
        copy(copyL12L0BAtom, l0BTensor,
             l1BTensor.slice(make_coord(l0BatchIndex, make_coord(0, 0)), make_shape(l0BatchSize, make_shape(K, N))));
        copy(copyL12BTAtom, l0BiasTensor,
             l1BiasTensor.slice(make_coord(l0BatchIndex, make_coord(0, 0)), make_shape(l0BatchSize, make_shape(1, N))));
    }
}
```

### mmad Batch Computation

Each Batch's mmad operation corresponds to an independent Bias. Since the hardware mad instruction does not support batch data matrix multiply-add operations, mmad operations must be looped along the Batch dimension, with each iteration taking a single input matrix A, matrix B, and Bias as input operands for computation.

```cpp
for (uint32_t l0CBatchIndex = 0; l0CBatchIndex < l0BatchSize; l0CBatchIndex++) {
    mmad(mmadAtom.with(mmad_params{static_cast<uint16_t>(M), static_cast<uint16_t>(N), static_cast<uint16_t>(K), unit_flag_mode::disable, true}),
        l0CTensor.slice(make_coord(l0CBatchIndex, make_coord(0, 0)), make_shape(1, make_shape(M, N))),
        l0ATensor.slice(make_coord(l0CBatchIndex, make_coord(0, 0)), make_shape(1, make_shape(M, K))),
        l0BTensor.slice(make_coord(l0CBatchIndex, make_coord(0, 0)), make_shape(1, make_shape(K, N))),
        l0BiasTensor.slice(make_coord(l0CBatchIndex, make_coord(0, 0)), make_shape(1, make_shape(1, N))));
}
```

### Matrix Batch Transfer Out

After completing `L0BatchSize` quantity of matrix multiply-add computations, call the copy interface with `copy_l0c_to_gm` `Operation` data transfer capability to transfer result matrices C from L0C Buffer to Global Memory.
```cpp
copy(copyL0C2GMAtom,
     gmC.slice(make_coord(l1batchIndex + l0BatchIndex, make_coord(0, 0)),
               make_shape(l0BatchSize, make_shape(M, N))),
     l0CTensor);
```

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables
  Configure environment variables according to the [installation method](https://gitcode.com/cann/asc-devkit/blob/master/docs/en/quick_start.md) of the CANN development kit in the current environment.

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
