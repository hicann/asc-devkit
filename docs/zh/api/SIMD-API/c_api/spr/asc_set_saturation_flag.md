# asc_set_saturation_flag

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

按饱和模式类型设置对应饱和开关。`enable_sat`表示API语义上的“是否开启饱和”，接口内部会按模式处理不同语义。

各模式下`enable_sat`的含义如下：

**表1** 饱和模式与enable_sat语义

| saturation_mode | enable_sat=true | enable_sat=false | 说明 |
| :--- | :--- | :--- | :--- |
| FLOAT | 饱和 | 非饱和 | 浮点计算及部分浮点精度转换场景的饱和控制，默认为非饱和状态。浮点数计算时支持half数据类型；浮点数精度转换时支持hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、half、bfloat16_t数据类型。。 |
| FLOAT8 | 饱和 | 非饱和 | fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t相关场景的饱和控制，默认为饱和状态。 |
| INT | 饱和 | 截断 | 整数计算溢出时的饱和/截断控制，默认为非饱和状态。 |
| CAST | 饱和 | 截断 | 浮点转整数或整数转整数时的精度转换饱和控制，默认为饱和状态。仅在饱和策略为`USE_GLOBAL`时生效。 |


## 函数原型

```c
__aicore__ inline void asc_set_saturation_flag(asc_saturation_mode saturation_mode, bool enable_sat)
```

## 参数说明

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| saturation_mode | 输入 | 饱和模式类型，取值见[asc_saturation_mode](../defs/enum/asc_saturation_mode.md)。 |
| enable_sat | 输入 | 是否开启饱和。`true`表示开启饱和，`false`表示关闭饱和（对INT为截断；对FLOAT/FLOAT8/CAST为非饱和或截断，见上表）。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- `enable_sat`是API语义参数：同一布尔值在不同模式下的关闭语义不同（INT为截断，FLOAT/FLOAT8/CAST为非饱和或截断），请以表1为准。
- 当`saturation_mode`为`CAST`时，还需将饱和策略设置为`asc_override_strategy::USE_GLOBAL`，CAST才会生效。可通过[asc_set_saturation_strategy](asc_set_saturation_strategy.md)配置。
- 本接口仅修改对应模式的饱和配置，不影响其他模式。

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

数据搬运与Reg cast模式可参考[Cast样例](../../../../../../examples/02_simd_c_api/03_c_api/02_reg_vector_compute/cast)。
