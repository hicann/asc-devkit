---
okftype: reference.concept
description: "介绍Reg矢量计算C API的寄存器级编程模型、Vector Function使用流程与通用约束，并给出搬入计算搬出的完整编程示例。"
---
# 概述

## Reg矢量计算概述

Reg矢量计算C API提供面向Reg矢量计算架构的寄存器级矢量计算能力，用户可以通过该API直接对芯片中涉及Vector计算的寄存器进行操作，实现更大的灵活性和更好的性能。Reg矢量计算C API与Memory矢量计算C API功能相似，但与Memory矢量计算C API的输入和输出数据通常位于Unified Buffer（UB）不同，Reg矢量计算C API的输入或输出数据可使用矢量数据寄存器`vector_<dtype>`（`dtype`为占位符，表示`half`、`float`等C API支持的数据类型）和掩码寄存器`vector_bool`。对于计算类API，其功能是从给定的寄存器获取数据，进行计算，并将结果保存在给定的寄存器。对于搬运类API，其功能是实现UB和寄存器的数据搬运。由此可见，Reg矢量计算C API相较于Memory矢量计算C API，将数据搬运和Reg计算过程交给用户自主控制，从而实现更大的开发自由度。

### 如何使用Reg矢量计算API

基于寄存器的编程模型是指将数据从UB通过Reg搬运指令（Reg数据搬入）加载到寄存器中，进行复杂的数学计算后通过Reg搬运指令（Reg数据搬出）搬出到UB中，所有的计算逻辑均在寄存器中完成，从而减少UB中间数据的反复读写，大大提升整体性能，具体流程如下所示：

**图1**  Reg矢量计算

![](../../../figures/reg_vector_overview.png "Reg矢量计算")

以[Reg矢量计算编程示例](#reg矢量计算编程示例)为例，完整的Vector Function计算过程由以下几部分组成：

- 编写和调用Vector Function（向量函数）。这些函数使用`__simd_vf__`标记，可在核函数（Kernel）中调用；
- 定义矢量数据寄存器`vector_<dtype>`、掩码寄存器`vector_bool`，更多Reg数据类型请参见[Reg数据类型定义](../defs/type/data_type_definition.md)；
- 编写循环处理多个VL长度的数据；[asc_update_mask](./reg_mask/asc_update_mask.md)系列接口用于更新参与计算的mask，每次循环都会消耗一个VL长度的元素；
- 循环内调用Reg数据搬入接口连续对齐搬入（[asc_loadalign](./load/asc_loadalign.md)）从UB中搬入单个VL长度数据；
- 循环内调用Reg计算接口[asc_add](./reg_arith/asc_add.md)完成单次Repeat计算；
- 循环内调用Reg数据搬出接口连续对齐搬出（[asc_storealign](./store/asc_storealign.md)）往UB中搬出计算后的数据。

### Reg矢量计算编程示例

```cpp
constexpr uint32_t ELEMENT_COUNT = 64;

__simd_vf__ inline void add_vf(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1)
{
    vector_float dst_reg;
    vector_float src0_reg;
    vector_float src1_reg;
    uint32_t count = ELEMENT_COUNT;
    vector_bool mask = asc_update_mask_b32(count);
    asc_loadalign(src0_reg, src0);
    asc_loadalign(src1_reg, src1);
    asc_add(dst_reg, src0_reg, src1_reg, mask);
    asc_storealign(dst, dst_reg, mask);
}

__global__ __vector__ void asc_add_kernel(__gm__ float* dst, __gm__ float* src0, __gm__ float* src1)
{
    asc_init();
    __ubuf__ float dst_local[ELEMENT_COUNT];
    __ubuf__ float src0_local[ELEMENT_COUNT];
    __ubuf__ float src1_local[ELEMENT_COUNT];

    // GM数据搬运至UB地址src0_local及src1_local
    asc_copy_gm2ub_align(src0_local, src0, ELEMENT_COUNT * sizeof(float));
    asc_copy_gm2ub_align(src1_local, src1, ELEMENT_COUNT * sizeof(float));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

    // 调用vf函数
    asc_vf_call<add_vf>(dst_local, src0_local, src1_local);

    // UB地址dst_local数据搬运至GM
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(dst, dst_local, ELEMENT_COUNT * sizeof(float));
    asc_sync();
}
```

>[!NOTE] 说明
>- GM与UB间的数据搬运需通过C API数据搬运接口完成；
>- Vector Function的流水类型为PIPE_V，Vector Function内部如存在UB地址重叠或跨流水依赖，需要根据具体接口约束插入[asc_mem_bar](./reg_sync/asc_mem_bar.md)。

### 通用约束

- 通过引用参数输出结果的Reg矢量计算C API，在非AIV上调用时直接返回。
- 通过函数返回值输出结果的Reg矢量计算C API，在非AIV上调用时返回对应矢量类型的默认构造值。
- Reg矢量计算C API接口需要在Vector Function（`__simd_vf__` 标记的函数）内调用，不支持在`__aicore__`函数中直接调用，调用关系请参考[SIMD BuiltIn关键字](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)。
- 对于支持配置`mask`参数的Reg矢量计算C API接口，`mask`需通过[掩码设置接口](./reg_mask/reg_mask.md)预先赋值后再传入，未赋值的掩码寄存器内容不确定，会导致有效元素位置错误。

## API列表

### VF调用

**表1** VF调用接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_vf_call](vf_call/asc_vf_call.md) | 在Kernel中启动Vector Function（VF）子任务，VF函数内可调用Reg矢量计算C API。 | `utils/base/helpers.h` |

### Reg数据搬入

Reg数据搬入接口用于将Unified Buffer（UB）中的数据搬入矢量数据寄存器、掩码寄存器或非对齐寄存器。具体接口介绍请参考：[Reg数据搬入概述](load/reg_load_overview.md)。

### Reg聚合搬入

**表2** Reg聚合搬入接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_gather](ub_gather/asc_gather.md) | 根据索引位置`index`将源操作数`src`按元素收集到目的操作数`dst`中（UB源收集模式）。 | `c_api/reg_compute/gather/ub_gather.h` |
| [asc_gather_datablock](ub_gather/asc_gather_datablock.md) | 给定源操作数在UB中的基地址和索引，根据索引位置将源操作数按DataBlock收集到目的操作数中。 | `c_api/reg_compute/gather/ub_gather.h` |

### Reg数据搬出

Reg数据搬出接口用于将矢量数据寄存器、掩码寄存器或非对齐寄存器中的数据写入Unified Buffer（UB）。具体接口介绍请参考：[Reg数据搬出概述](store/reg_store_overview.md)。

### Reg离散搬出

**表3** Reg离散搬出接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_scatter](scatter/asc_scatter.md) | 根据索引值`index`将源操作数`src`中的元素分散到目的操作数`dst`中。 | `c_api/reg_compute/scatter/ub_scatter.h` |

### Reg间搬运

**表4** Reg间搬运接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_copy](reg_copy/asc_copy.md) | 完成数据在寄存器内的搬运。 | `c_api/reg_compute/reg_copy.h` |
| [asc_extract_mask](reg_copy/asc_extract_mask.md) | 从矢量数据寄存器中提取掩码寄存器，通过函数返回值返回结果。 | `c_api/reg_compute/reg_copy.h` |
| [asc_mask](reg_copy/asc_mask.md) | 通过函数返回值返回矢量数据寄存器或掩码寄存器的掩码复制结果。 | `c_api/reg_compute/reg_copy.h` |

### 基础算术

基础算术接口提供寄存器级的逐元素算术运算。具体接口介绍请参考：[基础算术概述](reg_arith/overview.md)。

### 逻辑计算

**表5** 逻辑计算接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_and](reg_logic/asc_and.md) | 根据`mask`对源操作数`src0`、`src1`执行按位与（&）操作，将结果作为返回值返回或写入目的操作数`dst`。 | `c_api/reg_compute/compute/reg_logic.h` |
| [asc_not](reg_logic/asc_not.md) | 根据`mask`对源操作数`src`执行按位取反操作，将结果作为返回值返回或写入目的操作数`dst`。 | `c_api/reg_compute/compute/reg_logic.h` |
| [asc_or](reg_logic/asc_or.md) | 根据`mask`对源操作数`src0`、`src1`执行按位或（\|）操作，将结果作为返回值返回或写入目的操作数`dst`。 | `c_api/reg_compute/compute/reg_logic.h` |
| [asc_shiftleft](reg_logic/asc_shiftleft.md) | 根据掩码`mask`对输入数据`src0`，按照`src1`对应元素进行左移操作，完成后将结果写入`dst`中。 | `c_api/reg_compute/compute/reg_logic.h` |
| [asc_shiftleft_scalar](reg_logic/asc_shiftleft_scalar.md) | 根据`mask`对源操作数`src`执行左移，左移的位数由输入参数`value`决定，将结果写入目的操作数`dst`。 | `c_api/reg_compute/compute/reg_logic.h` |
| [asc_shiftright](reg_logic/asc_shiftright.md) | 根据掩码`mask`对输入数据`src0`，按照`src1`对应元素进行右移操作，完成后将结果写入`dst`中。 | `c_api/reg_compute/compute/reg_logic.h` |
| [asc_shiftright_scalar](reg_logic/asc_shiftright_scalar.md) | 根据`mask`对源操作数`src`执行右移，右移的位数由输入参数`value`决定，将结果写入目的操作数`dst`。 | `c_api/reg_compute/compute/reg_logic.h` |
| [asc_xor](reg_logic/asc_xor.md) | 根据`mask`对输入的`src0`、`src1`按元素异或（^）进行操作，将结果写入`dst`。 | `c_api/reg_compute/compute/reg_logic.h` |

### 归约计算

**表6** 归约计算接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_pair_reduce_sum](reg_reduce/asc_pair_reduce_sum.md) | 根据`mask`将源操作数`src`中每两个相邻元素相加，得到计算结果。结果连续保存在目的操作数`dst`的低半部分，`dst`的高半部分置0。 | `c_api/reg_compute/compute/reg_reduce.h` |
| [asc_reduce_max](reg_reduce/asc_reduce_max.md) | 根据`mask`对源操作数`src`进行归约最大值操作。结果保存在`dst`的第0个元素，最大值在`src`中的索引原始位模式保存在第1个元素，其余元素置0；存在多个最大值时保留最小索引。 | `c_api/reg_compute/compute/reg_reduce.h` |
| [asc_reduce_max_datablock](reg_reduce/asc_reduce_max_datablock.md) | 根据`mask`对每个`DataBlock`（32B）内的元素求最大值，得到各`DataBlock`的归约结果。结果依次保存在`dst`的低位。 | `c_api/reg_compute/compute/reg_reduce.h` |
| [asc_reduce_min](reg_reduce/asc_reduce_min.md) | 根据`mask`对源操作数`src`进行归约最小值操作。结果保存在`dst`的第0个元素，最小值在`src`中的索引原始位模式保存在第1个元素，其余元素置0；存在多个最小值时保留最小索引。 | `c_api/reg_compute/compute/reg_reduce.h` |
| [asc_reduce_min_datablock](reg_reduce/asc_reduce_min_datablock.md) | 根据`mask`对每个`DataBlock`（32B）内的元素求最小值，得到各`DataBlock`的归约结果。结果依次保存在`dst`的低位。 | `c_api/reg_compute/compute/reg_reduce.h` |
| [asc_reduce_sum](reg_reduce/asc_reduce_sum.md) | 根据`mask`将源操作数`src`中参与计算的元素求和，归约结果保存在`dst`的第0个元素。 | `c_api/reg_compute/compute/reg_reduce.h` |
| [asc_reduce_sum_datablock](reg_reduce/asc_reduce_sum_datablock.md) | 根据`mask`将每个`DataBlock`（32B）中参与计算的元素相加，得到各`DataBlock`的归约结果。结果依次保存在`dst`的低位。 | `c_api/reg_compute/compute/reg_reduce.h` |

### 聚合操作

**表7** 聚合操作接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_gather](reg_gather/asc_gather.md) | 根据索引位置`index`将源操作数`src`按元素收集到目的操作数`dst`中（寄存器源收集模式）。 | `c_api/reg_compute/compute/reg_gather.h` |

### 广播操作

**表8** 广播操作接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_duplicate](reg_broadcast/asc_duplicate.md) | 根据`mask`将源操作数`src`的最低位元素填充到目的操作数`dst`。 | `c_api/reg_compute/compute/reg_broadcast.h` |
| [asc_duplicate_highest](reg_broadcast/asc_duplicate_highest.md) | 根据`mask`将源操作数`src`的最高位元素填充到目的操作数`dst`，`dst`中未被`mask`筛选的元素置为0。 | `c_api/reg_compute/compute/reg_broadcast.h` |
| [asc_duplicate_highest_merge](reg_broadcast/asc_duplicate_highest_merge.md) | 根据`mask`将源操作数`src`的最高位元素填充到目的操作数`dst`，`dst`中未被`mask`筛选的元素保留原值。 | `c_api/reg_compute/compute/reg_broadcast.h` |
| [asc_duplicate_merge](reg_broadcast/asc_duplicate_merge.md) | 根据`mask`将源操作数`src`的最低位元素填充到目的操作数`dst`，`dst`中未被`mask`筛选的元素保留原值。 | `c_api/reg_compute/compute/reg_broadcast.h` |
| [asc_duplicate_scalar](reg_broadcast/asc_duplicate_scalar.md) | 根据`mask`将`value`填充到目的操作数`dst`。 | `c_api/reg_compute/compute/reg_broadcast.h` |
| [asc_duplicate_scalar_merge](reg_broadcast/asc_duplicate_scalar_merge.md) | 根据`mask`将`value`填充到目的操作数`dst`，`dst`中未被`mask`筛选的元素保留原值。 | `c_api/reg_compute/compute/reg_broadcast.h` |

### 复合计算

**表9** 复合计算接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_abs_sub](reg_fused/asc_abs_sub.md) | 逐元素计算差的绝对值。 | `c_api/reg_compute/compute/reg_fused.h` |
| [asc_axpy](reg_fused/asc_axpy.md) | 根据`mask`对源操作数`src`、`value`进行按元素做乘加操作，将结果写入目的操作数`dst`。 | `c_api/reg_compute/compute/reg_fused.h` |
| [asc_fma](reg_fused/asc_fma.md) | 逐元素乘加运算。 | `c_api/reg_compute/compute/reg_fused.h` |
| [asc_leakyrelu](reg_fused/asc_leakyrelu.md) | 按元素执行Leaky ReLU（Leaky Rectified Linear Unit）操作。 | `c_api/reg_compute/compute/reg_fused.h` |
| [asc_madd](reg_fused/asc_madd.md) | madd（multiply-add），对源操作数执行逐元素乘法和加法。 | `c_api/reg_compute/compute/reg_fused.h` |
| [asc_mul_scalar_float2half_rn](reg_fused/asc_mul_scalar_float2half_rn.md) | 将`float`类型源操作数逐元素乘以标量后，按照RINT舍入模式转换为`half`类型。 | `c_api/reg_compute/compute/reg_fused.h` |
| [asc_mula](reg_fused/asc_mula.md) | 根据掩码将两个源寄存器按元素相乘，并与目的寄存器累加。 | `c_api/reg_compute/compute/reg_fused.h` |
| [asc_prelu](reg_fused/asc_prelu.md) | 源操作数`src0`大于等于0的情况下直接将`src0`写入目的操作数`dst`，否则将`src0` * `src1`的结果写入`dst`。 | `c_api/reg_compute/compute/reg_fused.h` |
| [asc_relu](reg_fused/asc_relu.md) | 逐元素执行ReLU运算。 | `c_api/reg_compute/compute/reg_fused.h` |
| [asc_exp_sub](reg_fused/asc_exp_sub.md) | 根据`mask`将`src0`与`src1`按元素相减，并计算以差值为指数的自然指数函数。 | `c_api/reg_compute/compute/reg_fused.h` |

### 比较计算

**表10** 比较计算接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_eq](reg_compare/asc_eq.md) | 对源操作数逐元素比较是否相等。 | `c_api/reg_compute/compute/reg_compare.h` |
| [asc_eq_scalar](reg_compare/asc_eq_scalar.md) | 对源操作数逐元素比较是否与标量相等。 | `c_api/reg_compute/compute/reg_compare.h` |
| [asc_ge](reg_compare/asc_ge.md) | ge（greater than or equal to），对源操作数执行逐元素比较。 | `c_api/reg_compute/compute/reg_compare.h` |
| [asc_ge_scalar](reg_compare/asc_ge_scalar.md) | ge（greater than or equal to），对源操作数与标量执行逐元素比较。 | `c_api/reg_compute/compute/reg_compare.h` |
| [asc_gt](reg_compare/asc_gt.md) | gt（greater than），对源操作数执行逐元素比较。 | `c_api/reg_compute/compute/reg_compare.h` |
| [asc_gt_scalar](reg_compare/asc_gt_scalar.md) | gt（greater than），对源操作数与标量执行逐元素比较。 | `c_api/reg_compute/compute/reg_compare.h` |
| [asc_le](reg_compare/asc_le.md) | le（less than or equal to），对源操作数执行逐元素比较。 | `c_api/reg_compute/compute/reg_compare.h` |
| [asc_le_scalar](reg_compare/asc_le_scalar.md) | le（less than or equal to），对源操作数与标量执行逐元素比较。 | `c_api/reg_compute/compute/reg_compare.h` |
| [asc_lt](reg_compare/asc_lt.md) | lt（less than），对源操作数执行逐元素比较。 | `c_api/reg_compute/compute/reg_compare.h` |
| [asc_lt_scalar](reg_compare/asc_lt_scalar.md) | lt（less than），对源操作数与标量执行逐元素比较。 | `c_api/reg_compute/compute/reg_compare.h` |
| [asc_ne](reg_compare/asc_ne.md) | ne（not equal），对源操作数执行逐元素比较。 | `c_api/reg_compute/compute/reg_compare.h` |
| [asc_ne_scalar](reg_compare/asc_ne_scalar.md) | `src`中的每个元素逐个与标量`value`比较大小，如果`src_i` != `value`，则输出结果`dst`对应比特位为1，否则为0。 | `c_api/reg_compute/compute/reg_compare.h` |

### 排布变换

**表11** 排布变换接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_deintlv](reg_permute_sel/asc_deintlv.md) | 给定源操作数`src0`和`src1`，将`src0`和`src1`中的元素解交织存入结果操作数`dst0`和`dst1`中。 | `c_api/reg_compute/compute/reg_permute_sel.h` |
| [asc_intlv](reg_permute_sel/asc_intlv.md) | 将源操作数`src0`和`src1`中的元素交织存入目的操作数`dst0`和`dst1`中。 | `c_api/reg_compute/compute/reg_permute_sel.h` |
| [asc_pack_to_high](reg_permute_sel/asc_pack_to_high.md) | 将源操作数中的元素选取低8位（b16）、低16位（b32）、低32位（b64）写入目的操作数的高半部分（当数据类型为`vector_bool`时，每2bit选取其中的低位1bit）。 | `c_api/reg_compute/compute/reg_permute_sel.h` |
| [asc_pack_to_low](reg_permute_sel/asc_pack_to_low.md) | 将源操作数中的元素选取低8位（b16）、低16位（b32）、低32位（b64）写入目的操作数的低半部分（当数据类型为`vector_bool`时，每2bit选取其中的低位1bit）。 | `c_api/reg_compute/compute/reg_permute_sel.h` |
| [asc_select](reg_permute_sel/asc_select.md) | 根据`mask`的比特位值，从源操作数`src0`、`src1`中选择元素，得到目的操作数`dst`。 | `c_api/reg_compute/compute/reg_permute_sel.h` |
| [asc_squeeze](reg_permute_sel/asc_squeeze.md) | 将`src`中被`mask`选择的有效元素依次复制到`dst`中，有效元素从低到高连续排列，剩余位置元素置为0。 | `c_api/reg_compute/compute/reg_permute_sel.h` |
| [asc_squeeze_and_storeunalign](reg_permute_sel/asc_squeeze_and_storeunalign.md) | reg计算数据搬运接口，适用于从矢量数据寄存器中筛选出有效元素后按照连续非32B对齐的起始地址连续搬出到UB的场景。 | `c_api/reg_compute/compute/reg_permute_sel.h` |
| [asc_squeeze_and_storeunalign_finalize](reg_permute_sel/asc_squeeze_and_storeunalign_finalize.md) | reg计算数据搬运接口，适用于从矢量数据寄存器中筛选出有效元素后按照连续非32B对齐的起始地址连续搬出到UB的尾块场景。 | `c_api/reg_compute/compute/reg_permute_sel.h` |
| [asc_squeeze_and_storeunalign_init](reg_permute_sel/asc_squeeze_and_storeunalign_init.md) | 对AR特殊寄存器进行清零操作。AR寄存器用于配合[asc_squeeze_and_storeunalign](reg_permute_sel/asc_squeeze_and_storeunalign.md)及[asc_squeeze_and_storeunalign_finalize](reg_permute_sel/asc_squeeze_and_storeunalign_finalize.md)使用。 | `c_api/reg_compute/compute/reg_permute_sel.h` |
| [asc_unpack](reg_permute_sel/asc_unpack.md) | 矢量解包操作。 | `c_api/reg_compute/compute/reg_permute_sel.h` |
| [asc_unsqueeze](reg_permute_sel/asc_unsqueeze.md) | 根据`mask`进行解压缩，将生成的数据输出到`dst`。 | `c_api/reg_compute/compute/reg_permute_sel.h` |

### 索引操作

**表12** 索引操作接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_arange](reg_index/asc_arange.md) | 以传入的`value`为起始值，生成递增/递减的索引，并将生成的索引保存在`dst`中。 | `c_api/reg_compute/compute/reg_index.h` |

### 直方图

**表13** 直方图接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_cumulative_histogram](reg_histogram/asc_cumulative_histogram.md) | 对直方图数据进行累计统计。 | `c_api/reg_compute/compute/reg_histogram.h` |
| [asc_frequency_histogram](reg_histogram/asc_frequency_histogram.md) | 对直方图数据进行频率统计。 | `c_api/reg_compute/compute/reg_histogram.h` |

### 掩码寄存器操作

**表14** 掩码寄存器操作接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_create_mask](reg_mask/asc_create_mask.md) | 根据入参生成相应的掩码寄存器。 | `c_api/reg_compute/compute/reg_mask.h` |
| [asc_get_mask_spr](reg_mask/asc_get_mask_spr.md) | 从特殊寄存器SPR{MASK1, MASK0}读取`mask`值并根据数据类型格式返回对应的`mask`数据，MASK0、MASK1均为64bit的寄存器。 | `c_api/reg_compute/compute/reg_mask.h` |
| [asc_update_mask](reg_mask/asc_update_mask.md) | 根据`value`大小生成对应的掩码寄存器中的值。 | `c_api/reg_compute/compute/reg_mask.h` |

### 地址寄存器操作

**表15** 地址寄存器操作接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_update_addr_reg](reg_addr_reg/asc_update_addr_reg.md) | 地址寄存器通过该接口初始化，然后在循环之中使用地址寄存器存储地址偏移量。 | `c_api/reg_compute/compute/reg_addr_reg.h` |

### 类型转换

**表16** 类型转换接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_bfloat162e1m2x2](reg_convert/asc_bfloat162e1m2x2.md) | 将`bfloat16_t`类型转换为`fp4x2_e1m2_t`类型，并支持指定转换结果的写入位置。 | `c_api/reg_compute/reg_convert.h` |
| [asc_bfloat162e2m1x2](reg_convert/asc_bfloat162e2m1x2.md) | 将`bfloat16_t`类型转换为`fp4x2_e2m1_t`类型，并支持指定转换结果的写入位置。 | `c_api/reg_compute/reg_convert.h` |
| [asc_bfloat162float](reg_convert/asc_bfloat162float.md) | 将`bfloat16_t`类型转换为`float`类型，无舍入模式。 | `c_api/reg_compute/reg_convert.h` |
| [asc_bfloat162half](reg_convert/asc_bfloat162half.md) | 将`bfloat16_t`类型转换为`half`类型，并支持多种舍入模式和饱和/非饱和模式。 | `c_api/reg_compute/reg_convert.h` |
| [asc_bfloat162int32](reg_convert/asc_bfloat162int32.md) | 将`bfloat16_t`类型数据转换为`int32_t`类型，并支持多种舍入模式。 | `c_api/reg_compute/reg_convert.h` |
| [asc_ceil](reg_convert/asc_ceil.md) | 将源操作数中的浮点数元素按照CEIL（向正无穷方向舍入）模式舍入到整数值，结果保持原浮点数据类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_e1m2x22bfloat16](reg_convert/asc_e1m2x22bfloat16.md) | 将`fp4x2_e1m2_t`类型转换为`bfloat16_t`类型，并支持指定源数据的读取位置。 | `c_api/reg_compute/reg_convert.h` |
| [asc_e2m1x22bfloat16](reg_convert/asc_e2m1x22bfloat16.md) | 将`fp4x2_e2m1_t`类型转换为`bfloat16_t`类型，并支持指定源数据的读取位置。 | `c_api/reg_compute/reg_convert.h` |
| [asc_e4m32float](reg_convert/asc_e4m32float.md) | 将`fp8_e4m3fn_t`类型转换为`float`类型，并支持指定源数据的读取位置。 | `c_api/reg_compute/reg_convert.h` |
| [asc_e5m22float](reg_convert/asc_e5m22float.md) | 将`fp8_e5m2_t`类型转换为`float`类型，并支持指定源数据的读取位置。 | `c_api/reg_compute/reg_convert.h` |
| [asc_float2bfloat16](reg_convert/asc_float2bfloat16.md) | 将`float`类型转换为`bfloat16_t`类型，并支持多种舍入模式。 | `c_api/reg_compute/reg_convert.h` |
| [asc_float2e4m3](reg_convert/asc_float2e4m3.md) | 将`float`类型转换为`fp8_e4m3fn_t`类型，并支持指定转换结果的写入位置。 | `c_api/reg_compute/reg_convert.h` |
| [asc_float2e5m2](reg_convert/asc_float2e5m2.md) | 将`float`类型转换为`fp8_e5m2_t`类型，并支持指定转换结果的写入位置。 | `c_api/reg_compute/reg_convert.h` |
| [asc_float2half](reg_convert/asc_float2half.md) | 将`float`类型转换为`half`类型，并支持多种舍入模式。 | `c_api/reg_compute/reg_convert.h` |
| [asc_float2hif8](reg_convert/asc_float2hif8.md) | 将`float`类型转换为`hifloat8_t`类型，并支持指定转换结果的写入位置。 | `c_api/reg_compute/reg_convert.h` |
| [asc_float2int16](reg_convert/asc_float2int16.md) | 将`float`类型转换为`int16_t`类型，并支持多种舍入模式。 | `c_api/reg_compute/reg_convert.h` |
| [asc_float2int32](reg_convert/asc_float2int32.md) | 将`float`类型转换为`int32_t`类型，并支持多种舍入模式。 | `c_api/reg_compute/reg_convert.h` |
| [asc_float2int64](reg_convert/asc_float2int64.md) | 将`float`类型转换为`int64_t`类型，并支持多种舍入模式。 | `c_api/reg_compute/reg_convert.h` |
| [asc_floor](reg_convert/asc_floor.md) | 将源操作数中的浮点数元素按照FLOOR（向负无穷方向舍入）模式舍入到整数值，结果保持原浮点数据类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_half2bfloat16](reg_convert/asc_half2bfloat16.md) | 将`half`类型数据转换为`bfloat16_t`类型，并支持多种舍入模式。 | `c_api/reg_compute/reg_convert.h` |
| [asc_half2float](reg_convert/asc_half2float.md) | 将`half`类型数据转换为`float`类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_half2hif8](reg_convert/asc_half2hif8.md) | 将`half`类型转换为`hifloat8_t`类型，并支持多种舍入模式。 | `c_api/reg_compute/reg_convert.h` |
| [asc_half2int16](reg_convert/asc_half2int16.md) | 将`half`类型数据转换为`int16_t`类型，并支持多种舍入模式。 | `c_api/reg_compute/reg_convert.h` |
| [asc_half2int32](reg_convert/asc_half2int32.md) | 将`half`类型数据转换为`int32_t`类型，并支持多种舍入模式。 | `c_api/reg_compute/reg_convert.h` |
| [asc_half2int4x2](reg_convert/asc_half2int4x2.md) | 将`half`类型转换为`int4x2_t`类型，并支持多种舍入模式和饱和/非饱和模式。 | `c_api/reg_compute/reg_convert.h` |
| [asc_half2int8](reg_convert/asc_half2int8.md) | 将`half`类型转换为`int8_t`类型，并支持多种舍入模式。 | `c_api/reg_compute/reg_convert.h` |
| [asc_half2uint8](reg_convert/asc_half2uint8.md) | 将`half`类型转换为`uint8_t`类型，并支持多种舍入模式。 | `c_api/reg_compute/reg_convert.h` |
| [asc_hif82float](reg_convert/asc_hif82float.md) | 将`hifloat8_t`类型转换为`float`类型，并支持指定源数据的读取位置。 | `c_api/reg_compute/reg_convert.h` |
| [asc_hif82half](reg_convert/asc_hif82half.md) | 将`hifloat8_t`类型数据转换为`half`类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_int162float](reg_convert/asc_int162float.md) | 将`int16_t`类型转换为`float`类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_int162half](reg_convert/asc_int162half.md) | 将`int16_t`类型转换为`half`类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_int162int32](reg_convert/asc_int162int32.md) | 将`int16_t`类型转换为`int32_t`类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_int162uint32](reg_convert/asc_int162uint32.md) | 将`int16_t`类型转换为`uint32_t`类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_int162uint8](reg_convert/asc_int162uint8.md) | 将`int16_t`类型转换为`uint8_t`类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_int322float](reg_convert/asc_int322float.md) | 将`int32_t`类型转换为`float`类型，并支持多种舍入模式。 | `c_api/reg_compute/reg_convert.h` |
| [asc_int322int16](reg_convert/asc_int322int16.md) | 将`int32_t`类型转换为`int16_t`类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_int322int64](reg_convert/asc_int322int64.md) | 将`int32_t`类型转换为`int64_t`类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_int322uint16](reg_convert/asc_int322uint16.md) | 将`int32_t`类型转换为`uint16_t`类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_int322uint8](reg_convert/asc_int322uint8.md) | 将`int32_t`类型转换为`uint8_t`类型，并支持指定转换结果的写入位置。 | `c_api/reg_compute/reg_convert.h` |
| [asc_int4x22bfloat16](reg_convert/asc_int4x22bfloat16.md) | 将`int4x2_t`类型转换为`bfloat16_t`类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_int4x22half](reg_convert/asc_int4x22half.md) | 将`int4x2_t`类型转换为`half`类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_int4x22int16](reg_convert/asc_int4x22int16.md) | 将`int4x2_t`类型转换为`int16_t`类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_int642float](reg_convert/asc_int642float.md) | 将`int64_t`类型数据转换为`float`类型，并支持多种舍入模式。 | `c_api/reg_compute/reg_convert.h` |
| [asc_int642int32](reg_convert/asc_int642int32.md) | 将`int64_t`类型转换为`int32_t`类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_int82half](reg_convert/asc_int82half.md) | 将`int8_t`类型转换为`half`类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_int82int16](reg_convert/asc_int82int16.md) | 将`int8_t`类型转换为`int16_t`类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_int82int32](reg_convert/asc_int82int32.md) | 将`int8_t`类型转换为`int32_t`类型，并支持指定源数据的读取位置。 | `c_api/reg_compute/reg_convert.h` |
| [asc_rint](reg_convert/asc_rint.md) | 将源操作数中的浮点数元素按照RINT（四舍六入五成双）模式舍入到整数值，结果保持原浮点数据类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_round](reg_convert/asc_round.md) | 将源操作数中的浮点数元素按照ROUND（四舍五入）模式舍入到整数值，结果保持原浮点数据类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_trunc](reg_convert/asc_trunc.md) | 将源操作数中的浮点数元素按照TRUNC（向零方向截断）模式截断到整数值，结果保持原浮点数据类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_uint162uint32](reg_convert/asc_uint162uint32.md) | 将`uint16_t`类型转换为`uint32_t`类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_uint162uint8](reg_convert/asc_uint162uint8.md) | 将`uint16_t`类型转换为`uint8_t`类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_uint322int16](reg_convert/asc_uint322int16.md) | 将`uint32_t`类型转换为`int16_t`类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_uint322uint16](reg_convert/asc_uint322uint16.md) | 将`uint32_t`类型转换为`uint16_t`类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_uint322uint8](reg_convert/asc_uint322uint8.md) | 将`uint32_t`类型转换为`uint8_t`类型，并支持指定转换结果的写入位置。 | `c_api/reg_compute/reg_convert.h` |
| [asc_uint82half](reg_convert/asc_uint82half.md) | 将`uint8_t`类型转换为`half`类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_uint82uint16](reg_convert/asc_uint82uint16.md) | 将`uint8_t`类型转换为`uint16_t`类型。 | `c_api/reg_compute/reg_convert.h` |
| [asc_uint82uint32](reg_convert/asc_uint82uint32.md) | 将`uint8_t`类型转换为`uint32_t`类型，并支持指定源数据的读取位置。 | `c_api/reg_compute/reg_convert.h` |

### 同步控制

**表17** 同步控制接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_mem_bar](reg_sync/asc_mem_bar.md) | Reg计算宏函数内不同流水线之间的同步指令。 | `c_api/reg_compute/reg_sync.h` |
