# ms_sanitizer Example Description

## Overview

msSanitizer is an anomaly detection tool based on the Ascend AI processor. It detects memory access violations, memory leaks, unused memory, race conditions, and uninitialized access during Ascend C example execution. This example uses Add computation as the carrier to demonstrate how to run examples through the msSanitizer tool and locate anomalies.

The code in this example is a correct implementation. Users can modify `ms_sanitizer.asc` according to the instructions in the "Anomaly Scenario Detection" section, rebuild, and experience the corresponding anomaly detection capabilities through the msSanitizer tool.

Refer to the "Environment Preparation" section in [Operator Development Tools](https://www.hiascend.com/document/redirect/CannCommercialToolOpDev) for detailed installation instructions and steps.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|----------------------|
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── ms_sanitizer
│   ├── CMakeLists.txt       // Build project file
│   ├── ms_sanitizer.asc     // Ascend C example implementation and host-side invocation example
│   └── README.md            // Example description
```

## Example Description

This example uses `float` type by default to perform element-wise addition of two tensors:

```text
z = x + y
```

The shapes of inputs `x`, `y` and output `z` are all `[8, 2048]`. The host side constructs input data and golden data directly, copies the input data to the device side, launches the `add_custom` Kernel, which splits data by `blockNum = 8` and performs Add computation, and finally copies the result back to the host side for accuracy comparison.

## Anomaly Scenario Detection

When reproducing anomaly scenarios, modify the code in `ms_sanitizer.asc` according to the corresponding instructions first, then rebuild and run the corresponding detection command in the `build` directory. If only the source code is modified without rebuilding, `mssanitizer` still runs the old `demo` and does not detect newly introduced errors.

- **Memory Detection**

  - Illegal read/write: Anomaly caused by accessing unallocated memory.

    Users can comment out the correct `DataCopy` and use the incorrect `DataCopy` to reproduce this scenario. `xLocal` is allocated with size `blockLength`, but the transfer length is incorrectly written as `blockLength * 2`, which exceeds the allocated size of `xLocal`, thus triggering an illegal read/write.

    ```cpp
    // 1. Correct DataCopy.
    AscendC::DataCopy(xLocal, xGm, blockLength);
    // 2. Incorrect example: Transfer length written as blockLength * 2 triggers illegal read/write.
    // AscendC::DataCopy(xLocal, xGm, blockLength * 2);
    ```

    Rebuild and run detection:

    ```bash
    make -B demo
    mssanitizer ./demo
    ```

  - Unaligned access: Memory access does not meet byte alignment requirements.

    Users can comment out the correct `DataCopy` and use the incorrect `DataCopy` to reproduce this scenario. During `DataCopy` transfer from GM to UB, the UB-side address must meet 32-byte alignment requirements. `xLocal[5]` does not meet the 32-byte alignment requirement, thus triggering unaligned access.

    ```cpp
    // 1. Correct DataCopy.
    AscendC::DataCopy(xLocal, xGm, blockLength);
    // 3. Incorrect example: xLocal[5] does not meet 32-byte alignment, triggering unaligned access.
    // AscendC::DataCopy(xLocal[5], xGm, blockLength);
    ```

    Rebuild and run detection:

    ```bash
    make -B demo
    mssanitizer ./demo
    ```

  - Memory leak: Allocated memory is used but not freed, causing memory usage to increase continuously during program execution.

    Users can comment out `aclrtFree(zDevice)` to reproduce this scenario. Before commenting out, `zDevice` is freed normally. After commenting out, `zDevice` is not freed after use, thus triggering a memory leak.

    Notice: Pass `--leak-check=yes` when calling `mssanitizer` to enable memory leak detection.

    ```cpp
    // Correctly free device-side output memory. Remove this line to trigger memory leak detection.
    aclrtFree(zDevice);
    ```

    Rebuild and run detection:

    ```bash
    make -B demo
    mssanitizer ./demo --leak-check=yes
    ```

  - Unused allocated memory: Anomaly caused by allocating memory without using it.

    Users can comment out the correct `aclrtMalloc` and use the incorrect `aclrtMalloc` to reproduce this scenario. `xDevice` actually only uses `totalByteSize` bytes, but the incorrect example allocates 5 times the size, where the excess portion is unused, thus triggering unused allocated memory detection.

    Notice: Pass `--check-unused-memory=yes` when calling `mssanitizer` to enable unused allocated memory detection.

    ```cpp
    // 1. Correct device-side input memory allocation.
    aclrtMalloc((void**)&xDevice, totalByteSize, ACL_MEM_MALLOC_HUGE_FIRST);
    // 2. Incorrect example: Only totalByteSize is used, but 5 times the size is allocated, triggering unused memory detection.
    // aclrtMalloc((void**)&xDevice, totalByteSize * 5, ACL_MEM_MALLOC_HUGE_FIRST);
    ```

    Rebuild and run detection:

    ```bash
    make -B demo
    mssanitizer ./demo --check-unused-memory=yes
    ```

- **Race Condition Detection**

  Race condition detection is used to locate memory access race issues in parallel execution. Users can comment out the `SetFlag` and `WaitFlag` corresponding to `MTE2_V` to reproduce this scenario. This synchronization pair ensures that after MTE2 completes the GM-to-UB transfer, Vector reads `xLocal` and `yLocal` to execute Add. After removal, Vector may read UB data before the transfer completes, thus triggering race condition detection.

  Notice: Pass `--tool=racecheck` when calling `mssanitizer` to enable race condition detection.

  ```cpp
  // Before Add reads xLocal/yLocal, it must wait for MTE2 transfer to complete; removal triggers RAW race.
  AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
  AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
  ```

  Rebuild and run detection:

  ```bash
  make -B demo
  mssanitizer ./demo --tool=racecheck
  ```

- **Uninitialized Access Detection**

  Uninitialized access detection is used to locate anomalies caused by reading uninitialized values after memory allocation. Users can comment out `zGm.SetGlobalBuffer` to reproduce this scenario. The device-side `zGm` is not initialized before use, thus triggering uninitialized access detection.

  Notice: Pass `--tool=initcheck` when calling `mssanitizer` to enable uninitialized access detection.

  ```cpp
  // Correctly initialize zGm. Remove this line to trigger uninitialized access detection.
  zGm.SetGlobalBuffer(z + block_idx * blockLength, blockLength);
  ```

  Rebuild and run detection:

  ```bash
  make -B demo
  mssanitizer ./demo --tool=initcheck
  ```

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j

  # Run the corresponding mssanitizer command based on debugging requirements.
  mssanitizer ./demo
  mssanitizer ./demo --leak-check=yes
  mssanitizer ./demo --check-unused-memory=yes
  mssanitizer ./demo --tool=racecheck
  mssanitizer ./demo --tool=initcheck
  ```

- Build options

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` | NPU architecture: `dav-2201` corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products |

- Execution result

  When accuracy comparison succeeds, the output is as follows:

  ```bash
  test pass!
  ```