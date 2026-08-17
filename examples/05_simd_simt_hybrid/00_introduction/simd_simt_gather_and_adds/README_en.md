# SIMD and SIMT Hybrid Programming for Gather and Adds Computation

## Overview
This sample implements gather and adds computation based on SIMD and SIMT hybrid programming mode. It uses SIMT programming to implement discrete memory access operation gather, and SIMD programming to implement continuous memory access operation adds.


## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure

```text
├── simd_simt_gather_and_adds
│   ├── scripts
│   │   ├── gen_data.py         // Input data and golden data generation script
│   │   └── verify_result.py    // Golden data comparison file
│   ├── CMakeLists.txt          // cmake build file
│   ├── gather_and_adds.asc     // Ascend C sample implementation & call sample
│   ├── data_utils.h            // Data read and write functions
│   └── README.md
```

## Sample Description

- Sample Function:  
  Calculation Formula:

  ```
  output[i] = input[index[i]] + 1
  ```

- Sample Specifications:
  <table>
  <tr><td rowspan="1" align="center">Sample Type(OpType)</td><td colspan="4" align="center">gather & adds</td></tr>
  <tr><td rowspan="3" align="center">Sample Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[100000]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">index</td><td align="center">[8192]</td><td align="center">uint32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Sample Output</td><td align="center">output</td><td align="center">[8192]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">gather_and_adds_kernel</td></tr>
  </table>

- Sample Implementation:  
  The SIMT unit and SIMD unit in the Vector Core share on-chip storage, which can be used to complete SIMT and SIMD hybrid programming. In this sample, the shape of the sample input index is [8192], the number of cores can be set to 8, each core processes 1024 data elements, the number of threads THREAD_COUNT is set to 1024, each thread processes 1 data element, and a single core only needs to call the simt_gather function once to complete the gather operation.

  > ⚠️ **Note** When the amount of data processed by a single core is greater than the set number of threads, the data needs to be split into multiple thread blocks. You can use asc_vf_call to call the simt_gather function multiple times to start multiple thread blocks to complete the operation of obtaining specified index data.

  Based on the above data partitioning, in the simd_adds function, the operation of adding 1 to 1024 data elements is performed.

  > ⚠️ **Note** The addition operation in simd_adds can actually be implemented directly in the simt_gather function quickly. The purpose of this sample is only to demonstrate the hybrid programming approach of SIMT and SIMD programming modes through a simple use case, which is not the best practice for this sample.

  The implementation flow of the gather & adds sample is mainly divided into 3 steps: simt_gather, simd_adds, and copying data from UB to GM.

  (1) simt_gather obtains data with specified indexes from GM (Global Memory) input.
  ```
  uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
  ...
  uint32_t gather_idx = index[idx];
  ...
  local_output[threadIdx.x] = input[gather_idx];
  ```

  (2) simd_adds performs the add 1 operation on the data in UB (Unified Buffer). Call asc_loadalign to move data from UB (Unified Buffer) to the register, call asc_add_scalar to complete the add 1 operation and output to the destination register, and finally call asc_storealign to move data from the register to UB. Repeat the above operation to complete the add 1 operation on 1024 data elements.
  ```
  for (uint16_t i = 0; i < repeat_times; i++) {
      mask_reg = asc_update_mask_b32(count);
      asc_loadalign(src_reg0, input + i * one_repeat_size);
      asc_add_scalar(dst_reg0, src_reg0, ADDS_ADDEND, mask_reg);
      asc_storealign(output + i * one_repeat_size, dst_reg0, mask_reg);
  }
  ```

  (3) Copy the output data from UB (Unified Buffer) to GM (Global Memory).

- Call Implementation:  
  Use the kernel call operator <<<>>> to call the kernel function.

## Compilation and Execution

Execute the following steps in the root directory of this sample to compile and execute the sample.
- Configure Environment Variables  
  Configure environment variables according to the [installation method](../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit package on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When the installation directory is not specified, it is installed to `/usr/local/Ascend` by default.

- Sample Execution

  Execute the following command in this sample directory.
  ```bash
  mkdir -p build && cd build;                                               # Create and enter build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # Compile project (default npu mode)
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                    # Execute sample
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify output result correctness
  ```

  When using NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Note:** Before switching compilation modes, you need to clear the cmake cache. You can execute `rm CMakeCache.txt` in the build directory and then run cmake again.

- Compilation Options Description

  | Option | Available Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: Ascend 950PR/Ascend 950DT |

- Execution Result  
  The execution result is as follows, indicating that the precision comparison is successful.
  ```
  test pass!
  ```
