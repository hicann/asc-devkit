# 整数除法的快速算法<a name="ZH-CN_TOPIC_INTEGER_FAST_DIV"></a>

>[!NOTE]说明 
>该性能优化建议适用于如下型号：
     <!-- npu="950" id1 -->
>-   Ascend 950PR&950DT系列产品
     <!-- end id1 -->
<!-- @ref: asc-devkit/res/docs/zh/guide/operator_practice/simd_simt_hybrid_optimization/instruction_optimization/fast_integer_division_res.md#id1 -->

【优先级】低

【描述】整数除法指令在AI处理器上相比乘法和移位，执行开销显著更高。当除数固定时，可以使用基于乘法和移位的快速除法算法，在SIMD标量计算逻辑中调用[asc_get_uintdiv_magic_and_shift](../../../../../../docs/zh/api/SIMT-API/math_functions/integer_math_functions/asc_get_uintdiv_magic_and_shift.md)接口根据除数预计算乘法魔数magic和移位量shift，在SIMT并行计算中调用[asc_uintdiv](../../../../../../docs/zh/api/SIMT-API/math_functions/integer_math_functions/asc_uintdiv.md)接口使用乘法和移位操作，得到与普通整数除法一致的结果，从而降低每次除法的执行开销。

【样例介绍】以uint32类型整数除法算子为例，算子功能为将输入input的每个元素除以固定除数divisor，并将结果写入输出output。样例对比在SIMT中直接使用普通除法，以及针对固定除数预计算magic和shift后使用乘法和移位替代普通除法的性能差异。完整的算子实现代码请参考[SIMD与SIMT混合编程实现快速除法算子样例](../../../../../../examples/05_simd_simt_hybrid/02_best_practices/simd_simt_integer_fast_div)。

**表1**  样例规格

| 名称 | name | shape | data type | format |
|:---|:---|:---|:---|:---|
| 算子输入 | input | [8388608] | uint32 | ND |
| 算子输入 | divisor | 标量 | uint32 | - |
| 算子输出 | output | [8388608] | uint32 | ND |

SIMT线程层次结构为：

-   核函数（Kernel）启动线程块数：4096
-   单次SIMT VF调用线程数：2048

【反例】

基于SIMT的普通整数除法实现：对应样例中的场景0（SCENARIO\_NUM=0）。在该场景中SIMT每个线程处理1个元素，并直接使用执行开销较高的普通除法完成计算，代码如下。

```cpp
__simt_vf__ __launch_bounds__(THREAD_LIMIT) inline void simt_normal_div(
    __ubuf__ uint32_t* input,
    __ubuf__ uint32_t* output,
    uint32_t divisor,
    uint32_t total_length)
{
    uint32_t local_idx = threadIdx.x;
    uint32_t idx = blockIdx.x * blockDim.x + local_idx;

    if (idx >= total_length) {
        return;
    }

    uint32_t value = input[local_idx];
    uint32_t result = value / divisor;
    output[local_idx] = result;
}
```

场景0和场景1采用相同的数据搬运和写回流程：每个线程块将负责处理的对应数据从GM搬运到Unified Buffer（UB），在SIMT VF中完成计算并将结果写入UB，随后在核函数（Kernel）中将结果从UB写回GM。两个场景的性能差异主要来自SIMT计算过程中的除法实现方式。

【正例】

基于SIMD与SIMT混合编程的快速除法实现：对应样例中的场景1（SCENARIO\_NUM=1）。当除数固定时，可以在SIMD标量计算逻辑中调用快除预计算接口[asc_get_uintdiv_magic_and_shift](../../../../../../docs/zh/api/SIMT-API/math_functions/integer_math_functions/asc_get_uintdiv_magic_and_shift.md)计算所需的magic和shift，再在SIMT计算中调用快除接口[asc_uintdiv](../../../../../../docs/zh/api/SIMT-API/math_functions/integer_math_functions/asc_uintdiv.md)以乘法和移位组合运算的方式替代普通除法。

对于uint32类型整数除法，可作如下变形，将除法计算转换为乘法和移位操作：

$$
\left\lfloor \frac{n}{d} \right\rfloor =
\left\lfloor \frac{n}{d} \times \frac{2^s}{2^s} \right\rfloor =
\left\lfloor \frac{2^s}{d} \times \frac{n}{2^s} \right\rfloor =
\left\lfloor {m} \times \frac{n}{2^s} \right\rfloor
$$

其中 $n$ 为被除数，$d$ 为除数，$m = \frac{2^s}{d}$。$n$ 除以 $2^s$ 可通过右移 $s$ 位实现。为了避免 $m \times n$ 乘法溢出，将 $m$ 拆分为：

$$
m = (m - 2^{32}) + 2^{32}
$$

令 $magic = m - 2^{32}$，代入上式可得：

$$
\left\lfloor \frac{n}{d} \right\rfloor =
\left\lfloor \frac{n \times {magic} + n \times 2^{32}}{2^s} \right\rfloor =
\left\lfloor \left(\frac{n \times magic}{2^{32}} + n\right) >> (s-32) \right\rfloor =
\left\lfloor \left(\frac{n \times magic}{2^{32}} + n\right) >> shift \right\rfloor
$$

其中magic和shift即为快速除法所需的乘法魔数和移位数。magic和shift的计算只与固定除数有关，可以在SIMD标量计算逻辑中调用[asc_get_uintdiv_magic_and_shift](../../../../../../docs/zh/api/SIMT-API/math_functions/integer_math_functions/asc_get_uintdiv_magic_and_shift.md)接口预计算，在SIMT计算过程中复用，相关代码如下。

```cpp
__global__ __vector__ void integer_div_kernel(
    __gm__ uint32_t* input,
    __gm__ uint32_t* output,
    uint32_t divisor,
    uint32_t total_length)
{
    // 在标量计算逻辑中，根据固定除数预计算magic和shift。
    uint32_t magic = 0;
    uint32_t shift = 0;
    asc_get_uintdiv_magic_and_shift(&magic, &shift, divisor);

    uint32_t block_offset = block_idx * THREAD_COUNT;

    __ubuf__ uint32_t input_buf[THREAD_COUNT];
    __ubuf__ uint32_t output_buf[THREAD_COUNT];
    uint32_t blk_length = THREAD_COUNT * sizeof(uint32_t);
    asc_copy_gm2ub_align(
        input_buf, input + block_offset, 1, blk_length, 0, 0, false, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);

    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

    // 将magic和shift传入SIMT VF，完成快速除法计算。
    asc_vf_call<simt_fast_div>(dim3(THREAD_COUNT), input_buf, output_buf, magic, shift, total_length);

    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);

    asc_copy_ub2gm_align(
        output + block_offset, output_buf, 1, blk_length, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
}
```

在SIMT计算过程中，每个线程读取待处理的被除数value，调用[asc_uintdiv](../../../../../../docs/zh/api/SIMT-API/math_functions/integer_math_functions/asc_uintdiv.md)接口完成乘法和移位组合运算，得到与value / divisor等价的结果。关键代码如下。

```cpp
__simt_vf__ __launch_bounds__(THREAD_LIMIT) inline void simt_fast_div(
    __ubuf__ uint32_t* input,
    __ubuf__ uint32_t* output,
    uint32_t magic,
    uint32_t shift,
    uint32_t total_length)
{
    uint32_t local_idx = threadIdx.x;
    uint32_t idx = blockIdx.x * blockDim.x + local_idx;

    if (idx >= total_length) {
        return;
    }

    uint32_t value = input[local_idx];
    uint32_t result = asc_uintdiv(value, magic, shift);
    output[local_idx] = result;
}
```

【性能对比】

在线程块数相同、输入输出规格相同、均使用SIMD搬运接口完成GM与UB之间数据搬运的情况下，对比场景0和场景1的性能数据如下。

除Task Duration外，其余指标均为所有Thread Block上的平均值。

| 场景 | 实现方式 | 数据量 | Task Duration\(μs\) | aiv\_vec\_time\(μs\) | aiv\_scalar\_time\(μs\) |
|:---|:---|---:|---:|---:|---:|
| 场景0 | 普通除法 | 8388608 | 117.229 | 0.497 | 0.128 |
| 场景1 | 快速除法 | 8388608 | 103.889 | 0.270 | 0.142 |

相比场景0，场景1使用乘法和移位替代普通整数除法，Task Duration从117.229μs降低至103.889μs，端到端耗时下降约11.4%。aiv\_vec\_time从0.497μs降低至0.270μs，下降约45.6%，说明将普通除法替换为乘法和移位后，计算指令耗时明显降低。aiv\_scalar\_time从0.128μs增加至0.142μs，主要原因是快速除法需要在Scalar计算单元中额外计算magic和shift；但Task Duration仍然下降，说明快速除法带来的计算收益可以覆盖Scalar计算开销。

【总结】对于除数固定的SIMT整数除法场景，可以在SIMD标量计算逻辑中调用[asc_get_uintdiv_magic_and_shift](../../../../../../docs/zh/api/SIMT-API/math_functions/integer_math_functions/asc_get_uintdiv_magic_and_shift.md)接口预计算乘法魔数magic和移位量shift，并在SIMT计算过程中调用[asc_uintdiv](../../../../../../docs/zh/api/SIMT-API/math_functions/integer_math_functions/asc_uintdiv.md)接口复用该结果完成快速除法。通过将普通整数除法替换为乘法和移位操作，可以有效降低除法计算开销。
