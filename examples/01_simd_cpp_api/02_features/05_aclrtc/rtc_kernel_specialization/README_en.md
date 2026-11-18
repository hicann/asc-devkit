# ACLRTC Kernel Specialization Example

## Overview

This example demonstrates Kernel-level constant specialization: generate compilation resources through the standard operator package build workflow, specialize the MatmulCustom Kernel with its Matmul tiling values using `aclrtcKernelSpecialization`, and load and execute the specialized ELF.

The example uses fixed inputs and tiling values to show how specialization and launch parameters are related. You can also compare Kernel duration and pipeline metrics before and after specialization using the operator profiling tool.

## Supported Products and CANN Versions

**Table 1**  Supported Products and CANN Versions

| Product | CANN Version |
| --- | --- |
| Ascend 950PR&950DT products | 9.3.0 |
| Atlas A3 products | 9.3.0 |
| Atlas A2 products | 9.3.0 |

## Directory Structure

```text
rtc_kernel_specialization/
├── CMakeLists.txt                 # Host build configuration
├── main.cpp                       # Specialization, loading, execution, and verification
├── runtime_resources.h            # Runtime resource cleanup
├── custom_op/                     # MatmulCustom operator and Kernel sources
│   ├── CMakeLists.txt             # Operator package build configuration
│   ├── op_host/
│   │   ├── CMakeLists.txt
│   │   └── matmul_custom_host.cpp
│   └── op_kernel/
│       ├── CMakeLists.txt
│       ├── matmul_custom.cpp
│       └── matmul_custom_tiling.h
└── README.md
```

## Example Description

The Kernel computes `C = A × B` for three `[512, 512]` matrices in ND format without transposition or bias. A and B are FP16, and C is FP32. A contains values from -2 to 2 in increments of 0.25; B is the identity matrix, so the expected C equals A. The five Kernel parameters are `a`, `b`, `c`, `workspace`, and `tiling`. Only the fifth parameter, `tiling`, is specialized. The Host uses the Matmul tiling API to generate the parameters for the current device and prints the block count and tile sizes.

## Example Implementation

The interface calls are visible in `main.cpp` in this order:

1. Initialize ACL, Context, and Stream. Allocate regular Host memory and Device memory for inputs and output, and Device memory for tiling.
2. Prepare matrices A and B and the Host reference result. Copy A and B to the Device, generate tiling with the Matmul tiling API, and allocate Device workspace as needed.
3. In the default mode, populate `aclrtcKernelSpecRequest` with the Host address and byte size of the fifth parameter, pass `-g` and `-fno-ident` as separate elements of `options` to generate debug information and suppress compiler identification information, and call `aclrtcKernelSpecialization` to generate the specialized ELF. Skip this step when an existing binary path is passed directly.
4. Load the specified Kernel binary with `aclrtBinaryLoadFromFile` and retrieve the Kernel function with `aclrtBinaryGetFunction`.
5. Copy tiling to the Device, initialize the output buffer, and call `aclrtLaunchKernelWithArgsArray` using the original five-parameter ABI.
6. Synchronize the Stream, copy the output to the Host, and verify the result.
7. Unload the binary, release Host and Device memory, destroy the Stream and Context, and finalize ACL.

## Build and Run

Run the following steps in the root directory of this example.

- Configure environment variables
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  1. Build the operator package and Kernel Spec resources:

     This example sets `KERNEL_SPEC_MODE Normal` in the `npu_op_package` call in `custom_op/CMakeLists.txt` to generate specialization resources for ordinary Kernels. For operator project build configuration, see [Basic Process](../../../../../docs/zh/guide/programming_guide/advanced_programming/aclnn_operator_development/compilation_and_deployment/basic_process.md).

     Replace `ascendxxx` with the AI processor series on which the example will run. `ASCEND_COMPUTE_UNIT` is case-sensitive; use a lowercase value supported by the build tool.

     ```bash
     rm -rf build/operator
     cmake -S custom_op -B build/operator -DASCEND_COMPUTE_UNIT=ascendxxx
     cmake --build build/operator --target binary package -j8
     ```

     The build generates the Kernel compilation resources, the Manifest, and the operator installation package `custom_opp_*.run`.

  2. Install the operator package into the example's local `build/opp` directory:

     ```bash
     rm -rf build/opp
     mkdir -p build/opp
     bash build/operator/custom_opp_*.run --quiet --install-path="$(pwd)/build/opp"
     source build/opp/vendors/customize/bin/set_env.bash
     export ASCEND_CUSTOM_OPP_PATH="$(pwd)/build/opp/vendors/customize"
     ```

  3. Read the `resourceId` and Kernel entry from the Manifest generated by this build. Continue in the same terminal so that both values are available when you run `demo`.

     ```bash
     manifest=$(find build/operator -type f -name '*_manifest.json' -print -quit)
     resource_id=$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1]))["resource_id"])' "$manifest")
     kernel_entry=$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1]))["kernels"][0]["kernel_name"])' "$manifest")
     printf 'resourceId=%s\nkernelEntry=%s\n' "$resource_id" "$kernel_entry"
     ```

  4. Build the Host program:

     ```bash
     rm -rf build/host
     cmake -S . -B build/host
     cmake --build build/host -j8
     ```

     The root `CMakeLists.txt` compiles the example program into `build/host/demo`.

  5. Run on the NPU:

     ```bash
     device_id=3
     ./build/host/demo \
         "$resource_id" \
         "$kernel_entry" \
         build/specialized_kernel.elf \
         "$device_id"
     ```

     The arguments are the resource ID, Kernel entry, specialized ELF output path, and device ID. Set `device_id` to an available device.

     > [!WARNING]
     > **Notice**: This example supports only NPU execution. CPU debugging and SIM simulation modes are not supported.

- Execution result

  Expected output:

  ```text
  Numerical check passed: 262144 float elements
  Example passed
  ```

## Performance Debugging

### Performance Collection

Use `msprof op` from the current CANN environment to collect Kernel task duration and pipeline metrics. Complete "Build and Run" first, then run these steps from the example root in the same terminal.

1. **Specify the two Kernel binaries to collect**

   ```bash
   original_kernel_path=$(find build/operator/op_kernel -type f -name "${kernel_entry%_*}.o" -print -quit)
   specialized_kernel_path=build/specialized_kernel.elf
   ls -l "$original_kernel_path" "$specialized_kernel_path"
   mkdir -p build/profiling
   profile_root=$(mktemp -d build/profiling/kernel-XXXXXX)
   ```

   `mktemp -d` creates a separate directory for this collection, replaces `XXXXXX` with random characters, and stores the actual path in `profile_root`. Do not replace `XXXXXX` manually. Run this step only once; all five rounds use the same directory.

   The original binary comes from this operator package build, and the specialized ELF comes from step 5 of "Build and Run". Use identical inputs, tiling, and launch parameters. Regenerate the specialized ELF after rebuilding the operator package, changing tiling, or selecting another target device.

2. **Collect the original and specialized Kernels**

   Set the current round before running the two collection commands.

   ```bash
   round=1
   msprof op \
       --output="$profile_root/round-$round-original" \
       --warm-up=5 --launch-count=1 \
       --replay-mode=kernel --aic-metrics=PipeUtilization \
       ./build/host/demo \
       "$resource_id" "$kernel_entry" "$specialized_kernel_path" "$device_id" "$original_kernel_path"

   msprof op \
       --output="$profile_root/round-$round-specialized" \
       --warm-up=5 --launch-count=1 \
       --replay-mode=kernel --aic-metrics=PipeUtilization \
       ./build/host/demo \
       "$resource_id" "$kernel_entry" "$specialized_kernel_path" "$device_id" "$specialized_kernel_path"
   ```

   The command format is `demo resource_id kernel_entry output_kernel_path device_id [input_kernel_path]`. When `input_kernel_path` is supplied as the last argument, load that file directly and skip specialization; `output_kernel_path` is unused. The input path must refer to an existing regular file; an invalid path causes the program to exit with an error. Specify the file paths as follows:

   - `original_kernel_path`: the original Kernel `.o` file generated by the operator package build.
   - `specialized_kernel_path`: the ELF file generated by `aclrtcKernelSpecialization`. If the file has been renamed or moved, set the variable to its actual path.

   Both binaries follow the same execution and verification steps. The collection options are:

   - `--warm-up=5`: let the tool perform five Kernel warmup runs.
   - `--launch-count=1`: collect one Kernel launched by the application. This limits collected launches, not repeated measurements.
   - `--replay-mode=kernel`: let the tool replay the Kernel to collect `PipeUtilization` metrics. The tool collects the metrics in PMU event groups.

   After each command, use `echo $?` to confirm exit status 0. Check for `Numerical check passed`, `Example passed`, and `Warm Up enabled. times:5` in the output. Exclude failed captures and failed numerical checks.

3. **Collect five rounds**

   After round 1, repeat only step 2, changing `round=1` to `round=2`, `round=3`, `round=4`, and `round=5` in turn. Do not repeat step 1 to create another directory. Collect the original Kernel first in rounds 1, 3, and 5, and the specialized Kernel first in rounds 2 and 4. Run the commands sequentially. Each round produces one collection result per Kernel; reuse the same specialized ELF in every round.

### Viewing the Collected Data

Each output directory contains an `OPPROF_*` subdirectory. Locate the CSV files with these commands and open them in a spreadsheet:

```bash
find "$profile_root" -name 'OpBasicInfo.csv' -print
find "$profile_root" -name 'PipeUtilization.csv' -print
```

**Table 2**  Performance Metrics

| File | Field | Meaning and aggregation |
| --- | --- | --- |
| `OpBasicInfo.csv` | `Task Duration(us)` | Elapsed time between the start and end timestamps of one captured Kernel task |
| `PipeUtilization.csv` | `aic_time(us)` | Execution duration on each AI Cube Core |
| `PipeUtilization.csv` | `aic_scalar_time(us)` | Scalar duration |
| `PipeUtilization.csv` | `aic_cube_time(us)` | Cube matrix multiply-accumulate duration |
| `PipeUtilization.csv` | `aic_mte1_time(us)` | L1-to-L0A/L0B transfer instruction duration, excluding transfer waits |
| `PipeUtilization.csv` | `aic_mte2_time(us)` | GM-to-AI-Core transfer instruction duration |
| `PipeUtilization.csv` | `aic_mte3_time(us)` | AI-Core-to-GM transfer instruction duration |
| `PipeUtilization.csv` | `aic_fixpipe_time(us)` | L0C-to-GM/L1 result transfer instruction duration; includes instruction waits when UnitFlag is enabled |

Duration is used for the comparison; corresponding pipeline cycle ratios are available in the same CSV under `*_ratio` fields.

- **Per-core data**: `block_id` and `sub_block_id` identify cores in `PipeUtilization.csv`. Each row is not a separate measurement. Average the valid AI Core values in each column (for example, using `AVERAGE` in a spreadsheet) to obtain the round's pipeline metrics.
- **Round means**: calculate the arithmetic mean of five round results for each Kernel. Aggregate task duration and pipeline duration separately. Missing values or `NA`/`N/A` must not be treated as zero.
- **Comparison**: change = (specialized mean − original mean) / original mean × 100%. Negative values indicate reduced duration; positive values indicate increased duration. Speedup = original mean / specialized mean.
- **Scope**: compare only device Kernel tasks and pipeline metrics. Exclude specialization, Host initialization, binary loading, Host/Device copies, and Host submission and synchronization API durations. The tool performs warmup; no warmup rows need to be removed from the CSV manually.
- **Interpretation**: replay cache conditions may differ from consecutive application execution. Use identical tool settings for both Kernels. Pipeline activities overlap, so their durations cannot be added to obtain task duration.

### Measured Performance

**Table 3**  Performance Test Configuration

| Item | Test configuration |
| --- | --- |
| Product | Atlas A2 products |
| Host | aarch64, Ubuntu 24.04.4 LTS |
| Software | CANN 9.3.0, driver 25.5.1 |
| Matrices | M = N = K = 512. A and B are ND FP16; C is ND FP32. B is the identity matrix. |
| Tiling and compiler options | Both Kernels use identical tiling and the `-g` and `-fno-ident` compiler options. See the sample output for the block count and tile sizes. |
| Collection | `msprof op`, `PipeUtilization`, Kernel replay, five tool warmup runs, five independent collections per Kernel |

The following results were collected in five independent rounds using the settings above. Each round contains one device Kernel task per version.

**Table 4**  Kernel Task Duration by Round

| Round | Original Task Duration (μs) | Specialized Task Duration (μs) |
| --- | ---: | ---: |
| 1 | 9.32 | 7.28 |
| 2 | 9.38 | 7.56 |
| 3 | 8.60 | 7.74 |
| 4 | 9.04 | 7.38 |
| 5 | 8.90 | 7.70 |
| **Mean** | **9.048** | **7.532** |

For each pipeline metric, the values are averaged across the valid compute cores in each round and then across all five rounds.

**Table 5**  Mean Kernel Task and Pipeline Durations

| Metric | Original mean (μs) | Specialized mean (μs) | Change |
| --- | ---: | ---: | ---: |
| `Task Duration(us)` | 9.048 | 7.532 | -16.76% |
| `aic_time(us)` | 7.786 | 6.441 | -17.27% |
| `aic_scalar_time(us)` | 4.640 | 2.330 | -49.79% |
| `aic_cube_time(us)` | 1.449 | 1.450 | +0.07% |
| `aic_mte1_time(us)` | 1.703 | 1.684 | -1.14% |
| `aic_mte2_time(us)` | 4.138 | 4.606 | +11.31% |
| `aic_mte3_time(us)` | 0.001 | 0.001 | 0.00% |
| `aic_fixpipe_time(us)` | 2.858 | 3.006 | +5.16% |

- The specialized Kernel had a lower Task Duration in all five rounds. The mean decreased by 16.76%. Mean Scalar duration decreased by 49.79%, consistent with reduced runtime scalar processing after specialization.
