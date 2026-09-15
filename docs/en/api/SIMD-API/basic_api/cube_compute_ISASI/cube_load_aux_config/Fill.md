# Fill<a name="ZH-CN_TOPIC_0000001834660669"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-27T15:22:26.521Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
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
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->
## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

Initializes the **LocalTensor** at a specific physical storage location to a specific value. Only **LocalTensor** initialization on L1 Buffer/L0A Buffer/L0B Buffer is supported.

As shown in the following figure, the last three rows in the data space are initialized to 0. Configure the **InitConstValueParams** parameters as follows: **initValue**=0 indicates that the initialization value is 0, **repeatTimes**=5 indicates five iterations along the row direction, **blockNum**=3 indicates that each repeat clears three 32-byte data blocks in the column direction to 0, and **dstGap**=5 indicates an interval of five data blocks between adjacent repeats.

**Figure 1** Fill schematic diagram in L1 Buffer<a name="zh-cn_topic_0000002512331632_fig197511640134513"></a>  
![](../../../../figures/Fill_L1.png "Fill schematic diagram in L1 Buffer")

As shown in the following figure, all values in the memory space are initialized to 0. Configure the **InitConstValueParams** parameters as follows: **initValue**=0 indicates that the initialization value is 0, **repeatTimes**=5 indicates five iterations along the row direction, **blockNum**=2 indicates that each repeat clears two 512-byte data blocks in the column direction to 0, and **dstGap**=3 indicates an interval of three data blocks between adjacent repeats.

**Figure 2** Fill schematic diagram in L0A Buffer<a name="zh-cn_topic_0000002512331632_fig11499125816478"></a>  
![](../../../../figures/Fill_L0A.png)

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T, typename U = PrimT<T>, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__aicore__ inline void Fill(const LocalTensor<T>& dst, const InitConstValueParams<U>& initConstValueParams)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template Parameter Description

| Parameter | Description |
| ------- | ------ |
| T | Data type of **dst**. |
| U | Data type of the initialization value.<br>&nbsp;&nbsp;&bull;When **dst** uses a basic data type, **U** and the data type **T** of **dst** must be consistent; otherwise, compilation fails.<br>&nbsp;&nbsp;&bull;When **dst** uses the [TensorTrait](../../aux_data_structures/TensorTrait/TensorTrait.md) type, the **LiteType** of **U** and the data type **T** of **dst** must be consistent; otherwise, compilation fails.<br>&nbsp;&nbsp;The last template parameter is used only for the data type check described above and does not require user attention. |

**Table 2** Parameter Description

| Parameter | Input/Output | Description |
| --------- | ---------- | ------ |
| dst | Output | Destination operand, that is, the result cube, of type **LocalTensor**. |
| InitConstValueParams | Input | Initialization-related parameters, of type **InitConstValueParams**.<br>For the specific definition, see ${INSTALL_DIR}/include/ascendc/basic_api/API/kernel_struct_mm.h. Replace ${INSTALL_DIR} with the file storage path after CANN software installation.<br>For parameter descriptions, see [Table 3](#table3). |

<a name="table3"></a>
**Table 3** InitConstValueParams Structure Parameter Description

| Parameter | Description |
| --------- | ------ |
| repeatTimes | Number of iterations. Default value: 0. Value range: repeatTimes∈[0, 32767]. **Note: repeatTimes = 0 means no execution, and the instruction is treated as a NOP (no operation).** |
| blockNum | Number of data blocks initialized per iteration. Value range: blockNum∈[0, 32767]. Default value: 0. **Note: blockNum = 0 means no execution, and the instruction is treated as a NOP (no operation).**<br>&nbsp;&nbsp;&bull; When the physical storage location of **dst** is L1 Buffer (TPosition: A1/B1), the size of each block is 32B.<br>&nbsp;&nbsp;&bull; When the physical storage location of **dst** is L0A Buffer (TPosition: A2)/L0B Buffer (TPosition: B2), the size of each block is 512B. |
| dstGap | Distance between the end address of the previous iteration and the start address of the next iteration of the destination operand.<br>&nbsp;&nbsp;&bull; When the physical storage location of **dst** is L1 Buffer (TPosition: A1/B1), the unit is 32B.<br>&nbsp;&nbsp;&bull; When the physical storage location of **dst** is L0A Buffer (TPosition: A2)/L0B Buffer (TPosition: B2), the unit is 512B.<br>&nbsp;&nbsp;Value range: dstGap∈[0, 32767]. Default value: 0. |
| initValue | Initialization value. The supported data types are consistent with those of **dst**. |

## Data Type

<!-- npu="910" id10 -->Atlas training products support the following data types: **half**.<!-- end id10 --><br>
<!-- npu="310p" id11 -->Atlas inference products AI Core support the following data types: **int16_t**, **uint16_t**, **half**.<!-- end id11 --><br>
<!-- npu="910b" id12 -->Atlas A2 training products/Atlas A2 inference products support the following data types: **int16_t**, **uint16_t**, **half**, **bfloat16_t**, **int32_t**, **uint32_t**, **float**.<!-- end id12 --><br>
<!-- npu="A3" id13 -->Atlas A3 training products/Atlas A3 inference products support the following data types: **int16_t**, **uint16_t**, **half**, **bfloat16_t**, **int32_t**, **uint32_t**, **float**.<!-- end id13 --><br>
<!-- npu="310b" id14 -->Atlas 200I/500 A2 inference products support the following data types: **int16_t**, **uint16_t**, **half**, **bfloat16_t**, **int32_t**, **uint32_t**, **float**.<!-- end id14 --><br>
<!-- npu="950" id15 -->Ascend 950PR/Ascend 950DT support the following data types: **int16_t**, **uint16_t**, **half**, **bfloat16_t**, **int32_t**, **uint32_t**, **float**.<!-- end id15 --><br>

## Return Value

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- When the destination operand is located in **L0A Buffer**/**L0B Buffer**, the instruction occupies the **PIPE_MTE1** pipeline, and the destination operand address must be 512-byte aligned. When the destination operand is located in **L1 Buffer**, the instruction occupies the **PIPE_MTE2** pipeline, and the destination address must be 32-byte aligned.
- When **repeatTimes** is 0 or **blockNum** is 0, the instruction performs no operation.
- Different models support different configurable parameters when setting **InitConstValueParams**.
    - In the scenario where only the iteration count and initialization value can be configured, other parameters are invalid. Each iteration processes a fixed amount of data (512 bytes), with no gap between iterations.
    - In the scenario where all parameters can be configured, the iteration count, initialization value, number of data blocks processed per iteration, and gap between iterations are supported.
    - In particular, for the following models, only the iteration count and initialization value can be configured.

        <!-- npu="910" id18 -->
        - Atlas training products.
        <!-- end id18 -->
        <!-- npu="310p" id19 -->
        - Atlas inference products AI Core.
        <!-- end id19 -->

- The supported physical storage locations are **L1 Buffer** (**TPosition**: **A1/B1**), **L0A Buffer** (**TPosition**: **A2**), and **L0B Buffer** (**TPosition**: **B2**).
    <!-- npu="950" id20 -->
    - Specifically for **Ascend 950PR/Ascend 950DT**, the supported physical storage location is **L1 Buffer** (**TPosition**: **A1/B1**).
    <!-- end id20 -->
    - If the physical storage location is **L1 Buffer** (**TPosition**: **A1/B1**), the start address must be 32B-aligned.
    - If the physical storage location is **L0A Buffer** (**TPosition**: **A2**) or **L0B Buffer** (**TPosition**: **B2**), the start address must be 512B-aligned.

## Example<a name="section642mcpsimp"></a>

```cpp
uint32_t mLength = 16;
uint32_t kLength = 16;
Fill(leftCube, {1, static_cast<uint16_t>(mLength * kLength * sizeof(float) / 32), 0, 1}); // Fill leftCube with data of value 1 for a length of mLength * kLength, filling at a granularity of 32B.
```

If the API is used to clear a LocalTensor on the L1 Buffer/L0A Buffer/L0B Buffer to zero, see [Fill example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/06_compatibility_guide/fill).
