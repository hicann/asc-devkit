# asc_get_saturation_flag

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

按饱和模式类型读取当前是否开启饱和，并返回布尔类型的结果。

## 函数原型

```c
__aicore__ inline bool asc_get_saturation_flag(asc_saturation_mode saturation_mode)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| saturation_mode | 输入 | 饱和模式类型，取值见[asc_saturation_mode](../defs/enum/asc_saturation_mode.md)。 |

## 返回值说明

`true`表示当前该模式下饱和已开启；`false`表示未开启（对INT为截断；对FLOAT/FLOAT8/CAST为非饱和或截断）。

## 流水类型

PIPE_S

## 约束说明

- 返回值是API语义层结果：`true`表示该模式下饱和已开启，`false`表示未开启（对INT为截断；对FLOAT/FLOAT8/CAST为非饱和或截断）。各模式语义差异见[asc_set_saturation_flag](asc_set_saturation_flag.md)表1。
- 对`CAST`模式，即使本接口返回`true`，也需同时满足饱和策略为`USE_GLOBAL`，CAST饱和配置才会在精度转换中真正生效。详见[asc_set_saturation_strategy](asc_set_saturation_strategy.md)。

## 调用示例

场景：动态量化场景，溢出需饱和到±MAX而非截断。须先将策略设为`USE_GLOBAL`并开启CAST饱和。

```cpp
// SIMD_VF外：asc_get_saturation_flag查询当前CAST饱和开关
bool cast_sat_enabled = asc_get_saturation_flag(asc_saturation_mode::CAST);
asc_override_strategy old_strategy = asc_get_saturation_strategy();

if (!cast_sat_enabled) {
    asc_set_saturation_strategy(asc_override_strategy::USE_GLOBAL);
    asc_set_saturation_flag(asc_saturation_mode::CAST, true);
}

asc_copy_gm2ub_align(act_ub, act_gm, act_burst_bytes);
asc_copy_gm2ub_align(bias_ub, bias_gm, bias_burst_bytes);
asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

// 矢量计算循环内：激活+bias加法后float→int32（全局CAST饱和生效）

// SIMD_VF外：若本段临时开启过CAST饱和，则恢复进入本段前的配置
if (!cast_sat_enabled) {
    asc_set_saturation_flag(asc_saturation_mode::CAST, false);
    asc_set_saturation_strategy(old_strategy);
}
```

数据搬运与Reg cast模式可参考[Cast样例](../../../../../../examples/02_simd_c_api/03_c_api/02_reg_vector_compute/cast)。
