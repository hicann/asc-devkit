# asc_dci

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

数据缓存失效，该指令用于使整个数据缓存无效化，且不会对处于“脏”状态的缓存行执行写回操作。换言之，所有缓存行将被直接标记为无效，其修改过但尚未同步至主存的数据将被丢弃。

## 函数原型

```cpp
__aicore__ inline void asc_dci()
```

## 参数说明

无

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

在调用asc_dci之前，需调用[asc_sync_data_barrier](../sync/asc_sync_data_barrier.md)插入DSB_ALL指令，等待所有内存访问指令执行结束。

## 调用示例

```cpp
asc_sync_data_barrier(mem_dsb_t::DSB_ALL);
asc_dci();
```
