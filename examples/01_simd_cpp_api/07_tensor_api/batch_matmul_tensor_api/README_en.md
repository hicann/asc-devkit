# Batch Matmul Example

## Overview

This example implements batched matrix multiplication (Batch Matmul) with bias using the Tensor API programming paradigm. The input matrices A, B, and Bias all use half data type, and the output matrix C also uses half data type. Neither matrix A nor matrix B is transposed. The core computation logic is defined as follows:
```text
C[b] = A[b] * B[b] + Bias[b], b = 0, 1, ..., B - 1
```

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| <cann-filter npu-type="950">Ascend 950PR&950DT products</cann-filter> | >= CANN 9.3.0 |

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

1. Global Memory -> L1 Buffer: Call the copy interface with `copy_gm_to_l1` `Operation` transfer capability to transfer `l1_batch_size` of matrix A, matrix B, and Bias from Global Memory to L1 Buffer.
2. L1 Buffer -> L0A Buffer/L0B Buffer: Call the copy interface with `copy_l1_to_l0a` and `copy_l1_to_l0b` `Operation` transfer capability to transfer `l0_batch_size` of matrix A and matrix B from L1 Buffer to L0 Buffer.
3. L1 Buffer -> BiasTable Buffer: Call the copy interface with `copy_l1_to_biastable` `Operation` transfer capability to transfer `l0_batch_size` of Bias from L1 Buffer to BiasTable Buffer.
4. mmad: Loop through matrices A, batch, and Bias on L0 Buffer `l0_batch_size` times, computing one batch group of A, batch matrix multiplication plus Bias each time and writing to L0C Buffer.
5. L0C Buffer -> Global Memory: Call the copy interface with `copy_l0c_to_gm` `Operation` transfer capability to transfer `l0_batch_size` result matrices C out to Global Memory at once.

### Batch Matmul Definition

Batch matrix multiplication (batch matmul) is an extension of standard matrix multiplication along the batch dimension. The core logic is: for a batch of data containing multiple matrices, perform standard matrix multiplication on each matrix in the batch individually, outputting the same number of result matrices.

In scenarios with Bias, if the input matrices A, B, and Bias have shapes `[B, m, k]`, `[B, k, n]`, and `[B, 1, n]` respectively, the output matrix C has shape `[B, m, n]`. For any batch `i`, take `A[i]` and `B[i]` for standard matrix multiplication, then add `Bias[i]` to produce `C[i]`.

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

The default batch quantity transferred from Global Memory to L1 Buffer for processing each time is `l1_batch_size=32`.
The default batch quantity transferred from L1 Buffer to L0A Buffer, L0B Buffer, L0C Buffer for computation each time is `l0_batch_size=4`.

### Tensor Construction

The format of matrices A, B, C in Global Memory is `ND`, the format of matrices A, B in L1 Buffer is `NZ`, the format of matrix A in L0 Buffer is `NZ`, the format of matrix B in L0B Buffer is `ZN`, the format of matrix C in L0C Buffer is `NZ`, and the format of Bias in Global Memory, L1 Buffer, and L0 Buffer is all `ND`.

```cpp
//Construct Layout through make_frame_layout, build Tensor through make_tensor.

auto gm_a = make_tensor(make_mem_ptr(a), make_frame_layout<nd_ext_layout_ptn>(batch, m, k));
auto gm_b = make_tensor(make_mem_ptr(b), make_frame_layout<nd_ext_layout_ptn>(batch, k, n));
auto gm_c = make_tensor(make_mem_ptr(c), make_frame_layout<nd_ext_layout_ptn>(batch, m, n));
auto gm_bias = make_tensor(make_mem_ptr(bias), make_frame_layout<nd_ext_layout_ptn>(batch, 1, n));

auto l1_a_tensor = make_tensor(make_mem_ptr(l1_a_buf), make_frame_layout<nz_layout_ptn, T>(l1_batch_size, m, k));
auto l1_b_tensor = make_tensor(make_mem_ptr(l1_b_buf), make_frame_layout<nz_layout_ptn, T>(l1_batch_size, k, n));
auto l1_bias_tensor = make_tensor(make_mem_ptr(l1_bias_buf), make_frame_layout<nd_ext_layout_ptn, T>(l1_batch_size, 1, n));
auto l0_a_tensor = make_tensor(make_mem_ptr(l0_a_buf), make_frame_layout<nz_layout_ptn, T>(l0_batch_size, m, k));
auto l0_b_tensor = make_tensor(make_mem_ptr(l0_b_buf), make_frame_layout<zn_layout_ptn, T>(l0_batch_size, k, n));
auto l0_c_tensor = make_tensor(make_mem_ptr(l0_c_buf), make_frame_layout<nz_layout_ptn>(l0_batch_size, m, n));
auto l0_bias_tensor = make_tensor(make_mem_ptr(l0_bias_buf), make_frame_layout<nd_ext_layout_ptn>(l0_batch_size, 1, n));
```

### Matrix Batch Transfer In

The example controls the batch quantity entering L1 Buffer from Global Memory each time through `l1_batch_size`, and controls the batch quantity transferred from L1 Buffer to L0A Buffer, L0B Buffer, L0C Buffer each time through `l0_batch_size`, thereby avoiding transferring all batch data at once which would exceed on-chip memory limits.

```cpp
for (uint32_t l1_batch_index = b_index_start; l1_batch_index < b_index_end; l1_batch_index += l1_batch_size) {
    uint32_t cur_l1_batch_size = min(l1_batch_size, b_index_end - l1_batch_index);
    copy(copy_gm2l1_atom, l1_a_tensor,
         gm_a.slice(make_coord(l1_batch_index, make_coord(0, 0)), make_shape(cur_l1_batch_size, make_shape(m, k))));
    copy(copy_gm2l1_atom, l1_b_tensor,
         gm_b.slice(make_coord(l1_batch_index, make_coord(0, 0)), make_shape(cur_l1_batch_size, make_shape(k, n))));
    copy(copy_gm2l1_atom, l1_bias_tensor,
         gm_bias.slice(make_coord(l1_batch_index, make_coord(0, 0)), make_shape(cur_l1_batch_size, make_shape(1, n))));

    for (uint32_t l0_batch_index = 0; l0_batch_index < cur_l1_batch_size; l0_batch_index += l0_batch_size) {
        uint32_t cur_l0_batch_size = min(l0_batch_size, cur_l1_batch_size - l0_batch_index);
        copy(copy_l12l0a_atom, l0_a_tensor,
             l1_a_tensor.slice(make_coord(l0_batch_index, make_coord(0, 0)), make_shape(cur_l0_batch_size, make_shape(m, k))));
        copy(copy_l12l0b_atom, l0_b_tensor,
             l1_b_tensor.slice(make_coord(l0_batch_index, make_coord(0, 0)), make_shape(cur_l0_batch_size, make_shape(k, n))));
        copy(copy_l12bt_atom, l0_bias_tensor,
             l1_bias_tensor.slice(make_coord(l0_batch_index, make_coord(0, 0)), make_shape(cur_l0_batch_size, make_shape(1, n))));
    }
}
```

### mmad Batch Computation

Each Batch's mmad operation corresponds to an independent Bias. Since the hardware mad instruction does not support batch data matrix multiply-add operations, mmad operations must be looped along the Batch dimension, with each iteration taking a single input matrix A, matrix B, and Bias as input operands for computation.

```cpp
for (uint32_t l0_c_batch_index = 0; l0_c_batch_index < cur_l0_batch_size; l0_c_batch_index++) {
    mmad(mmad_atom.with(mmad_params{static_cast<uint16_t>(m), static_cast<uint16_t>(n), static_cast<uint16_t>(k), unit_flag_mode::disable, true}),
        l0_c_tensor.slice(make_coord(l0_c_batch_index, make_coord(0, 0)), make_shape(1, make_shape(m, n))),
        l0_a_tensor.slice(make_coord(l0_c_batch_index, make_coord(0, 0)), make_shape(1, make_shape(m, k))),
        l0_b_tensor.slice(make_coord(l0_c_batch_index, make_coord(0, 0)), make_shape(1, make_shape(k, n))),
        l0_bias_tensor.slice(make_coord(l0_c_batch_index, make_coord(0, 0)), make_shape(1, make_shape(1, n))));
}
```

### Matrix Batch Transfer Out

After completing `l0_batch_size` quantity of matrix multiply-add computations, call the copy interface with `copy_l0c_to_gm` `Operation` data transfer capability to transfer result matrices C from L0C Buffer to Global Memory.
```cpp
copy(copy_l0c2gm_atom,
     gm_c.slice(make_coord(l1_batch_index + l0_batch_index, make_coord(0, 0)),
               make_shape(cur_l0_batch_size, make_shape(m, n))),
     l0_c_tensor);
```

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables
  Configure environment variables according to the [installation method](../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.

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
  | `CMAKE_ASC_ARCHITECTURES` |`dav-3510` | NPU architecture: dav-3510 corresponds to Ascend 950PR&950DT products |

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
