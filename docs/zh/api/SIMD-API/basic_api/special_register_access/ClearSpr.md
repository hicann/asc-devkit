# ClearSpr<a name="ZH-CN_TOPIC_0000002537397560"></a>

## 产品支持情况<a name="section364964311912"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/special_register_access/ClearSpr_res.md#id1 -->

## 功能说明<a name="section11971757181915"></a>

头文件路径为：`"basic_api/kernel_operator_sys_var_intf.h"`。

对指定的特殊寄存器进行清零。当前支持[表SpecialPurposeReg模板参数说明](GetSpr.md#table37531617424)。

## 函数原型<a name="section113251712205"></a>

```cpp
template <SpecialPurposeReg spr>
__aicore__ inline void ClearSpr()
```

## 参数说明<a name="section1116018170208"></a>

**表1** 模板参数说明

| 参数名 | 描述 |
|--------|------|
| spr | 特殊寄存器，类型为SpecialPurposeReg枚举类，具体的取值请参考[表SpecialPurposeReg模板参数说明](GetSpr.md#table37531617424)。 |

## 数据类型<a name="section3853852112218"></a>

无

## 返回值说明<a name="section16895132314202"></a>

无

## 约束说明<a name="section162221734202016"></a>

本接口只能在VF函数外调用，命名空间为AscendC，函数标记符为\_\_aicore\_\_。如果需要在VF内调用，命名空间为AscendC::Reg，函数标记符为\_\_simd\_callee\_\_，具体请参考[ClearSpr](../reg_vector_compute/system_variable_access/ClearSpr.md)。

## 调用示例<a name="section849174212202"></a>

无
