# SIMT Kernel Log Function Example

## Overview

This example demonstrates the complete usage of the Ascend log function in SIMT programming mode, including log screen output, log file storage, and log level control. By configuring environment variables, developers can flexibly control log output behavior to assist with operator development and issue diagnosis.

For more log function details, refer to: [Ascend Log Function Reference](https://hiascend.com/document/redirect/CannCommunitylogref)

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Version

- \>= CANN 9.1.0

## Directory Structure

```
├── 09_log
│   ├── CMakeLists.txt         // CMake build file
│   ├── log.asc                // SIMT Kernel example code
│   └── README.md
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
  This example demonstrates the complete flow of Kernel-side exception throwing and Host-side error catching by constructing an exception scenario:

  - Kernel Implementation  
    Use standard C `assert` assertions in the Kernel function. When the condition is not met, a device-side exception is triggered. To avoid all threads printing repeatedly, typically only `thread 0` triggers the assertion.

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
    // Catch Kernel execution errors (including errors triggered by assert)
    ASCENDC_CHECK(aclrtGetLastError(ACL_RT_THREAD_LEVEL));
    ASCENDC_CHECK(aclrtSynchronizeDevice());
    ```

## Build and Run

Run the following steps in the root directory of this example to build and execute the operator.

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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # Build the project (default npu mode)
  ./demo
  ```

  When using NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Before switching build modes, clean the cmake cache by running `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build Options Description
  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: this example only supports dav-3510 (Ascend 950PR/Ascend 950DT) |

- Execution Result (using non-screen-output mode as an example; for file storage details, refer to [Ascend Log Function Reference](https://hiascend.com/document/redirect/CannCommunitylogref))  
  After execution, the following information is printed:
  ```bash
  [INFO] Input shape: 12288
  [INFO] Launching kernel with assert(total_length < 100)...
  Ascend Error: <your_path>/09_log/log.asc:88 code=507035 EZ9999: Inner Error!
  ...
  rtDeviceSynchronize execution failed, reason=vector core exception
  ...

  [ASSERT] <your_path>/09_log/log.asc:47: void add_custom(float *, float *, float *, uint64_t): Assertion `total_length < 100 && "Total length exceeds expected limit!"' failed.
  [INFO] Execution completed. Check for error messages above.
  ```

  > **Note:**
  > - `<your_path>` represents the absolute path of the example code directory, which is replaced with the actual path in the output
  > - Key error message interpretation:
  >   - `Ascend Error: ... code=507035 EZ9999: Inner Error!` -- Low-level error code returned by the ACL interface
  >   - `vector core exception` -- AI Core execution exception, Kernel task failed
  >   - `Assertion 'total_length < 100 ...' failed` -- Specific location and condition of the assertion failure, helping locate the problem code

  At the same time, log files are generated in the `./log` directory, containing detailed run logs.
