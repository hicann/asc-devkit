# asc_get_store_atomic_config

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

获取原子操作启用位与原子操作类型的值，可用于校验[asc_set_store_atomic_config_v1](asc_set_store_atomic_config_v1.md)和[asc_set_store_atomic_config_v2（废弃）](../../deprecated_interface/asc_set_store_atomic_config_v2_deprecated.md)设置的原子操作启用位和原子操作类型的值是否符合预期。

<!-- npu="950" id8 -->
**Ascend 950PR/Ascend 950DT产品上该接口已废弃。原子加操作可以直接使用[asc_atomic_add](asc_atomic_add.md)实现。无需再对原子操作启用位和原子操作类型的值进行配置和校验。**
<!-- end id8 -->

## 函数原型

```cpp
__aicore__ inline void asc_get_store_atomic_config(asc_store_atomic_config& config)
```

## 参数说明

**表1** 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| config | 输出 | 用于获取原子操作启用位和原子操作类型的值，详细说明请参考[asc_store_atomic_config](../../defs/union/asc_store_atomic_config.md)。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

需配合[asc_set_store_atomic_config_v1](asc_set_store_atomic_config_v1.md)和[asc_set_store_atomic_config_v2（废弃）](../../deprecated_interface/asc_set_store_atomic_config_v2_deprecated.md)接口使用，该接口用于设置原子操作启用位和原子操作类型的值。

## 调用示例

```cpp
uint16_t atomic_type = 1; // 开启原子操作,进行原子操作的数据类型为float，值为1
uint16_t atomic_op = 0; // 求和操作,值为0
asc_set_store_atomic_config_v1(atomic_type, atomic_op);
asc_store_atomic_config get_config;    // 用于获取原子操作启用位和原子操作类型的值
asc_get_store_atomic_config(get_config);    // get_config.atomic_type = 1; get_config.atomic_op = 0;
```
