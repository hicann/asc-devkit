# asc_create_mask

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

## 功能说明

创建掩码寄存器（vector_bool），支持b8、b16、b32三种位宽模式。pat_mode参数指定mask的模式，即指定哪些位置的元素参与计算。

位宽模式说明：
- b8模式：每个bit对应一个8bit元素（共256元素）。
- b16模式：每2个bit为一组对应一个16bit元素（共128元素）。
- b32模式：每4个bit为一组对应一个32bit元素（共64元素）。

**图1** asc_create_mask原理

![asc_create_mask原理](../../figures/capi_create_mask.png)

## 函数原型

```cpp
asc_create_mask_b8(pat_mode);
asc_create_mask_b16(pat_mode);
asc_create_mask_b32(pat_mode);
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| pat_mode | 输入 | mask模式，取值如下：<br>&bull; PAT_ALL：所有元素设置为有效数据，全部参与计算<br>&bull; PAT_VL1：最低1个元素设置为有效数据<br>&bull; PAT_VL2：最低2个元素设置为有效数据<br>&bull; PAT_VL3：最低3个元素设置为有效数据<br>&bull; PAT_VL4：最低4个元素设置为有效数据<br>&bull; PAT_VL8：最低8个元素设置为有效数据<br>&bull; PAT_VL16：最低16个元素设置为有效数据<br>&bull; PAT_VL32：最低32个元素设置为有效数据<br>&bull; PAT_VL64：最低64个元素设置为有效数据<br>&bull; PAT_VL128：最低128个元素设置为有效数据<br>&bull; PAT_M3：下标为3的倍数的元素设置为有效数据<br>&bull; PAT_M4：下标为4的倍数的元素设置为有效数据<br>&bull; PAT_H：低一半的元素设置为有效数据<br>&bull; PAT_Q：低四分之一的元素设置为有效数据<br>&bull; PAT_ALLF：所有元素设置为无效元素，均不参与计算 |

## 返回值说明

vector_bool，掩码寄存器。

## 约束说明

掩码寄存器的数量上限为8，超过上限的掩码寄存器会写入预留的8K UB内存中，可能引起性能劣化。编译器会自动复用生命周期结束的寄存器和预留内存，若两者均可用，优先复用寄存器。

## 调用示例

```cpp
__simd_vf__ inline void create_mask_vf(__ubuf__ half* dst_addr, __ubuf__ half* src_addr, uint16_t one_repeat_size, uint16_t repeat_time)
{
    vector_half dst;
    vector_half src;
    vector_store_unalign ureg;
    vector_bool mask = asc_create_mask_b16(PAT_M4);
    asc_clear_ar_spr();
    for (uint16_t i = 0; i < repeat_time; ++i) {
        asc_loadalign_postupdate(src, src_addr, one_repeat_size);
        asc_squeeze_with_status(dst, src, mask);
        asc_storeunalign_postupdate(dst_addr, ureg, dst);
    }
}
```
