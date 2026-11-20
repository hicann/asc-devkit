# Overview

<!-- md-trans-meta sourceCommit=5c07153f668df7dd8585d7a94d76b4114ca4b1ac translatedAt=2026-11-05T03:54:22.318Z pushedAt=2026-11-18T01:25:30.975Z nl=2 -->

The Reg Vector Compute API is developed for the Reg vector computing architecture. It allows you to directly operate the registers involved in vector computing on the chip, achieving greater flexibility and better performance. The Reg Vector Compute API is functionally similar to the Memory Vector Compute API. However, unlike the Memory Vector Compute API, whose input and output data must be LocalTensor, the input or output data of the Reg Vector Compute API are all Reg vector computing registers. For compute APIs, the function is to obtain data from the given registers, perform computation, and store the result in the given registers. For data movement APIs, the function is to move data between the Unified Buffer (UB) and registers. Therefore, compared with the Memory Vector Compute API, the Reg Vector Compute API leaves the data movement and Reg computation processes under your control, achieving greater development freedom.

## How to Use the Reg Vector Compute API

The register-based programming model loads data from LocalTensor into registers through Reg data movement instructions (Reg data load), performs complex mathematical computation (Compute), and then moves the data back to LocalTensor through Reg data movement instructions (Reg data store). All computation logic is completed in registers, which reduces data movement between LocalTensors and greatly improves overall performance. The specific process is as follows:

**Figure 1**  Reg vector computing  
![](../../../figures/reg_vector_overview.png "Reg vector computing")

Using the [Reg Vector Compute Programming Example](#reg-vector-compute-programming-example) as an example, a complete VectorFunction computation process consists of the following parts:
- Write and call VectorFunctions (vector functions). These functions are marked with __simd_vf__ and are called through [asc\_vf\_call](./vf_call/asc_vf_call.md);
- Define vector data registers [RegTensor](./register_data_types/RegTensor.md) and mask registers [MaskReg](./register_data_types/MaskReg.md);
- Write a for loop and map it to a hardware loop ([HardwareLoop](./key_features/key_features.md#hardware-loop)); the [UpdateMask](./register_data_types/MaskReg.md#mask-setting-method) function updates the mask involved in the computation, and each loop consumes one VL length of elements.
- Call the Reg data load API in the loop to perform continuous aligned load ([LoadAlign](./reg_data_load/LoadAlign_continuous.md)) to load a single VL length of data from UB;
- Call the Reg compute API Add in the loop to complete a single Repeat computation;
- Call the Reg data store API in the loop to perform continuous aligned store ([StoreAlign](./reg_data_store/StoreAlign_continuous.md)) to store the computed data to UB;

## Reg Vector Compute Programming Example

```cpp
template <typename T>
__simd_vf__ inline void AddVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}

template <typename T>
__aicore__ inline void Compute()
{
    // Move GM data to UB addresses src0Addr and src1Addr.
    ...
    // Call the vf function.
    asc_vf_call<AddVF<T>>(dstAddr, src0Addr, src1Addr, count, oneRepeatSize, repeatTimes);
    // Move data at UB address dstAddr to GM.
    ...
}
```

>[!NOTE]Note 
>- Data movement between GM and UB must be completed through [GM and UB data movement](../memory_vector_compute/data_move/data_move.md).
>- The pipeline type of Vector Function is PIPE_V, and synchronization dependencies within Vector Function are inserted through the [LocalMemBar](./sync_control/LocalMemBar.md) API.
