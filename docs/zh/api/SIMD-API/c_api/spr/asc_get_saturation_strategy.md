# asc_get_saturation_strategy

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

头文件路径为：`"c_api/spr/spr.h"`。

读取当前饱和控制策略，默认为开启全局饱和配置：

- 当前为单指令/API路径配置时，返回`asc_override_strategy::USE_API`。
- 当前为全局饱和配置时，返回`asc_override_strategy::USE_GLOBAL`。

常用于临时切换策略前保存原配置，或与[asc_set_saturation_strategy](asc_set_saturation_strategy.md)配合做回读校验。

## 函数原型

```c
__aicore__ inline asc_override_strategy asc_get_saturation_strategy()
```

## 参数说明

无

## 返回值说明

当前饱和控制策略，类型为[asc_override_strategy](../defs/enum/asc_override_strategy.md)。

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

场景：动态量化场景，溢出需饱和到±MAX而非截断。须先将策略设为`USE_GLOBAL`并开启CAST饱和。

```cpp
// SIMD_VF外：保存并切换饱和策略（动态量化段开始前）
asc_override_strategy old_strategy = asc_get_saturation_strategy();
bool old_cast_sat = asc_get_saturation_flag(asc_saturation_mode::CAST);
asc_set_saturation_strategy(asc_override_strategy::USE_GLOBAL);
asc_set_saturation_flag(asc_saturation_mode::CAST, true);

asc_copy_gm2ub_align(act_ub, act_gm, act_burst_bytes);
asc_copy_gm2ub_align(bias_ub, bias_gm, bias_burst_bytes);
asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

// 矢量计算循环内：激活+bias加法后float→int32（全局CAST饱和生效）

// SIMD_VF外：恢复进入本段前的配置
asc_set_saturation_flag(asc_saturation_mode::CAST, old_cast_sat);
asc_set_saturation_strategy(old_strategy);
```
