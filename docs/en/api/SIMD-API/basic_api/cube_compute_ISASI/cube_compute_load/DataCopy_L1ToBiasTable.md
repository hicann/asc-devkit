# DataCopy (L1ToBiasTable Buffer Data Copy)<a id="ZH-CN_TOPIC_0000002538071240"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T16:09:03.190Z -->

## Applicable Products<a id="zh-cn_topic_0000002566886667_section796754519912"></a>

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
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
## Description<a id="zh-cn_topic_0000002566886667_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

**DataCopy** data copy supports moving the **Bias** parameters used in cube computation from the L1 Buffer to the BiasTable Buffer.

<!-- npu="A3,910b" id10 -->
For Atlas A2 training products/Atlas A2 inference products and Atlas A3 training products/Atlas A3 inference products, the total size of the BiasTable Buffer is 1 KB.
<!-- end id10 -->
<!-- npu="950" id11 -->
For Ascend 950PR/Ascend 950DT, the total size of the BiasTable Buffer is 4 KB.
<!-- end id11 -->

## Prototype<a id="zh-cn_topic_0000002566886667_section82039854412"></a>

- Continuous Copy scenario.

  ```cpp
  template <typename T>
  __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
  ```
- Continuous or non-continuous copy scenario (high-dimensional slicing).

  ```cpp
  template <typename T>
  __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
  ```
- Continuous or non-continuous copy scenario, supporting inconsistent data types between the source operand and the destination operand.

  ```cpp
  template <typename T, typename U>
  __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& repeatParams)
  ```

## Parameters<a id="zh-cn_topic_0000002566886667_section16128134420472"></a>

**Table 1** Template Parameter Description


| Parameter Name | Input/Output | Meaning                                                                                                 |
| -------------- | ------------ | ------------------------------------------------------------------------------------------------------- |
| T, U           | Input        | Data type of the operand. For supported types, see [Data Type](#zh-cn_topic_0000002566886667_section4219135304818). |

**Table 2** Parameter Description


| Parameter Name | Input/Output | Meaning                                                                                                                                                                                                                                        |
| -------------- | ------------ | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| dst            | Output       | Destination operand of type **LocalTensor**. Stored in the BiasTable Buffer (**TPosition** is C2). The destination address must be 64-byte aligned.                                                                                            |
| src            | Input        | Source operand of type **LocalTensor**. Stored in the L1 Buffer (**TPosition** is C1). The source address must be 32-byte aligned.                                                                                                              |
| count          | Input        | Number of elements to be copied. `count * sizeof(T)` must be 32-byte aligned. If not aligned, the copy amount is rounded down to 32-byte alignment.                                                                                             |
| repeatParams   | Input        | Copy parameters of type **DataCopyParams**. This parameter configures the size, number, and gap of the data blocks to be copied, and supports both non-continuous and continuous copy.<br>For the definition of the structure parameters above, see \$\{INSTALL\_DIR\}/include/ascendc/basic\_api/API/kernel\_struct\_data\_copy.h, where \$\{INSTALL\_DIR\} is the path where the CANN software is installed. See [Table 3](#zh-cn_topic_0000002566886667_table9182515919) for parameter descriptions. |

**Table 3** DataCopyParams Structure Parameter Definition<a id="zh-cn_topic_0000002566886667_table9182515919"></a>


| Parameter Name | Meaning                                                                                                                                                                                                                                                                                                     |
| -------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| blockCount     | Number of consecutive data blocks to be copied. Type: **uint16_t**. Value range: blockCount ∈ [0, 4095].<br>**Note: blockCount = 0 means no copy is performed, and the API is treated as NOP (No Operation).**                                                                                        |
| blockLen       | Length of each consecutive data block to be copied. Type: **uint16_t**. Value range: blockLen ∈ [0, 65535].<br>The unit of blockLen varies by product model. For details, see [Constraint Description](#zh-cn_topic_0000002566886667_section2045914466492).<br>**Note: blockLen = 0 means no copy is performed, and the API is treated as NOP (No Operation).**                                                                      |
| srcGap         | Gap between adjacent consecutive data blocks of the source operand (the gap between the end of the previous data block and the start of the next data block), in units of DataBlock (32 bytes). Type: **uint16_t**. srcGap ∈ [0, 2^16-1].<br>&nbsp;&nbsp;&bull; When blockCount = 1, srcGap is meaningless and can be set to 0.<br>&nbsp;&nbsp;&bull; When blockCount ≠ 1 and srcGap = 0, the first data block of the source operand is copied repeatedly. |
| dstGap         | Gap between adjacent consecutive data blocks of the destination operand (the gap between the end of the previous data block and the start of the next data block). Type: **uint16_t**. dstGap ∈ [0, 2^16-1].<br>The unit of dstGap varies by product model. For details, see [Constraint Description](#zh-cn_topic_0000002566886667_section2045914466492).<br>&nbsp;&nbsp;&bull; When blockCount = 1, dstGap is meaningless and can be set to 0.<br>&nbsp;&nbsp;&bull; When blockCount ≠ 1 and dstGap = 0, each data block copied into the destination cube overwrites the first data block. |

## Data Type<a id="zh-cn_topic_0000002566886667_section4219135304818"></a>

The source operand and destination operand support different data types. For details, see the following:

<!-- npu="950" id8 -->
For Ascend 950PR/Ascend 950DT, **src** supports the following data types: half, bfloat16_t, int32_t, float; **dst** supports the following data types: int32_t, float.
<!-- end id8 -->
<!-- npu="A3" id9 -->
For Atlas A3 training products/Atlas A3 inference products, **src** supports the following data types: half, int32_t, float; **dst** supports the following data types: int32_t, float.
<!-- end id9 -->
<!-- npu="910b" id12 -->
For Atlas A2 training products/Atlas A2 inference products, **src** supports the following data types: half, int32_t, float; **dst** supports the following data types: int32_t, float.
<!-- end id12 -->

## Return Value

None

## Constraints<a id="zh-cn_topic_0000002566886667_section2045914466492"></a>

- The physical memory of **dst** is the BiasTable Buffer, whose start address must be 64-byte aligned. The physical memory of **src** is the L1 Buffer, whose start address must be 32-byte aligned.
- Value range of **blockCount**: **blockCount**∈[0, 4095]. When **blockCount** = 0, the instruction is not executed and this API is treated as a NOP (No Operation).
- **srcGap**∈[0, 2^16-1], **dstGap**∈[0, 2^16-1].
- If the destination **dst** address in the BiasTable Buffer overflows, an exception is raised.
- The unit of **blockLen** varies by model. For details, see the following:
  <!-- npu="950" id13 -->
  - For Ascend 950PR/Ascend 950DT, the unit is 32 bytes, and **blockLen** must be an even number.
  <!-- end id13 -->
  <!-- npu="A3" id14 -->
  - For Atlas A3 training products/Atlas A3 inference products, the unit is 64 bytes.
  <!-- end id14 -->
  <!-- npu="910b" id15 -->
  - For Atlas A2 training products/Atlas A2 inference products, the unit is 64 bytes.
  <!-- end id15 -->
- The unit of **dstGap** varies by model. For details, see the following:
  <!-- npu="950" id16 -->
  - For Ascend 950PR/Ascend 950DT, the unit is 32 bytes.
  <!-- end id16 -->
  <!-- npu="A3" id17 -->
  - For Atlas A3 training products/Atlas A3 inference products, the unit is 64 bytes.
  <!-- end id17 -->
  <!-- npu="910b" id18 -->
  - For Atlas A2 training products/Atlas A2 inference products, the unit is 64 bytes.
  <!-- end id18 -->

## Example<a id="zh-cn_topic_0000002566886667_section088124295117"></a>

Copy the Bias parameters from the L1 Buffer to the BiasTable Buffer.

```cpp
// c1Addr = 128 * 1024, c2Addr = 0; when __NPU_ARCH__ == 3510, c12c2Params = {1, 32, 0, 0}; otherwise, {1, 16, 0, 0}.
AscendC::LocalTensor<outputType> bias1Local(AscendC::TPosition::C1, c1Addr, biasSizeAlign);
AscendC::LocalTensor<outputType> bias2Local(AscendC::TPosition::C2, c2Addr, biasSizeAlign);

if constexpr (SCENARIO_NUM == 4) {
    SplitBias(bias1Local, bias2Local);
}

__aicore__ inline void SplitBias(AscendC::LocalTensor<outputType> bias1Local,
                                AscendC::LocalTensor<outputType> bias2Local)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    AscendC::DataCopyParams c12c2Params =
        {1, static_cast<uint16_t>(biasSizeAlign * sizeof(outputType) / 32), 0, 0};
#else
    AscendC::DataCopyParams c12c2Params =
        {1, static_cast<uint16_t>(biasSizeAlign * sizeof(outputType) / 64), 0, 0};
#endif
    AscendC::DataCopy(bias2Local, bias1Local, c12c2Params);
}
```

For the complete example, see Scenario 4 in the [data_copy_gm2l1 sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_gm2l1).
