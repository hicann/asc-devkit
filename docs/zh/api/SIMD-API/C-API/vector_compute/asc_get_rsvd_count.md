# asc_get_rsvd_count

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

此接口用于获取执行[asc_reduce](./asc_reduce.md)操作后剩余的元素数量。

## 函数原型

```cpp
  __aicore__ inline int64_t asc_get_rsvd_count()
```

## 参数说明

无

## 返回值说明

执行[asc_reduce](./asc_reduce.md)操作后剩余的元素数量。

## 流水类型

PIPE_S

## 约束说明

- 需和[asc_reduce](./asc_reduce.md)操作配合使用。
- 需通过同步操作确保[asc_reduce](./asc_reduce.md)执行完成后再调用本接口获取结果。

## 调用示例

```cpp
// 初始化dst、src0、src1和total_length(参与计算的数据长度)
int total_length = 256;
__ubuf__ uint32_t src0[total_length];
__ubuf__ uint32_t src1[total_length];
__ubuf__ uint32_t dst[total_length];
asc_reduce(dst, src0, src1, total_length);
asc_sync_notify(PIPE_V, PIPE_S, 0); // 设置等待和同步信号
asc_sync_wait(PIPE_V, PIPE_S, 0);
int64_t result = asc_get_rsvd_count();  // 获取结果
```
