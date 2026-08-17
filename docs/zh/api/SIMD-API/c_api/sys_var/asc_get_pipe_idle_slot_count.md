# 指令发射队列空闲槽位查询

## 产品支持情况

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

## 功能说明

本接口为试验接口，在后续版本中可能会调整或改进，不保证后续兼容性。请开发者在使用过程中关注后续版本更新。

本接口用于获取指定流水线对应发射队列（Issue Queue）当前的空闲槽位数量。空闲槽位表示发射队列还能接收的待发射指令数量，可用于观察队列占用情况。包含以下六个函数，各函数查询的流水线、适用的AI Core和最大空闲槽位数量如下：

<a id="table1"></a>
**表1**  接口、流水线与最大值对应关系

| 函数 | 被查询的流水线 | 适用AI Core | 最大空闲槽位数量 |
| --- | --- | --- | --- |
| `asc_get_pipe_mte2_idle_slot_count` | PIPE_MTE2 | AIC、AIV | AIC：16；AIV：16 |
| `asc_get_pipe_mte3_idle_slot_count` | PIPE_MTE3 | AIV | 16 |
| `asc_get_pipe_v_idle_slot_count` | PIPE_V | AIV | 32 |
| `asc_get_pipe_m_idle_slot_count` | PIPE_M | AIC | 16 |
| `asc_get_pipe_mte1_idle_slot_count` | PIPE_MTE1 | AIC | 32 |
| `asc_get_pipe_fix_idle_slot_count` | PIPE_FIX | AIC | 32 |

## 函数原型

  ```cpp
  // 获取PIPE_MTE2发射队列的空闲槽位数量。
  __aicore__ inline uint8_t asc_get_pipe_mte2_idle_slot_count()
  // 获取PIPE_MTE3发射队列的空闲槽位数量。
  __aicore__ inline uint8_t asc_get_pipe_mte3_idle_slot_count()
  // 获取PIPE_V发射队列的空闲槽位数量。
  __aicore__ inline uint8_t asc_get_pipe_v_idle_slot_count()
  // 获取PIPE_M发射队列的空闲槽位数量。
  __aicore__ inline uint8_t asc_get_pipe_m_idle_slot_count()
  // 获取PIPE_MTE1发射队列的空闲槽位数量。
  __aicore__ inline uint8_t asc_get_pipe_mte1_idle_slot_count()
  // 获取PIPE_FIX发射队列的空闲槽位数量。
  __aicore__ inline uint8_t asc_get_pipe_fix_idle_slot_count()
  ```

## 参数说明

无

## 返回值说明

返回对应发射队列当前的空闲槽位数量，数据类型为`uint8_t`。

## 流水类型

PIPE_S。

## 约束说明

- 使用本接口前，需要包含头文件，头文件路径为：`c_api/experimental/asc_get_pipe_idle_slot_count.h`。
- 使用前需要增加编译选项`-DCANN_ASC_USE_EXPERIMENTAL`开启试验特性。
- 各接口仅支持[表1 接口、流水线与最大值对应关系](#table1)所列的AI Core类型。在不支持的AI Core类型上调用时，接口返回0。此时无法区分发射队列没有空闲槽位和当前AI Core类型不支持该接口这两种情况。

## 调用示例

以下示例查询PIPE_MTE2发射队列是否存在空闲槽位。当空闲槽位数量大于0时，`has_mte2_idle_slot`返回`true`，否则返回`false`。

```cpp
#include "c_api/experimental/asc_get_pipe_idle_slot_count.h"

__aicore__ inline bool has_mte2_idle_slot()
{
    return asc_get_pipe_mte2_idle_slot_count() > 0;
}
```
