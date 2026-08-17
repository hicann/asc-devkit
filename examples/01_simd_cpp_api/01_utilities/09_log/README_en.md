# Ascend Log Function Example

## Overview

This example demonstrates the complete usage of the Ascend log function in SIMD programming mode, including log screen output, log file storage, and log level control. By configuring environment variables, developers can flexibly control log output behavior to assist with example development and issue diagnosis.

For more log function details, refer to: [Ascend Log Function Reference](https://hiascend.com/document/redirect/CannCommunitylogref)

## Supported Products and CANN Software Version

| Product | CANN Software Version |
|---------|-----------------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 training series products/Atlas A3 inference series products | >= CANN 9.0.0 |
| Atlas A2 training series products/Atlas A2 inference series products | >= CANN 9.0.0 |

## Directory Structure

```
├── 09_log
│   ├── CMakeLists.txt          // CMake build file
│   ├── data_utils.h            // Data read and write functions
│   ├── log.asc                 // Ascend C Kernel example code
│   ├── README.md
│   └── scripts
│       ├── gen_data.py         // Input data and golden data generation script
│       └── verify_result.py    // Verification script for comparing output data with golden data
```

## Example Description

- Log Function Description:
  - **Log Screen Output**: Control whether logs are output to standard output (screen) through environment variables for real-time viewing
  - **Log File Storage**: Specify log file storage path through environment variables for subsequent analysis
  - **Log Level**: Control log output level through environment variables to filter log content as needed

  Log Level Description:

  | Value | Level | Description |
  |----|------|------|
  | 0 | DEBUG | Output all logs (DEBUG/INFO/WARNING/ERROR), most detailed information |
  | 1 | INFO | Output INFO and above level logs (INFO/WARNING/ERROR) |
  | 2 | WARNING | Output WARNING and above level logs (WARNING/ERROR) |
  | 3 | ERROR | Output only ERROR level logs |
  | 4 | NULL | Do not output logs |

- Example Implementation:
  This example implements matrix multiplication (Matmul) using the static tensor programming mode. By constructing an exception scenario, it demonstrates the complete flow of Kernel-side exception throwing and Host-side error catching:

  - Kernel Implementation
    Use standard C `assert` assertions in the Kernel function. When the condition is not met, a device-side exception is triggered.

  - Host Implementation
    After Kernel execution, the Host side uses the `ASCENDC_CHECK` macro to catch and print error information. `ASCENDC_CHECK` is a utility macro for catching and printing ACL errors, defined as follows:

    ```c
    #define ASCENDC_CHECK(expr) do { \
        aclError ret = (expr); \
        if (ret != ACL_SUCCESS) { \
            fprintf(stderr, "Ascend Error: %s:%d code=%d %s\n", \
                __FILE__, __LINE__, ret, aclGetRecentErrMsg()); \
        } \
    } while(0)
    ```

    Pass ACL interface calls as parameters. The macro automatically checks the return value and prints error information on failure.

    ```c
    ASCENDC_CHECK(aclrtGetLastError(ACL_RT_THREAD_LEVEL));
    ASCENDC_CHECK(aclrtSynchronizeDevice());
    ```

## Build and Run

Run the following steps in the root directory of this example to build and execute the example.

- Configure Environment Variables
  Configure environment variables based on the [installation method](../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Configure Log Environment Variables (NPU Mode Only)
  This example supports controlling log output behavior through environment variables (the following environment variables only take effect in NPU mode):
  ```bash
  export ASCEND_PROCESS_LOG_PATH=./log        # Log file storage path
  export ASCEND_SLOG_PRINT_TO_STDOUT=1        # Control whether logs are printed to screen (1: enabled, 0: disabled)
  export ASCEND_GLOBAL_LOG_LEVEL=1            # Control log level
  ```

  > Notice:
  > - Log environment variables control the log output of Ascend internal libraries (RUNTIME/ASCENDCL, and so on). `printf` output inside the Kernel is not affected by these environment variables.
  > - Log screen output and file storage are mutually exclusive. When screen output is enabled (`ASCEND_SLOG_PRINT_TO_STDOUT=1`), even if `ASCEND_PROCESS_LOG_PATH` is configured, logs will not be stored to files.
  >   - In screen output mode, logs can be saved through shell redirection: `./demo > err.log 2>&1`
  >   - In file storage mode, disable screen output and configure `ASCEND_PROCESS_LOG_PATH`; logs will be written to the specified directory

- Run the Example

  Run the following commands in this example directory.

  ```bash
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # Build the project (default npu mode)
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo
  ```

  When using CPU debugging or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debugging mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Before switching build modes, clean the cmake cache by running `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build Options Description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debugging, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture:<br>dav-2201 corresponds to Atlas A2 training series products/Atlas A2 inference series products and Atlas A3 training series products/Atlas A3 inference series products<br>dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution Result (NPU mode, using non-screen-output mode as an example; for file storage details, refer to [Ascend Log Function Reference](https://hiascend.com/document/redirect/CannCommunitylogref))
  After execution, the following information is printed:
  ```bash
  [INFO] Matrix shape: M=256, K=64, N=256
  [INFO] Launching kernel with assert(M < 100)...
  Ascend Error: <your_path>/09_log/log.asc:<line> code=507015 EZ9999: Inner Error!
  ...
  rtDeviceSynchronize execution failed, reason=aicore exception
  ...

  [ASSERT] <your_path>/09_log/log.asc:<line>: : Assertion `M < 100 && "M exceeds expected limit!"' failed.
  [INFO] Execution completed. Check for error messages above.
  ```

  > **Note:**
  > - `<your_path>` represents the absolute path of the example code directory, which is replaced with the actual path in the output
  > - `<line>` represents the line number of the assertion or error catch, which is replaced with the actual number in the output
  > - Key error message interpretation:
  >   - `Ascend Error: ... code=507015 EZ9999: Inner Error!` -- Low-level error code returned by the ACL interface
  >   - `aicore exception` -- AI Core execution exception, Kernel task failed
  >   - `Assertion 'M < 100 ...' failed` -- Specific location and condition of the assertion failure, helping locate the problem code
