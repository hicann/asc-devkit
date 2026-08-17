# 算子实现

以下内容以gather & adds算子为例，介绍SIMD与SIMT混合算子的实现过程。完整代码请参见[SIMD与SIMT混合编程实现gather&adds算子样例](../../../../../examples/05_simd_simt_hybrid/00_introduction/simd_simt_gather_and_adds)。

>[!NOTE]说明 
>本样例的目的是通过一个简单的算子实现展示SIMD与SIMT混合编程方式，仅作为功能介绍。

## 算子功能

gather & adds算子从长度为100000的一维向量input中，按index指定的位置提取8192个数据，并对提取后的数据执行加1计算。算子输出output中第i个数据为：

```cpp
output[i] = input[index[i]] + 1
```

计算过程可拆分为gather和adds两个阶段：gather阶段根据index从input中离散读取数据，并将结果写入UB；adds阶段对UB上的连续中间数据执行加1计算，计算结果原地写回UB，最后将结果数据搬运到GM上的output中。

其中，gather阶段包含索引映射和离散访问，使用SIMT表达每个线程的数据访问逻辑；adds阶段处理连续数据，使用SIMD完成向量计算。

## 算子分析

**图 1**  gather & adds算子整体执行流水  
![](../../figures/gather_adds_overall_pipeline.png "gather & adds算子整体执行流水")

gather & adds算子整体执行流水如图1所示。simt\_gather和simd\_adds同属Vector流水，按调用顺序执行；将adds结果从UB搬出到GM属于MTE3搬出流水。由于Vector流水与MTE3搬出流水异步并行执行，MTE3搬出操作可能在Vector流水写入adds结果完成前读取UB数据，因此需在搬出前插入同步指令，确保adds结果写入UB后再启动搬出。

**图 2**  gather & adds算子执行流程  
![](../../figures/gather_adds_execution_flow.png "gather & adds算子执行流程")

gather & adds算子的详细执行流程如图2所示。基于该流程，按照输入输出、核函数参数、执行配置和VF函数参数的顺序分析gather & adds算子的实现方式。

**输入输出分析：** gather & adds算子有两个输入input和index，输出为output。其中，input为原始数据，index表示需要从input中读取的数据位置，output保存读取并加1后的结果。

本样例中，input的数据类型为float，shape为\[100000\]；index的数据类型为uint32\_t，shape为\[8192\]，取值范围为\[0, 100000\)；output的数据类型与input相同，shape与index相同。算子输入输出[format](../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md)均为ND。

**核函数名称和参数：** 根据上述输入输出和执行流程，本样例中核函数命名为gather\_and\_adds\_kernel，函数原型如下。

```cpp
__global__ __vector__ void gather_and_adds_kernel(
    __gm__ float* input, __gm__ uint32_t* index, __gm__ float* output, uint32_t input_total_length,
    uint32_t index_total_length)
```

当前SIMD与SIMT混合编程仅使用Vector Core（AIV核），核函数可使用[\_\_vector\_\_](../../programming_guide/language_extension/simd_builtin_keywords.md#section1074418132518)标识只启动AIV核。

其中，input\_total\_length和index\_total\_length分别表示input和index的长度。gather阶段需要根据index访问input，input\_total\_length用于校验index中保存的input访问位置是否越界，index\_total\_length用于校验线程访问index的位置是否越界。

**执行配置：** 明确SIMT线程配置和分核策略。

本例中每个SIMT线程处理1个数据，单核线程数设置为1024，即单核处理1024个数据。算子输入index长度为8192，核数为8192 / 1024 = 8，单个核调用1次simt\_gather函数即可完成本核的gather运算。

**SIMT VF函数名称和参数：** 根据SIMT线程配置策略，本样例中SIMT VF函数命名为simt\_gather，参数说明如下。

| 参数 | 参数类型 | 说明 |
|:---|:---|:---|
| input | \_\_gm\_\_ float\* | 输入input的GM地址 |
| index | \_\_gm\_\_ uint32\_t\* | 输入index的GM地址 |
| local\_output | \_\_ubuf\_\_ float\* | SIMT gather阶段的输出地址 |
| input\_total\_length | uint32\_t | input的数据长度 |
| index\_total\_length | uint32\_t | index的数据长度 |
| output\_total\_length | uint32\_t | 单核上local\_output的数据长度 |

**SIMD VF函数名称和参数：** 本样例中SIMD VF函数命名为simd\_adds，参数说明如下。

| 参数 | 参数类型 | 说明 |
|:---|:---|:---|
| output | \_\_ubuf\_\_ float\* | SIMD adds阶段的输出地址 |
| input | \_\_ubuf\_\_ float\* | SIMD adds阶段的输入地址 |
| count | uint32\_t | 单核处理的数据总量 |
| one\_repeat\_size | uint32\_t | 每轮处理的float元素个数 |
| repeat\_times | uint16\_t | 完成count个元素所需的循环次数 |

通过以上分析，得到Ascend C gather & adds算子的设计规格如下：

**表 1**  gather & adds算子设计规格

<table>
<tr><td rowspan="1" align="center">算子类型（OpType）</td><td colspan="4" align="center">Gather_Adds</td></tr>
<tr><td rowspan="3" align="center">算子输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">input</td><td align="center">100000</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">index</td><td align="center">8192</td><td align="center">uint32_t</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">算子输出</td><td align="center">output</td><td align="center">8192</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center"><code>gather_and_adds_kernel</code></td></tr>
<tr><td rowspan="1" align="center">SIMT VF函数名</td><td colspan="4" align="center"><code>simt_gather</code></td></tr>
<tr><td rowspan="1" align="center">SIMD VF函数名</td><td colspan="4" align="center"><code>simd_adds</code></td></tr>
</table>

## 核函数定义与实现

根据[核函数](../../programming_guide/advanced_programming/advanced_ai_core_programming_model/simd_simt_hybrid_programming/kernel_and_vf_functions.md#zh-cn_topic_0000002571578013_section156822920311)中介绍的规则进行核函数的定义。本样例中，定义gather\_and\_adds\_kernel核函数，代码如下：

```cpp
__global__ __vector__ void gather_and_adds_kernel(
    __gm__ float* input, __gm__ uint32_t* index, __gm__ float* output, uint32_t input_total_length,
    uint32_t index_total_length)
{
    ...
    // 1. 调用SIMT函数获取指定索引的数据
    asc_vf_call<simt_gather>(
        dim3(THREAD_COUNT), input, index, local_output, input_total_length,
        index_total_length, index_total_length_per_block);

    ...
    // 2. 调用SIMD函数完成加1操作
    asc_vf_call<simd_adds>(
        local_output, local_output, index_total_length_per_block, one_repeat_size, repeat_times);

    // 依赖PIPE_V和PIPE_MTE3流水同步
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);

    // 3. 将数据从UB搬运到GM
    asc_copy_ub2gm_align(output + index_total_length_per_block * block_idx, local_output, 1,
        index_total_length_per_block * sizeof(float), asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
}
```

核函数中先后启动SIMT VF函数simt\_gather和SIMD VF函数simd\_adds：simt\_gather将GM中的离散数据读取到UB，simd\_adds对UB上的gather结果执行加1计算，并将结果原地写回UB。

对于基于SIMD编程计算并写入UB的结果local\_output，调用[asc\_copy\_ub2gm\_align](../../../api/SIMD-API/c_api/vector_data_move/asc_copy_ub2gm_align/asc_copy_ub2gm_align.md)接口搬出到GM前，需要通过[asc\_sync\_notify](../../../api/SIMD-API/c_api/sync/asc_sync_notify.md)/[asc\_sync\_wait](../../../api/SIMD-API/c_api/sync/asc_sync_wait.md)接口控制Vector计算流水和MTE3搬出流水的同步，确保adds计算结果写入UB后再启动搬出。

## SIMT VF实现

根据上述对SIMT VF函数的参数分析，定义SIMT VF函数原型，代码如下：

```cpp
constexpr uint32_t THREAD_COUNT = 1024;

__simt_vf__ __launch_bounds__(THREAD_COUNT) inline void simt_gather(
    __gm__ float* input, __gm__ uint32_t* index, __ubuf__ float* local_output, uint32_t input_total_length,
    uint32_t index_total_length, uint32_t output_total_length)
{
    ...
    // 根据当前核号和线程号计算全局index数组中的元素位置
    uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    ...
    // 读取线程应获取的数据在输入input中的索引
    uint32_t gather_idx = index[idx];
    ...
    // 将input中索引为gather_idx的数据存到UB上
    local_output[threadIdx.x] = input[gather_idx];
}
```

>[!NOTE]说明 
>在SIMT编程中，\_\_launch\_bounds\_\_\(thread\_num\)是可选配置，用于在编译期指定核函数启动的最大线程数（如果不配置，thread\_num默认为1024），使用时请注意：thread\_num \>= x \* y \* z （即：asc\_vf\_call的第一个参数：dim3\{x, y, z\}）, 线程数thread\_num的取值范围为1到2048。最大线程数决定了每个线程可分配的寄存器数量，具体对应关系请见[表5](../../programming_guide/language_extension/simt_builtin_keywords.md#table1715318510594)，寄存器用于存储线程中的局部变量，若局部变量的个数超出寄存器个数，容易出现栈溢出等问题。

使用[\_\_simt\_vf\_\_](../../programming_guide/language_extension/simd_simt_hybrid_builtin_keywords.md#__simt_vf__)函数类型限定符标识SIMT VF函数入口，使其可以被[asc\_vf\_call](../../../api/SIMT-API/SIMD_SIMT_hybrid_programming_intro/extended_syntax/kernel_function_config.md#asc_vf_call调用)调用。

simt\_gather函数实现从输入input中获取指定索引的数据。函数中首先计算线程应处理数据的索引，然后通过赋值操作将数据存储到Unified Buffer上。

本例中核数设置为8，线程的层次结构为\{1024, 1, 1\}，数据总量为8192（8 \* 1024）。每个线程处理一个index元素，全局index位置计算逻辑如下：

```cpp
uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
```

其中，[blockIdx](../../programming_guide/language_extension/simt_builtin_keywords.md#li1676053814914)用于获取当前核id。[blockDim](../../programming_guide/language_extension/simt_builtin_keywords.md#li076017381191)用于获取线程三维层次结构\{x, y, z\}，本例中为\{1024, 1, 1\}，其中第2，3维度均为1，使用一维层次结构，因此线程数可写作blockDim.x。[threadIdx](../../programming_guide/language_extension/simt_builtin_keywords.md#li7760123814919)用于获取三维线程索引\{x, y, z\}，本例中仅使用第1维x，可通过threadIdx.x获取当前线程的id。

## SIMD VF实现

根据上述对SIMD VF函数的参数分析，定义SIMD VF函数原型，代码如下：

```cpp
constexpr float ADDS_ADDEND = 1.0f;

__simd_vf__ inline void simd_adds(
    __ubuf__ float* output, __ubuf__ float* input, uint32_t count, uint32_t one_repeat_size, uint16_t repeat_times)
{
    // 初始化SIMD矢量数据寄存器，源操作数
    vector_float src_reg0;
    // 初始化SIMD矢量数据寄存器，目的操作数
    vector_float dst_reg0;
    // 初始化SIMD矢量计算标记寄存器
    vector_bool mask_reg;

    for (uint16_t i = 0; i < repeat_times; i++) {
        mask_reg = asc_update_mask_b32(count);
        // 从UB搬运数据到SIMD矢量数据寄存器
        asc_loadalign(src_reg0, input + i * one_repeat_size);
        // 调用asc_add_scalar接口完成加1运算
        asc_add_scalar(dst_reg0, src_reg0, ADDS_ADDEND, mask_reg);
        // 从SIMD矢量数据寄存器搬运数据到UB
        asc_storealign(output + i * one_repeat_size, dst_reg0, mask_reg);
    }
}
```

使用\_\_simd\_vf\_\_函数类型限定符标识SIMD VF函数入口，使其可以被[asc\_vf\_call](../../../api/SIMD-API/basic_api/reg_vector_compute/vf_call/asc_vf_call.md)调用。

simd\_adds函数实现对Unified Buffer上的数据做加1计算。函数中循环repeat\_times次完成单核数据处理。input为SIMT阶段写入UB的gather结果，output为加1后的UB输出；本样例调用时input和output均传入local\_output。one\_repeat\_size表示每轮处理的float元素个数，repeat\_times表示完成count个元素所需的循环次数。

循环中，使用[asc_loadalign](../../../api/SIMD-API/c_api/reg/reg_load/asc_loadalign.md)接口将数据从Unified Buffer搬运到SIMD矢量数据寄存器，使用[asc_add_scalar](../../../api/SIMD-API/c_api/reg/arithmetic_compute/asc_add_scalar.md)接口完成数据加1运算，使用[asc_storealign](../../../api/SIMD-API/c_api/reg/reg_store/asc_storealign.md)接口将数据从SIMD矢量数据寄存器搬运到Unified Buffer。
