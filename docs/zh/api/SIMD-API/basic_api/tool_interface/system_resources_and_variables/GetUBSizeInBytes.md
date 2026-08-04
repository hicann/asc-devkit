# GetUBSizeInBytes

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

头文件路径为：`"basic_api/kernel_operator_sys_var_intf.h"`。

获取UB空间的大小，单位为Byte。开发者根据UB的大小来计算循环次数等参数值。

## 函数原型

```cpp
__aicore__ inline constexpr uint32_t GetUBSizeInBytes()
```

## 参数说明

无

## 返回值说明

UB空间的大小。

<!-- npu="950" id8 -->
Ascend 950PR/Ascend 950DT架构下，返回常量248KB，单位为Byte。
<!-- end id8 -->

## 约束说明

无

## 调用示例

本调用示例通过GetUBSizeInBytes获取的UB空间大小，来计算tileNum的值。完整的算子样例请参考：[get\_ub\_size样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/09_utils/get_ub_size)。

```cpp
// totalLength为待处理的总数据长度(元素个数)
this->totalLength = totalLength;
// GetUBSizeInBytes() / sizeof(half) -> 计算UB能容纳多少个half类型元素
// 除2 -> 预留50%的UB空间
if (totalLength > AscendC::GetUBSizeInBytes() / sizeof(half) / 2) {
    this->tileLength = AscendC::GetUBSizeInBytes() / sizeof(half) / 2;
} else { // 防止分片大小超过实际数据总量
    this->tileLength = this->totalLength;
}
// 需要迭代的分片数量
this->tileNum = this->totalLength / this->tileLength;
```
