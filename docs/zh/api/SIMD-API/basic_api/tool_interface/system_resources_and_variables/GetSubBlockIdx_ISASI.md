# GetSubBlockIdx(ISASI)

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
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetSubBlockIdx_ISASI_res.md#id1 -->

## 功能说明

头文件路径为： `"basic_api/kernel_operator_sys_var_intf.h"`。

在[分离模式架构](../../../../../guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md)下，获取逻辑AI Core上Cube Core（AIC）或者Vector Core（AIV）的逻辑索引。

GetSubBlockIdx在使用`__mix__(1, 2)`作为[函数执行空间限定符](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#section1074418132518)的场景下，可以用于区分同一个核上的两个不同Vector Core，取值分别为0和1。

## 函数原型

```cpp
__aicore__ inline int64_t GetSubBlockIdx()
```

## 参数说明

无

## 返回值说明

返回逻辑AI Core上Cube Core（AIC）或者Vector Core（AIV）的逻辑索引。

SubBlockIdx的取值范围为\[0, SubBlockNum\)，SubBlockNum可以通过[GetSubBlockNum](GetSubBlockNum_ISASI.md)获取。

## 约束说明

无

## 调用示例

```cpp
int64_t subBlockID = AscendC::GetSubBlockIdx();
```
