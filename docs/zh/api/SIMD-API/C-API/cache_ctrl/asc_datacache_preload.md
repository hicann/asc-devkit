# asc_datacache_preload

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
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

从源地址所在的特定GM地址预加载数据到Data Cache中。

## 函数原型

```cpp
__aicore__ inline void asc_datacache_preload(__gm__ uint64_t* address, int64_t offset)
```

## 参数说明

|参数名|输入/输出|描述|
|------------|------------|-----------|
| address     | 输入     | 源操作数的起始地址。   |
| offset     | 输入     | 在源操作数上偏移offset大小开始加载数据，单位为Bytes。|

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

频繁调用此接口可能会导致保留栈拥塞，此时，该接口将被视为NOP指令，阻塞Scalar流水。因此不建议频繁调用此接口。

## 调用示例

```cpp
// 假设x为外部传入的地址，每个核计算16个数据，block_idx为内置变量，通过block_idx计算偏移实现多核间数据隔离
__gm__ float* x_gm = x + block_idx * 16;
int64_t cacheOffset = 0;
asc_datacache_preload(reinterpret_cast<__gm__ uint64_t*>(x_gm), cacheOffset);
```
