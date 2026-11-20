# Non-Contiguous Aligned Store (StoreAlign)<a name="ZH-CN_TOPIC_0000001956110781"></a>

<!-- md-trans-meta sourceCommit=af3f6adafe94d121d34283378bfbe7e348767ea3 translatedAt=2026-11-05T03:34:47.876Z pushedAt=2026-11-18T01:25:30.941Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- npu="9050" id8 -->
- Kirin 9050 products: Supported
<!-- end id8 -->
<!-- npu="kirin1" id9 -->
- KirinDev0001 products: Supported
<!-- end id9 -->
<!-- npu="kirin2" id10 -->
- KirinDev0002 products: Supported
<!-- end id10 -->
<!-- npu="kirin3" id11 -->
- KirinDev0003 products: Supported
<!-- end id11 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/reg_data_store/StoreAlign_noncontinuous_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is: `"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`.

StoreAlign moves data from [RegTensor](../register_data_types/RegTensor.md) to the Unified Buffer (UB) in a non-contiguous manner. It provides two types of APIs:

- Normal movement API: After completing one transfer, the UB address will not be automatically updated, and each iteration requires manual address update.
- PostUpdate extended movement API: After completing one transfer, the UB address will be automatically updated, and no manual address update is required in each iteration.

## Prototype<a name="section620mcpsimp"></a>

- Normal Transfer API

    ```cpp
    template <typename T = DefaultType, DataCopyMode dataMode, typename U>
    __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg, uint32_t dataBlockStride, MaskReg& mask)
    ```

- PostUpdate Extended Transfer API

    ```cpp
    template <typename T = DefaultType, DataCopyMode dataMode, PostLiteral postMode, typename U>
    __simd_callee__ inline void StoreAlign(__ubuf__ T*& dstAddr, U& srcReg, uint32_t dataBlockStride, uint32_t repeatStride, MaskReg& mask)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
|-----|-----|
| T | Data type of the destination operand. For supported data types, see [Data Type](#data-type). |
| dataMode | Copy mode in the non-contiguous alignment scenario. It is of the **DataCopyMode** type. For details, see [Table 3 DataCopyMode parameter description](#table-3-datacopymode-parameter-description). |
| postMode | Controls whether to enable the UB address automatic update function (post update). Type: **PostLiteral**. For specific parameter description, see [PostLiteral](../aux_data_types/PostLiteral.md). |
| U | **RegTensor** type of the source operand. For example RegTensor\<half>, which is automatically deduced by the compiler and does not need to be manually specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
|-----|-----|-----|
| dstAddr | Input/Output | Destination operand, the UB start address, which must be 32-byte aligned. |
| srcReg | Input | Source operand, of type [RegTensor](../register_data_types/RegTensor.md). |
| dataBlockStride | Input | A single instruction copies 8 DataBlocks at a time. This parameter indicates the interval between adjacent DataBlocks (the interval between the head of the preceding DataBlock and the head of the following DataBlock), in DataBlock (32B). |
| repeatStride | Input | Address update step, in DataBlock (32B). repeatStride * sizeof(T) must be 32-byte aligned.<br>&bull; POST_MODE_NORMAL: The actual UB start address for the copy is dstAddr + repeatStride * 32B / sizeof(T), and the address is not updated after the copy.<br>&bull; POST_MODE_UPDATE: The actual UB start address for the copy is dstAddr, and after the copy the address is updated as dstAddr += repeatStride * 32B / sizeof(T). |
| mask | Input | Valid indication of the operation on the source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md).<br>Note: When all elements in a DataBlock are filtered out as invalid by the mask, no error is reported even if the UB is out of bounds. |

<a id="table-3-datacopymode-parameter-description"></a>
**Table 3** DataCopyMode parameter description

| Parameter | Meaning |
|-----|-----|
| DATA_BLOCK_COPY | DataBlock copy mode, that is, copying in units of DataBlock (32B). |

## Data Type

The data types of the destination operand and the source operand must be consistent. The supported data types are: b8, b16, b32.

## Return Value<a name="section1575141714439"></a>

None

## Constraints<a name="section11585101304320"></a>

- The address in the UB must be 32-byte aligned.
- The **regTrait** template parameter of **RegTensor** supports only **RegTraitNumOne**.
- **repeatStride** * 32B / **sizeof(T)** must be 32-byte aligned.

## Example<a name="section642mcpsimp"></a>

```cpp
template <typename T>
__simd_vf__ inline void Compute(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T>();
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        AscendC::Reg::LoadAlign<T, AscendC::Reg::DataCopyMode::DATA_BLOCK_COPY, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(srcReg, srcAddr, 1, i * 8, mask);
        AscendC::Reg::StoreAlign<T, AscendC::Reg::DataCopyMode::DATA_BLOCK_COPY, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(dstAddr, srcReg, 1, i * 8, mask);
    }
}
```

