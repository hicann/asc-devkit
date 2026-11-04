# asc\_vf\_call<a name="ZH-CN_TOPIC_0000002563057755"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T17:16:56.474Z pushedAt=2026-10-26T07:41:44.618Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/vf_call/asc_vf_call_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Used in SIMD programming scenarios to launch a SIMD VF (Vector Function) subtask.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <auto funcPtr, typename... Args>
__aicore__ inline void asc_vf_call(Args &&...args)
```

## Parameters<a name="section0866173114710"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :-- | :------------ | 
| funcPtr | Specifies the SIMD entry kernel function (Kernel). |
| Args | Defines variadic parameters used to pass arguments to the SIMD entry kernel function (Kernel). |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| :-- | :------------ | :-- |
| args | Input | Variadic parameters used to pass arguments to the SIMD entry kernel function (Kernel). |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

-   When asc\_vf\_call starts a SIMD VF subtask, the subtask function cannot be a member function of a class. Use a regular function or a class static function, and the entry function must use the \_\_simd\_vf\_\_ modifier macro.
-   When asc_vf_call starts a SIMD VF subtask, the input parameters support only Ascend C built-in data types (half, int32_t, uint32_t, float, etc.) and pointer, array, and structure types composed of them, and pointer types must point to Unified Buffer (UB) memory.

## Example<a name="section1316724610428"></a>

Use the SIMD VF function to perform addition on UB data.

```cpp
// SIMD function.
template <typename T>
__simd_vf__ inline void AddVF(
    __ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> src0Reg;
    AscendC::Reg::RegTensor<T> src1Reg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(src0Reg, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(src1Reg, src1Addr + i * oneRepeatSize);
        AscendC::Reg::Add(dstReg, src0Reg, src1Reg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}

template <typename T>
__aicore__ inline void Compute()
{
    AscendC::LocalTensor<T> dst = outQueueZ.AllocTensor<T>();
    AscendC::LocalTensor<T> src0 = inQueueX.DeQue<T>();
    AscendC::LocalTensor<T> src1 = inQueueY.DeQue<T>();
    constexpr uint16_t oneRepeatSize = AscendC::GetVecLen() / sizeof(T);
    uint32_t count = 512;
    // Round up to calculate the number of loop iterations.
    uint16_t repeatTimes = AscendC::CeilDivision(count, oneRepeatSize);
    __ubuf__ T* dstAddr = (__ubuf__ T*)dst.GetPhyAddr();
    __ubuf__ T* src0Addr = (__ubuf__ T*)src0.GetPhyAddr();
    __ubuf__ T* src1Addr = (__ubuf__ T*)src1.GetPhyAddr();
    asc_vf_call<AddVF<T>>(dstAddr, src0Addr, src1Addr, count, oneRepeatSize, repeatTimes);
    outQueueZ.EnQue(dst);
    inQueueX.FreeTensor(src0);
    inQueueY.FreeTensor(src1);
}
```
