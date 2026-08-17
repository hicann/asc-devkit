# Scalar Atomic Operations Example

## Overview

This example introduces the implementation flow of scalar atomic addition and scalar atomic compare-and-swap on Global Memory (GM) addresses, based on the `AtomicAdd` and `AtomicCas` APIs. Atomic operations ensure data consistency when multiple cores access the same memory address in parallel, avoiding data race issues. Note that atomic operations involve the scalar computation unit. If there is a data dependency with the transfer unit (MTE2/MTE3), synchronization events must be manually inserted.

> **API tip:** In addition to the `AtomicAdd` and `AtomicCas` APIs used in this example, Ascend C also provides `AtomicExch`, `AtomicMax`, and `AtomicMin` APIs. The calling method is the same as `AtomicAdd`; simply replace the function name to switch.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```
├── scalar_atomic_operations
│   ├── scripts
│   │   ├── gen_data.py               // Input data and ground truth generation script
│   │   └── verify_result.py          // Verification script to check output data against ground truth
│   ├── CMakeLists.txt                // Build configuration file
│   ├── data_utils.h                  // Data read/write functions
│   ├── scalar_atomic_operations.asc  // Ascend C example implementation & invocation example
│   └── README.md                     // Example description document
```

## Detailed Scenario Description

This example switches between different scenarios through the build parameter `SCENARIO_NUM`:

**Scenario 1: AtomicAdd Atomic Addition**
  Three cores execute in parallel, each atomically adding 1 to the first element of GM (dst[0]). The final value of dst[0] is the initial value plus 3.

**Scenario 2: AtomicCas Atomic Compare-and-Swap**
  Three cores execute in parallel, each checking whether the first element of GM equals the expected value 1. If equal, it is replaced with newValue 2; if not equal, no modification is made.

<table border="2">
<caption>Table 1: Scenario Configuration Comparison</caption>
<tr><th>scenarioNum</th><th>Atomic Operation API</th><th>Output Shape</th><th>Data Type</th><th>Description</th></tr>
<tr><td>1</td><td>AtomicAdd</td><td>[1, 256]</td><td>int32</td><td>Three cores perform atomic addition of 1 on the first GM element in parallel</td></tr>
<tr><td>2</td><td>AtomicCas</td><td>[1, 256]</td><td>uint32</td><td>Three cores perform atomic compare-and-swap on the first GM element in parallel (if value is 1, replace with 2)</td></tr>
</table>

## Build and Run

Run the following steps in the root directory of this example to build and run it.
- Configure environment variables
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  SCENARIO_NUM=1  # Set the scenario number (values: 1, 2)
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # Build the project
  python3 ../scripts/gen_data.py -scenarioNum $SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin -scenarioNum $SCENARIO_NUM  # Verify whether the output result is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # CPU debug mode
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` (default) | NPU architecture: AtomicAdd and AtomicCas only support dav-3510 (corresponding to Ascend 950PR/Ascend 950DT) |
  | `SCENARIO_NUM` | `1` (default), `2` | Scenario number: 1 (AtomicAdd atomic addition), 2 (AtomicCas atomic compare-and-swap) |

- Execution result

  The following execution result indicates that the precision comparison is successful.

  ```bash
  test pass!
  ```
