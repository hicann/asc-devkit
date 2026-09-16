# Overview
<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T22:53:55.083Z -->

The Reg vector calculation API is developed for the Reg vector calculation architecture. With this API, users can directly operate the registers involved in vector calculation on the chip, achieving greater flexibility and better performance. The Reg vector calculation API is functionally similar to the Memory vector calculation API. However, unlike the Memory vector calculation API, whose input and output data must be **LocalTensor**, the input or output data of the Reg vector calculation API are all Reg vector calculation registers. For calculation APIs, the function is to obtain data from the given registers, perform the calculation, and store the result in the given registers. For data movement APIs, the function is to implement data movement between the UB (Unified Buffer) and the registers. It can be seen that, compared with the Memory vector calculation API, the Reg vector calculation API hands over the data movement and Reg calculation processes to users for autonomous control, thereby achieving greater development freedom.

## How to Use the Reg Vector Calculation API
The register-based programming model loads data from a LocalTensor into registers through Reg move instructions (Reg data move-in), performs complex mathematical computation (Compute), and then moves the data out to the LocalTensor through Reg move instructions (Reg data move-out). All computation logic is completed in registers, which reduces data movement between LocalTensors and greatly improves overall performance. The specific process is as follows:

**Figure 1** Reg vector calculation  
![](../../../figures/reg_vector_calc_overview.png "Reg vector calculation")

Taking the [Reg vector calculation programming example](#reg-vector-calculation-programming-example) as an example, the complete VectorFunction computation process consists of the following parts:
- Write and call VectorFunctions (vector functions). These functions are marked with `__simd_vf__` and are called through [asc\_vf\_call](./vf_call/asc_vf_call.md);
- Define the vector data register [RegTensor](./register_data_types/RegTensor.md) and the mask register [MaskReg](./register_data_types/MaskReg.md);
- Write a for loop mapped to a hardware loop ([HardwareLoop](./key_features/key_feature_description.md#hardware-loop)); the [UpdateMask](./register_data_types/MaskReg.md#mask-setting-method) function is used to update the mask involved in the computation, and each loop consumes one VL-length of elements.
- Call the Reg data move-in API for continuous aligned move-in ([LoadAlign](./reg_data_load/LoadAlign_continuous.md)) within the loop to move in a single VL-length of data from the UB;
- Call the Reg computation API Add within the loop to complete a single Repeat computation;
- Call the Reg data move-out API for continuous aligned move-out ([StoreAlign](./reg_data_store/StoreAlign_continuous.md)) within the loop to move the computed data out to the UB;

## Reg Vector Calculation Programming Example

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
    // Move data from UB address dstAddr to GM.
    ...
}
```

>[!NOTE] Note
>- Data movement between GM and UB must be completed through [GM and UB Data Movement](../memory_vector_compute/data_move/data_movement.md).
>- The pipeline type of Vector Function is PIPE_V, and synchronization dependencies inside Vector Function are inserted through the [LocalMemBar](./sync_control/LocalMemBar.md) API.
