# reg数据类型定义

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

矢量数据寄存器用于存储矢量数据，其位宽为VL（Vector Length），可存储VL/sizeof(T)的数据（T表示数据类型）。在Ascend 950PR/Ascend 950DT版本中，VL = 256B。例如对于矢量数据类型vector_float，该寄存器可存储的元素数量为256B / sizeof(float) = 64个。

以下是以位宽为分类，列举的所有矢量数据寄存器的数据类型：

**表1** 位宽分类

| 位宽 | 矢量数据寄存器类型 |
| --- | --- |
| b8 | vector_int8_t、vector_uint8_t、vector_int4x2_t、vector_fp4x2_e2m1_t、vector_fp4x2_e1m2_t、vector_hifloat8_t、vector_fp8_e4m3fn_t、vector_fp8_e5m2_t、vector_fp8_e8m0_t |
| b16 | vector_int16_t、vector_uint16_t、vector_half、vector_bfloat16_t |
| b32 | vector_int32_t、vector_uint32_t、vector_float |
| b64 | vector_int64_t、vector_uint64_t |

**注意：**
- vector_int4x2_t、vector_fp4x2_e2m1_t、vector_fp4x2_e1m2_t这三个矢量数据类型在内存中的排布需要将两个元素打包为一个字节的存储单元。

- 矢量数据寄存器数据上限为32，超出上限的寄存器数据会写入预留的8K Unified Buffer（UB）内存中，可能会引发性能劣化。编译器会自动复用生命周期结束的寄存器和预留内存，若两者均可用，优先复用寄存器。

- 寄存器的生命周期限定于单个VF内部。

## 掩码寄存器

掩码寄存器的类型为vector_bool，用于矢量计算中筛选参与计算的元素，其位宽为VL/8（即256/8=32字节=256bit）。当搬运掩码寄存器时，ubuf中地址偏移单位为字节。

如图1所示，当操作数类型为b8时，每一个element对应1bit mask；当操作数类型为b16时，每一个element对应2bit mask，且仅2bit中的最低位是有效的；当操作数类型为b32时，每一个element对应4bit mask，且仅4bit中的最低位是有效的。

**图1**  掩码寄存器

![掩码寄存器](../../figures/capi_mask.png)

掩码寄存器设置支持多种灵活配置方式，可根据实际计算场景选择不同设置方式，以满足不同计算场景的需求。具体设置方式如下表所示：

**表2** 掩码寄存器设置方式

| 编号 | 设置方式 | 涉及接口 | 说明 |
| :-- | :------------ | :------------ | :------------ |
| 1 | 调用接口设置 | [asc_create_mask](asc_create_mask.md) | 可以设置参数指定mask的模式，即指定哪些位置的元素参与计算。 |
| 2 | 调用接口设置 | [asc_update_mask](asc_update_mask.md) |&bull; 在循环外调用时设置固定的mask，mask表示在一次循环计算中前count个元素参与计算，每次以循环计算均使用此mask。<br>&bull; 在循环内调用时，表示计算中前count个元素参与计算，每次循环mask会自动更新。|
| 3 | 从UB搬入 | [asc_loadalign_postupdate](../reg_load/asc_loadalign_postupdate.md) | 将掩码从UB搬入掩码寄存器。 |
| 4 | 从矢量数据寄存器搬入 | [asc_copy](../reg_load/asc_copy.md) | 从矢量数据寄存器搬运至掩码寄存器。 |

**注意：**
掩码寄存器数量上限为8。超出限制上限的寄存器数据会写入预留的8K UB内存中，可能会引起性能劣化。编译器会自动复用生命周期结束的寄存器和预留内存，若两者均可用，优先复用寄存器。

## 非对齐寄存器

非对齐寄存器包括vector_load_unalign和vector_store_unalign。这些寄存器作为缓冲区，用于在UB和矢量数据寄存器之间进行连续的非对齐数据搬运，其中非对齐特指数据起始地址未按32字节对齐。在搬运过程中，非对齐数据首先被加载到专用的非对齐寄存器，随后通过相应的搬运接口完成数据的分块读取或写入。

在读非对齐地址前，应该先通过[asc_loadunalign_pre](../reg_load/asc_loadunalign_pre.md)初始化，然后再调用[asc_loadunalign](../reg_load/asc_loadunalign.md)/[asc_loadunalign_postupdate](../reg_load/asc_loadunalign_postupdate.md)进行搬运。在写非对齐地址时，应该先调用[asc_storeunalign](../reg_store/asc_storeunalign.md)/[asc_storeunalign_postupdate](../reg_store/asc_storeunalign_postupdate.md)，再调用[asc_storeunalign_post](../reg_store/asc_storeunalign_post.md)/[asc_storeunalign_post_postupdate](../reg_store/asc_storeunalign_post_postupdate.md)进行后处理。

**注意：**
vector_load_unalign寄存器和vector_store_unalign寄存器的数量上限均为4，超过数量上限会报错。

### 调用示例

```cpp
__simd_vf__ inline void neg_vf(__ubuf__ int8_t* dst_addr, __ubuf__ int8_t* src_addr, uint32_t count, uint16_t one_repeat_size, uint16_t repeat_time)
{
    vector_int8_t src;
    vector_int8_t dst;
    vector_load_unalign ureg0;
    vector_store_unalign ureg1;
    vector_bool mask;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        mask = asc_update_mask_b8(count);
        asc_loadunalign_pre(ureg0, src_addr + i * one_repeat_size); // 非对齐搬入前的初始化
        asc_loadunalign(src, ureg0, src_addr + i * one_repeat_size); // 配合vector_load_unalign的使用，非对齐搬入源数据
        asc_neg(dst, src, mask);
        asc_storeunalign(dst_addr + i * one_repeat_size, ureg1, dst, one_repeat_size); // 配合vector_store_unalign的使用，非对齐搬出目的数据
        asc_storeunalign_post(dst_addr + i * one_repeat_size, ureg1, 0); // 处理非对齐搬出的尾块
    }
}
```

## 地址寄存器

地址寄存器的数据类型为addr_reg，用于存储地址偏移量。addr_reg通过[asc_update_addr_reg](asc_update_addr_reg.md)初始化，然后在循环之中使用addr_reg存储地址偏移量。addr_reg在每层循环中根据所设置的步长进行自增。

addr_reg的地址计算公式为：offset = Σ(index_i × stride_i)，其中index_i为第i层循环的迭代变量，stride_i为第i层循环的步长。

**注意：**
- addr_reg为地址偏移量寄存器，仅支持部分搬运指令使用，请根据Reg数据搬运接口函数原型选择，除了通过addr_reg设置地址偏移，支持用户自行累加地址或通过PostUpdate模式进行地址自增，完整特性参考[reg_load](../reg_load/reg_load.md)和[reg_store](../reg_store/reg_store.md)。通过AddrReg地址偏移进行搬运时，需要满足对应搬运指令的地址对齐约束。

- addr_reg的数量上限为8，超过8个可能会导致性能劣化。编译器会自动复用生命周期已经结束的addr_reg寄存器。

- 由于硬件循环(HardwareLoop)限制，addr_reg最多支持4层循环轴。

### 调用示例<a id="example4"></a>

- 单参数版本
    ```cpp
    __simd_vf__ inline void add_vf(__ubuf__ int8_t* dst_addr, __ubuf__ int8_t* src0_addr, __ubuf__ int8_t* src1_addr, uint32_t count,
        uint16_t one_repeat_size, uint16_t repeat_time)
    {
        vector_int8_t src0;
        vector_int8_t src1;
        vector_int8_t dst;
        vector_bool mask;
        addr_reg addr_reg;
        for (uint16_t i = 0; i < repeat_time; ++i) {
            addr_reg = asc_update_addr_reg_b8(one_repeat_size); // 通过初始化addr_reg，每一次循环，地址偏移one_repeat_size
            mask = asc_update_mask_b8(count);
            asc_loadalign(src0, src0_addr, addr_reg);
            asc_loadalign(src1, src1_addr, addr_reg);
            asc_add(dst, src0, src1, mask);
            asc_storealign(dst_addr, dst, addr_reg, mask);
        }
    }
    ```

- 多参数版本（以4参数为例）
    ```cpp
    __simd_vf__ inline void add_4d_vf(
        __ubuf__ float* dst_addr,
        __ubuf__ float* src_addr,
        uint32_t n_stride,    // N间偏移 = C×H×W (float元素数)
        uint32_t c_stride,    // C间偏移 = H×W (float元素数)
        uint32_t h_stride,    // H间偏移 = W (float元素数)
        uint32_t w_stride,    // W间偏移 = 64 (一个VL的float数)
        uint16_t N, uint16_t C, uint16_t H, uint16_t W)
    {
        vector_float src_reg, dst_reg;
        vector_bool mask = asc_create_mask_b32(PAT_ALL);
        addr_reg a_reg;
        for (uint16_t n = 0; n < N; n++) {
            for (uint16_t c = 0; c < C; c++) {
                for (uint16_t h = 0; h < H; h++) {
                    for (uint16_t w = 0; w < W; w++) {
                        a_reg = asc_update_addr_reg_b32(n_stride, c_stride, h_stride, w_stride);
                        asc_loadalign(src_reg, src_addr, a_reg);
                        asc_add(dst_reg, src_reg, src_reg, mask);
                        asc_storealign(dst_addr, dst_reg, a_reg, mask);
                    }
                }
            }
        }
    }
    ```
