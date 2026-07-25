# Unlock<a name="ZH-CN_TOPIC_0000002406681009"></a>

## 产品支持情况<a name="section73648168211"></a>

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

## 功能说明<a name="section54681522111017"></a>

头文件路径为：`"basic_api/kernel_common.h"`。

直到当前流水的前序指令执行完成后，根据MutexID释放对应Mutex。

> [!NOTE]说明
> Unlock接口需与[Lock](Lock.md)接口配合使用，对同一个MutexID，Lock和Unlock必须严格成对出现。详细约束请参考[Lock约束说明](Lock.md#section184751024101111)。

## 函数原型<a name="section1568410468104"></a>

```cpp
template <pipe_t pipe>
static __aicore__ inline void Unlock(MutexID id)
```

## 参数说明<a name="section74061251191017"></a>

**表 1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| pipe | 模板参数，表示流水类别。支持的流水类型为PIPE_S/PIPE_M/PIPE_V/PIPE_MTE1/PIPE_MTE2/PIPE_MTE3/PIPE_FIX。 |

**表 2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| id | 输入 | 进行流水同步管理的MutexID。在不同编程范式中，该ID的获取以及释放方式不同，详细情况参考[Lock约束说明](Lock.md#section184751024101111)。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section184751024101111"></a>

参考[Lock约束说明](Lock.md#section184751024101111)。

## 调用示例<a name="section123275308128"></a>

Lock与Unlock必须严格配对使用，详细示例请参考[Lock调用示例](Lock.md#section123275308128)。
