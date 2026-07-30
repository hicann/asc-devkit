# asc_store_dev

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

不经过DCache向GM地址上写数据。
当多核操作GM地址时，如果数据无法对齐到Cache Line，经过DCache的方式下，由于按照Cache Line大小进行读写，会导致多核数据随机覆盖的问题。此时，可以采用不经过DCache直接读写GM地址的方式，从而避免上述随机覆盖的问题。

## 函数原型

```cpp
__aicore__ inline void asc_store_dev(__gm__ int8_t* addr, int8_t value)

__aicore__ inline void asc_store_dev(__gm__ uint8_t* addr, uint8_t value)

__aicore__ inline void asc_store_dev(__gm__ int16_t* addr, int16_t value)

__aicore__ inline void asc_store_dev(__gm__ uint16_t* addr, uint16_t value)

__aicore__ inline void asc_store_dev(__gm__ int32_t* addr, int32_t value)

__aicore__ inline void asc_store_dev(__gm__ uint32_t* addr, uint32_t value)

__aicore__ inline void asc_store_dev(__gm__ int64_t* addr, int64_t value)

__aicore__ inline void asc_store_dev(__gm__ uint64_t* addr, uint64_t value)
```

## 参数说明

|参数名|输入/输出|描述|
|------------|------------|-----------|
| addr     | 输出     | 目标GM地址。|
| value     | 输入     | 待写入目标的数据。|

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

```c++
// addr是外部输入的GM地址，value是待写入GM内存的数据，类型为 int32_t
int32_t value = 2;
asc_store_dev(addr, value);
```
