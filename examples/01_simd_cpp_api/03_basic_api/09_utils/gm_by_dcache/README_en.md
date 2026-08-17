# gm_by_dcache Example

## Overview

This example introduces reading and writing data from GM addresses through DCache (Data Cache).

The Scalar unit accesses Global Memory by first accessing the Data Cache within each core, which creates Cache consistency issues between Data Cache and Global Memory. Use the DataCacheCleanAndInvalid interface to refresh the Cache to ensure Cache consistency.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── gm_by_dcache
│   ├── CMakeLists.txt      // Build configuration file
│   ├── gm_by_dcache.asc    // Ascend C example implementation & invocation example
│   └── README.md           // Example documentation
```

## Example Description

- Example functionality:
  Implements ReduceSum operation on an input matrix using multiple cores, writing the sum of all data to the output matrix. Uses workspace to save intermediate results computed by each core, as well as the computation completion status of each core.

- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">GmByDcache</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[4, 1024]</td><td align="center">float</td></tr>
  <tr><td align="center">workspace</td><td align="center">[1, 128]</td><td align="center">int32_t</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[1, 1]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">gm_by_dcache_custom</td></tr>
  </table>

- Example implementation:
  - Kernel implementation
    This example uses 4 cores to complete the computation, with each core processing the sum of 1024 elements in the corresponding row.

    **Step 1: Each core performs reduction computation separately**

    - Move input data from GM (Global Memory) (corresponding row of 1024 elements) to UB (Unified Buffer)
    - Call `ReduceRepeat<AscendC::ReduceType::SUM>` interface, reducing every 256B data to one sum element, resulting in 16 floats total
    - Move the computation result to workspace on GM

    **Step 2: Each core writes a flag**

    - Write a flag to workspace on GM through the Scalar unit, indicating that the core's computation is complete
    - Call DataCacheCleanAndInvalid interface to immediately write out existing data in Data Cache

    **Step 3: Core 0 reads flags and completes the final reduction computation**

    - All cores except core 0 return and stop running
    - Core 0 calls DataCacheCleanAndInvalid interface to get the latest data
    - Read all cores' flags through the Scalar unit sequentially, checking whether other cores have completed computation
    - If all other cores' flags meet expectations, call `ReduceRepeat<AscendC::ReduceType::SUM>` interface again to complete the final reduction computation

  - Invocation implementation
    Use the kernel invocation syntax <<<>>> to call the kernel function, starting 4 cores for parallel execution.

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
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Build the project, default npu mode
  ./demo                           # Run the example
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```