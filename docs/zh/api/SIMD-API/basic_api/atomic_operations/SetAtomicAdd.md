# SetAtomicAdd<a name="ZH-CN_TOPIC_0000001487536650"></a>

## 产品支持情况

<!-- npu="950" id13 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id13 -->
<!-- npu="A3" id14 -->
- Atlas A3系列产品：支持
<!-- end id14 -->
<!-- npu="910b" id15 -->
- Atlas A2系列产品：支持
<!-- end id15 -->
<!-- npu="310b" id16 -->
- Atlas 200I/500 A2推理产品：支持
<!-- end id16 -->
<!-- npu="310p" id17 -->
- Atlas推理系列产品AI Core：支持
<!-- end id17 -->
<!-- npu="310p" id18 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id18 -->
<!-- npu="910" id19 -->
- Atlas训练系列产品：支持
<!-- end id19 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/atomic_operations/SetAtomicAdd_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_set_atomic_intf.h"`。

对后续目的地址为GM的数据搬运开启原子累加。原子累加过程：将待拷贝的内容和GM已有内容进行求和，然后将求和结果写入GM。SetAtomicAdd接口可通过模板参数设定不同的累加数据类型。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void SetAtomicAdd()
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
|--------|------|
| T | 设定不同的原子累加操作数据类型。 |

## 数据类型

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品，支持的数据类型为int8_t、int16_t、half、bfloat16_t、int32_t、float。
<!-- end id1 -->

<!-- npu="A3" id2 -->
- Atlas A3系列产品，支持的数据类型为int8_t、int16_t、half、bfloat16_t、int32_t、float。
<!-- end id2 -->

<!-- npu="910b" id3 -->
- Atlas A2系列产品，支持的数据类型为int8_t、int16_t、half、bfloat16_t、int32_t、float。
<!-- end id3 -->

<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品，支持的数据类型为int16_t、half、int32_t、float。
<!-- end id4 -->

<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core，支持的数据类型为int16_t、half、float。
<!-- end id5 -->

<!-- npu="910" id6 -->
- Atlas训练系列产品，支持的数据类型为float。
<!-- end id6 -->

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

<!-- npu="950" id7 -->
- Ascend 950PR&950DT系列产品，支持的数据通路为UB/L0C Buffer-\>GM。
<!-- end id7 -->

<!-- npu="A3" id8 -->
- Atlas A3系列产品，支持的数据通路为UB/L0C Buffer/L1 Buffer-\>GM。
<!-- end id8 -->

<!-- npu="910b" id9 -->
- Atlas A2系列产品，支持的数据通路为UB/L0C Buffer/L1 Buffer-\>GM。
<!-- end id9 -->

<!-- npu="310b" id10 -->
- Atlas 200I/500 A2推理产品，支持的数据通路为UB/L0C Buffer/L1 Buffer-\>GM。
<!-- end id10 -->

<!-- npu="310p" id11 -->
- Atlas推理系列产品AI Core，支持的数据通路为UB-\>GM。
<!-- end id11 -->

<!-- npu="910" id12 -->
- Atlas训练系列产品，支持的数据通路为UB-\>GM。
<!-- end id12 -->

- 累加操作完成后，建议通过[DisableDmaAtomic](DisableDmaAtomic.md)关闭原子累加，以免影响后续相关指令功能。
- 该接口执行前不会自动清零GM数据。开发者需根据算子逻辑判断是否清零，如需清零，请在执行前手动完成。
- SetAtomicAdd内部已集成与[SetAtomicType](SetAtomicType.md)相同的功能。建议调用上述接口时，通过设置模板参数显式指定原子操作的数据类型，无需额外调用SetAtomicType接口。
<!-- npu="A3,910b" id20 -->
<a id="npu_smi_saturation_mode"></a>

- 针对[NPU架构版本2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)，整数类型和bfloat16_t数据类型的默认计算模式**均为非饱和模式**。原子累加的饱和模式无法通过[SetSaturationStrategy](../special_register_access/SetSaturationStrategy.md)或[SetCtrlSpr](../special_register_access/SetCtrlSpr_ISASI.md)设置，需通过`npu-smi`命令在设备级进行配置，各命令的详细说明请参考[Ascend HDK对应产品的《npu-smi命令参考》](https://support.huawei.com/enterprise/zh/atlas-computing/ascend-hdk-pid-252764743?category=reference-guides&subcategory=command-reference)。CANN与Ascend HDK的对应关系，请参考[版本配套查询助手](https://www.hiascend.com/developer/download/compatibility)。

    - 整数类型：设置饱和模式（溢出值饱和为目标数据类型的最大值或最小值）或截断模式（即非饱和模式，溢出值保留低位、舍弃高位，默认为非饱和模式）。

        ```bash
        # 设置整数类型的饱和模式，value取1表示饱和模式，取0表示截断模式（非饱和模式，默认值）
        npu-smi set -t int-saturation-mode -d value

        # 查询当前配置
        npu-smi info -t int-saturation-mode
        ```

    - bfloat16_t：设置饱和模式（INF饱和为±MAX、NAN饱和为0）或非饱和模式（INF/NAN保持原输出，默认为非饱和模式）。

        ```bash
        # 设置bfloat16_t的饱和模式，value取1表示饱和模式，取0表示非饱和模式（默认值）
        npu-smi set -t bf16-saturation-mode -d value

        # 查询当前配置
        npu-smi info -t bf16-saturation-mode
        ```

        若未设置过bfloat16_t的饱和计算模式（例如设备重启后配置被清除），查询命令的回显如下：

        ```text
        The valid state is not set, please set saturation mode first.
        Failed to query "bf16-saturation-mode" info.
        ```

    - 若当前设备不支持上述命令，设置命令与查询命令的回显如下（以int-saturation-mode为例）：

        ```text
        This device does not support setting int-saturation-mode.
        This device does not support querying int-saturation-mode.
        ```

    - 上述命令需由root用户在物理机+特权容器场景或直通虚拟机+特权容器场景下执行；设备重启后配置不保留，整数类型的饱和模式恢复为默认的截断模式，bfloat16_t的饱和模式配置被清除。
<!-- end id20 -->

## 调用示例<a name="section177231425115410"></a>

```cpp
AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
AscendC::LocalTensor<T> srcLocal = ubAllocator.Alloc<T, SIZE>();

AscendC::DisableDmaAtomic();
AscendC::DataCopy(srcLocal, srcGlobal, SIZE);
AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
AscendC::SyncAll();

// 开启原子累加，将Unified Buffer（UB）数据原子累加到GM
AscendC::SetAtomicAdd<T>();
AscendC::DataCopy(dstGlobal, srcLocal, SIZE);

// 关闭原子累加
AscendC::DisableDmaAtomic();
```

完整样例请参考[DataMovementWithAtomicOperations样例](../../../../../../examples/01_simd_cpp_api/03_basic_api/06_atomic/data_movement_with_atomic_operations/data_movement_with_atomic_operations.asc)。
