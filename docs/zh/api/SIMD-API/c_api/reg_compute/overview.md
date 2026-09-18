# 概述
Reg矢量计算C API提供面向Reg矢量计算架构的寄存器级矢量计算能力，用户可以通过该API直接对芯片中涉及Vector计算的寄存器进行操作，实现更大的灵活性和更好的性能。Reg矢量计算C API与Memory矢量计算C API功能相似，但与Memory矢量计算C API的输入和输出数据通常位于Unified Buffer（UB）不同，Reg矢量计算C API的输入或输出数据可使用矢量数据寄存器`vector_<dtype>`（`dtype`为占位符，表示`half`、`float`等C API支持的数据类型）和掩码寄存器`vector_bool`。对于计算类API，其功能是从给定的寄存器获取数据，进行计算，并将结果保存在给定的寄存器。对于搬运类API，其功能是实现UB和寄存器的数据搬运。由此可见，Reg矢量计算C API相较于Memory矢量计算C API，将数据搬运和Reg计算过程交给用户自主控制，从而实现更大的开发自由度。

## 如何使用Reg矢量计算API
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

## Reg矢量计算编程示例

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

## 通用约束

- 通过引用参数输出结果的Reg矢量计算C API，在非AIV上调用时直接返回。
- 通过函数返回值输出结果的Reg矢量计算C API，在非AIV上调用时返回对应矢量类型的默认构造值。
- Reg矢量计算C API接口需要在Vector Function（`__simd_vf__` 标记的函数）内调用，不支持在`__aicore__`函数中直接调用，调用关系请参考[SIMD BuiltIn关键字](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)。
- 对于支持配置`mask`参数的Reg矢量计算C API接口，`mask`需通过[掩码设置接口](./reg_mask/reg_mask.md)预先赋值后再传入，未赋值的掩码寄存器内容不确定，会导致有效元素位置错误。
