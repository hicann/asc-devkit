# SIMT Programming Model Gather Operator Example

## Overview

This example implements a generalized shape Gather operator based on the Ascend C SIMT programming model, including the basic gather and enhanced gather_v2. The gather operator collects specified rows from a two-dimensional input tensor, while the gather_v2 operator supports collecting data along a specified dimension from multi-dimensional input tensors and supports batch_dims batch processing mode. This example demonstrates the development method for operators with discrete memory access in generalized scenarios.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Versions

- \>= CANN 9.0.0

## Directory Structure

```text
├── general_gather
│   ├── CMakeLists.txt         // cmake build file
│   ├── gather_v2.asc          // SIMT gather_v2 call example
│   ├── gather.asc             // SIMT gather call example
│   └── README.md
```

## Operator Description

### 1. gather Operator

- Operator Function:   
  The gather operator retrieves m rows of data at specified indices from a two-dimensional input tensor with shape [M,N]. The row indices for these m rows are specified by the input index. The calculation formula for the i-th row of the operator output is:

  ```text
  output[i] = input[index[i]]
  ```

- Operator Specification:
  <table>
  <tr><td rowspan="1" align="center">Operator Type(OpType)</td><td colspan="4" align="center">gather</td></tr>
  <tr><td rowspan="3" align="center">Operator Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[M,N]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">index</td><td align="center">[m] (m < M, m <= 65535 * 2048)</td><td align="center">uint32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Operator Output</td><td align="center">output</td><td align="center">[m,N]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">gather_custom</td></tr>
  </table>

- Data Partitioning:
  * gridDim: Dynamically allocated based on the specific input shape, with a maximum of 65535
  * blockDim: Dynamically allocated based on the specific input shape, with a maximum of 2048
  * Single thread processing: 1 row
  * Maximum processing capacity: 65535 * 2048 = 134215680 rows

- Operator Implementation:   
  The gather operator retrieves data at specified indices from the input (Global Memory). Based on the data partitioning described above, the implementation first calculates the data index that each thread should process, then stores one row of data to Global Memory through an assignment operation. Since the computation process is relatively simple, the kernel function limits the maximum number of threads to 2048.

- Invocation Implementation:   
  The kernel function is invoked using the kernel call operator `<<<>>>`.

### 2. gather_v2 Operator

- Operator Function:   
  The gather_v2 operator collects data along a specified dimension axis from a multi-dimensional input tensor. The indices tensor specifies the index positions to collect. It supports batch_dims batch processing mode, allowing different batches to use different index sets.
- Processing Flow:   
  For example, if the input tensor has shape [2,2,3,2] and the indices tensor has shape [2,2]:

  ```text
  input:
   [[[[ 1,  2],
      [ 3,  4],
      [ 5,  6]],

     [[ 7,  8],
      [ 9, 10],
      [11, 12]]],


    [[[13, 14],
      [15, 16],
      [17, 18]],

     [[19, 20],
      [21, 22],
      [23, 24]]]]

  indices:
   [[1, 2],
    [0, 1]]
  ```

  axis=2, batch_dims=1 indicates that the collection dimension is 2, and each batch uses different indices:
  - batch=0: output[0, :, :, :] = input[0, :, [1, 2], :], that is, collect slices corresponding to indices[0] on dimension 2 of input[0]
  - batch=1: output[1, :, :, :] = input[1, :, [0, 1], :], that is, collect slices corresponding to indices[1] on dimension 2 of input[1]

  ```text
  output:
   [[[[ 3,  4],
      [ 5,  6]],

     [[ 9, 10],
      [11, 12]]],

    [[[13, 14],
      [15, 16]],

     [[19, 20],
      [21, 22]]]]
  ```

- Operator Specification:
  <table>
  <tr><td rowspan="1" align="center">Operator Type(OpType)</td><td colspan="5" align="center">gather_v2</td></tr>
  <tr><td rowspan="5" align="center">Operator Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">description</td></tr>
  <tr><td align="center">input</td><td align="center">[128,20000,512]</td><td align="center">float</td><td align="center">ND</td><td align="center">multi-dimensional input tensor</td></tr>
  <tr><td align="center">indices</td><td align="center">[128,2048]</td><td align="center">uint32_t / int32_t</td><td align="center">ND</td><td align="center">index tensor, specifies collection positions</td></tr>
  <tr><td align="center">axis</td><td align="center">-</td><td align="center">int32_t</td><td align="center">-</td><td align="center">scalar, specifies the collection dimension</td></tr>
  <tr><td align="center">batch_dims</td><td align="center">-</td><td align="center">int32_t</td><td align="center">-</td><td align="center">scalar, specifies the batch processing dimension</td></tr>
  <tr><td rowspan="1" align="center">Operator Output</td><td align="center">output</td><td align="center">[128,2048,512]</td><td align="center">float</td><td align="center">ND</td><td align="center">output tensor after collection</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="5" align="center">gather_custom_v2</td></tr>
  </table>
- Constraint Description:
  * indices: The first batch_dims dimensions of indices must match the first batch_dims dimensions of input, that is, indices.shape[0:batch_dims] = input.shape[0:batch_dims]
  * axis: The collection dimension axis cannot be less than batch_dims and must be less than the number of dimensions of input, that is, batch_dims <= axis < input.rank
  * batch_dims: The number of batch dimensions cannot exceed the smaller dimension count of input and indices, that is, 0 <= batch_dims <= min(input.rank, indices.rank)
  * output.shape: input.shape[:axis] + indices.shape[batch_dims:] + input.shape[axis+1:]
  * In the example implementation, axis and batch_dims also support negative values and are converted to corresponding non-negative dimension indices before calculation
- Data Partitioning:
  * gridDim: Dynamically allocated based on the total amount of data to collect. It prioritizes calculating the required number of blocks based on blockDim; when the required number of blocks exceeds the device AIV core count, it uses the device AIV core count as the block count
  * blockDim: Dynamically allocated based on the specific total data amount, with a maximum of 2048
  * Single thread processing: Balanced allocation based on the specific data collection amount, with each thread processing a maximum difference of 1 element. In the kernel function, the total thread count serves as the loop stride, and each thread starts from the starting position begin = blockIdx.x * blockDim.x + threadIdx.x, traverses and processes elements with a stride of stride = gridDim.x * blockDim.x

  The advantages of this partitioning approach are:
  * Load balancing: The workload difference among all threads is at most 1 element, avoiding thread idling
  * Memory access friendly: Adjacent threads access consecutive memory addresses, facilitating coalesced memory access
- Operator Implementation:   
  The gather_v2 operator collects data along a specified dimension axis from a multi-dimensional input tensor. Based on the data partitioning strategy described above, each thread dynamically processes a portion of data. For each output index, it decomposes the one-dimensional output index into logical coordinates, finds the corresponding collection position based on indices, and finally calculates the linear index of the input and completes the data transfer.
- Invocation Implementation:   
  The kernel function is invoked using the kernel call operator `<<<>>>`.

## Compilation and Execution

Execute the following steps in the root directory of this example to compile and run the operator.

- Configure Environment Variables  
  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. If no installation directory is specified, it defaults to `/usr/local/Ascend`.

- Example Execution

  Execute the following commands in this example directory.

  ```bash
  mkdir -p build && cd build;   # Create and enter build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # Compile the project
  ./gather                      # Execute the example
  ./gather_v2                   # Execute the example
  ```

  Compilation Option Description

  | Option | Available Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU Architecture: This example only supports dav-3510 (Ascend 950PR/Ascend 950DT) |

  The execution result is as follows, indicating that the accuracy comparison succeeds:

  ```text
  [Success] Case accuracy is verification passed.
  ```