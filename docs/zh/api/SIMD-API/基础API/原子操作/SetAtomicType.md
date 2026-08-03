# SetAtomicType<a name="ZH-CN_TOPIC_0000001835083633"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id13 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id13 -->
<!-- npu="A3" id14 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id14 -->
<!-- npu="910b" id15 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id15 -->
<!-- npu="310b" id16 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id16 -->
<!-- npu="310p" id17 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id17 -->
<!-- npu="310p" id18 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id18 -->
<!-- npu="910" id19 -->
- Atlas 训练系列产品：不支持
<!-- end id19 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_set_atomic_intf.h"`。

通过设置模板参数来设定原子操作不同的数据类型。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void SetAtomicType()
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
|--------|------|
| T | 设定原子操作不同的数据类型。 |

## 数据类型

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT，支持的数据类型为int8_t、int16_t、half、bfloat16_t、int32_t、float。
<!-- end id1 -->

<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为int8_t、int16_t、half、bfloat16_t、int32_t、float。
<!-- end id2 -->

<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为int8_t、int16_t、half、bfloat16_t、int32_t、float。
<!-- end id3 -->

<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品，支持的数据类型为int16_t、half、int32_t、float。
<!-- end id4 -->

<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core，支持的数据类型为int16_t、half、float。
<!-- end id5 -->

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- [SetAtomicAdd](SetAtomicAdd.md)、[SetAtomicMax](SetAtomicMax_ISASI.md)、[SetAtomicMin](SetAtomicMin_ISASI.md)内部已集成与SetAtomicType相同的功能。建议调用上述接口时，通过设置模板参数显式指定原子操作的数据类型，无需额外调用SetAtomicType接口。
- 使用完成后，建议使用DisableDmaAtomic清空原子操作的状态，以免影响后续相关指令功能。

## 调用示例<a name="section177231425115410"></a>

根据约束说明可知，使用[SetAtomicAdd](SetAtomicAdd.md)、[SetAtomicMax](SetAtomicMax_ISASI.md)、[SetAtomicMin](SetAtomicMin_ISASI.md)时，已通过模板参数指定原子操作的数据类型，无需额外调用`SetAtomicType`接口，因此本接口不提供调用示例。
