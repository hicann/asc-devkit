# asc_half2int4_sync（废弃）

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：不支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
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

头文件路径为：`"c_api/composite/vector_compute_composite.h"`。

**`asc_half2int4_sync`、`asc_half2int4_rn_sync`、`asc_half2int4_rd_sync`、`asc_half2int4_rna_sync`、`asc_half2int4_ru_sync`和`asc_half2int4_rz_sync`接口已废弃，请使用[对应的非同步接口](../vector_compute/vector_convert/asc_half2int4.md)和同步接口[asc_sync](../sync/asc_sync.md)替代。**

将half类型数据转换为int4类型，支持多种舍入模式：

- RINT舍入模式：四舍六入五成双舍入
- ROUND舍入模式：四舍五入舍入
- FLOOR舍入模式：向负无穷舍入
- CEIL舍入模式：向正无穷舍入
- TRUNC舍入模式：向零舍入

## 函数原型

- 同步计算

    ```cpp
    //在转换有精度损失时表示RINT舍入模式，不涉及精度损失时代表不舍入
    __aicore__ inline void asc_half2int4_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    //RINT舍入模式
    __aicore__ inline void asc_half2int4_rn_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    //FLOOR舍入模式
    __aicore__ inline void asc_half2int4_rd_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    //ROUND舍入模式
    __aicore__ inline void asc_half2int4_rna_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    //CEIL舍入模式
    __aicore__ inline void asc_half2int4_ru_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    //TRUNC舍入模式
    __aicore__ inline void asc_half2int4_rz_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
    ```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst | 输出 | 目的操作数（矢量）的起始地址。 |
| src | 输入 | 源操作数（矢量）的起始地址。 |
| count | 输入 | 参与计算的元素个数。 |

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- dst、src的起始地址需要32字节对齐。
- 操作数地址重叠约束请参考[通用地址重叠约束](../general_description_and_constraints.md#通用地址重叠约束)。
- 当dst为int4b_t时，前n个数据计算接口的count必须为偶数。

## 调用示例

```cpp
constexpr uint32_t total_length = 256;
uint64_t offset = 0;
__ubuf__ half* src = (__ubuf__ half*)asc_get_phy_buf_addr(0);
offset += total_length * sizeof(half);
__ubuf__ int4b_t* dst = (__ubuf__ int4b_t*)asc_get_phy_buf_addr(offset);
asc_half2int4_sync(dst, src, total_length);
```
