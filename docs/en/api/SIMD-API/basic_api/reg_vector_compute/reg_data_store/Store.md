# Store

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:51:46.462Z pushedAt=2026-10-26T07:41:44.586Z nl=2 -->

## Applicable Products

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/reg_data_store/Store_res.md#id1 -->

## Description

The header file path is `"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`.

The Reg vector computation data movement API supports moving data out of a RegTensor to a Unified Buffer (UB) address **dstAddr** that is not 32-byte aligned. During continuous movement, the user must manually update the **dstAddr** address.

This API encapsulates [StoreUnAlign and StoreUnAlignPost](StoreUnAlign_continuous.md).

## Prototype

-   The transfer amount is **VL**.

    ```cpp
    template <typename T = DefaultType, typename U>
    __simd_callee__ inline void Store(__ubuf__ T* dstAddr, U& srcReg)
    ```

-   Transfers **count** data elements.

    ```cpp
    template <typename T = DefaultType, typename U>
    __simd_callee__ inline void Store(__ubuf__ T* dstAddr, U& srcReg, uint32_t count)
    ```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [Data Types](#data-types). |
| U | RegTensor type of the source operand, for example RegTensor\<half>, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2**  Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstAddr | Output | Destination operand, the UB start address, which does not need to be 32-byte aligned. |
| srcReg | Input | Source operand, of type [RegTensor](../register_data_types/RegTensor.md). |
| count | Input | Amount of data to be moved. For continuous movement, update the address manually: dstAddr = dstAddr + count.|

## Data Types

The data types of the destination operand and the source operand must be consistent.
- When the **regTrait** template parameter of **RegTensor** is **RegTraitNumOne**, the supported data types are: **b8**, **b16**, **b32**, and **b64**.
- When the **regTrait** template parameter of **RegTensor** is **RegTraitNumTwo**, the supported data types are: **complex32** and **b64**.

## Return Value

None

## Constraints

-   **count** cannot exceed the number of data elements that a **RegTensor** can store, that is, **count** <= 256B / sizeof(T).
-   The API internally defines an [UnalignRegForStore](../register_data_types/UnalignRegForLoad-UnalignRegForStore.md), and the maximum number of such registers is 4. If this limit is exceeded, the compiler reports an error.

## Example

- Move out one **VL** of data.
    ```cpp
    template<typename T>
    __simd_vf__ inline void LoadStoreVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t count, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            AscendC::Reg::Load(srcReg, srcAddr + i * count);
            AscendC::Reg::Store(dstAddr + i * count, srcReg);
        }
    }
    ```

- Move out **count** data elements.
    ```cpp
    template<typename T>
    __simd_vf__ inline void LoadStoreVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t count, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            AscendC::Reg::Load(srcReg, srcAddr + i * count);
            AscendC::Reg::Store(dstAddr + i * count, srcReg, count);
        }
    }
    ```
