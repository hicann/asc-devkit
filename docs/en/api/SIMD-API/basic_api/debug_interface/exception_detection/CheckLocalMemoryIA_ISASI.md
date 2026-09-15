# CheckLocalMemoryIA\(ISASI\)<a name="ZH-CN_TOPIC_0000001834740621"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T11:29:01.674Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not Supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not Supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_common_intf.h"`.
Checks the UB read/write behavior within the specified range. If read/write behavior occurs within the specified range, an EXCEPTION error is reported; otherwise, no error is reported.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void CheckLocalMemoryIA(const CheckLocalMemoryIAParam& checkParams)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Parameter Description

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| checkParams | Input | Configures the check behavior for UB access. The type is **CheckLocalMemoryIAParam**.<br>For the specific definition, see `${INSTALL_DIR}`/include/ascendc/basic_api/API/kernel_struct_mm.h. Replace `${INSTALL_DIR}` with the file storage path after the CANN software is installed.<br>For parameter description, see [Table 2](#table15780447181917). |

**Table 2**  Parameter description of the CheckLocalMemoryIAParam structure<a name="table15780447181917"></a>

| Parameter | Description |
| ------ | ------ |
| enableBit | Exception register to configure. Value range: enableBit∈[0,3], defaulting to 0.<br>• 0: Exception register 0.<br>• 1: Exception register 1.<br>• 2: Exception register 2.<br>• 3: Exception register 3. |
| startAddr | Start address to check, 32B-aligned. Value range: startAddr∈[0, 65535], defaulting to 0. For example, **startAddr** can be obtained through `LocalTensor.GetPhyAddr()/32`. |
| endAddr | End address to check, 32B-aligned. Value range: endAddr∈[0, 65535]. Defaulting to 0. |
| isScalarRead | Checks scalar read access.<br>•false: Disabled, defaulting to false.<br>•true: Enabled. |
| isScalarWrite | Checks scalar write access.<br>•false: Disabled, defaulting to false.<br>•true: Enabled. |
| isVectorRead | Checks vector read access.<br>•false: Disabled, defaulting to false.<br>•true: Enabled. |
| isVectorWrite | Checks vector write access.<br>•false: Disabled, defaulting to false.<br>•true: Enabled. |
| isMteRead | Checks MTE read access.<br>•false: Disabled, defaulting to false.<br>•true: Enabled. |
| isMteWrite | Checks MTE write access.<br>•false: Disabled, defaulting to false.<br>•true: Enabled. |
| isEnable | Whether to enable the exception register configured by the **enableBit** parameter.<br>•false: Disabled, defaulting to false.<br>•true: Enabled. |
| reserved | Reserved parameter. Reserved for future functionality. Developers do not need to pay attention to it for now and can use the default value. |

## Constraints<a name="section633mcpsimp"></a>

- The unit of **startAddr**/**endAddr** is 32B. The check range excludes **startAddr** and includes **endAddr**, that is, (startAddr,endAddr].
- After each call to this API, a reset is required (set **isEnable** to **false** to reset).
- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Example<a name="section642mcpsimp"></a>

This example checks whether the vector write access falls within the specified range (startAddr, endAddr]. In this example, the check detects that the vector write is within the specified range, and an error (**ACL_ERROR_RT_VECTOR_CORE_EXCEPTION**) is reported.

```cpp
AscendC::TPipe pipe;
AscendC::TQue<AscendC::TPosition::VECIN, 1> inQueueSrc0, inQueueSrc1;
AscendC::TQue<AscendC::TPosition::VECOUT, 1> outQueueDst;
pipe.InitBuffer(inQueueSrc0, 1, 512 * sizeof(half));
pipe.InitBuffer(inQueueSrc1, 1, 512 * sizeof(half));
pipe.InitBuffer(outQueueDst, 1, 512 * sizeof(half));
AscendC::LocalTensor<half> src0Local = inQueueSrc0.DeQue<half>();
AscendC::LocalTensor<half> src1Local = inQueueSrc1.DeQue<half>();
AscendC::LocalTensor<half> dstLocal = outQueueDst.AllocTensor<half>();
AscendC::CheckLocalMemoryIA({ 0, (uint32_t)(dstLocal.GetPhyAddr() / 32),(uint32_t)((dstLocal.GetPhyAddr() + 512 * sizeof(half)) / 32), false, false, false, true, false, false,
true });
```
