# Store

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T22:30:15.894Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
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

## Description

The header file path is `"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`.

The Reg vector computation data transfer API supports moving data out of a RegTensor to a non-32-byte-aligned Unified Buffer (UB) address **dstAddr**. During continuous transfer, the user must manually update the **dstAddr** address.

This API encapsulates [StoreUnAlign and StoreUnAlignPost](StoreUnAlign_continuous.md).

## Prototype

-   The transfer amount is VL.

    ```cpp
    template <typename T = DefaultType, typename U>
    __simd_callee__ inline void Store(__ubuf__ T* dstAddr, U& srcReg)
    ```

-   Transfers count data elements.

    ```cpp
    template <typename T = DefaultType, typename U>
    __simd_callee__ inline void Store(__ubuf__ T* dstAddr, U& srcReg, uint32_t count)
    ```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [data type](#data-type). |
| U | RegTensor type of the source operand, for example, RegTensor\<half>. It is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstAddr | Output | Destination operand, the starting address in the Unified Buffer (UB), which does not require 32-byte alignment. |
| srcReg | Input | Source operand, of type [RegTensor](../register_data_types/RegTensor.md). |
| count | Input | Amount of data to transfer. During continuous transfer, the address must be updated manually: dstAddr = dstAddr + count. |

## Data Type

The data types of the destination operand and the source operand must be consistent.
- When the **RegTensor** template parameter **regTrait** is **RegTraitNumOne**, the supported data types are **b8**, **b16**, **b32**, and **b64**.
- When the **RegTensor** template parameter **regTrait** is **RegTraitNumTwo**, the supported data types are **complex32** and **b64**.

## Return Value

None

## Constraints

-   **count** cannot exceed the number of data elements that a **RegTensor** can store, that is, count <= 256B / sizeof(T).
-   The API internally defines an [UnalignRegForStore](../register_data_types/UnalignRegForLoad-UnalignRegForStore.md), and the maximum number of such registers is 4.

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
