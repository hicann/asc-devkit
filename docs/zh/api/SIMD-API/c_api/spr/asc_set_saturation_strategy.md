# asc_set_saturation_strategy

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

设置饱和控制策略，默认为开启全局饱和配置：

- `asc_override_strategy::USE_API`：表示按单指令/API路径配置饱和。
- `asc_override_strategy::USE_GLOBAL`：表示使用全局饱和配置。

该策略尤其影响CAST类饱和：只有在`USE_GLOBAL`时，通过[asc_set_saturation_flag](asc_set_saturation_flag.md)配置的`asc_saturation_mode::CAST`才会生效。枚举含义详见[asc_override_strategy](../defs/enum/asc_override_strategy.md)。

## 函数原型

```c
__aicore__ inline void asc_set_saturation_strategy(asc_override_strategy strategy)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| strategy | 输入 | 饱和控制策略，取值见[asc_override_strategy](../defs/enum/asc_override_strategy.md)。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 当策略为`USE_API`时，全局CAST饱和配置不生效；若需要CAST全局饱和，必须先或同时将策略设置为`USE_GLOBAL`。
- 切换策略不会自动修改各[asc_saturation_mode](../defs/enum/asc_saturation_mode.md)饱和开关。

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
