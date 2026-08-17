# 通过SIMT实现分支判断<a name="ZH-CN_TOPIC_0000002552896537"></a>

<!-- npu="950" id1 -->
>[!NOTE]说明 
>该性能优化建议适用于如下型号：
>-   Ascend 950PR/Ascend 950DT
<!-- end id1 -->

【优先级】高

【描述】基于SIMD编程模型实现的批量数据计算性能很高，但在算子实现逻辑中涉及分支判断时，基于SIMD的计算操作会变得相对复杂，导致性能下降。此时，可以考虑采用SIMT方式，因为SIMT编程更为灵活，更适合处理分支判断的场景。

【样例介绍】以floor\_mod算子为例，算子功能为将输入x的每个元素除以输入y的对应元素，获取余数。该余数应与除数y具有相同的符号，且其绝对值应小于y的绝对值。在计算过程中，需要判断y中每个元素的符号以及余数与该元素绝对值的大小关系。完整的算子实现代码请参考[SIMT与SIMD混合编程高性能优化样例](../../../../../../examples/05_simd_simt_hybrid/02_best_practices/simd_simt_high_performance)。

**表1**  样例规格

<table>
<thead>
<tr>
<th>名称</th>
<th>name</th>
<th>shape</th>
<th>data type</th>
<th>format</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="2">算子输入</td>
<td>x</td>
<td>[8192, 8192]</td>
<td>int32</td>
<td>ND</td>
</tr>
<tr>
<td>y</td>
<td>[8192, 8192]</td>
<td>int32</td>
<td>ND</td>
</tr>
<tr>
<td>算子输出</td>
<td>z</td>
<td>[8192, 8192]</td>
<td>int32</td>
<td>ND</td>
</tr>
</tbody>
</table>

SIMT线程层次结构为：

-   Kernel启动核数：64
-   单次SIMT VF调用线程数：1024

【反例】

基于SIMD Reg矢量计算的floor\_mod算子实现：对应样例中的场景1（SCENARIO\_NUM=1）。该场景使用 `asc_copy_gm2ub_align` 和 `asc_copy_ub2gm_align` 完成GM与UB之间的数据搬运，在UB上使用SIMD VF函数实现计算。由于SIMD无法直接通过普通if else语句表达逐元素分支判断，因此需要使用多个矢量计算API完成符号比较、条件组合和结果选择，相关代码如下。

```cpp
__simd_vf__ inline void floor_mod_simd(
    __ubuf__ int32_t* z_addr, __ubuf__ int32_t* x_addr, __ubuf__ int32_t* y_addr, const uint32_t count)
{
    constexpr uint32_t one_repeat_size = asc_get_vf_len() / sizeof(int32_t);
    uint16_t loop_times = ceil_div(count, one_repeat_size);
    vector_int32_t x_value;
    vector_int32_t y_value;
    vector_int32_t mod_value;
    vector_int32_t temp_value;
    vector_int32_t default_value;
    vector_int32_t sign_value;

    vector_bool mask;
    vector_bool select_mask;
    vector_bool adjust_mask;
    uint32_t mask_count = count;

    asc_duplicate_scalar(default_value, int32_t(-1));
    asc_duplicate_scalar(sign_value, static_cast<int32_t>(FMOD_B32_SIGN));

    for (uint16_t i = 0; i < loop_times; i++) {
        mask = asc_update_mask_b32(mask_count);
        asc_loadalign(x_value, x_addr + i * one_repeat_size);
        asc_loadalign(y_value, y_addr + i * one_repeat_size);

        asc_div(temp_value, x_value, y_value, mask);
        asc_mul(temp_value, y_value, temp_value, mask);
        asc_sub(mod_value, x_value, temp_value, mask);

        asc_ne_scalar(select_mask, y_value, int32_t(0), mask);
        asc_select(temp_value, mod_value, default_value, select_mask);

        asc_add(mod_value, temp_value, y_value, mask);
        asc_ne_scalar(adjust_mask, temp_value, int32_t(0), mask);
        asc_and(x_value, temp_value, sign_value, mask);
        asc_and(y_value, y_value, sign_value, mask);
        asc_ne(select_mask, x_value, y_value, mask);
        asc_and(adjust_mask, select_mask, adjust_mask, mask);
        asc_select(mod_value, mod_value, temp_value, adjust_mask);
        asc_storealign(z_addr + i * one_repeat_size, mod_value, mask);
    }
}
```

【正例】

基于SIMT的floor\_mod算子实现：对应样例中的场景3（SCENARIO\_NUM=3）。该场景同样使用搬运接口完成GM与UB之间的数据搬运，在UB上采用SIMT编程方式实现计算过程，通过if else语句完成分支判断，代码如下所示。

```cpp
__simt_vf__ inline void floor_mod_simt_contiguous(
    __ubuf__ int32_t* x, __ubuf__ int32_t* y, __ubuf__ int32_t* z, uint32_t input_total_length)
{
    for (uint32_t index = threadIdx.x; index < input_total_length; index += blockDim.x) {
        int32_t y_value = y[index];
        const int32_t rem = x[index] % y_value;
        bool signs_differ = ((rem < 0) != (y_value < 0));
        if (signs_differ && (rem != 0)) {
            z[index] = rem + y_value;
        } else {
            z[index] = rem;
        }
    }
}
```

【性能对比】

在核数相同、输入输出规格相同、均使用搬运接口完成GM与UB之间数据搬运的情况下，对比场景1和场景3的性能数据如下。

除Task Duration外，其余指标均为所有Thread Block上的平均值。

| 场景 | 实现方式 | 核数 | Task Duration\(μs\) | aiv\_vec\_time\(μs\) | aiv\_vec\_ratio | aiv\_mte2\_time\(μs\) | aiv\_mte2\_ratio |
|:---|:---|:---:|---:|---:|---:|---:|---:|
| 场景1 | SIMD Reg矢量计算 | 64 | 525.736 | 509.987 | 0.989 | 217.341 | 0.421 |
| 场景3 | SIMT连续访问UB | 64 | 463.179 | 301.474 | 0.668 | 437.055 | 0.968 |

相比场景1，场景3使用SIMT直接表达floor\_mod中的分支判断逻辑，aiv\_vec\_time从509.987μs降低至301.474μs，下降约40.9%，说明Vector计算侧耗时明显降低。端到端Task Duration从525.736μs降低至463.179μs，下降约11.9%，场景3的aiv\_mte2\_ratio达到0.968，主要瓶颈已转为MTE2 Bound，即算子性能的最大影响转变为GM到UB的搬运效率。
