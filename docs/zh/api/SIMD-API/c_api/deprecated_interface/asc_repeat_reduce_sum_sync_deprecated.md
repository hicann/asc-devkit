# asc_repeat_reduce_sum_sync（废弃）

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

**`asc_repeat_reduce_sum_sync`接口已废弃，请使用[对应的非同步接口](../vector_compute/vector_reduce/asc_repeat_reduce_sum.md)和同步接口[asc_sync](../sync/asc_sync.md)替代。**

对每个Repeat内所有元素求和。

源操作数相加采用二叉树的方式，两两相加。以256个half类型数据求和为例，每个Repeat可以计算128个half类型数据，分成2个Repeat计算；每个Repeat内，通过二叉树的方式，两两相加。

需要注意的是，两两相加计算过程中，计算结果大于65504时结果保存为65504。例如，源操作数为[60000,60000,-30000,100]，首先60000+60000溢出，结果为65504，然后计算-30000+100=-29900，最后计算65504-29900=35604。

## 函数原型

- 同步计算

    ```cpp
    __aicore__ inline void asc_repeat_reduce_sum_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_sum_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    ```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|----|-----|-----|
| dst | 输出 | 目的操作数（矢量）的起始地址。 |
| src  | 输入 | 源操作数（矢量）的起始地址。|
| count | 输入 | 参与计算的元素个数。 |

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- 操作数地址重叠约束请参考[通用地址重叠约束](../general_description_and_constraints.md#通用地址重叠约束)。
- dst的起始地址，输入类型为half时需要2字节对齐，输入类型为float时需要4字节对齐。
- src的起始地址需要32字节对齐。


## 调用示例


```cpp
// 示例计算256个half的和，分2次迭代完成，计算结果为2组求和的值
constexpr uint32_t total_length = 256;
__ubuf__ half src[total_length];
constexpr uint32_t dst_length = 2;
__ubuf__ half dst[dst_length];

// 使用高维切分计算接口，需设置mask
asc_set_mask_norm();
asc_set_vector_mask(0xffffffffffffffff, 0xffffffffffffffff);    // 设置每个迭代内所有元素均参与计算。

uint8_t repeat = 2;
// 单位为2字节，连续存放
uint16_t dst_repeat_stride = 1;
uint16_t src_block_stride = 1;
uint16_t src_repeat_stride = 8;
asc_repeat_reduce_sum_sync(dst, src, repeat, dst_repeat_stride, src_block_stride, src_repeat_stride);
```

结果示例：

```
输入数据src：[1, 1, 1, ..., 1]
输出数据dst：[128, 128]
```
