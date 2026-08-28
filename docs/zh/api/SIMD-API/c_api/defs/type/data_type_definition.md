# Reg数据类型定义

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 矢量数据寄存器

矢量数据寄存器用于存储矢量数据，其长度为`VL`（Vector Length）。在Ascend 950PR/Ascend 950DT产品中，`VL`为256字节，可存储`VL / sizeof(dtype)`个`dtype`类型的元素。例如，`vector_float`可存储`256 / sizeof(float) = 64`个`float`类型的元素。

矢量数据寄存器类型按位宽类别分类如下：

**表1** 位宽类别

| 位宽类别 | 矢量数据寄存器类型 |
| --- | --- |
| b8 | vector_int8_t、vector_uint8_t、vector_int4x2_t、vector_fp4x2_e2m1_t、vector_fp4x2_e1m2_t、vector_hifloat8_t、vector_fp8_e4m3fn_t、vector_fp8_e5m2_t、vector_fp8_e8m0_t |
| b16 | vector_int16_t、vector_uint16_t、vector_half、vector_bfloat16_t |
| b32 | vector_int32_t、vector_uint32_t、vector_float |
| b64 | vector_int64_t、vector_uint64_t |

**注意：**

- `vector_int4x2_t`、`vector_fp4x2_e2m1_t`和`vector_fp4x2_e1m2_t`均为打包类型，两个4 bit元素共用一个字节的存储空间，因此归入`b8`位宽类别。

- 矢量数据寄存器的数量上限为32。超出上限的寄存器数据会写入预留的8KB Unified Buffer（UB）空间中，可能会引发性能劣化。编译器会自动复用生命周期结束的寄存器和预留空间；若两者均可用，则优先复用寄存器。

- 寄存器的生命周期限定于单个Vector Function（VF）内部。

## 掩码寄存器

掩码寄存器的类型为`vector_bool`，用于在矢量计算中筛选参与计算的元素，其长度为`VL / 8`，即32字节（256 bit）。

如图1所示，当操作数类型为`b8`时，每个元素对应1 bit掩码；当操作数类型为`b16`时，每个元素对应2 bit掩码，且仅最低有效位（LSB）有效；当操作数类型为`b32`时，每个元素对应4 bit掩码，且仅最低有效位有效。

**图1**  掩码寄存器

![掩码寄存器](../../figures/capi_mask.png)

`b64`操作数的每个元素对应8 bit掩码，且仅最低有效位有效。由于掩码创建接口不支持`b64`模式，需要先通过[asc_create_mask_b32](../../reg_compute/reg_mask/asc_create_mask.md)创建`b32`掩码，再通过[asc_unpack_lower](../../reg_compute/reg_permute_sel/asc_unpack.md)将其展开为`b64`掩码。例如，创建所有`b64`元素均有效的掩码：

```cpp
vector_bool mask_b32 = asc_create_mask_b32(PAT_ALL);
vector_bool mask_b64;
asc_unpack_lower(mask_b64, mask_b32);
```

掩码寄存器支持以下设置方式，可根据实际计算场景选择：

**表2** 掩码寄存器设置方式

| 编号 | 设置方式 | 涉及接口 | 说明 |
| :-- | :------------ | :------------ | :------------ |
| 1 | 调用接口设置 | [asc_create_mask](../../reg_compute/reg_mask/asc_create_mask.md) | 通过参数指定掩码模式，即指定参与计算的元素位置。 |
| 2 | 调用接口设置 | [asc_update_mask](../../reg_compute/reg_mask/asc_update_mask.md) | &bull; 在循环外调用时，设置固定掩码，表示每次循环中前`count`个元素参与计算。<br>&bull; 在循环内调用时，根据剩余的`count`生成当前循环的掩码，并自动更新`count`。 |
| 3 | 从UB搬入 | [asc_loadalign](../../reg_compute/load/asc_loadalign.md)、[asc_loadalign_postupdate](../../reg_compute/load/asc_loadalign_postupdate.md)等接口 | 将掩码从UB搬入掩码寄存器。 |
| 4 | 从矢量数据寄存器搬入 | [asc_copy](../../reg_compute/reg_copy/asc_copy.md) | 从`vector_uint16_t`或`vector_uint32_t`搬运至掩码寄存器。 |

**注意：**

掩码寄存器的数量上限为8。超出上限的寄存器数据会写入预留的8KB UB空间中，可能会引起性能劣化。编译器会自动复用生命周期结束的寄存器和预留空间；若两者均可用，则优先复用寄存器。

## 非对齐寄存器

非对齐寄存器包括`vector_load_unalign`和`vector_store_unalign`。这两类寄存器作为缓冲区，用于在UB和矢量数据寄存器之间连续搬运非对齐数据。本节中的非对齐是指数据起始地址未按32字节对齐。搬运过程中，接口通过非对齐寄存器缓存首块或尾块数据，再完成数据的连续读取或写入。

从非对齐地址搬入数据前，应先调用[asc_loadunalign_pre](../../reg_compute/load/asc_loadunalign_pre.md)初始化，再调用[asc_loadunalign](../../reg_compute/load/asc_loadunalign.md)或[asc_loadunalign_postupdate](../../reg_compute/load/asc_loadunalign_postupdate.md)搬入数据。向非对齐地址搬出数据时，应先调用[asc_storeunalign](../../reg_compute/store/asc_storeunalign.md)或[asc_storeunalign_postupdate](../../reg_compute/store/asc_storeunalign_postupdate.md)，再调用与搬出模式匹配的[asc_storeunalign_post](../../reg_compute/store/asc_storeunalign_post.md)或[asc_storeunalign_post_postupdate](../../reg_compute/store/asc_storeunalign_post_postupdate.md)进行后处理。

**注意：**

`vector_load_unalign`寄存器和`vector_store_unalign`寄存器的数量上限均为4，超过数量上限会报错。

### 调用示例

以下示例中，每次循环处理一个完整的`VL`。

```cpp
__simd_vf__ inline void neg_vf(__ubuf__ int8_t* dst_addr, __ubuf__ int8_t* src_addr,
                               uint16_t one_repeat_size, uint16_t repeat_count)
{
    vector_int8_t src_reg;
    vector_int8_t dst_reg;
    vector_load_unalign load_unalign_reg;
    vector_store_unalign store_unalign_reg;
    vector_bool mask = asc_create_mask_b8(PAT_ALL);
    asc_loadunalign_pre(load_unalign_reg, src_addr);
    for (uint16_t i = 0; i < repeat_count; ++i) {
        asc_loadunalign_postupdate(src_reg, load_unalign_reg, src_addr, one_repeat_size);
        asc_neg(dst_reg, src_reg, mask);
        asc_storeunalign_postupdate(dst_addr, store_unalign_reg, dst_reg, one_repeat_size);
    }
    asc_storeunalign_post_postupdate(dst_addr, store_unalign_reg, 0);
}
```

## 地址寄存器

地址寄存器的类型为`addr_reg`，用于存储地址偏移量。通过[asc_update_addr_reg](../../reg_compute/reg_addr_reg/asc_update_addr_reg.md)在循环内生成地址寄存器后，地址寄存器会根据各层循环设置的步长计算偏移量。

`addr_reg`的偏移量计算公式为`offset = Σ(index_i × stride_i)`，其中`index_i`为第`i`层循环的迭代变量，`stride_i`为对应循环层的步长。地址寄存器中的偏移量以元素个数为单位。调用`asc_update_addr_reg_b8`、`asc_update_addr_reg_b16`和`asc_update_addr_reg_b32`生成地址寄存器时，每个元素分别按1字节、2字节和4字节计算实际地址偏移。

**注意：**

- `addr_reg`仅适用于部分数据搬运接口，请根据具体接口的函数原型确认是否支持。除使用`addr_reg`外，还可以直接累加地址，或使用Post Update模式更新地址。完整的数据搬运方式请参见[Reg对齐搬入](../../reg_compute/load/load.md#reg对齐搬入)、[Reg非对齐搬入](../../reg_compute/load/load.md#reg非对齐搬入)、[Reg对齐搬出](../../reg_compute/store/store.md#reg对齐搬出)和[Reg非对齐搬出](../../reg_compute/store/store.md#reg非对齐搬出)。

- 使用`addr_reg`进行地址偏移时，实际访问地址必须满足对应数据搬运接口的地址对齐约束。

- `asc_update_addr_reg`未提供`b64`模式。搬运`int64_t`或`uint64_t`数据时，应使用`asc_update_addr_reg_b32`生成地址寄存器。一个`int64_t`或`uint64_t`元素占用两个`b32`偏移单元，因此传入`asc_update_addr_reg_b32`的各维偏移量必须设置为期望的64位元素偏移量的2倍。例如：

    ```cpp
    __simd_vf__ inline void load_int64_vf(__ubuf__ int64_t* src_addr,
                                          uint32_t elements_per_repeat,
                                          uint16_t repeat_count)
    {
        vector_int64_t src_reg;
        addr_reg offset_reg;
        for (uint16_t i = 0; i < repeat_count; ++i) {
            offset_reg = asc_update_addr_reg_b32(elements_per_repeat * 2);
            asc_loadalign(src_reg, src_addr, offset_reg);
        }
    }
    ```

- `addr_reg`的数量上限为8，超过8个可能会导致性能劣化。编译器会自动复用生命周期已结束的`addr_reg`寄存器。

- 受硬件循环（Hardware Loop）限制，`addr_reg`最多支持4层循环。

### 调用示例<a id="example4"></a>

- 单参数版本

    ```cpp
    __simd_vf__ inline void add_vf(__ubuf__ int8_t* dst_addr, __ubuf__ int8_t* src0_addr,
                                   __ubuf__ int8_t* src1_addr, uint32_t count,
                                   uint16_t one_repeat_size, uint16_t repeat_count)
    {
        vector_int8_t src0_reg;
        vector_int8_t src1_reg;
        vector_int8_t dst_reg;
        vector_bool mask;
        addr_reg offset_reg;
        for (uint16_t i = 0; i < repeat_count; ++i) {
            offset_reg = asc_update_addr_reg_b8(one_repeat_size);
            mask = asc_update_mask_b8(count);
            asc_loadalign(src0_reg, src0_addr, offset_reg);
            asc_loadalign(src1_reg, src1_addr, offset_reg);
            asc_add(dst_reg, src0_reg, src1_reg, mask);
            asc_storealign(dst_addr, dst_reg, offset_reg, mask);
        }
    }
    ```

- 多参数版本（以4个步长参数为例）

    ```cpp
    __simd_vf__ inline void addr_reg_4d_vf(
        __ubuf__ float* dst_addr,
        __ubuf__ float* src_addr,
        uint32_t n_stride,    // N维间偏移（float类型元素个数）
        uint32_t c_stride,    // C维间偏移（float类型元素个数）
        uint32_t h_stride,    // H维间偏移（float类型元素个数）
        uint32_t w_stride,    // W维间偏移（float类型元素个数）
        uint16_t n_count, uint16_t c_count, uint16_t h_count, uint16_t w_count)
    {
        vector_float src_reg, dst_reg;
        vector_bool mask = asc_create_mask_b32(PAT_ALL);
        addr_reg offset_reg;
        for (uint16_t n = 0; n < n_count; ++n) {
            for (uint16_t c = 0; c < c_count; ++c) {
                for (uint16_t h = 0; h < h_count; ++h) {
                    for (uint16_t w = 0; w < w_count; ++w) {
                        offset_reg = asc_update_addr_reg_b32(
                            w_stride, h_stride, c_stride, n_stride);
                        asc_loadalign(src_reg, src_addr, offset_reg);
                        asc_add(dst_reg, src_reg, src_reg, mask);
                        asc_storealign(dst_addr, dst_reg, offset_reg, mask);
                    }
                }
            }
        }
    }
    ```
