# GetSpr<a name="ZH-CN_TOPIC_0000002537557502"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:29:10.513Z pushedAt=2026-10-26T07:41:44.777Z nl=2 -->

## Applicable Products<a name="section364964311912"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/special_register_access/GetSpr_res.md#id1 -->

## Description<a name="section11971757181915"></a>

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

Obtains the value of a specified special-purpose register. Currently, [Table SpecialPurposeReg template parameter description](#table37531617424) is supported.

## Function Prototype<a name="section113251712205"></a>

```cpp
template <SpecialPurposeReg spr>
__aicore__ inline int64_t GetSpr()
```

## Parameters<a name="section1116018170208"></a>

**Table 1** Template parameter description

| Parameter | Description |
|--------|------|
| spr | Special-purpose register, of the **SpecialPurposeReg** enum class. For details about the values, see [SpecialPurposeReg template parameter description](#table37531617424). |

**Table 2** SpecialPurposeReg template parameter description<a name="table37531617424"></a>

| Value | Meaning |
|------|------|
| AR | The **AR** register is usually used together with the Reg vector compute APIs [Squeeze](../reg_vector_compute/compare_and_select/Squeeze.md) and [continuous unaligned store](../reg_vector_compute/reg_data_store/StoreUnAlign_continuous.md) in scenario 3 (no explicit offset needs to be passed). The register stores the total number of bytes of valid elements. |

## Data Types<a name="section3853852112218"></a>

The data type returned by the API is int64\_t.

## Return Value<a name="section16895132314202"></a>

Returns the value in the special-purpose register of type int64_t.

## Constraints<a name="section162221734202016"></a>

This API can only be called outside VF functions. The namespace is **AscendC**, and the function specifier is \_\_aicore\_\_.

## Example<a name="section849174212202"></a>

In the following example, [Squeeze](../reg_vector_compute/compare_and_select/Squeeze.md), the **Reg** vector computation API stores the total number of bytes of valid elements into the **AR** register. After the macro function ends, the value of the **AR** register (in bytes) is obtained through **GetSpr**.

```cpp
template <typename T>
__simd_vf__ inline void SqueezeVF(__ubuf__ T* xAddr, __ubuf__ T* yAddr, uint32_t repeatTimes, uint32_t oneRepeatSize)
{
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::M4>();
    AscendC::Reg::RegTensor<T> xReg;
    AscendC::Reg::RegTensor<T> yReg;
    AscendC::Reg::UnalignRegForStore ureg;
    AscendC::Reg::ClearSpr<AscendC::SpecialPurposeReg::AR>();
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        AscendC::Reg::LoadAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(xReg, xAddr, oneRepeatSize);
        AscendC::Reg::Squeeze<T, AscendC::Reg::GatherMaskMode::STORE_REG>(yReg, xReg, mask);
        AscendC::Reg::StoreUnAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(yAddr, yReg, ureg);
    }
    AscendC::Reg::StoreUnAlignPost(yAddr, ureg);
}

__aicore__ inline void Process()
{
    AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
    AscendC::LocalTensor<float> xLocal = ubAllocator.Alloc<float, 256>();
    AscendC::LocalTensor<float> yLocal = ubAllocator.Alloc<float, 64>();

    AscendC::DataCopy(xLocal, xGm, 256);
    AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

    __ubuf__ float* xAddr = reinterpret_cast<__ubuf__ float*>(xLocal.GetPhyAddr());
    __ubuf__ float* yAddr = reinterpret_cast<__ubuf__ float*>(yLocal.GetPhyAddr());
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(float);
    uint16_t repeatTimes = DivCeil(256, oneRepeatSize);

    asc_vf_call<SqueezeVF<float>>(xAddr, yAddr, repeatTimes, oneRepeatSize);
    AscendC::SetFlag<AscendC::HardEvent::V_S>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::V_S>(EVENT_ID0);
    int64_t arNum = AscendC::GetSpr<AscendC::SpecialPurposeReg::AR>();
    // Print via printf.
    AscendC::printf("The value of arNum is :%lld\n", arNum);

    AscendC::SetFlag<AscendC::HardEvent::S_MTE3>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::S_MTE3>(EVENT_ID0);
    AscendC::DataCopy(yGm, yLocal, 64);
}
```

The result is as follows:

```cpp
Input 256 float data (xLocal): [1.0 1.0 1.0 ... ] // The data is all 1.0.
Input 64 float data (yLocal): [1.0 1.0 1.0 ... ] // The data is all 1.0.
The value of arNum is: 256
```
