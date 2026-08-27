# SIMT栈溢出场景及优化样例

## 概述

本样例以Kernel内定义大数组变量、递归函数调用导致SIMT栈溢出为例，介绍Ascend C SIMT编程方式下栈溢出问题的定位方法和优化思路。

样例包含两个Case：

- Case 1：在栈空间中使用大数组导致栈溢出，可通过配置文件扩大栈空间，但更推荐使用UB存储大量临时数据。
- Case 2：使用递归函数时，递归层数过大导致栈溢出，需控制递归层数或使用迭代算法替代递归算法。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本

- \>= CANN 9.1.0

## 目录结构介绍

```text
├── stack_overflow
│   ├── CMakeLists.txt          // 样例构建脚本
│   ├── stack_overflow.asc      // SIMT栈溢出展现和优化样例
│   ├── acl.json                // aclInit配置文件，用于配置SIMT栈大小
│   ├── README_en.md
│   └── README.md
```

## 样例描述

本样例总共包含2个典型栈溢出场景，分别展现和优化SIMT栈溢出问题。

**Case说明**：

| Case   | 核心问题        | 对应核函数                                                                   | 解决方案          |
|--------|-------------|-------------------------------------------------------------------------|---------------|
| Case 1 | 在栈空间中存储过大数据 | simt_stack_overflow                                                     | 通过配置文件增加栈空间大小 |
| Case 2 | 递归函数调用深度过大  | simt_recursive_stack_overflow/simt_recursive_stack_overflow_optimized | 使用迭代算法替代递归算法  |

> **栈溢出问题说明**
>
> SIMT线程的栈空间默认大小有限。当Kernel函数中定义较大的线程局部数组，或函数递归调用深度过大时，SIMT栈空间需求会持续增加。若运行时栈空间需求超过配置上限，就会触发SIMT栈溢出。栈空间详细说明请参考[内存层级](../../../../docs/zh/guide/programming_guide/programming_model/ai_core_simt_programming/memory_hierarchy.md)。

**样例规格**：

<table>
<tr><td rowspan="1" align="center">样例类型（OpType）</td><td colspan="3" align="center">SimtStackOverflow</td></tr>
<tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
<tr><td align="center">input</td><td align="center">[2048]</td><td align="center">int32_t</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">output</td><td align="center">[2048]</td><td align="center">float</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="3" align="center">simt_stack_overflow/simt_recursive_stack_overflow/simt_recursive_stack_overflow_optimized</td></tr>
</table>

## 样例实现

本节以两组Case逐一分析局部大数组和递归调用两类SIMT栈溢出问题。每组Case包含溢出场景和优化场景，并给出对应的核心实现、错误定位方法和优化建议。

---

### Case 1：局部大数组栈溢出及配置栈大小

**样例目标**：展现在栈空间中使用大数组导致的SIMT栈溢出问题，并通过配置更大的SIMT栈空间验证修复。

**核心实现**：使用Kernel函数`simt_stack_overflow`，在每个线程的栈上分配大数组`float local_buffer[320]`，单线程数组占用`320 * 4B = 1280B`。

```cpp
__global__ void simt_stack_overflow(int32_t* input, float* output, uint32_t total_length)
{
    uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= total_length) {
        return;
    }
    float local_buffer[BUFFER_SIZE];

    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        local_buffer[i] = i + 1;
    }

    float sum = 1.0f;
    for (uint32_t i = 0; i < input[idx]; i++) {
        sum *= local_buffer[i];
    }
    output[idx] = sum;
}
```

**执行结果**：

- 精度异常，`[Failed] Case accuracy verification failed!`。

**通过日志信息初步定位问题**：

- Host侧在`aclrtSynchronizeStream()`接口之后调用`aclGetRecentErrMsg()`接口获取Device侧的错误信息，实现如下：

    ```cpp
    const char* err = aclGetRecentErrMsg();
    if (err != nullptr) {
        fprintf(stderr, "[Host] kernel error: %s\n", err);
    } else {
        printf("[Host] kernel completed.\n");
    }
    ```

    再次编译运行，打屏日志如下：

    ```text
    The extend info: errcode:(354) errorStr: The VEC SIMT stack overflows. Possible cause: The local variable is too large or there are too many local variables. subErrType: 0x4.
    ...
    [DFX_INFO]AI Core kernel execution failed, device_id=0, stream_id=61, report_stream_id=61, task_id=0, flip_num=0, fault kernel_name=_Z19simt_stack_overflowPiPfj, fault kernel info ext=_Z19simt_stack_overflowPiPfj, program id=0, hash=136201840501799437.[FUNC:GetError][FILE:stream.cc][LINE:1750]
    ...
    ```

    打屏日志中显示的关键错误信息为`errorStr: The VEC SIMT stack overflows`，表明核函数执行时出现栈溢出，中断退出。

**通过编译选项验证栈溢出问题**

- 为了验证栈溢出，在编译选项中添加`--cce-res-usage`，获取编译器计算的栈空间使用情况，重新编译，有如下编译日志：

    ```text
    [BISHENG] Function properties for _Z19simt_stack_overflowPiPfj_simt_entry: Stack size: 1280 bytes, Used register number: 16
    ```

    日志中的`Stack size: 1280 bytes`，表示核函数变量所需栈空间为`1280B`，默认情况下SIMT栈空间大小为`1152B`，所以确认出现栈溢出。

**分析代码问题**：

- 重新审视代码，其他函数调用和计算逻辑、定义变量都不复杂，只有在栈上定义的大数组`float local_buffer[320]`占用空间为1280B，最有可能导致栈溢出。

**解决方案**：

- 方案1：对于大数组的变量若适合放入UB空间，可迁移到UB空间，减少栈的占用，并且可以在线程间共享。
- 方案2：若确需在栈空间存储大量临时数据，可通过增大SIMT栈空间，以满足Kernel的栈空间需求，从而解决栈溢出问题。

   可通过`aclInit("acl.json")`接口中的`simt_stack_size`字段配置SIMT栈空间大小，代码示例如下：

    ```cpp
    aclInit("acl.json");
    ```

    ```json
    {
        "StackSize": {
            "simt_stack_size": 1280,
            "simt_divergence_stack_size": 512
        }
    }
    ```

    更多配置细节可参考[aclInit](https://hiascend.com/document/redirect/CannCommunityruntimeapiaclinit)接口中的“SIMT算子栈空间大小配置示例”章节，其中SIMT算子每个线程的栈空间大小配置字段为`simt_stack_size`。

    > [!NOTE]注意
    > SIMT栈空间默认大小为1152B。本样例将SIMT的每个线程栈空间配置为1280B，每个Thread Block最大线程数为2048，本样例测试环境可并发执行Thread Block的最大物理核数为72个，因此相比默认配置额外占用的GM空间为`(1280B - 1152B) * 2048 * 72 = 18MB`。

**结论**：

- 优化大数组：核函数实现时避免把大数组、大结构体等放在线程栈上，可将大数组、大结构体存放到UB空间上，减少栈空间占用。
- 增大SIMT栈空间：配置SIMT栈空间大小会急剧增加GM的占用，应结合业务规模评估。

### Case 2：递归栈溢出及迭代优化

**样例目标**：展现递归调用导致的SIMT栈溢出，并通过迭代实现验证修复。

**核心实现**：使用递归算法实现阶乘计算。

```cpp
__aicore__ float recursive_stack_stage(int32_t value)
{
    if (value < 1) {
        return 1.0f;
    }
    return recursive_stack_stage(value - 1) * value;
}

__global__ void simt_recursive_stack_overflow(int32_t* input, float* output, uint32_t total_length)
{
    uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= total_length) {
        return;
    }
    output[idx] = recursive_stack_stage(input[idx]);
}
```

**执行结果**：

- 精度异常，`[Failed] Case accuracy verification failed!`。

**通过日志信息初步定位问题**：

- Host侧在`aclrtSynchronizeStream()`接口之后调用`aclGetRecentErrMsg()`接口获取Device侧的错误信息，实现如下：

    ```cpp
    const char* err = aclGetRecentErrMsg();
    if (err != nullptr) {
        fprintf(stderr, "[Host] kernel error: %s\n", err);
    } else {
        printf("[Host] kernel completed.\n");
    }
    ```

    再次编译运行，打屏日志如下：

    ```text
    The extend info: errcode:(356) errorStr: VEC SIMT DVG stack overflows, which may be caused by too many conditional branches or too manay nested loops. subErrType: 0x4.
    ...
    [DFX_INFO]AI Core kernel execution failed, device_id=0, stream_id=61, report_stream_id=61, task_id=0, flip_num=0, fault kernel_name=_Z29simt_recursive_stack_overflowPiPfj, fault kernel info ext=_Z29simt_recursive_stack_overflowPiPfj, program id=0, hash=136201840501799437.
    ...
    ```

    打屏日志中显示的关键错误信息为`errorStr: VEC SIMT DVG stack overflows`，表明核函数执行时出现分支（Divergence）栈溢出，中断退出。

**通过简化代码确认问题**：

- 删除递归函数的判断分支，代码如下：

    ```cpp
    __aicore__ float recursive_stack_stage(int32_t value)
    {
        return recursive_stack_stage(value - 1) * value;
    }
    ```

    再次编译运行，打屏日志如下：

    ```text
    The extend info: errcode:(354) errorStr: The VEC SIMT stack overflows. Possible cause: The local variable is too large or there are too many local variables. subErrType: 0x4.
    ...
    [DFX_INFO]AI Core kernel execution failed, device_id=0, stream_id=61, report_stream_id=61, task_id=0, flip_num=0, fault kernel_name=_Z29simt_recursive_stack_overflowPiPfj, fault kernel info ext=_Z29simt_recursive_stack_overflowPiPfj, program id=0, hash=11904994319956742437.[FUNC:GetError][FILE:stream.cc][LINE:1750]
    ...
    ```

    打屏日志中显示的关键错误信息为`errorStr: The VEC SIMT stack overflows`，表明核函数执行时出现栈溢出，中断退出。

**分析代码问题**：

- 重新审视代码，核函数中除递归条件的判断和递归调用外无过多的逻辑实现。因此递归场景下if分支被嵌套，导致分支（Divergence）栈溢出。
- 当删除递归函数的判断分支时，出现栈溢出，说明递归场景实际上使通用栈和分支（Divergence）栈都溢出。

**解决方案**：

- 方案1：将递归算法改为迭代算法，减少对SIMT分支（Divergence）栈空间和SIMT栈空间的需求。

    使用迭代算法实现阶乘计算，代码如下：

    ```cpp
    __aicore__ float recursive_stack_stage_optimized(int32_t value)
    {
        float result = 1.0f;
        for (int32_t i = 2; i <= value; ++i) {
            result *= i;
        }
        return result;
    }

    __global__ void simt_recursive_stack_overflow_optimized(int32_t* input, float* output, uint32_t total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= total_length) {
            return;
        }
        output[idx] = recursive_stack_stage_optimized(input[idx]);
    }
    ```

- 方案2：若确定需要使用递归调用，可通过增大SIMT分支（Divergence）栈空间和栈空间，满足Kernel对SIMT分支（Divergence）栈空间和SIMT栈空间的需求，同时要严格控制递归深度。

    更多配置细节可参考[aclInit](https://hiascend.com/document/redirect/CannCommunityruntimeapiaclinit)接口中的“SIMT算子栈空间大小配置示例”章节，其中SIMT算子的分支（Divergence）栈空间大小配置字段为`simt_divergence_stack_size`，SIMT算子每个线程的栈空间大小配置字段为`simt_stack_size`。

> [!NOTE]注意
> 递归调用的场景对栈空间的需求过高，实际开发场景中不建议使用递归调用。

**结论**：

- 优化递归算法：核函数实现时避免使用递归调用，优先将递归方法改为迭代方法计算，减少栈空间和分支（Divergence）栈空间的占用。
- 增大SIMT分支（Divergence）栈空间和栈空间：配置SIMT分支（Divergence）栈空间大小和栈空间大小会增加对GM的占用，应结合业务规模评估。

## 优化建议

| 场景      | 问题原因               | 优化方案                                          |
|---------|--------------------|-----------------------------------------------|
| 大数组局部变量 | Kernel内定义较大的线程局部数组 | 优先迁移到UB空间或减少局部数组规模；确需较大栈空间时可自定义SIMT栈空间大小（不建议） |
| 递归调用    | 递归深度过大导致栈帧持续累积     | 优先改为迭代实现；增加SIMT分支（Divergence）栈空间大小和栈空间大小（不建议）    |
| 临时变量过多  | 临时变量定义过多           | 减少临时变量，能用地址引用的尽量不用临时变量，可减少寄存器的使用              |
| 分支过多    | 调用函数层次过深           | 优化分支处理逻辑，减少Warp内的分支跳转                         |
| 复杂的结构体  | 传入复杂结构体            | 对于复杂的结构体，可以规划UB或GM内存用于存放结构体数据，避免传参过大            |

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量。

    请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

    ```bash
    source ${install_path}/cann/set_env.sh
    ```

  > **说明**：`${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

- 编译并运行样例。

    ```bash
    SCENARIO_NUM=1                       # 选择执行场景，可选1-4
    mkdir -p build && cd build;          # 创建并进入build目录
    cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;  # 编译工程
    ./demo                               # 执行样例
    ```

**编译选项说明**：

| 选项                        | 可选值        | 说明                                                                   |
|---------------------------|------------|----------------------------------------------------------------------|
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构，本样例仅支持dav-3510（Ascend 950PR/Ascend 950DT）                      |
| `SCENARIO_NUM`            | `1-4`      | 样例场景编号，默认值为1。1：Case 1的溢出场景，2：Case 1的优化场景，3：Case 2的溢出场景，4：Case 2的优化场景 |

**执行结果示例**：

- Case 1的栈溢出场景，日志中会出现如下错误信息：

    ```text
    The extend info: errcode:(354) errorStr: The VEC SIMT stack overflows. Possible cause: The local variable is too large or there are too many local variables. subErrType: 0x4.
    ```

- Case 2的栈溢出场景，日志中会出现如下错误信息：

    ```text
    The extend info: errcode:(356) errorStr: VEC SIMT DVG stack overflows, which may be caused by too many conditional branches or too manay nested loops. subErrType: 0x4.
    ```

- Case 1和Case 2的优化验证场景，运行成功后输出如下信息：

    ```text
    test pass!
    ```
