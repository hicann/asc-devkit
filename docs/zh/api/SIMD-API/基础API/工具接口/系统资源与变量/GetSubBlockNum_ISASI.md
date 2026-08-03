# GetSubBlockNum(ISASI)

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

## 功能说明

头文件路径为：`"basic_api/kernel_operator_sys_var_intf.h"`。

在[分离模式架构](../../../../../guide/编程指南/高级编程/硬件实现/基本架构.md)下，获取当前配置中，一个逻辑AI Core上Cube Core（AIC）或者Vector Core（AIV）的数量。

## 函数原型

```cpp
__aicore__ inline int64_t GetSubBlockNum()
```

## 参数说明

无

## 返回值说明

在融合编译场景下，不同[算子类型](../../../../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md#section1074418132518)在AIC和AIV上调用该接口的返回值如下：

**表1** 融合编译场景返回值列表

| 算子类型 | 限定符 | AIC | AIV |
| --- | --- | --- | --- |
| Vector算子 | \_\_vector\_\_ | - | 1 |
| Cube算子 | \_\_cube\_\_ | 1 | - |
| Mix算子 | \_\_mix\_\_(0, 1) | - | 1 |
| Mix算子 | \_\_mix\_\_(1, 0) | 1 | - |
| Mix算子 | \_\_mix\_\_(1, 1) | 1 | 1 |
| Mix算子 | \_\_mix\_\_(1, 2) | 1 | 2 |

在自定义算子工程和Kernel直调工程场景下，不同Kernel类型（通过[设置Kernel类型](../../Kernel-Tiling/设置Kernel类型.md)设置）在AIC和AIV上调用该接口的返回值如下：

**表2** 自定义算子工程和Kernel直调工程场景返回值列表

| Kernel类型 | KERNEL_TYPE_AIV_ONLY | KERNEL_TYPE_AIC_ONLY | KERNEL_TYPE_MIX_AIC_1_2 | KERNEL_TYPE_MIX_AIC_1_1 | KERNEL_TYPE_MIX_AIC_1_0 | KERNEL_TYPE_MIX_AIV_1_0 |
| --- | --- | --- | --- | --- | --- | --- |
| AIV | 1 | - | 2 | 1 | - | 1 |
| AIC | - | 1 | 1 | 1 | 1 | - |

**注意，两种场景中返回值的含义相同，仅设置类型的方式不同，编写算子时推荐使用融合编译。**

## 约束说明

无

## 调用示例

```cpp
int64_t subBlockNum = AscendC::GetSubBlockNum();
```
