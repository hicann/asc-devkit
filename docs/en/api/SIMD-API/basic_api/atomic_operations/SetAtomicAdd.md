# SetAtomicAdd<a name="ZH-CN_TOPIC_0000001487536650"></a>

<!-- md-trans-meta sourceCommit=9438de7fdc69ddeeaffbc929f8de5e675de9504c translatedAt=2026-09-23T10:28:43.974Z pushedAt=2026-10-26T07:41:43.955Z nl=2 -->

## Applicable Products

<!-- npu="950" id13 -->
- Ascend 950PR&950DT products: supported
<!-- end id13 -->
<!-- npu="A3" id14 -->
- Atlas A3 products: supported
<!-- end id14 -->
<!-- npu="910b" id15 -->
- Atlas A2 products: supported
<!-- end id15 -->
<!-- npu="310b" id16 -->
- Atlas 200I/500 A2 inference products: supported
<!-- end id16 -->
<!-- npu="310p" id17 -->
- Atlas inference products AI Core: supported
<!-- end id17 -->
<!-- npu="310p" id18 -->
- Atlas inference products Vector Core: not supported
<!-- end id18 -->
<!-- npu="910" id19 -->
- Atlas training products: supported
<!-- end id19 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/atomic_operations/SetAtomicAdd_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_set_atomic_intf.h"`.

Enables atomic accumulation for subsequent data transfers whose destination address is GM. The atomic accumulation process sums the content to be copied with the existing content in GM, and then writes the summation result to GM. The **SetAtomicAdd** API can set different accumulation data types through template parameters.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void SetAtomicAdd()
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
|--------|------|
| T | Sets the data type for different atomic accumulation operations. |

## Data Type

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products support the following data types: int8_t, int16_t, half, bfloat16_t, int32_t, and float.
<!-- end id1 -->

<!-- npu="A3" id2 -->
- Atlas A3 products support the following data types: int8_t, int16_t, half, bfloat16_t, int32_t, and float.
<!-- end id2 -->

<!-- npu="910b" id3 -->
- Atlas A2 products support the following data types: int8_t, int16_t, half, bfloat16_t, int32_t, and float.
<!-- end id3 -->

<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products support the following data types: int16_t, half, int32_t, and float.
<!-- end id4 -->

<!-- npu="310p" id5 -->
- Atlas inference products AI Core support the following data types: int16_t, half, and float.
<!-- end id5 -->

<!-- npu="910" id6 -->
- Atlas training products support the following data type: float.
<!-- end id6 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

<!-- npu="950" id7 -->
- For Ascend 950PR&950DT products, the supported data paths are UB/L0C Buffer-\>GM.
<!-- end id7 -->

<!-- npu="A3" id8 -->
- For Atlas A3 products, the supported data paths are UB/L0C Buffer/L1 Buffer-\>GM.
<!-- end id8 -->

<!-- npu="910b" id9 -->
- For Atlas A2 products, the supported data paths are UB/L0C Buffer/L1 Buffer-\>GM.
<!-- end id9 -->

<!-- npu="310b" id10 -->
- For Atlas 200I/500 A2 inference products, the supported data paths are UB/L0C Buffer/L1 Buffer-\>GM.
<!-- end id10 -->

<!-- npu="310p" id11 -->
- For the Atlas inference products AI Core, the supported data path is UB->GM.
<!-- end id11 -->

<!-- npu="910" id12 -->
- For Atlas training products, the supported data paths are UB-\>GM.
<!-- end id12 -->

- After the accumulation operation is complete, it is recommended to disable atomic accumulation by calling [DisableDmaAtomic](DisableDmaAtomic.md) to avoid affecting the functionality of subsequent related instructions.
- This API does not automatically clear the GM data before execution. Developers need to determine whether to clear it based on the operator logic. If clearing is required, perform it manually before execution.
- SetAtomicAdd has integrated the same functionality as [SetAtomicType](SetAtomicType.md). It is recommended that when calling the above API, you explicitly specify the data type of the atomic operation by setting the template parameter, without the need to additionally call the SetAtomicType API.
<!-- npu="A3,910b" id20 -->
<a id="npu_smi_saturation_mode"></a>

- For [NPU architecture version 2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md), the default computation mode of integer types and the bfloat16_t data type is **non-saturation**. The saturation mode of atomic accumulation cannot be set through [SetSaturationStrategy](../special_register_access/SetSaturationStrategy.md) or [SetCtrlSpr](../special_register_access/SetCtrlSpr_ISASI.md). It must be configured at the device level through the `npu-smi` command. For details about each command, see the *npu-smi Command Reference* of the corresponding Ascend HDK product at [Ascend HDK](https://support.huawei.com/enterprise/en/atlas-computing/ascend-hdk-pid-252764743?category=reference-guides&subcategory=command-reference). For the mapping between CANN and Ascend HDK, see the [Version Matching Query Assistant](https://www.hiascend.com/developer/download/compatibility).

    - Integer type: Set the saturation mode (the overflow value is saturated to the maximum or minimum value of the target data type) or the truncation mode (that is, the non-saturation mode, in which the overflow value retains the low bits and discards the high bits; the default is the non-saturation mode).

        ```bash
        # Set the saturation mode for integer types. A value of 1 indicates the saturation mode, and a value of 0 indicates the truncation mode (non-saturation mode, the default value).
        npu-smi set -t int-saturation-mode -d value

        # Query the current configuration.
        npu-smi info -t int-saturation-mode
        ```

    - bfloat16_t: Set the saturation mode (INF is saturated to ±MAX and NAN is saturated to 0) or the non-saturation mode (INF/NAN retain the original output; the default is the non-saturation mode).

        ```bash
        # Set the saturation mode for bfloat16_t. A value of 1 indicates the saturation mode, and a value of 0 indicates the non-saturation mode (the default value).
        npu-smi set -t bf16-saturation-mode -d value

        # Query the current configuration.
        npu-smi info -t bf16-saturation-mode
        ```

        If the saturation computation mode of bfloat16_t has never been set (for example, the configuration is cleared after the device restarts), the query command returns the following output:

        ```text
        The valid state is not set, please set saturation mode first.
        Failed to query "bf16-saturation-mode" info.
        ```

    - If the current device does not support the preceding commands, the setting command and the query command return the following output (using int-saturation-mode as an example):

        ```text
        This device does not support setting int-saturation-mode.
        This device does not support querying int-saturation-mode.
        ```

    - The preceding commands must be executed by the root user in a physical machine + privileged container scenario or a passthrough VM + privileged container scenario. The configuration is not retained after the device restarts: the saturation mode of the integer type is restored to the default truncation mode, and the saturation mode configuration of bfloat16_t is cleared.
<!-- end id20 -->

## Example<a name="section177231425115410"></a>

```cpp
AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
AscendC::LocalTensor<T> srcLocal = ubAllocator.Alloc<T, SIZE>();

AscendC::DisableDmaAtomic();
AscendC::DataCopy(srcLocal, srcGlobal, SIZE);
AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
AscendC::SyncAll();

// Enable atomic accumulation to atomically accumulate Unified Buffer (UB) data into GM.
AscendC::SetAtomicAdd<T>();
AscendC::DataCopy(dstGlobal, srcLocal, SIZE);

// Disable atomic accumulation.
AscendC::DisableDmaAtomic();
```

For the complete sample, see [DataMovementWithAtomicOperations sample](../../../../../../examples/01_simd_cpp_api/03_basic_api/06_atomic/data_movement_with_atomic_operations/data_movement_with_atomic_operations.asc).
