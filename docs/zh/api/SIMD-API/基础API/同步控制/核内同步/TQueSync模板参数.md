# 模板参数<a name="ZH-CN_TOPIC_0000001973155037"></a>

## 产品支持情况<a name="section1550532418810"></a>

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
- Atlas 200I/500 A2 推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：支持
<!-- end id7 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_block_sync_intf.h"`。

TQueSync类的模板参数用于指定源流水和目标流水，目的流水等待源流水。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template<pipe_t src, pipe_t dst>
class TQueSync {
public:
    __aicore__ inline void SetFlag(TEventID id);
    __aicore__ inline void WaitFlag(TEventID id);
};
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| src | 输入 | 源流水。支持的流水参考[硬件流水类型](核内同步能力概述.md#硬件流水类型)。 |
| dst | 输入 | 目的流水。支持的流水参考[硬件流水类型](核内同步能力概述.md#硬件流水类型)。 |

## 返回值说明<a name="section640mcpsimp"></a> 

无

## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例

请参考[SetFlag-WaitFlag](./SetFlag-WaitFlag.md)接口的调用示例。
