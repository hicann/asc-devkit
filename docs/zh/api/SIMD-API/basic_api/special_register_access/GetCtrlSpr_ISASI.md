# GetCtrlSpr(ISASI)

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/special_register_access/GetCtrlSpr_ISASI_res.md#id1 -->

## 功能说明

头文件路径为：`"basic_api/kernel_operator_common_intf.h"`。

读取CTRL寄存器（控制寄存器）特定比特位上的值。

## 函数原型

```cpp
template <int8_t startBit, int8_t endBit>
__aicore__ static inline int64_t GetCtrlSpr()
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| startBit | 起始比特位索引。 |
| endBit | 终止比特位索引。 |

## 数据类型

本接口为寄存器访问接口，不涉及张量数据类型操作。返回值类型固定为int64_t。

## 返回值说明

CTRL寄存器对应比特位上的值。

## 约束说明

- startBit和endBit的取值范围为[0, 63]。
- startBit小于等于endBit。
- 各比特位的说明请参考[SetCtrlSpr(ISASI)参数说明](./SetCtrlSpr_ISASI.md#参数说明)。

## 调用示例

如下为读取CTRL[48]比特位数值的示例。

```cpp
int64_t valueCtrlAfterSet = AscendC::GetCtrlSpr<48, 48>();
```

更多示例请参考[CtrlSpr样例](../../../../../../examples/01_simd_cpp_api/03_basic_api/09_utils/ctrl_spr)。
