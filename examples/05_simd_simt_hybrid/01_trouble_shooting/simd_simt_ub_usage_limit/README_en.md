# UB Out-of-Bounds Troubleshooting Sample for SIMD and SIMT Hybrid Programming

## Overview

This sample uses an output accuracy issue caused by a UB out-of-bounds access in SIMD and SIMT hybrid programming to demonstrate how UB capacity, memory layout, and actual access ranges can be used to locate and fix the invalid UB access. It then uses multiple Cases to explain UB limits under different VF configurations, the layout relationship between static and dynamic UB, and the runtime and accuracy results of out-of-bounds accesses to different UB regions.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Versions

- \>= CANN 9.1.0

## Directory Structure

```text
├── simd_simt_ub_usage_limit
│   ├── CMakeLists.txt                   // CMake build file
│   ├── case0_ub_out_of_bounds.asc       // Case 0 implementation
│   ├── case1_ub_limit.asc               // Case 1 implementation
│   ├── case2_ub_layout.asc              // Case 2 implementation
│   ├── case3_out_of_bounds_result.asc   // Case 3 implementation
│   ├── figures                          // Image resources for README
│   ├── README.md                        // Chinese documentation
│   └── README_en.md                     // English documentation
```

## Example Description

This sample contains four Cases covering UB out-of-bounds troubleshooting, capacity boundaries, and memory layout.

**Case description**:

| Case | Scenario | Main Content |
| --- | --- | --- |
| Case 0 | Output accuracy troubleshooting | Present the output accuracy issue, locate the cause by checking execution status, calculation logic, and access ranges, and verify the fix |
| Case 1 | Usable UB limits in different VF scenarios | Determine usable UB limits under different VF-call, reserved-UB, and Data Cache configurations |
| Case 2 | Static and Dynamic UB Layout Across Execution Locations | Explain the static and dynamic UB layout relationship across execution locations |
| Case 3 | UB out-of-bounds access results at different execution locations | Explain the results when MainScalar and SIMT VF access UB regions out of bounds |

**Example specifications**:

<table>
<tr><td align="center">Example Type (OpType)</td><td colspan="3" align="center">SimdSimtUbUsageLimit</td></tr>
<tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
<tr><td align="center">input</td><td align="center">[8192]/[10240]/[512]</td><td align="center">uint32_t</td></tr>
<tr><td align="center">Example Output</td><td align="center">output</td><td align="center">[8192]/[10240]/[512]</td><td align="center">uint32_t</td></tr>
<tr><td rowspan="4" align="center">Kernel Name</td><td align="center">Case 0</td><td colspan="2" align="center"><code>static_ub_out_of_bounds_kernel</code> / <code>static_ub_bias_offset_fixed_kernel</code></td></tr>
<tr><td align="center">Case 1</td><td colspan="2" align="center"><code>ub_limit_main_scalar_kernel</code> / <code>ub_limit_simd_vf_kernel</code> / <code>ub_limit_simt_vf_kernel</code> / <code>ub_limit_simd_simt_vf_kernel</code></td></tr>
<tr><td align="center">Case 2</td><td colspan="2" align="center"><code>ub_layout_kernel</code></td></tr>
<tr><td align="center">Case 3</td><td colspan="2" align="center"><code>scalar_ub_access_result_kernel</code> / <code>simt_vf_ub_access_result_kernel</code></td></tr>
</table>

Case 0 can use `[8192]` or `[10240]` input and output. Case 3 uses `[512]` input and output. Cases 1 and 2 verify UB capacity and layout and do not perform GM input/output data checks.

## Example Implementation

The Cases follow an issue presentation and fix, UB-limit determination, memory-layout explanation, and out-of-bounds result analysis sequence.

This sample uses the compile macro `SCENARIO_NUM` to control 15 test scenarios grouped into four Cases.

| Case | SCENARIO_NUM | Scenario |
| --- | --- | --- |
| Case 0 | 0-1 | Show the accuracy issue caused by incorrect UB access-range calculation and the fixed result |
| Case 1 | 2-5 | Compare the maximum usable UB limits with no VF call, SIMD VF only, SIMT VF only, and both SIMD VF and SIMT VF |
| Case 2 | 6 | Show the static and dynamic UB layout relationship across MainScalar, SIMD VF, and SIMT VF |
| Case 3 | 7-14 | Compare runtime and accuracy results after MainScalar writes target UB regions and SIMT VF writes target UB regions |

### Case 0: Output Accuracy Troubleshooting

**Objective**:

Present an output accuracy issue caused by UB out-of-bounds access, and show the analysis process of locating the issue step by step through execution status, calculation logic, and UB access ranges.

**Core implementation**:

Case 0 implements a simplified calibration calculation for input data. The `input` is the data to be calibrated. The kernel generates the calibration value `bias` from the lower 8 bits of `input` and generates `mask` from the lowest bit of `input` to control whether the `input` data is calibrated. The final calculation is:

```text
bias[i] = input[i] & 0xff
mask[i] = input[i] & 1
output[i] = input[i] + bias[i] * mask[i]
```

The kernel splits the input into 8 KB data tiles and divides the allocated static UB into four UB tiles, which are reused to store `input`/`mask` and `bias` during calculation. For each data tile, the SIMT VF first moves the `input` of that data tile from GM into the current UB tile. The kernel then generates `bias` from the UB `input` and overwrites the original `input` region with `mask`. Finally, the SIMT VF reads the original `input` from GM, reads `mask` from the original `input` region in the current UB tile, reads `bias` from the `bias` region, calculates `output`, and writes it back to GM.

The static UB tiles and data layout in Case 0 are shown below:

![Case 0 static UB tiles and data layout](./figures/case0_static_ub_layout_en.png "Case 0 static UB tiles and data layout")

In this Case, `tile_id` indicates the data tile currently being processed, and `ub_base` indicates the start element index of the UB region reused by the current data tile. The key code is described in blocks according to the SIMT VF calculation logic and the kernel call flow.

The SIMT VF moves the `input` of the current data tile into UB. In the output stage, it reads the original `input` from GM and reads `mask` and `bias` from UB to calculate `output`:

```cpp
// In the SIMT scenario, 216 KB of static UB is available after subtracting
// 8 KB of reserved UB and 32 KB of Data Cache from the 256 KB UB.
constexpr uint32_t UB_BYTES = 216 * 1024;
constexpr uint32_t UB_WORDS = UB_BYTES / sizeof(uint32_t);
constexpr uint32_t UB_TILE_COUNT = 4;
constexpr uint32_t UB_TILE_WORDS = UB_WORDS / UB_TILE_COUNT;
constexpr uint32_t DATA_TILE_WORDS = 2048; // Each data tile has 2048 uint32_t elements, that is, 8 KB.
constexpr uint32_t BIAS_OFFSET = DATA_TILE_WORDS;

__simt_vf__ __launch_bounds__(THREAD_COUNT) inline void simt_prepare_input_ub(
    __gm__ const uint32_t* input, __ubuf__ uint32_t* static_ub, uint32_t ub_base, uint32_t tile_id)
{
    const uint32_t gm_base = tile_id * DATA_TILE_WORDS;
    for (uint32_t index = threadIdx.x; index < DATA_TILE_WORDS; index += blockDim.x) {
        const uint32_t input_data = input[gm_base + index];
        static_ub[ub_base + index] = input_data;
    }
}

__simt_vf__ __launch_bounds__(THREAD_COUNT) inline void simt_store_calibrated_output(
    __gm__ const uint32_t* input, __ubuf__ uint32_t* static_ub, __gm__ uint32_t* output,
    uint32_t ub_base, uint32_t word_count, uint32_t tile_id)
{
    const uint32_t gm_base = tile_id * DATA_TILE_WORDS;
    for (uint32_t index = threadIdx.x; index < word_count; index += blockDim.x) {
        const uint32_t input_data = input[gm_base + index];
        const uint32_t mask = static_ub[ub_base + index];
        const uint32_t bias = static_ub[ub_base + BIAS_OFFSET + index];
        output[gm_base + index] = input_data + bias * mask;
    }
}
```

The scalar logic in the kernel generates `bias` and `mask` from the `input` in UB. `bias_base` indicates the start element index of the `bias` region for the current data tile:

```cpp
__aicore__ inline void generate_bias_and_mask_values(
    __ubuf__ uint32_t* static_ub, uint32_t ub_base, uint32_t tile_id)
{
    const uint32_t bias_base = tile_id * UB_TILE_WORDS + BIAS_OFFSET;
    for (uint32_t index = 0; index < DATA_TILE_WORDS; ++index) {
        const uint32_t input = static_ub[ub_base + index];
        static_ub[bias_base + index] = input & 0xff;
        static_ub[ub_base + index] = input & 1;
    }
}
```

The kernel main loop processes data tiles in sequence: it calls the SIMT VF to move `input` into UB, generates `bias` and `mask`, and then calls the SIMT VF again to calculate and write back `output`:

```cpp
__global__ __vector__ void static_ub_out_of_bounds_kernel(
    __gm__ const uint32_t* input, __gm__ uint32_t* output, uint32_t input_words)
{
    __ubuf__ uint32_t static_ub[UB_WORDS];
    uint8_t mutex_id = 0;

    const uint32_t input_tile_count = input_words / DATA_TILE_WORDS;
    for (uint32_t tile_id = 0; tile_id < input_tile_count; ++tile_id) {
        const uint32_t ub_base = (tile_id & (UB_TILE_COUNT - 1)) * UB_TILE_WORDS;

        asc_lock(PIPE_V, mutex_id);
        asc_vf_call<simt_prepare_input_ub>(dim3(THREAD_COUNT), input, static_ub, ub_base, tile_id);
        asc_unlock(PIPE_V, mutex_id);

        asc_lock(PIPE_S, mutex_id);
        generate_bias_and_mask_values(static_ub, ub_base, tile_id);
        asc_unlock(PIPE_S, mutex_id);

        asc_lock(PIPE_V, mutex_id);
        asc_vf_call<simt_store_calibrated_output>(
            dim3(THREAD_COUNT), input, static_ub, output, ub_base, DATA_TILE_WORDS, tile_id);
        asc_unlock(PIPE_V, mutex_id);
    }
}
```

**Execution result**:

| SCENARIO_NUM | Input Size | Data Tile Count | Result |
| --- | ---: | ---: | --- |
| 0 | 32 KB | 4 | Accuracy check passed |
| 0 | 40 KB | 5 | Accuracy check failed |

When the input size is 32 KB, the kernel processes four data tiles and prints the following information:

```text
test pass!
```

When the input size is 40 KB, the kernel processes five data tiles and output accuracy verification fails:

```text
[Failed] Case accuracy verification failed!
```

**Troubleshooting**:

The following troubleshooting process focuses on the accuracy issue that occurs with a 40 KB input size. Accuracy issues are usually located in the following order: execution status, calculation logic, and memory corruption.

1. Check whether the kernel executes normally. Configure the log file path and set whether logs are printed to stdout as needed. Then check the log files for typical kernel launch, synchronization, or AICore execution errors such as `Ascend Error`, `EZ9999`, `vector core exception`, and `ASSERT`. For log environment variables, see the [Ascend log example](../../../01_simd_cpp_api/01_utilities/09_log/README_en.md).

    ```bash
    export ASCEND_PROCESS_LOG_PATH=./log       # Set the log path.
    export ASCEND_SLOG_PRINT_TO_STDOUT=0       # Control whether logs are printed to stdout.
    ./ub_usage_limit 10240                     # Run the scenario with a 40 KB input size.
    ```

    After execution, log files are generated under `./log`. In this scenario, the log files do not show kernel launch, synchronization, or AICore execution errors, so the accuracy issue shown above is not directly caused by kernel execution failure. The next step is to check why the output data does not match the expected result.

2. Check the calculation logic inside the kernel. First check whether the Host-side Golden formula matches the kernel output formula. The Host initializes input by global element index and calculates Golden as `input + (input & 0xff) * (input & 1)`, which matches the calibration calculation in the kernel:

    ```cpp
    input[gm_base + index] = INPUT_BASE_VALUE + gm_base + index;
    const uint32_t input_data = INPUT_BASE_VALUE + gm_base + index;
    const uint32_t expected = input_data + (input_data & 0xff) * (input_data & 1);
    ```

    Then check how the kernel generates `bias` and `mask` and writes the final output. The kernel writes `bias` as `input & 0xff` and overwrites the original `input` region with `input & 1` to obtain `mask`. In the final output stage, it reads the original `input` from GM again and writes `input + bias * mask` back to GM:

    ```cpp
    // ub_base is passed in by the kernel main loop and indicates the start
    // element index of the UB region reused by the current data tile.
    static_ub[bias_base + index] = input & 0xff;
    static_ub[ub_base + index] = input & 1;
    const uint32_t input_data = input[gm_base + index];
    const uint32_t mask = static_ub[ub_base + index];
    const uint32_t bias = static_ub[ub_base + BIAS_OFFSET + index];
    output[gm_base + index] = input_data + bias * mask;
    ```

    The Host-side Golden and the kernel calculation intent are consistent, and no obvious issue is found in the input data or calculation flow. If the output is still incorrect, the next step is to check whether memory has been overwritten and data has been corrupted.

3. Print intermediate calculation variables. The previous checks show that the kernel formula itself is consistent, but the output is still incorrect. Therefore, temporary prints can be added at intermediate calculation positions to observe related runtime variables and calculation results.

    For example, add the following temporary print after `bias_base` is calculated in `generate_bias_and_mask_values`. `bias_base` indicates the start element index of the `bias` region calculated from the current `tile_id`:

    ```cpp
    const uint32_t bias_base = tile_id * UB_TILE_WORDS + BIAS_OFFSET;
    printf("tile_id=%u, bias_base=%u\n", tile_id, bias_base);
    ```

    Printed values are as follows:

    ```text
    [AIV Block 0/1] tile_id=0, bias_base=2048
    [AIV Block 0/1] tile_id=1, bias_base=15872
    [AIV Block 0/1] tile_id=2, bias_base=29696
    [AIV Block 0/1] tile_id=3, bias_base=43520
    [AIV Block 0/1] tile_id=4, bias_base=57344
    ```

    The printed values show that when the kernel processes `tile_id = 0` to `3`, the `bias` positions are inside the static UB range. When the kernel reaches `tile_id = 4`, `bias_base` continues to increase by data tile ID and reaches 57344. The converted write start byte offset is:

    ```text
    bias write start byte offset = bias_base * 4 B = 57344 * 4 B = 229376 B = 224 KB
    ```

    After the write start byte offsets are obtained, compare them with the UB layout in this Case to determine the actual access ranges.

4. Confirm the actual access range from the UB layout. Case 0 uses 216 KB of static UB and configures zero dynamic UB. By default, the remaining space contains an 8 KB reserved region and 32 KB of Data Cache. Data Cache is SIMT-specific, caches GM data accessed by SIMT threads, and ranges from 32 KB to 128 KB.

    ![Case 0 actual UB access range](./figures/case0_access_range_en.png "Case 0 actual UB access range")

    The Data Cache size is `min(256 KB - 216 KB - 0 KB - 8 KB, 128 KB) = 32 KB`, so it starts at `256 KB - 32 KB = 224 KB`. The calculated `bias` write range is 224 KB to 232 KB, which enters Data Cache. This access may corrupt data cached for SIMT GM access. When the later SIMT VF continues to read GM input and calculate output, an accuracy error may appear.

**Solution**:

- When generating `bias`, calculate the start position of the `bias` region from the start element index of the UB region reused by the current data tile.

    ```cpp
    __aicore__ inline void generate_bias_and_mask_values_fixed(__ubuf__ uint32_t* static_ub, uint32_t ub_base)
    {
        const uint32_t bias_base = ub_base + BIAS_OFFSET;
        for (uint32_t index = 0; index < DATA_TILE_WORDS; ++index) {
            const uint32_t input = static_ub[ub_base + index];
            static_ub[bias_base + index] = input & 0xff;
            static_ub[ub_base + index] = input & 1;
        }
    }
    ```

- After the `bias_base` calculation is fixed, the intermediate access is inside the declared static UB range and output accuracy verification passes:

    | SCENARIO_NUM | Input Size | Data Tile Count | Result |
    | --- | ---: | ---: | --- |
    | 1 | 40 KB | 5 | Accuracy check passed |

    ```text
    [Case0] mode=fixed
    test pass!
    ```

**Conclusion**:

When a UB out-of-bounds access occurs, it may not cause a kernel runtime error, and the logs may not contain explicit exception information. In this Case, `bias` is written out of bounds into Data Cache, which corrupts data cached for SIMT GM access and finally appears as an accuracy error. Therefore, when locating an accuracy issue caused by UB out-of-bounds access, check not only the log result but also the calculation logic and the actual access range calculated from runtime parameters.

Case 0 shows that a UB out-of-bounds access may appear as an accuracy failure. Mixed programming involves different execution locations such as MainScalar, SIMD VFs, and SIMT VFs. UB is used both for static or dynamic memory allocated at these locations and for data communication between VFs and MainScalar. To help developers avoid out-of-bounds UB access, Cases 1 through 3 explain UB limits under different VF configurations, the layout relationship between static and dynamic UB, and the results of out-of-bounds access to different regions.

### Case 1: Usable UB Limits in Different VF Scenarios

**Objective**:

Determine the maximum usable UB limits under different VF-call scenarios.

**Core implementation**:

This Case covers four paths: no VF call, SIMD VF only, SIMT VF only, and both SIMD VF and SIMT VF. When a VF is called, it verifies the maximum usable UB limit under the default configuration, with the VF reserve disabled, and with all reserved UB disabled.

Each Vector Core has 256 KB of UB. When a VF is called, the default configuration calculates the usable UB limit with 8 KB of reserved UB (6 KB for the VF reserve and 2 KB for Ascend C APIs). In this Case, the no-VF-call path accesses static UB only from MainScalar, does not involve a VF stack, and does not call Ascend C APIs that depend on reserved UB. Therefore, this path verifies the limit using the full 256 KB of UB. `--cce-disable-vf-stack-reserved-ubuf` disables the 6 KB VF reserve, and `--cce-disable-asc-reserved-ubuf` disables the 2 KB Ascend C API reserve. According to the [SIMT memory hierarchy](../../../../docs/zh/guide/programming_guide/programming_model/ai_core_simt_programming/memory_hierarchy.md#共享内存大小的限制), Data Cache is SIMT-specific, caches GM data accessed by SIMT threads, and ranges from 32 KB to 128 KB. When the hybrid programming scenario contains a SIMT VF, part of UB must be allocated as Data Cache. Therefore, the maximum space available to static and dynamic UB is obtained by reserving at least 32 KB for Data Cache. The actual Data Cache size is calculated from the remaining UB space after subtracting static memory, dynamic memory, and reserved UB.

| SCENARIO_NUM | VF Call | Data Cache Constraint | Usable UB in Default Configuration (8 KB Reserved for VF-Call Paths) | Usable UB with VF Reserve Disabled (2 KB Reserved) | Usable UB with All Reserved UB Disabled (0 KB Reserved) |
| --- | --- | ---: | ---: | ---: | ---: |
| 2 | No VF call | None | Max. 256 KB | - | - |
| 3 | SIMD VF only | None | Max. 248 KB | Max. 254 KB | Max. 256 KB |
| 4 | SIMT VF only | At least 32 KB | Max. 216 KB | Max. 222 KB | Max. 224 KB |
| 5 | Both SIMD VF and SIMT VF | At least 32 KB | Max. 216 KB | Max. 222 KB | Max. 224 KB |

The Ascend C code calculates the maximum usable UB size from the VF-call scenario, reserved-UB configuration, and Data Cache space, and then declares a static UB array named `static_ub` with that size. Different paths use separate kernels and write `static_ub` from MainScalar, a SIMD VF, a SIMT VF, or both SIMD VF and SIMT VF. The kernel accesses the tail element of `static_ub` and writes it to one GM location so that the limit position participates in the current kernel access. The Host determines whether the current UB size exceeds the limit based on the kernel launch/synchronization result and error information:

```cpp
// VF_RESERVED_BYTES: 0 or 6KB; ASC_RESERVED_BYTES: 0 or 2KB;
// DATA_CACHE_BYTES: 0 or 32KB; EXTRA_BYTES is 4B when checking the over-limit result.
constexpr uint32_t UB_BYTES = UB_TOTAL_BYTES - VF_RESERVED_BYTES - ASC_RESERVED_BYTES -
                              DATA_CACHE_BYTES + EXTRA_BYTES;
constexpr uint32_t UB_WORDS = UB_BYTES / sizeof(uint32_t);

// Scenario 1: no VF call.
__global__ __vector__ void ub_limit_main_scalar_kernel(__gm__ uint32_t* output)
{
    __ubuf__ uint32_t static_ub[UB_WORDS];
    ...
    output[0] = static_ub[UB_WORDS - 1];
}

// Scenario 2: SIMD VF only.
__global__ __vector__ void ub_limit_simd_vf_kernel(__gm__ uint32_t* output)
{
    __ubuf__ uint32_t static_ub[UB_WORDS];
    ...
    asc_vf_call<simd_fill_static_ub>(static_ub, UB_WORDS);
    ...
    output[0] = static_ub[UB_WORDS - 1];
}

// Scenario 3: SIMT VF only.
__global__ __vector__ void ub_limit_simt_vf_kernel(__gm__ uint32_t* output)
{
    __ubuf__ uint32_t static_ub[UB_WORDS];
    ...
    asc_vf_call<simt_fill_static_ub>(dim3(THREAD_COUNT), static_ub, UB_WORDS, FILL_VALUE_BASE);
    ...
    output[0] = static_ub[UB_WORDS - 1];
}

// Scenario 4: both SIMD VF and SIMT VF.
__global__ __vector__ void ub_limit_simd_simt_vf_kernel(__gm__ uint32_t* output)
{
    __ubuf__ uint32_t static_ub[UB_WORDS];
    ...
    asc_vf_call<simd_fill_static_ub>(static_ub, UB_WORDS);
    asc_vf_call<simt_fill_static_ub>(dim3(THREAD_COUNT), static_ub, UB_WORDS, FILL_VALUE_BASE);
    ...
    output[0] = static_ub[UB_WORDS - 1];
}
```

**Execution result**:

This Case first allocates `static_ub` at the theoretical usable UB limit of the current scenario to confirm that the kernel can launch and run successfully. It then increases the `static_ub` allocation by 4 bytes, that is, by one more `uint32_t` element, to observe the runtime result after exceeding the UB limit. In this case, compilation still succeeds, but kernel launch fails and the error indicates that the shared memory size exceeds the limit.

| SCENARIO_NUM | VF Call | Result at Limit | Result After Adding 4 B Beyond the Limit |
| --- | --- | --- | --- |
| 2 | No VF call | Passed | Kernel launch failed |
| 3 | SIMD VF only | All configurations passed | Kernel launch failed for all configurations |
| 4 | SIMT VF only | All configurations passed | Kernel launch failed for all configurations |
| 5 | Both SIMD VF and SIMT VF | All configurations passed | Kernel launch failed for all configurations |

When the limit configuration runs successfully, the output is as follows:

```text
[Case1] result=PASSED
```

After increasing the `static_ub` allocation from the theoretical usable UB limit of the current scenario to "theoretical usable UB limit + 4 B", kernel launch fails and prints the following key error information:

```text
[Host] kernel error: ... Reason: the sum of dynamic shared memory and kernel shared memory exceeds the maximum limit.
[ERROR] [Bisheng] Launch kernel failure! ret 107000
```

**Conclusion**:

In SIMD and SIMT hybrid programming, whether a VF is called, whether a SIMT VF is included, and whether reserved UB is disabled affect the usable UB limit. When a VF is called, the default configuration subtracts the 6 KB VF reserve and the 2 KB Ascend C API reserve. After the corresponding reserved space is disabled by compiler options, the usable UB limit is recalculated with the released space. When a SIMT VF is included, UB space must also be reserved for Data Cache. In actual development, determine the usable UB limit of the current kernel based on these factors.

> **Note:** After [--cce-disable-vf-stack-reserved-ubuf](../../../../docs/zh/guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#ZH-CN_TOPIC_0000002462746461) is used, the reserved 6 KB SIMD VF stack space can be used as normal UB space, but the compiler can no longer use this space to cache register spill data. Developers need to ensure that register spills do not occur. After [--cce-disable-asc-reserved-ubuf](../../../../docs/zh/guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#ZH-CN_TOPIC_0000002462746461) is used, the 2 KB UB space originally reserved for [selected Ascend C APIs](../../../../docs/zh/guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md#section_reserved_ubuf_api) can be used as normal UB space, but these APIs become unavailable and cause a compilation error when called. See the [3510 architecture specification](../../../../docs/zh/guide/programming_guide/advanced_programming/hardware_implementation/architecture_spec/npu_arch_3510.md#compile-options-description) for the restrictions of both options.

### Case 2: Static and Dynamic UB Layout Across Execution Locations

**Objective**:

Explain that static UB declarations in MainScalar, a SIMD VF, and a SIMT VF jointly occupy the current kernel's static memory region and must be counted together. Also explain that the three differently named dynamic UB declarations have the same start address and share the region configured at kernel launch.

**Core implementation**:

Case 2 declares static UB separately in MainScalar, a SIMD VF, and a SIMT VF. According to the [SIMD and SIMT hybrid programming memory hierarchy](../../../../docs/zh/guide/programming_guide/advanced_programming/advanced_ai_core_programming_model/simd_simt_hybrid_programming/memory_hierarchy.md#ub划分), static memory can be allocated in MainScalar, SIMD VF, and SIMT VF in mixed programming, and dynamic memory follows static memory. For the actual static UB layout, see that document. MainScalar declares a 1 KB `main_static_ub` array, the SIMD VF declares 2 KB, and the SIMT VF declares 4 KB. In this Case, all three declarations satisfy the default requirements, so the static memory region occupies 7168 bytes.

MainScalar, the SIMD VF, and the SIMT VF each access their static UB array so that these static UB declarations participate in the current kernel layout. The following code shows the static and dynamic UB declarations in MainScalar, the SIMD VF, and the SIMT VF:

```cpp
// MainScalar
extern __ubuf__ uint32_t main_dynamic_ub[];
__ubuf__ uint32_t main_static_ub[MAIN_STATIC_WORDS];
// SIMD VF
extern __ubuf__ uint32_t simd_dynamic_ub[];
__ubuf__ uint32_t simd_static_ub[SIMD_STATIC_WORDS];
// SIMT VF
extern __ubuf__ uint32_t simt_dynamic_ub[];
__ubuf__ uint32_t simt_static_ub[SIMT_STATIC_WORDS];
```

MainScalar, the SIMD VF, and the SIMT VF convert the start addresses of their static and dynamic UB arrays to integers and write them back to GM for the Host to print and compare. The printed addresses belong to the AICore UB address space and are used only for the layout comparison in this Case.

**Execution result**:

The static UB start addresses in MainScalar, the SIMD VF, and the SIMT VF are `0x0`, `0x400`, and `0xc00`, respectively. The static memory region occupies 7168 bytes, which is `0x1c00` in hexadecimal. Dynamic UB follows static memory, so its start address is `0x1c00`. The result is as follows:

| SCENARIO_NUM | Scenario | Result |
| --- | --- | --- |
| 6 | Static and dynamic UB layout | Passed |

```text
[Case2] static_total_bytes=7168
[Case2] main_static_ub=0x0, simd_static_ub=0x400, simt_static_ub=0xc00
[Case2] main_dynamic_ub=0x1c00, simd_dynamic_ub=0x1c00, simt_dynamic_ub=0x1c00
[Case2] result=PASSED
```

In this Case, the three static UB start addresses accumulate according to the declaration sizes. The dynamic UB start address is `0x1c00` in all three locations, matching the address corresponding to the static memory region in this Case.

**Conclusion**:

Static UB declarations in MainScalar, the SIMD VF, and the SIMT VF jointly occupy the current kernel's static memory region and must be counted together. Dynamic UB follows static memory. The three dynamic arrays have different names but the same start address and share the region configured at kernel launch. For more UB layout information, see [SIMD and SIMT hybrid programming memory hierarchy](../../../../docs/zh/guide/programming_guide/advanced_programming/advanced_ai_core_programming_model/simd_simt_hybrid_programming/memory_hierarchy.md#ub划分).

### Case 3: UB Out-of-Bounds Access Results at Different Execution Locations

**Objective**:

Allocate a static UB region, construct scenarios where MainScalar and SIMT VF access target UB regions, and observe the results when different execution locations access UB regions out of bounds.

**Core implementation**:

Declare a 4 KB static UB array named `static_ub`. The first 2 KB stores the input data `input`, and the last 2 KB stores the calibration value `bias` used for output calculation. `output` is written back to GM, and the calculation is `output = input + bias`. The kernel first prepares `input` and `bias` within the valid UB range, and then selects different execution locations to access the target UB region.

For out-of-bounds access to the target UB region, this Case constructs two scenarios: MainScalar write and SIMT VF write. In the MainScalar write scenarios, MainScalar additionally writes `bias` to the target UB region, and then the SIMT VF still reads `bias` from `static_ub` and calculates the output. In the SIMT VF write scenarios, the SIMT VF additionally writes `bias` to the target UB region, and then reads `bias` from `static_ub` and calculates the output.

Case 3 does not add compiler options that disable reserved UB and configures zero dynamic UB. In this configuration, Data Cache reaches its 128 KB maximum. According to the [3510 architecture specification](../../../../docs/zh/guide/programming_guide/advanced_programming/hardware_implementation/architecture_spec/npu_arch_3510.md#compile-options-description) and [SIMT memory hierarchy](../../../../docs/zh/guide/programming_guide/programming_model/ai_core_simt_programming/memory_hierarchy.md), an 8 KB reserved region is placed before Data Cache. The resulting layout is:

```text
Data Cache size = min(256 KB - 4 KB - 0 KB - 8 KB, 128 KB) = 128 KB
Data Cache start = 256 KB - 128 KB = 128 KB
Reserved-region start = 128 KB - 8 KB = 120 KB
Unoccupied ordinary UB space size = 120 KB - 4 KB = 116 KB
```

Only the first 4 KB is valid for the static array. To compare results at different access locations, the sample covers four target regions: valid static UB, unoccupied ordinary UB space, reserved space, and Data Cache. The Host divides the byte offset of the target region by 4 B to obtain the `static_ub` array index `access_index` and passes it to the kernel:

![Case 3 UB layout and access positions](./figures/case3_ub_layout_en.png "Case 3 UB layout and access positions")

The access ranges of the four target regions are as follows. `access_index` is the `uint32_t` array index corresponding to the target start offset.

| Target Region | Target Start Offset | `access_index` | 2 KB Target Range |
| --- | ---: | ---: | --- |
| Valid static UB | 2048 B | 512 | [2 KB, 4 KB) |
| Unoccupied ordinary UB space | 63488 B | 15872 | [62 KB, 64 KB) |
| Reserved space | 122880 B | 30720 | [120 KB, 122 KB) |
| Data Cache | 131072 B | 32768 | [128 KB, 130 KB) |

Case 3 uses `SCENARIO_NUM` to select the access method and target region:

| SCENARIO_NUM | Access Method | Target Region |
| --- | --- | --- |
| 7 / 11 | MainScalar write / SIMT VF write | Valid static UB |
| 8 / 12 | MainScalar write / SIMT VF write | Unoccupied ordinary UB space |
| 9 / 13 | MainScalar write / SIMT VF write | Reserved space |
| 10 / 14 | MainScalar write / SIMT VF write | Data Cache |

The kernel accesses the selected target region by using the runtime index `access_index`. The key code is as follows. In the common preparation stage, the SIMT VF writes `input` and `bias` to `static_ub`:

```cpp
constexpr uint32_t UB_BYTES = 4 * 1024;
constexpr uint32_t UB_WORDS = UB_BYTES / sizeof(uint32_t);
constexpr uint32_t DATA_WORDS = 512; // Process 2 KB input and generate 2 KB bias.
constexpr uint32_t BIAS_OFFSET = DATA_WORDS;

__simt_vf__ __launch_bounds__(THREAD_COUNT) inline void simt_prepare_input_ub(
    __gm__ const uint32_t* input, __ubuf__ uint32_t* static_ub)
{
    for (uint32_t index = threadIdx.x; index < DATA_WORDS; index += blockDim.x) {
        const uint32_t input_data = input[index];
        static_ub[index] = input_data;
        static_ub[BIAS_OFFSET + index] = input_data & 0xff;
    }
}
```

In the MainScalar access scenarios, MainScalar additionally writes `bias` to the target UB region:

```cpp
__aicore__ inline void copy_bias_to_target_by_scalar(__ubuf__ uint32_t* static_ub, uint32_t bias_index)
{
    __ubuf__ volatile uint32_t* static_ub_ptr = static_ub;
    for (uint32_t index = 0; index < DATA_WORDS; ++index) {
        const uint32_t bias = static_ub[BIAS_OFFSET + index];
        static_ub_ptr[bias_index + index] = bias;
    }
}
```

In the SIMT VF access scenarios, the SIMT VF additionally writes `bias` to the target UB region:

```cpp
__simt_vf__ __launch_bounds__(THREAD_COUNT) inline void simt_copy_bias_to_target(
    __ubuf__ uint32_t* static_ub, uint32_t bias_index)
{
    for (uint32_t index = threadIdx.x; index < DATA_WORDS; index += blockDim.x) {
        const uint32_t bias = static_ub[BIAS_OFFSET + index];
        static_ub[bias_index + index] = bias;
    }
}
```

After the target UB region is written, the SIMT VF reads data from the `bias` region in `static_ub` and calculates the output:

```cpp
__simt_vf__ __launch_bounds__(THREAD_COUNT) inline void simt_store_output_with_prepared_bias(
    __gm__ const uint32_t* input, __ubuf__ uint32_t* static_ub, __gm__ uint32_t* output)
{
    for (uint32_t index = threadIdx.x; index < DATA_WORDS; index += blockDim.x) {
        const uint32_t input_data = input[index];
        const uint32_t bias = static_ub[BIAS_OFFSET + index];
        output[index] = input_data + bias;
    }
}
```

**Execution result**:

The results for the access configurations are as follows:

| SCENARIO_NUM | Access Method | Target Region | Kernel Runtime Result | Accuracy | Result Description |
| --- | --- | --- | --- | --- | --- |
| 7 | MainScalar write | Valid static UB | Succeeded | Passed | Valid access |
| 8 | MainScalar write | Unoccupied ordinary UB space | Succeeded | Passed | Out of bounds, but no visible failure in this run |
| 9 | MainScalar write | Reserved space | Succeeded | Passed | Out of bounds, but no visible failure in this run |
| 10 | MainScalar write | Data Cache | Succeeded | Failed | No runtime error is reported, but an accuracy error occurs |
| 11 | SIMT VF write | Valid static UB | Succeeded | Passed | Valid access |
| 12 | SIMT VF write | Unoccupied ordinary UB space | Failed | - | VEC UB out of bounds |
| 13 | SIMT VF write | Reserved space | Failed | - | VEC UB out of bounds |
| 14 | SIMT VF write | Data Cache | Failed | - | VEC UB out of bounds |

When MainScalar writes the valid static UB range, unoccupied ordinary UB space, and reserved space, no accuracy error is observed, and the log prints the following information:

```text
test pass!
```

When MainScalar writes Data Cache, an accuracy error is observed, and the log prints the following information:

```text
[Failed] Case accuracy verification failed!
```

When SIMT VF writes the valid static UB range, no accuracy error is observed, and the log prints the following information:

```text
test pass!
```

When SIMT VF writes unoccupied ordinary UB space, reserved space, or Data Cache, kernel execution fails and prints the following key error information:

```text
[Case3] mode=simt_vf, region=unallocated_ub
[Host] kernel error: EZ9999: Inner Error!
The extend info: errcode:(341) errorStr: The address for VEC to access UB is out of bounds. subErrType: 0x4.
```

**Analysis**:

Passing the accuracy check only means that no accuracy error is observed in the output data. It does not mean that regions after the allocated static UB can be accessed out of bounds. The [SIMT memory hierarchy](../../../../docs/zh/guide/programming_guide/programming_model/ai_core_simt_programming/memory_hierarchy.md#共享内存大小的限制) states that an access beyond an allocated static or dynamic UB range may not immediately cause an allocation failure, but it can corrupt Data Cache and lead to unstable results or accuracy errors. In the current results, MainScalar writes to Data Cache report no runtime error but cause an accuracy error, while SIMT VF writes to out-of-bounds target regions trigger a VEC UB out-of-bounds error.

**Conclusion**:

When accessing UB, ensure that the access range does not exceed the allocated UB range. Out-of-bounds access to unoccupied ordinary UB space, reserved space, or Data Cache is an invalid UB access. Writing to Data Cache may corrupt the data cache used by SIMT to access GM, leading to unstable results or accuracy errors. The behavior of UB out-of-bounds access differs by execution location: MainScalar out-of-bounds writes report no runtime error but may cause accuracy errors, while SIMT VF out-of-bounds access triggers a VEC UB out-of-bounds error.

## Build and Run

Run the following steps in the sample root directory to build and execute the sample.

- Configure environment variables.

    Configure the environment according to the [CANN development kit installation method](../../../../docs/zh/quick_start.md#prepare&install).

    ```bash
    source ${install_path}/cann/set_env.sh
    ```

  > **Note**: `${install_path}` is the CANN package installation directory. If no installation directory is specified, the default path is `/usr/local/Ascend`.

- Build and run the sample.

    ```bash
    SCENARIO_NUM=3                              # Select a scenario: 0-14
    mkdir -p build && cd build                  # Create and enter the build directory
    cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 \
      -DSCENARIO_NUM=${SCENARIO_NUM} ..         # Configure the project
    make -j                                     # Build the project
    ./ub_usage_limit                            # Run the sample
    ```

**Build options**:

| Option | Valid Value | Description |
| --- | --- | --- |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture. This sample supports only dav-3510 (Ascend 950PR/Ascend 950DT) |
| `SCENARIO_NUM` | `0`-`14` | Scenario number. The default value is `3`. CMake compiles only the Ascend C file of the selected Case |
| `CMAKE_ASC_FLAGS` | ASC compiler options | In Case 1, add `--cce-disable-vf-stack-reserved-ubuf` to disable the 6 KB VF reserve, and add `--cce-disable-asc-reserved-ubuf` to disable the 2 KB Ascend C API reserve |
| `CASE1_OVER_LIMIT` | `ON`, `OFF` | Whether Case 1 allocates 4 B more than the usable UB limit. The default is `OFF` |

The following examples show common build and run commands.

For Case 0, use a 40 KB input size to observe the issue scenario:

```bash
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=0 ..
make -j
./ub_usage_limit 10240
```

Use `SCENARIO_NUM=1` to run the fixed scenario of Case 0:

```bash
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=1 ..
make -j
./ub_usage_limit 10240
```

For Case 1, use `SCENARIO_NUM=2-5` to select the VF-call scenario, and append compiler options that disable reserved UB through `CMAKE_ASC_FLAGS` if needed. The following example calls both a SIMD VF and a SIMT VF with all reserved UB disabled:

```bash
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 \
  -DSCENARIO_NUM=5 \
  -DCMAKE_ASC_FLAGS="--cce-disable-vf-stack-reserved-ubuf --cce-disable-asc-reserved-ubuf" ..
make -j
./ub_usage_limit
```

For Case 3, use `SCENARIO_NUM=7-14` to select the access method and target region. The following example uses MainScalar to write Data Cache:

```bash
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=10 ..
make -j
./ub_usage_limit
```

**Execution result examples**:

The following examples show execution results for each Case.

- For the issue scenario of Case 0, the following information is displayed after execution:

    ```text
    [Case0] mode=baseline
    [Failed] Case accuracy verification failed!
    ```

    For the fixed scenario, the following information is displayed after successful execution:

    ```text
    [Case0] mode=fixed
    test pass!
    ```

- For Case 1, the run succeeds within the theoretical UB limit, and the log prints the following information:

    ```text
    [Case1] result=PASSED
    ```

    After Case 1 allocates one more `uint32_t` element than the theoretical UB limit, kernel launch fails and prints the following key error information:

    ```text
    [Host] kernel error: ... Reason: the sum of dynamic shared memory and kernel shared memory exceeds the maximum limit.
    [ERROR] [Bisheng] Launch kernel failure! ret 107000
    ```

- For Case 2, the following information is displayed after execution:

    ```text
    [Case2] static_total_bytes=7168
    [Case2] main_static_ub=0x0, simd_static_ub=0x400, simt_static_ub=0xc00
    [Case2] main_dynamic_ub=0x1c00, simd_dynamic_ub=0x1c00, simt_dynamic_ub=0x1c00
    [Case2] result=PASSED
    ```

- When MainScalar in Case 3 writes the valid static UB range, unoccupied ordinary UB space, and reserved space, no accuracy error is observed, and the log prints the following information:

    ```text
    [Case3] mode=main_scalar, region=static_ub
    test pass!

    [Case3] mode=main_scalar, region=unallocated_ub
    test pass!

    [Case3] mode=main_scalar, region=reserved_ub
    test pass!
    ```

    When MainScalar writes Data Cache, an accuracy error is observed, and the log prints the following information:

    ```text
    [Case3] mode=main_scalar, region=data_cache
    [Failed] Case accuracy verification failed!
    ```

    When SIMT VF writes an out-of-bounds target region, kernel execution fails and prints the following key error information:

    ```text
    [Case3] mode=simt_vf, region=unallocated_ub
    [Host] kernel error: EZ9999: Inner Error!
    The extend info: errcode:(341) errorStr: The address for VEC to access UB is out of bounds. subErrType: 0x4.
    ```
