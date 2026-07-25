# AllocMutexID \(ISASI\)<a name="ZH-CN_TOPIC_0000002352639064"></a>

## 产品支持情况<a name="section1550532418810"></a>

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

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_common.h"`。

从框架获取并占用一个MutexID，与[ReleaseMutexID](ReleaseMutexID-(ISASI).md)配合使用，管理MutexID的获取和释放。获取的MutexID可以传入[Mutex::Lock](Lock.md)/[Mutex::Unlock](Unlock.md)接口使用，此时Mutex::Lock/Unlock可以与TQue等其他接口配合使用。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline MutexID AllocMutexID()
```

## 参数说明<a name="section622mcpsimp"></a>

无

## 返回值说明<a name="section640mcpsimp"></a>

返回MutexID，其类型定义如下，每个ID表示一个Mutex锁。

```cpp
using MutexID = uint8_t;
```

## 约束说明<a name="section633mcpsimp"></a>

MutexID有数量限制，使用结束后应立即调用[ReleaseMutexID](ReleaseMutexID-(ISASI).md)释放，防止MutexID耗尽。

## 调用示例<a name="section837496171220"></a>

示例请参考[Lock调用示例](Lock.md#section123275308128)。
