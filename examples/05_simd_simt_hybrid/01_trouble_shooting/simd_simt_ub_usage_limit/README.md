# SIMD与SIMT混合编程UB越界定位样例

## 概述

本样例以SIMD与SIMT混合编程场景下UB访问越界导致输出精度异常为例，介绍如何结合UB容量、内存布局和实际访问范围定位并修复UB访问越界。样例随后通过多个Case说明不同VF配置下的UB可用上限、静态UB和动态UB的布局关系，以及越界访问不同UB区域时的运行和精度结果。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本

- \>= CANN 9.1.0

## 目录结构介绍

```text
├── simd_simt_ub_usage_limit
│   ├── CMakeLists.txt                   // cmake编译文件
│   ├── case0_ub_out_of_bounds.asc       // Case 0样例实现
│   ├── case1_ub_limit.asc               // Case 1样例实现
│   ├── case2_ub_layout.asc              // Case 2样例实现
│   ├── case3_out_of_bounds_result.asc   // Case 3样例实现
│   ├── figures                          // README中的图片资源
│   ├── README.md                        // 中文说明
│   └── README_en.md                     // 英文说明
```

## 样例描述

本样例包含4个Case，围绕UB访问越界定位、容量边界和内存布局展开。

**Case说明**：

| Case | 场景 | 主要内容 |
| --- | --- | --- |
| Case 0 | 输出精度异常定位 | 展现输出精度异常，并通过执行状态、计算逻辑和访问范围排查定位原因、验证修复方法 |
| Case 1 | 不同VF场景下的UB可用上限 | 确定不同VF调用、预留空间和Data Cache配置下的UB可用上限 |
| Case 2 | 不同执行位置的静态UB和动态UB布局 | 说明MainScalar、SIMD VF和SIMT VF中的静态UB和动态UB布局关系 |
| Case 3 | 不同执行位置的UB越界访问结果 | 说明MainScalar和SIMT VF越界访问UB区域时的结果 |

**样例规格**：

<table>
<tr><td align="center">样例类型（OpType）</td><td colspan="3" align="center">SimdSimtUbUsageLimit</td></tr>
<tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
<tr><td align="center">input</td><td align="center">[8192]/[10240]/[512]</td><td align="center">uint32_t</td></tr>
<tr><td align="center">样例输出</td><td align="center">output</td><td align="center">[8192]/[10240]/[512]</td><td align="center">uint32_t</td></tr>
<tr><td rowspan="4" align="center">核函数名</td><td align="center">Case 0</td><td colspan="2" align="center"><code>static_ub_out_of_bounds_kernel</code> / <code>static_ub_bias_offset_fixed_kernel</code></td></tr>
<tr><td align="center">Case 1</td><td colspan="2" align="center"><code>ub_limit_main_scalar_kernel</code> / <code>ub_limit_simd_vf_kernel</code> / <code>ub_limit_simt_vf_kernel</code> / <code>ub_limit_simd_simt_vf_kernel</code></td></tr>
<tr><td align="center">Case 2</td><td colspan="2" align="center"><code>ub_layout_kernel</code></td></tr>
<tr><td align="center">Case 3</td><td colspan="2" align="center"><code>scalar_ub_access_result_kernel</code> / <code>simt_vf_ub_access_result_kernel</code></td></tr>
</table>

Case 0可使用`[8192]`或`[10240]`输入输出；Case 3使用`[512]`输入输出。Case 1和Case 2用于验证UB容量和布局，不涉及GM输入输出数据校验。

## 样例实现

本节按照“问题展现与修复、UB可用上限确认、内存布局说明、越界结果分析”的顺序介绍各Case。

本样例通过编译宏`SCENARIO_NUM`控制15个测试场景并归为4类Case。

| Case | SCENARIO_NUM | 场景 |
| --- | --- | --- |
| Case 0 | 0-1 | 展示UB访问范围计算错误引发的精度问题及修复结果 |
| Case 1 | 2-5 | 对比不调用VF、仅调用SIMD VF、仅调用SIMT VF、同时调用SIMD VF和SIMT VF时的UB最大可用上限 |
| Case 2 | 6 | 展示MainScalar、SIMD VF和SIMT VF中的静态UB和动态UB布局关系 |
| Case 3 | 7-14 | 对比MainScalar写目标UB区域和SIMT VF写目标UB区域后的运行及精度结果 |

### Case 0：输出精度异常定位

**样例目标**：

展现UB访问越界导致输出精度异常的问题，并展示通过执行状态、计算逻辑和UB访问范围逐步定位问题的分析过程。

**核心实现**：

Case 0实现对输入数据的简化校准计算。输入`input`表示待校准数据，Kernel根据`input`的低8 bit生成校准值`bias`，根据`input`的最低bit生成`mask`，用于控制是否校准`input`数据。最终计算关系如下：

```text
bias[i] = input[i] & 0xff
mask[i] = input[i] & 1
output[i] = input[i] + bias[i] * mask[i]
```

Kernel按8KB粒度将输入数据拆分为多个数据块，并将申请的静态UB划分为4个UB tile，用于轮流保存各数据块计算过程中的`input`/`mask`和`bias`。处理每个数据块时，SIMT VF先将该数据块的`input`从GM搬入当前使用的UB tile；Kernel随后根据UB中的`input`生成`bias`，并将原`input`区域覆盖为`mask`；最后SIMT VF从GM重新读取原始`input`，从当前UB tile的原`input`区域读取`mask`，从`bias`区域读取`bias`，计算`output`并写回GM。

Case 0的静态UB tile和数据布局如下图所示：

![Case 0静态UB tile与数据布局](./figures/case0_static_ub_layout.png "Case 0静态UB tile与数据布局")

本Case中，`tile_id`表示当前处理的数据块编号，`ub_base`表示当前数据块复用的UB区域起始元素下标。下面按SIMT VF内计算逻辑和Kernel调用流程分块说明关键代码。

SIMT VF负责将当前数据块的`input`搬入UB，并在输出阶段读取GM中的原始`input`和UB中的`mask`、`bias`计算`output`：

```cpp
// SIMT场景下，256KB UB扣除8KB预留空间和32KB Data Cache后，可用静态UB为216KB。
constexpr uint32_t UB_BYTES = 216 * 1024;
constexpr uint32_t UB_WORDS = UB_BYTES / sizeof(uint32_t);
constexpr uint32_t UB_TILE_COUNT = 4;
constexpr uint32_t UB_TILE_WORDS = UB_WORDS / UB_TILE_COUNT;
constexpr uint32_t DATA_TILE_WORDS = 2048; // 每个数据块包含2048个uint32_t元素，即8KB。
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

Kernel中的标量逻辑根据UB中的`input`生成`bias`和`mask`。其中，`bias_base`表示当前数据块对应的`bias`区域起始元素下标：

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

Kernel主循环按数据块依次调用SIMT VF搬入`input`、生成`bias`和`mask`、再调用SIMT VF计算并写回`output`：

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

**执行结果**：

| SCENARIO_NUM | 输入数据量 | 数据块数量 | 执行结果 |
| --- | ---: | ---: | --- |
| 0 | 32KB | 4 | 精度校验通过 |
| 0 | 40KB | 5 | 精度校验失败 |

输入数据量为32KB时，Kernel处理4个数据块，运行后输出如下信息：

```text
test pass!
```

输入数据量为40KB时，Kernel处理5个数据块，输出精度校验失败，运行后输出如下信息：

```text
[Failed] Case accuracy verification failed!
```

**问题定位**：

以下针对输入数据量为40KB时出现的精度异常进行定位。精度问题通常按“执行状态确认、计算逻辑排查、内存踩踏分析”的顺序定位。

1. 确认Kernel是否正常执行。先配置日志落盘路径，并按需设置日志是否打屏。随后检查日志文件中是否出现`Ascend Error`、`EZ9999`、`vector core exception`、`ASSERT`等Kernel启动、同步或AICore执行异常信息。日志环境变量的使用方式可参考[Ascend日志功能样例](../../../01_simd_cpp_api/01_utilities/09_log/README.md)。

    ```bash
    export ASCEND_PROCESS_LOG_PATH=./log       # 日志落盘路径
    export ASCEND_SLOG_PRINT_TO_STDOUT=0       # 控制日志是否打屏
    ./ub_usage_limit 10240                     # 执行输入数据量为40KB的场景
    ```

    执行完成后，`./log`目录下会生成日志文件。当前场景的日志文件中未观察到Kernel启动、同步或AICore执行异常，说明前述精度异常不是由Kernel执行失败直接导致。接下来需要继续排查输出数据不符合预期的原因。

2. 排查Kernel计算逻辑。先核对Host侧Golden与Kernel输出公式是否一致。Host侧输入按全局元素下标生成，Golden按`input + (input & 0xff) * (input & 1)`计算，与Kernel的校准计算一致：

    ```cpp
    input[gm_base + index] = INPUT_BASE_VALUE + gm_base + index;
    const uint32_t input_data = INPUT_BASE_VALUE + gm_base + index;
    const uint32_t expected = input_data + (input_data & 0xff) * (input_data & 1);
    ```

    再核对Kernel中`bias`、`mask`生成和最终输出逻辑。Kernel将`bias`写为`input & 0xff`，并将原`input`区域覆盖为`input & 1`得到`mask`。最终输出阶段重新读取GM中的原始`input`，并按`input + bias * mask`写回GM：

    ```cpp
    // ub_base由Kernel主循环传入，表示当前数据块复用的UB区域起始元素下标。
    static_ub[bias_base + index] = input & 0xff;
    static_ub[ub_base + index] = input & 1;
    const uint32_t input_data = input[gm_base + index];
    const uint32_t mask = static_ub[ub_base + index];
    const uint32_t bias = static_ub[ub_base + BIAS_OFFSET + index];
    output[gm_base + index] = input_data + bias * mask;
    ```

    Host侧Golden和Kernel计算意图一致，输入数据和计算流程没有明显异常；正常运行仍出现精度错误时，需要进一步考虑是否存在内存被踩、数据变脏的问题。

3. 打印中间计算变量。前面已确认Kernel计算公式本身一致，但输出仍不符合预期，因此可以在中间计算位置临时增加打印，观察相关运行时变量和计算结果。

    例如，在`generate_bias_and_mask_values`中`bias_base`计算后临时增加如下打印。`bias_base`表示按当前`tile_id`计算出的`bias`区域起始元素下标：

    ```cpp
    const uint32_t bias_base = tile_id * UB_TILE_WORDS + BIAS_OFFSET;
    printf("tile_id=%u, bias_base=%u\n", tile_id, bias_base);
    ```

    打印结果如下：

    ```text
    [AIV Block 0/1] tile_id=0, bias_base=2048
    [AIV Block 0/1] tile_id=1, bias_base=15872
    [AIV Block 0/1] tile_id=2, bias_base=29696
    [AIV Block 0/1] tile_id=3, bias_base=43520
    [AIV Block 0/1] tile_id=4, bias_base=57344
    ```

    打印结果显示Kernel处理`tile_id = 0`～`3`时，生成`bias`的位置均在静态UB范围内；处理到`tile_id = 4`时，`bias_base`仍按数据块编号继续增长到57344。换算后的写入起始字节偏移为：

    ```text
    bias写入起始字节偏移 = bias_base * 4B = 57344 * 4B = 229376B = 224KB
    ```

    得到写入起始字节偏移后，还需要结合本Case的UB布局，判断对应的实际访问范围。

4. 结合UB布局确认实际访问范围。Case 0申请216KB静态UB，不配置动态UB。默认情况下，其余空间包括8KB预留空间和32KB Data Cache。Data Cache是SIMT专有空间，用于缓存SIMT线程访问GM的数据，空间范围为32KB～128KB。

    ![Case 0 UB实际访问范围](./figures/case0_access_range.png "Case 0 UB实际访问范围")

    Data Cache大小为`min(256KB - 216KB - 0KB - 8KB, 128KB) = 32KB`，起始地址为`256KB - 32KB = 224KB`。前面计算出的`bias`写入范围为224KB～232KB，进入Data Cache所在范围。该访问可能破坏SIMT访问GM时缓存的数据，后续SIMT VF继续读取GM输入并计算输出时，可能出现精度异常。

**解决方案**：

- 生成`bias`时，使用当前数据块复用的UB区域起始下标计算`bias`区域起始位置。

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

- 修复`bias_base`计算后，中间访问回到静态UB声明范围内，输出精度校验通过：

    | SCENARIO_NUM | 输入数据量 | 数据块数量 | 执行结果 |
    | --- | ---: | ---: | --- |
    | 1 | 40KB | 5 | 精度校验通过 |

    ```text
    [Case0] mode=fixed
    test pass!
    ```

**结论**：

出现UB越界访问时，不一定触发Kernel运行错误，日志中也不一定有明确的异常信息。本Case中，`bias`越界写入Data Cache，破坏了SIMT访问GM时缓存的数据，并最终表现为精度异常。因此，定位UB越界导致的精度问题时，除了观察日志结果，还需要结合计算逻辑和运行时参数计算出的实际访问范围一起排查。

Case 0说明了UB访问越界可能表现为精度异常。混合编程场景涉及MainScalar、SIMD VF、SIMT VF等不同执行位置，UB既是各执行位置申请静态或动态内存的空间，也用于VF与MainScalar之间的数据通信。为避免开发者越界使用UB，下面通过Case 1～3说明不同VF配置下的UB可用上限、静态UB与动态UB的布局关系，以及越界访问不同区域时的结果。

### Case 1：不同VF场景下的UB可用上限

**样例目标**：

确定不同VF调用场景下的UB最大可用上限。

**核心实现**：

本Case包含四条路径：不调用VF、仅调用SIMD VF、仅调用SIMT VF、同时调用SIMD VF和SIMT VF。存在VF调用时，分别验证默认配置、关闭VF预留和关闭全部预留时的UB最大可用上限。

单个Vector Core共有256KB UB。存在VF调用时，默认配置下有8KB预留空间（6KB VF预留和2KB Ascend C API预留）；不调用VF的路径，不预留该部分UB空间。`--cce-disable-vf-stack-reserved-ubuf`用于关闭6KB VF预留，`--cce-disable-asc-reserved-ubuf`用于关闭2KB Ascend C API预留。根据[SIMT内存层级](../../../../docs/zh/guide/programming_guide/programming_model/ai_core_simt_programming/memory_hierarchy.md#共享内存大小的限制)，Data Cache是SIMT专有空间，用于缓存SIMT线程访问GM的数据，空间范围为32KB～128KB。包含SIMT VF的混合编程场景下，需要从UB中划分Data Cache；因此，应按“至少保留32KB Data Cache”反推静态UB与动态UB可占用的最大空间。Data Cache实际大小由UB总量扣除静态内存、动态内存和预留空间后的剩余空间计算。

| SCENARIO_NUM | VF调用 | Data Cache约束 | 默认配置场景可用UB大小（预留8KB） | 关闭VF预留场景可用UB大小（预留2KB） | 关闭全部预留场景可用UB大小（预留0KB） |
| --- | --- | ---: | ---: | ---: | ---: |
| 2 | 不调用VF | 无 | 最大256KB | - | - |
| 3 | 仅调用SIMD VF | 无 | 最大248KB | 最大254KB | 最大256KB |
| 4 | 仅调用SIMT VF | 至少32KB | 最大216KB | 最大222KB | 最大224KB |
| 5 | 同时调用SIMD VF和SIMT VF | 至少32KB | 最大216KB | 最大222KB | 最大224KB |

本Case由Ascend C代码根据VF调用场景、预留配置和Data Cache空间计算最大可用UB大小，并声明对应大小的静态UB数组`static_ub`。不同路径使用独立Kernel实现，分别由MainScalar、SIMD VF、SIMT VF或SIMD VF+SIMT VF写入`static_ub`。Kernel访问`static_ub`尾元素并写回一个GM位置，确保上限位置参与当前Kernel访问；Host侧根据Kernel启动/同步结果和错误信息判断当前UB大小是否超过上限：

```cpp
// VF_RESERVED_BYTES：0或6KB；ASC_RESERVED_BYTES：0或2KB；
// DATA_CACHE_BYTES：0或32KB；EXTRA_BYTES用于多申请4B的越界验证。
constexpr uint32_t UB_BYTES = UB_TOTAL_BYTES - VF_RESERVED_BYTES - ASC_RESERVED_BYTES -
                              DATA_CACHE_BYTES + EXTRA_BYTES;
constexpr uint32_t UB_WORDS = UB_BYTES / sizeof(uint32_t);

// 场景1：不调用VF。
__global__ __vector__ void ub_limit_main_scalar_kernel(__gm__ uint32_t* output)
{
    __ubuf__ uint32_t static_ub[UB_WORDS];
    ...
    output[0] = static_ub[UB_WORDS - 1];
}

// 场景2：仅调用SIMD VF。
__global__ __vector__ void ub_limit_simd_vf_kernel(__gm__ uint32_t* output)
{
    __ubuf__ uint32_t static_ub[UB_WORDS];
    ...
    asc_vf_call<simd_fill_static_ub>(static_ub, UB_WORDS);
    ...
    output[0] = static_ub[UB_WORDS - 1];
}

// 场景3：仅调用SIMT VF。
__global__ __vector__ void ub_limit_simt_vf_kernel(__gm__ uint32_t* output)
{
    __ubuf__ uint32_t static_ub[UB_WORDS];
    ...
    asc_vf_call<simt_fill_static_ub>(dim3(THREAD_COUNT), static_ub, UB_WORDS, FILL_VALUE_BASE);
    ...
    output[0] = static_ub[UB_WORDS - 1];
}

// 场景4：同时调用SIMD VF和SIMT VF。
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

**执行结果**：

本Case先按当前场景的理论UB可用上限申请`static_ub`，确认Kernel可以正常启动和运行；随后将`static_ub`的申请大小增加4B，也就是多申请一个`uint32_t`元素，用于观察超过UB上限后的运行结果。此时Kernel仍可完成编译，但启动失败，错误信息提示共享内存大小超过上限。

| SCENARIO_NUM | VF调用 | 上限配置结果 | 上限基础上多申请4B后的结果 |
| --- | --- | --- | --- |
| 2 | 不调用VF | 运行通过 | Kernel启动失败 |
| 3 | 仅调用SIMD VF | 各配置均运行通过 | 各配置均在Kernel启动时失败 |
| 4 | 仅调用SIMT VF | 各配置均运行通过 | 各配置均在Kernel启动时失败 |
| 5 | 同时调用SIMD VF和SIMT VF | 各配置均运行通过 | 各配置均在Kernel启动时失败 |

上限配置运行通过时，输出如下：

```text
[Case1] result=PASSED
```

将`static_ub`的申请大小从当前场景的理论UB可用上限增加到“理论UB可用上限+4B”后，Kernel启动失败，关键错误信息如下：

```text
[Host] kernel error: ... Reason: the sum of dynamic shared memory and kernel shared memory exceeds the maximum limit.
[ERROR] [Bisheng] Launch kernel failure! ret 107000
```

**结论**：

在SIMD与SIMT混合编程场景下，是否调用VF、是否包含SIMT VF以及是否关闭预留空间会影响UB可用上限。存在VF调用时，默认配置下需要扣除6KB VF预留和2KB Ascend C API预留；使用编译选项关闭对应预留空间后，可用UB上限按释放的预留空间重新计算。包含SIMT VF时，还需要从UB中为Data Cache保留空间。实际开发中应根据上述因素确定当前Kernel可使用的UB上限。

> **注意：** 使用[--cce-disable-vf-stack-reserved-ubuf](../../../../docs/zh/guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#ZH-CN_TOPIC_0000002462746461)后，原本预留的6KB SIMD VF栈空间可作为普通UB空间使用，但编译器无法再使用该空间缓存寄存器溢出数据，开发者需确保不发生寄存器溢出。使用[--cce-disable-asc-reserved-ubuf](../../../../docs/zh/guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#ZH-CN_TOPIC_0000002462746461)后，原本用于支持[部分Ascend C API](../../../../docs/zh/guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md#section_reserved_ubuf_api)的2KB UB空间可作为普通UB空间使用，但上述API不可用，调用时会产生编译错误。两个选项的具体限制请参考[3510架构规格](../../../../docs/zh/guide/programming_guide/advanced_programming/hardware_implementation/architecture_spec/npu_arch_3510.md#compile-options-description)。

### Case 2：不同执行位置的静态UB和动态UB布局

**样例目标**：

说明MainScalar、SIMD VF和SIMT VF中声明的静态UB共同占用当前Kernel的静态内存区域，统计UB用量时需要合计；同时说明三处名称不同的动态UB声明具有相同首地址，共用Kernel启动时配置的动态UB空间。

**核心实现**：

Case 2分别在MainScalar、SIMD VF和SIMT VF中声明静态UB。根据[SIMD与SIMT混合编程内存层级](../../../../docs/zh/guide/programming_guide/advanced_programming/advanced_ai_core_programming_model/simd_simt_hybrid_programming/memory_hierarchy.md#ub划分)，混合编程模式下可在这三个执行空间申请静态内存，动态内存位于静态内存之后；静态UB的实际排布请参考该文档。MainScalar声明1KB的`main_static_ub`，SIMD VF声明2KB静态UB，SIMT VF声明4KB静态UB。本Case中三处声明大小均满足默认要求，因此静态内存区域占用为：

```text
静态内存区域占用 = 1KB + 2KB + 4KB = 7168B
```

MainScalar、SIMD VF和SIMT VF分别访问各自声明的静态UB数组，使静态UB声明参与当前Kernel布局。以下代码展示MainScalar、SIMD VF和SIMT VF中的静态UB和动态UB声明：

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

MainScalar、SIMD VF和SIMT VF分别将各自静态UB数组和动态UB数组的首地址转换为整数并写回GM，由Host侧打印并比较。打印的地址属于AICore UB地址空间，仅用于本Case中的布局比较。

**执行结果**：

本Case中MainScalar、SIMD VF和SIMT VF的静态UB首地址分别为`0x0`、`0x400`和`0xc00`；静态内存区域总占用为7168B，换算为十六进制是`0x1c00`。动态UB位于静态内存之后，因此首地址为`0x1c00`。运行结果如下：

| SCENARIO_NUM | 场景 | 执行结果 |
| --- | --- | --- |
| 6 | 静态UB和动态UB布局 | 运行通过 |

```text
[Case2] static_total_bytes=7168
[Case2] main_static_ub=0x0, simd_static_ub=0x400, simt_static_ub=0xc00
[Case2] main_dynamic_ub=0x1c00, simd_dynamic_ub=0x1c00, simt_dynamic_ub=0x1c00
[Case2] result=PASSED
```

本Case中三处静态UB首地址按声明大小依次累加；动态UB首地址均为`0x1c00`，与本Case静态内存区域占用对应的地址一致。

**结论**：

MainScalar、SIMD VF和SIMT VF中声明的静态UB共同占用当前Kernel的静态内存区域，统计UB用量时需要合计。动态UB位于静态内存之后；MainScalar、SIMD VF和SIMT VF声明的动态UB名称不同，但首地址相同，共用Kernel启动时配置的动态UB空间。更多UB排布说明请参考[SIMD与SIMT混合编程内存层级](../../../../docs/zh/guide/programming_guide/advanced_programming/advanced_ai_core_programming_model/simd_simt_hybrid_programming/memory_hierarchy.md#ub划分)。

### Case 3：不同执行位置的UB越界访问结果

**样例目标**：

申请一块静态UB空间，构造MainScalar和SIMT VF越界访问UB区域的场景，观察不同执行位置越界访问UB区域的结果。

**核心实现**：

申请4KB静态UB数组`static_ub`，前2KB存放输入数据`input`，后2KB存放用于计算输出的校准值`bias`。`output`写回GM，计算关系为`output = input + bias`。Kernel先在申请静态UB内准备`input`和`bias`，再选择不同执行位置访问目标UB区域。

对于越界访问目标UB区域，本Case构造MainScalar写和SIMT VF写两个场景。MainScalar写场景中，MainScalar将`bias`额外写入目标UB区域，随后SIMT VF仍从`static_ub`中读取`bias`计算输出；SIMT VF写场景中，SIMT VF将`bias`额外写入目标UB区域，再从`static_ub`中读取`bias`并计算输出。

Case 3未添加关闭预留空间的编译选项，不配置动态UB。在该配置下，Data Cache达到128KB上限。根据[3510架构规格](../../../../docs/zh/guide/programming_guide/advanced_programming/hardware_implementation/architecture_spec/npu_arch_3510.md#compile-options-description)和[SIMT内存层级](../../../../docs/zh/guide/programming_guide/programming_model/ai_core_simt_programming/memory_hierarchy.md)，Data Cache之前还包含8KB预留空间。Case 3的UB布局计算如下：

```text
Data Cache大小 = min(256KB - 4KB - 0KB - 8KB, 128KB) = 128KB
Data Cache起点 = 256KB - 128KB = 128KB
预留空间起点 = 128KB - 8KB = 120KB
未被占用的普通UB空间大小 = 120KB - 4KB = 116KB
```

`static_ub`的合法访问范围为前4KB。为对比不同访问位置的结果，样例覆盖静态UB合法范围、未被占用的普通UB空间、预留空间和Data Cache四个目标区域。Host侧将目标区域的字节偏移除以4B，得到`static_ub`数组下标`access_index`并传入Kernel：

![Case 3 UB布局与访问位置](./figures/case3_ub_layout.png "Case 3 UB布局与访问位置")

四个目标区域的访问范围如下。`access_index`表示目标起始偏移对应的`uint32_t`数组下标。

| 目标区域 | 目标起始偏移 | `access_index` | 2KB目标范围 |
| --- | ---: | ---: | --- |
| 静态UB | 2048B | 512 | [2KB, 4KB) |
| 未被占用的普通UB空间 | 63488B | 15872 | [62KB, 64KB) |
| 预留空间 | 122880B | 30720 | [120KB, 122KB) |
| Data Cache | 131072B | 32768 | [128KB, 130KB) |

Case 3通过`SCENARIO_NUM`选择访问方式和目标区域：

| SCENARIO_NUM | 访问方式 | 目标区域 |
| --- | --- | --- |
| 7 / 11 | MainScalar写 / SIMT VF写 | 静态UB |
| 8 / 12 | MainScalar写 / SIMT VF写 | 未被占用的普通UB空间 |
| 9 / 13 | MainScalar写 / SIMT VF写 | 预留空间 |
| 10 / 14 | MainScalar写 / SIMT VF写 | Data Cache |

Kernel根据运行时下标`access_index`访问选定目标区域，关键代码如下。公共准备阶段先由SIMT VF将`input`和`bias`写入`static_ub`：

```cpp
constexpr uint32_t UB_BYTES = 4 * 1024;
constexpr uint32_t UB_WORDS = UB_BYTES / sizeof(uint32_t);
constexpr uint32_t DATA_WORDS = 512; // 每次处理2KB input，并生成2KB bias。
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

MainScalar访问场景中，MainScalar将`bias`额外写入目标UB区域：

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

SIMT VF访问场景中，SIMT VF将`bias`额外写入目标UB区域：

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

写入目标UB区域后，SIMT VF从`static_ub`中的`bias`区域读取数据并计算输出：

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

**执行结果**：

不同访问配置的执行结果如下：

| SCENARIO_NUM | 访问方式 | 目标区域 | Kernel运行结果 | 精度结果 | 结果说明 |
| --- | --- | --- | --- | --- | --- |
| 7 | MainScalar写 | 静态UB合法范围 | 成功 | 通过 | 合法访问 |
| 8 | MainScalar写 | 未被占用的普通UB空间 | 成功 | 通过 | 已越界，本次运行未出现可见异常 |
| 9 | MainScalar写 | 预留空间 | 成功 | 通过 | 已越界，本次运行未出现可见异常 |
| 10 | MainScalar写 | Data Cache | 成功 | 失败 | 未报告运行错误，但出现精度异常 |
| 11 | SIMT VF写 | 静态UB合法范围 | 成功 | 通过 | 合法访问 |
| 12 | SIMT VF写 | 未被占用的普通UB空间 | 失败 | - | VEC访问UB越界 |
| 13 | SIMT VF写 | 预留空间 | 失败 | - | VEC访问UB越界 |
| 14 | SIMT VF写 | Data Cache | 失败 | - | VEC访问UB越界 |

MainScalar写静态UB合法范围、未被占用的普通UB空间和预留空间时，运行未观察到精度异常，日志中输出如下信息：

```text
test pass!
```

MainScalar写Data Cache时，运行出现精度异常，日志中输出如下信息：

```text
[Failed] Case accuracy verification failed!
```

SIMT VF写静态UB合法范围时，运行未观察到精度异常，日志中输出如下信息：

```text
test pass!
```

SIMT VF写未被占用的普通UB空间、预留空间和Data Cache时，Kernel运行失败，关键错误信息如下：

```text
[Case3] mode=simt_vf, region=unallocated_ub
[Host] kernel error: EZ9999: Inner Error!
The extend info: errcode:(341) errorStr: The address for VEC to access UB is out of bounds. subErrType: 0x4.
```

**分析**：

精度校验通过只能说明输出数据未观察到精度异常，不能说明申请的静态UB后面的区域可以越界访问。[SIMT内存层级](../../../../docs/zh/guide/programming_guide/programming_model/ai_core_simt_programming/memory_hierarchy.md#共享内存大小的限制)指出，访问超出已申请的静态UB或动态UB范围后，不一定立即表现为内存分配失败，但可能破坏Data Cache并导致计算结果不稳定或精度异常。当前结果中，MainScalar写Data Cache未报告运行错误但出现精度异常；SIMT VF写越界目标区域则触发VEC访问UB越界错误。

**结论**：

访问UB时，应确保访问范围不超过已申请的UB范围。越界访问未被占用的普通UB空间、预留空间和Data Cache，都属于错误访问；其中写入Data Cache可能破坏SIMT访问GM使用的数据缓存，导致结果不稳定或精度异常。不同执行位置对UB越界访问的表现不同：MainScalar越界写不报告运行错误但可能出现精度异常，SIMT VF越界访问则会触发VEC访问UB越界错误。

## 编译运行

在本样例根目录下执行如下步骤，编译并运行样例。

- 配置环境变量。

    请根据当前环境中CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

    ```bash
    source ${install_path}/cann/set_env.sh
    ```

  > **说明**：`${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

- 编译并运行样例。

    ```bash
    SCENARIO_NUM=3                              # 选择场景，可选0~14
    mkdir -p build && cd build                  # 创建并进入build目录
    cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 \
      -DSCENARIO_NUM=${SCENARIO_NUM} ..         # 配置工程
    make -j                                     # 编译工程
    ./ub_usage_limit                            # 运行样例
    ```

**编译选项说明**：

| 选项 | 可选值 | 说明 |
| --- | --- | --- |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构，本样例仅支持dav-3510（Ascend 950PR/Ascend 950DT） |
| `SCENARIO_NUM` | `0`~`14` | 场景编号，默认值为`3`。CMake根据该选项只编译对应Case的Ascend C文件 |
| `CMAKE_ASC_FLAGS` | ASC编译选项 | Case 1中可通过追加`--cce-disable-vf-stack-reserved-ubuf`关闭6KB VF预留，追加`--cce-disable-asc-reserved-ubuf`关闭2KB Ascend C API预留 |
| `CASE1_OVER_LIMIT` | `ON`、`OFF` | Case 1是否在可用UB上限基础上多申请4B，默认值为`OFF` |

以下给出几个常用场景的编译运行示例。

Case 0输入数据量为40KB时，可观察问题场景：

```bash
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=0 ..
make -j
./ub_usage_limit 10240
```

使用`SCENARIO_NUM=1`可运行Case 0修复场景：

```bash
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=1 ..
make -j
./ub_usage_limit 10240
```

Case 1通过`SCENARIO_NUM=2~5`选择VF调用场景，并可通过`CMAKE_ASC_FLAGS`追加关闭预留空间的编译选项。以“同时调用SIMD VF和SIMT VF，并关闭全部预留”为例：

```bash
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 \
  -DSCENARIO_NUM=5 \
  -DCMAKE_ASC_FLAGS="--cce-disable-vf-stack-reserved-ubuf --cce-disable-asc-reserved-ubuf" ..
make -j
./ub_usage_limit
```

Case 3通过`SCENARIO_NUM=7~14`选择访问方式和目标区域。以MainScalar写Data Cache为例：

```bash
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=10 ..
make -j
./ub_usage_limit
```

**执行结果示例**：

以下展示各Case的执行结果示例。

- Case 0问题场景运行后输出如下信息：

    ```text
    [Case0] mode=baseline
    [Failed] Case accuracy verification failed!
    ```

    修复场景运行成功后输出如下信息：

    ```text
    [Case0] mode=fixed
    test pass!
    ```

- Case 1在理论UB上限内运行通过，日志中输出如下信息：

    ```text
    [Case1] result=PASSED
    ```

    在理论UB上限基础上多申请一个`uint32_t`元素后，Kernel启动失败，关键错误信息如下：

    ```text
    [Host] kernel error: ... Reason: the sum of dynamic shared memory and kernel shared memory exceeds the maximum limit.
    [ERROR] [Bisheng] Launch kernel failure! ret 107000
    ```

- Case 2运行后输出如下信息：

    ```text
    [Case2] static_total_bytes=7168
    [Case2] main_static_ub=0x0, simd_static_ub=0x400, simt_static_ub=0xc00
    [Case2] main_dynamic_ub=0x1c00, simd_dynamic_ub=0x1c00, simt_dynamic_ub=0x1c00
    [Case2] result=PASSED
    ```

- Case 3的MainScalar写静态UB合法范围、未被占用的普通UB空间和预留空间时，运行未观察到精度异常，日志中输出如下信息：

    ```text
    [Case3] mode=main_scalar, region=static_ub
    test pass!

    [Case3] mode=main_scalar, region=unallocated_ub
    test pass!

    [Case3] mode=main_scalar, region=reserved_ub
    test pass!
    ```

    MainScalar写Data Cache时，运行出现精度异常，日志中输出如下信息：

    ```text
    [Case3] mode=main_scalar, region=data_cache
    [Failed] Case accuracy verification failed!
    ```

    SIMT VF写越界目标区域时，Kernel运行失败，关键错误信息如下：

    ```text
    [Case3] mode=simt_vf, region=unallocated_ub
    [Host] kernel error: EZ9999: Inner Error!
    The extend info: errcode:(341) errorStr: The address for VEC to access UB is out of bounds. subErrType: 0x4.
    ```
