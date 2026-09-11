# Atomic Intrinsics Example

## Overview

This example demonstrates the usage of Ascend C SIMT atomic operation interfaces.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Versions

- \>= CANN 9.1.0

## Directory Structure

```text
├── atomic_intrinsics
│   ├── CMakeLists.txt         // Build project file
│   └── atomic_intrinsics.asc  // Ascend C SIMT kernel implementation & host invocation sample
```

## Example Description

- Functionality: Invoke atomic operation interfaces provided by Ascend C SIMT in a kernel to perform atomic addition, subtraction, exchange, compare-and-swap, increment, decrement, bitwise AND, bitwise OR, and bitwise XOR on 11 elements of the `output_data` array, and the results are stored in the 11 elements of the `output_data` array respectively.

- Computation process:

  - The kernel launches `64` thread blocks, each with `256` threads, totaling `16384` threads
  - Each thread performs one atomic operation on each of the 11 elements in the `output_data` array, indexed by the global thread ID `tid`
  - After the kernel completes, the host copies the results back and compares them element-wise with the CPU reference implementation

- Example specifications:

  <table>
  <tr><td rowspan="1" align="center">OpType</td><td colspan="4" align="center">Atomic Intrinsics</td></tr>
  <tr><td rowspan="2" align="center">Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">output_data</td><td align="center">[11]</td><td align="center">int32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Output</td><td align="center">output_data</td><td align="center">[11]</td><td align="center">int32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Name</td><td colspan="4" align="center">atomic_intrinsics_kernel</td></tr>
  </table>

## Example Implementation

In the kernel `atomic_intrinsics_kernel`, each thread performs one atomic operation on each of the 11 elements in the `output_data` array. Different array indices correspond to different atomic operations, as shown in the following table:

| Array Index | Interface |
| :--: | :-- |
| 0 | asc_atomic_add |
| 1 | asc_atomic_sub |
| 2 | asc_atomic_exch |
| 3 | asc_atomic_max |
| 4 | asc_atomic_min |
| 5 | asc_atomic_inc |
| 6 | asc_atomic_dec |
| 7 | asc_atomic_cas |
| 8 | asc_atomic_and |
| 9 | asc_atomic_or |
| 10 | asc_atomic_xor |

Key code:

```cpp
__global__ void atomic_intrinsics_kernel(int* output_data)
{
    const unsigned int tid = blockIdx.x * blockDim.x + threadIdx.x;

    asc_atomic_add(&output_data[0], 5);
    asc_atomic_sub(&output_data[1], 5);
    asc_atomic_exch(&output_data[2], tid);
    asc_atomic_max(&output_data[3], tid);
    asc_atomic_min(&output_data[4], tid);
    asc_atomic_inc((unsigned int*)&output_data[5], 11u);
    asc_atomic_dec((unsigned int*)&output_data[6], 100u);
    asc_atomic_cas(&output_data[7], tid - 1, tid);
    asc_atomic_and(&output_data[8], 2 * tid + 1);
    asc_atomic_or(&output_data[9], 1u << (tid & 31u));
    asc_atomic_xor(&output_data[10], tid);
}
```


## Build and Run

Run the following steps in the root directory of this example to build and execute the example.

- Configure Environment Variables
   Configure environment variables based on the [installation method](../../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Example
  Run the following commands in this example directory.
  ```bash
  mkdir build && cd build;                              # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;  # Build the project
  ./atomic_intrinsics                                   # Run the example
  ```

  When using NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # NPU simulation mode
  ```

  > **Note:** Clear the cmake cache before switching build modes. Execute `rm CMakeCache.txt` in the build directory, then run cmake again.

- Build Options Description

  | Option | Values | Description |
  | ------------------------- | ---------- | ---------------------------------------------------------- |
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: this example only supports dav-3510 (Ascend 950PR/Ascend 950DT) |

  The following output indicates that the atomic operation verification is successful.

  ```text
  Atomic Intrinsics Example starting...
  Atomic Intrinsics Example completed, returned OK
  ```
