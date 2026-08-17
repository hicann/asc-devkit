# Inter-Core Sequential Synchronization Example

## Overview

This example implements inter-core sequential synchronization in deterministic computation scenarios based on three interfaces: InitDetermineComputeWorkspace, WaitPreBlock, and NotifyNextBlock. **These three interfaces must be used together** to ensure that multiple AIV cores execute strictly in ascending order of blockIdx, which is applicable to scenarios requiring deterministic computation. This example simulates 8 cores performing data processing, using deterministic computation interfaces to guarantee inter-core execution order and performing atomic accumulation to ensure deterministic computation results.

> **Notice:** This example is only applicable to the TPipe and TQue programming model.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── sequential_block_sync
│   ├── scripts
│   │   ├── gen_data.py                // Script for generating input data and ground truth data
│   │   └── verify_result.py           // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt                 // Build project file
│   ├── data_utils.h                   // Data read/write functions
│   ├── sequential_block_sync.asc      // Ascend C example implementation & invocation example
│   └── README.md                      // Example documentation
```

## Example Description

This example uses 8 cores working collaboratively, with each core processing 256 float data elements. InitDetermineComputeWorkspace is used to initialize the synchronization state of GM shared memory, and then WaitPreBlock and NotifyNextBlock ensure that cores execute in ascending order of blockIdx. Each core splits the input data into two tiles (128 elements each) and writes them to the output buffer through atomic accumulation, guaranteeing deterministic computation results.

### Example Specifications

<table>
<caption>Table 1: Example Input/Output Specifications</caption>
<tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[256]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">workspace</td><td align="center">[256] Bytes</td><td align="center">uint8_t</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[256]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="5" align="center">kernel_sequential_block_sync</td></tr>
<tr><td rowspan="1" align="center">Number of Cores</td><td colspan="5" align="center">8</td></tr>
</table>

### Computation Flow

1. **Initialization phase**: Call InitDetermineComputeWorkspace to initialize the synchronization state of GM shared memory
2. **Data transfer-in**: Transfer 256 elements from GM to UB, split into two tiles (128 elements each)
3. **Inter-core synchronization**: Wait for the preceding core to complete through WaitPreBlock
4. **Atomic accumulation**: Enable SetAtomicAdd and atomically accumulate data into GM
5. **Notify subsequent core**: Notify the subsequent core that it can start execution through NotifyNextBlock


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
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j;    # Build the project
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin  # Verify whether the output result is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # CPU debug mode
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
