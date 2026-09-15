# DataCopyPad (UB to L1 Non-Aligned Data Copy)

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T16:19:54.381Z -->

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

## Description<a name="section618mcpsimp"></a>

> [!NOTE]Description
> This API is implemented through software simulation. It is built on top of the Matmul high-level API and uses the workspace GM space in the Matmul high-level API as the data relay space. Data is first moved into GM and then into the L1 Buffer. Therefore, before using this API, you must first use REGISTER_MATMUL to register the high-level API.

This API provides non-aligned data copy from the Unified Buffer to the L1 Buffer.

## Prototype<a name="section620mcpsimp"></a>

- Path: **Local Memory** -> **Local Memory**. The actual copy process is **UB** -> **GM** -> **L1 Buffer** (**TSCM**).

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopyPad(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams, const Nd2NzParams& nd2nzParams)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>

| Parameter | Description |
| --- | --- |
| T | Data type of the operand and **paddingValue** (the value to be filled). |

**Table 2** API parameter description

<a name="zh-cn_topic_0235751031_table33761356"></a>

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand, of type **LocalTensor**.<br>The start address of **LocalTensor** must be 32-byte aligned. |
| src | Input | Source operand, of type **LocalTensor**.<br>The start address of **LocalTensor** must be 32-byte aligned. |
| dataCopyParams | Input | Copy parameters.<br>&nbsp;&nbsp;&bull; Of type **DataCopyExtParams**. For details about the parameters, see [Table 4](#table10572141063919). |
| nd2nzParams | Input | When copying data from UB to L1 Buffer (VECIN/VECOUT to TSCM), ND-to-NZ data format conversion can be performed. **nd2nzParams** is the parameter used to control data format conversion, of type **Nd2NzParams**. For details about the parameters, see [DataCopy (UBToL1 conversion-ND2NZ copy)](DataCopy_UBToL1_ND2NZ.md).<br>**Note: For this API, ndNum of Nd2NzParams can only be set to 1.** |

**Table 3** DataCopyExtParams structure parameter definition

<a name="table10572141063919"></a>

| Parameter | Description |
| --- | --- |
| blockCount | Specifies the number of consecutive transferred data blocks contained in this instruction. The data type is **uint16_t**, and the value range is blockCount ∈ [1, 4095]. |
| blockLen | Specifies the length of each consecutive transferred data block in this instruction. **This instruction supports unaligned copy**, and **the unit of each consecutive transferred data block length is byte**. The data type is **uint32_t**, and the value range is blockLen ∈ [1, 2097151]. **blockLen must be an integer multiple of sizeof(T). Note that UB must not exceed the limit.** |
| srcStride | Source operand, the interval between adjacent consecutive data blocks (the interval between the tail of the previous data block and the head of the following data block). The unit is dataBlock (32 bytes).<br>The data type is **uint32_t**. **srcStride** must not exceed the value range of this data type. |
| dstStride | Destination operand, the interval between adjacent consecutive data blocks (the interval between the tail of the previous data block and the head of the following data block). The unit is dataBlock (32 bytes). The data type is **uint32_t**. **dstStride** must not exceed the value range of this data type. |
| rsv | Reserved field. |

The following provides configuration examples for the following scenarios:

- **UB->L1 Buffer (VECIN/VECOUT->TSCM)**

    > [!CAUTION]Note
    > The internal implementation involves communication between the AIC and the AIV. The actual copy path is UB (VECIN/VECOUT) -> GM -> L1 Buffer (TSCM). **Sending communication messages incurs overhead, which affects performance.**

    As shown in [Figure 1 UB->L1 Buffer (VECIN/VECOUT->TSCM) copy diagram](#fig9329040132719), the process of copying from UB to GM and then to L1 Buffer is illustrated: in the example, the data type is half, a single datablock (32 bytes) contains 16 half elements, and A1\~A6, B1\~B6, and C1\~C6 in the source operand are the data to be copied.

    - For the copy from UB to GM, the data storage format does not change and remains ND.
        - **blockCount** is the number of adjacent consecutive transferred data blocks to be copied, set to 3;
        - **blockLen** is the size of one adjacent consecutive transferred data block (in bytes), set to 6 \* 32 = 192;
        - **srcStride** is the interval between adjacent consecutive data blocks of the source operand (the interval between the tail of the previous data block and the head of the following data block). The physical location of the source operand is UB, and its unit is datablock. The two adjacent consecutive transferred data blocks (A1\~A6 and B1\~B6) are separated by one A7; therefore, **srcStride** is set to 1;
        - **dstStride** is the interval between adjacent consecutive data blocks of the destination operand (the interval between the tail of the previous data block and the head of the following data block). The logical location of the destination operand is GM, and its unit is byte. The two adjacent consecutive transferred data blocks (A1\~A6 and B1\~B6) are separated by two blank datablocks; therefore, **dstStride** is set to 64 bytes.

    - For the copy from GM to L1 Buffer, the data storage format is converted from ND to NZ.
        - **ndNum** is fixed to 1, that is, A1~A6, B1~B6, and C1~C6 are treated as one entire ndCube;
        - **nValue** is the number of rows of the ndCube, that is, 3 rows;
        - **dValue** is the number of elements contained in one row of the ndCube, that is, 6 \* 16 = 96 elements;
        - **srcNdCubeStride** is the distance between adjacent ndMatrices. Because only one ndCube is involved, it can be set to 0;
        - **srcDValue** indicates the number of elements between row x and row x+1 of the ndCube, such as the distance from A1 to B1, that is, 8 datablocks, 8 \* 16 = 128 elements;
        - **dstNzC0Stride** is the number of datablocks between adjacent datablocks in the same row of src in the NZ cube, such as the distance from A1 to A2, that is, 7 datablocks (A1 + blank + B1 + blank + C1 + blank \* 2);
        - **dstNzNStride** is the number of datablocks between adjacent rows of the ndCube in src in the NZ cube, such as the distance from A1 to B1, that is, 2 datablocks (A1 + blank);
        - **dstNzCubeStride** is the number of elements between adjacent NZ matrices. Because only one NZ cube is involved, it can be set to 0.

    **Figure 1** UB->L1 Buffer (VECIN/VECOUT->TSCM) copy diagram<a name="fig9329040132719"></a>  

    ![](../../../../figures/vecin_vecout_tscm_move.png "VECIN-VECOUT--TSCM copy diagram")

## Data Types

<!-- npu="950" id8 -->
Ascend 950PR/Ascend 950DT: The supported data types are bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, and complex64.
<!-- end id8 -->

<!-- npu="A3" id9 -->
Atlas A3 training products/Atlas A3 inference products: The supported data types are int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id9 -->

<!-- npu="910b" id10 -->
Atlas A2 training products/Atlas A2 inference products: The supported data types are int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, and double.
<!-- end id10 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

<!-- npu="950" id11 -->
- In particular, for Ascend 950PR/Ascend 950DT, when setting the **srcStride** and **dstStride** parameters, the data type is **int64_t**, with a value range of [0, 65535].
<!-- end id11 -->

## Example<a name="section177231425115410"></a>

The following example shows the scenario illustrated in [Figure 1](#fig9329040132719): the data type is half, a single DataBlock (32 bytes) contains 16 half elements, and the ND-format data in UB is transferred to the L1 Buffer in NZ format via GM as an intermediate.

```cpp
constexpr uint32_t dataBlockElemCount = 16;
constexpr uint32_t srcElemCount = (3 * 7) * dataBlockElemCount;
constexpr uint32_t dstElemCount = (7 * 6) * dataBlockElemCount;
constexpr uint32_t tscmAddr = 0;

// Source operand: UB.
AscendC::LocalTensor<half> srcLocal(AscendC::TPosition::VECIN, ubAddr, srcElemCount);

// Destination operand: L1 Buffer.
AscendC::LocalTensor<half> dstLocal(AscendC::TPosition::TSCM, tscmAddr, dstElemCount);

AscendC::DataCopyExtParams copyParams;
// In the VECIN->GM phase, copy 3 consecutive transferred data blocks.
copyParams.blockCount = 3;
// Each consecutive transferred data block contains 6 DataBlocks.
copyParams.blockLen = 6 * 32;
// The source operand is in UB, in units of DataBlock; skip 1 DataBlock between adjacent rows.
copyParams.srcStride = 1;
// The intermediate destination is GM, in units of bytes; reserve 2 blank DataBlocks, that is, 64B, between adjacent rows.
copyParams.dstStride = 64;
// Reserved field, fixed to 0.
copyParams.rsv = 0;

AscendC::Nd2NzParams nd2nzParams;
// This API constrains ndNum to be set to 1 only.
nd2nzParams.ndNum = 1;
// The ND cube height is 3 rows.
nd2nzParams.nValue = 3;
// The width of each row is 6 DataBlocks.
nd2nzParams.dValue = 6 * dataBlockElemCount;
// There is only one ND cube, so no interval between adjacent ND matrices is involved.
nd2nzParams.srcNdCubeStride = 0;
// The interval between the start addresses of adjacent rows in GM is 8 DataBlocks.
nd2nzParams.srcDValue = 8 * dataBlockElemCount;
// In the destination NZ, the interval between adjacent DataBlocks of the same source row is 7 DataBlocks.
nd2nzParams.dstNzC0Stride = 7;
// In the destination NZ, the interval between the start addresses of adjacent source rows after conversion is 2 DataBlocks.
nd2nzParams.dstNzNStride = 2;
// There is only one NZ cube.
nd2nzParams.dstNzCubeStride = 0;

AscendC::DataCopyPad(dstLocal, srcLocal, copyParams, nd2nzParams);
```
