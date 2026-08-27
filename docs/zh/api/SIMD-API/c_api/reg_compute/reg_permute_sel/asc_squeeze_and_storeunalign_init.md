# asc_squeeze_and_storeunalign_init

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

对AR特殊寄存器进行清零操作。AR寄存器用于配合[asc_squeeze_and_storeunalign](asc_squeeze_and_storeunalign.md)及[asc_squeeze_and_storeunalign_finalize](asc_squeeze_and_storeunalign_finalize.md)使用：当调用asc_squeeze_and_storeunalign后，有效元素的总字节数会被存入AR寄存器用于asc_squeeze_and_storeunalign接口内自动地址偏移。在首次调用asc_squeeze_and_storeunalign之前，需调用本接口将AR寄存器清零。

## 函数原型

```cpp
__simd_callee__ inline void asc_squeeze_and_storeunalign_init()
```

## 参数说明

无

## 返回值说明

无

## 约束说明

在首次调用asc_squeeze_and_storeunalign之前，需调用本接口将AR寄存器清零。

## 调用示例

```cpp
__simd_vf__ inline void vf_squeeze_and_storeunalign(__ubuf__ half* dst_addr, __ubuf__ half* src_addr, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_half dst;
    vector_half src;
    vector_store_unalign ureg;
    vector_bool mask = asc_create_mask_b16(PAT_M4);
    asc_squeeze_and_storeunalign_init();
    for (uint16_t i = 0; i < repeat_time; ++i) {
        asc_loadalign_postupdate(src, src_addr, one_repeat_size);
        asc_squeeze_and_storeunalign(dst_addr, dst, ureg, src, mask);
    }
    asc_squeeze_and_storeunalign_finalize(dst_addr, ureg);
}
```
