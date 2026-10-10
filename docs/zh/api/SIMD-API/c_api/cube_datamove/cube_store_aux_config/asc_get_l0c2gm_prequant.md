# asc_get_l0c2gm_prequant

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：不支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"c_api/cube_datamove/cube_datamove.h"`。

数据搬运过程中进行随路量化时，通过调用该接口获取量化操作前矢量的起始地址。

量化模式、参数编码和准备流程见[随路量化](../cube_store_key_features/accompanying_quantization.md)；量化与激活参数的配合见[随路量化与随路ReLU场景组合](../cube_store_key_features/accompanying_quantization_and_relu_scenario_combination.md)。

该状态由[asc_set_l0c_copy_config](asc_set_l0c_copy_config.md)配置，其`quant_pre_addr`参数的含义和配置覆盖行为参见对应接口。返回值是FPC寄存器中的配置字段，不是量化或激活计算后的数据。

## 函数原型

```cpp
__aicore__ inline uint64_t asc_get_l0c2gm_prequant()
```

## 参数说明

无

## 返回值说明

量化操作前矢量的起始地址。

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

```cpp
uint64_t prequant_addr = asc_get_l0c2gm_prequant();
```
