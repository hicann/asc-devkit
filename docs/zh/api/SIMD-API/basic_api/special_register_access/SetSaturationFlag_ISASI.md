# SetSaturationFlag(ISASI)

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
<!-- npu="x90" id8 -->
- Kirin X90：不支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：不支持
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/special_register_access/SetSaturationFlag_ISASI_res.md#id1 -->

## 功能说明

头文件路径为：`"basic_api/kernel_operator_common_intf.h"`。

设置不同数据类型的计算或精度转换时饱和模式的开启状态。饱和模式是当结果溢出或出现INF/NAN时的一种处理模式，具体处理方式如下：

- 当输出为整数时（整数计算、整数转整数、浮点数转整数），有限数值超出目标类型表示范围时产生溢出：开启饱和模式后，溢出结果被饱和为目标数据类型的最大值或最小值；非饱和模式时，溢出结果按目标数据类型位数截断，保留低位、舍弃高位。浮点数转整数时，输入为+inf或-inf，分别输出目标数据类型的最大值或最小值；输入为nan时输出0。该特殊值处理不受饱和模式开启状态影响。

- 当输出为浮点数时（浮点数计算、浮点数精度转换、整数转浮点数），数值超出目标浮点类型表示范围时（如高精度转低精度时绝对值过大）产生INF，非法运算（如0/0、INF-INF）或输入为INF/NAN时产生INF/NAN：开启饱和模式后，INF被饱和为±MAX（目标类型能表示的最大/最小有限值）、NAN被饱和为0；关闭饱和模式时，INF/NAN保持原输出。

本接口通过模板参数`SaturationMode`设置全局饱和模式，并可通过[GetSaturationFlag](GetSaturationFlag_ISASI.md)查询其开启状态。该配置主要用于支持饱和模式的计算和精度转换接口；精度转换场景可配合Memory矢量计算的[Cast](../memory_vector_compute/type_conversion/Cast.md)、[Truncate](../memory_vector_compute/type_conversion/Truncate_ISASI.md)，以及Reg矢量计算的[Cast](../reg_vector_compute/type_conversion/Cast.md)、[Truncate](../reg_vector_compute/type_conversion/Truncate.md)使用。

<!-- npu="950" id10 -->
在Ascend 950PR/Ascend 950DT上，部分精度转换接口可以使用接口自身的饱和配置，也可以使用本接口设置的全局饱和配置。可通过[SetSaturationStrategy](SetSaturationStrategy.md)选择使用接口自身的饱和配置或全局饱和配置。

Reg矢量计算Cast可通过`CastTrait::satMode`设置单接口饱和模式，Memory矢量计算Cast不提供该参数，其单接口配置为饱和模式；选择全局配置时，两类Cast由本接口的`SaturationMode::FLOAT`或`SaturationMode::CAST`控制，涉及FP8类型时还需配合`SaturationMode::FLOAT8`控制NAN处理。两类Truncate均不提供单接口饱和模式参数，选择全局配置时由`SaturationMode::FLOAT`控制；其中float数据类型仅支持不饱和模式。具体支持的数据类型和转换组合以各接口资料为准。
<!-- end id10 -->

## 函数原型

```cpp
template <SaturationMode mode>
__aicore__ inline void SetSaturationFlag(bool enableSat)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| mode | 饱和模式，取值如下：<br>&bull;`SaturationMode::FLOAT`：浮点数计算和浮点数精度转换的饱和模式，系统默认开启。<br>&bull;`SaturationMode::FLOAT8`：浮点数精度转换时，控制FP8类型输出为NAN时是否饱和为0，系统默认开启。该模式仅在精度转换接口处于饱和模式时生效，具体请参考[约束说明](#约束说明)。<br>&bull;`SaturationMode::INT`：整数计算的饱和模式，系统默认关闭。<br>&bull;`SaturationMode::CAST`：浮点数转整数或整数转整数时的精度转换饱和模式，系统默认开启。<br>各模式支持的产品请参考[约束说明](#约束说明)，影响的数据类型请参考[数据类型](#数据类型)。模板参数无默认值，调用本接口时必须显式指定。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| enableSat | 输入 | 是否开启`mode`指定的饱和模式。`true`表示开启，`false`表示关闭。该参数无默认值，调用本接口时必须显式传入。 |

## 数据类型

各饱和模式影响的数据类型如下：

- `SaturationMode::FLOAT`：

  <!-- npu="950" id11 -->
  - 对于Ascend 950PR/Ascend 950DT，影响的数据类型为：浮点数计算时仅half；浮点数精度转换时为hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、half、bfloat16_t，其中`fp8_e8m0_t`、`fp8_e5m2_t`和`fp8_e4m3fn_t`的NAN处理需配合`SaturationMode::FLOAT8`。
  <!-- end id11 -->

  <!-- npu="A3" id12 -->
  - 对于Atlas A3 训练系列产品/Atlas A3 推理系列产品，影响的数据类型为：half、bfloat16_t。
  <!-- end id12 -->

  <!-- npu="910b" id13 -->
  - 对于Atlas A2 训练系列产品/Atlas A2 推理系列产品，影响的数据类型为：half、bfloat16_t。
  <!-- end id13 -->

- `SaturationMode::FLOAT8`：

  <!-- npu="950" id14 -->
  - 对于Ascend 950PR/Ascend 950DT，影响的数据类型为：fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t。
  <!-- end id14 -->

  <!-- npu="A3" id15 -->
  - 对于Atlas A3 训练系列产品/Atlas A3 推理系列产品，不支持该饱和模式。
  <!-- end id15 -->

  <!-- npu="910b" id16 -->
  - 对于Atlas A2 训练系列产品/Atlas A2 推理系列产品，不支持该饱和模式。
  <!-- end id16 -->

- `SaturationMode::INT`，影响的数据类型为：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、int64_t、uint64_t。

- `SaturationMode::CAST`，影响的目标数据类型为：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、int64_t、uint64_t。

## 返回值说明

无

## 约束说明

<!-- npu="950" id17 -->
- 使用`SaturationMode::FLOAT8`前，需要先处于饱和模式（通过全局`SaturationMode::FLOAT`或单接口`satMode`开启）。
- 对于FP8类型的精度转换，输出为NAN时是否饱和为0由`SaturationMode::FLOAT8`控制：未开启`SaturationMode::FLOAT8`时，即使处于饱和模式（包括全局`SaturationMode::FLOAT`或单接口`satMode`），NAN仍保留；开启`SaturationMode::FLOAT8`后，在饱和模式生效时NAN饱和为0。
<!-- end id17 -->

## 调用示例

以下示例中，输入和输出`LocalTensor`已完成申请及初始化，`count`表示参与计算的元素个数。

- 通过[Adds](../memory_vector_compute/basic_arithmetic/Adds.md)体现`SaturationMode::FLOAT`的配置效果。假设`half`类型的`srcLocal`中各元素均为+inf。

    ```cpp
    // 开启浮点数计算的饱和模式，计算结果为65504。
    AscendC::SetSaturationFlag<AscendC::SaturationMode::FLOAT>(true);
    AscendC::PipeBarrier<PIPE_ALL>();
    AscendC::Adds(satDstLocal, srcLocal, static_cast<half>(0.0), count);

    // 关闭浮点数计算的饱和模式，计算结果为+inf。
    AscendC::SetSaturationFlag<AscendC::SaturationMode::FLOAT>(false);
    AscendC::PipeBarrier<PIPE_ALL>();
    AscendC::Adds(noSatDstLocal, srcLocal, static_cast<half>(0.0), count);
    ```

<!-- npu="950" id18 -->
- 对于Ascend 950PR/Ascend 950DT，通过Memory矢量计算[Cast](../memory_vector_compute/type_conversion/Cast.md)体现`SaturationMode::FLOAT8`的配置效果。假设`float`类型的`srcLocal`中各元素均为nan，两个输出Tensor的数据类型均为`fp8_e5m2_t`。

    ```cpp
    // 选择全局饱和配置，并开启浮点数精度转换的饱和模式。
    AscendC::SetSaturationStrategy<AscendC::OverrideStrategy::USE_GLOBAL>();
    AscendC::SetSaturationFlag<AscendC::SaturationMode::FLOAT>(true);

    // 开启FP8类型的NAN饱和处理，转换结果为0。
    AscendC::SetSaturationFlag<AscendC::SaturationMode::FLOAT8>(true);
    AscendC::PipeBarrier<PIPE_ALL>();
    AscendC::Cast<fp8_e5m2_t, float>(
        satDstLocal, srcLocal, AscendC::RoundMode::CAST_RINT, count);

    // 关闭FP8类型的NAN饱和处理，转换结果为nan。
    AscendC::SetSaturationFlag<AscendC::SaturationMode::FLOAT8>(false);
    AscendC::PipeBarrier<PIPE_ALL>();
    AscendC::Cast<fp8_e5m2_t, float>(
        noSatDstLocal, srcLocal, AscendC::RoundMode::CAST_RINT, count);
    ```
<!-- end id18 -->

- 通过[Add](../memory_vector_compute/basic_arithmetic/Add.md)体现`SaturationMode::INT`的配置效果。假设`int16_t`类型的`src0Local`和`src1Local`中各元素分别为32760和10。

    ```cpp
    // 开启整数计算的饱和模式，计算结果为32767。
    AscendC::SetSaturationFlag<AscendC::SaturationMode::INT>(true);
    AscendC::PipeBarrier<PIPE_ALL>();
    AscendC::Add(satDstLocal, src0Local, src1Local, count);

    // 关闭整数计算的饱和模式，计算结果为-32766。
    AscendC::SetSaturationFlag<AscendC::SaturationMode::INT>(false);
    AscendC::PipeBarrier<PIPE_ALL>();
    AscendC::Add(noSatDstLocal, src0Local, src1Local, count);
    ```

<!-- npu="950" id19 -->
- 对于Ascend 950PR/Ascend 950DT，通过Memory矢量计算[Cast](../memory_vector_compute/type_conversion/Cast.md)体现`SaturationMode::CAST`的配置效果。假设`int16_t`类型的`srcLocal`中各元素均为257，两个输出Tensor的数据类型均为`uint8_t`。

    ```cpp
    // 选择全局饱和配置。
    AscendC::SetSaturationStrategy<AscendC::OverrideStrategy::USE_GLOBAL>();

    // 开启整数精度转换的饱和模式，转换结果为255。
    AscendC::SetSaturationFlag<AscendC::SaturationMode::CAST>(true);
    AscendC::PipeBarrier<PIPE_ALL>();
    AscendC::Cast(satDstLocal, srcLocal, AscendC::RoundMode::CAST_NONE, count);

    // 关闭整数精度转换的饱和模式，转换结果为1。
    AscendC::SetSaturationFlag<AscendC::SaturationMode::CAST>(false);
    AscendC::PipeBarrier<PIPE_ALL>();
    AscendC::Cast(noSatDstLocal, srcLocal, AscendC::RoundMode::CAST_NONE, count);
    ```
<!-- end id19 -->
