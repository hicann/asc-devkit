# Squeeze<a name="ZH-CN_TOPIC_0000002009320090"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:04:17.954Z pushedAt=2026-10-26T07:41:44.491Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1  -->
- Ascend 950PR&950DT products: supported
<!-- end id1  -->
<!-- npu="A3" id2  -->
- Atlas A3 products: not supported
<!-- end id2  -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4  -->
- Atlas 200I/500 A2 inference product: not supported
<!-- end id4  -->
<!-- npu="310p" id5  -->
- Atlas inference products AI Core: not supported
<!-- end id5  -->
<!-- npu="310p" id6  -->
- Atlas inference products Vector Core: not supported
<!-- end id6  -->
<!-- npu="910" id7  -->
- Atlas training products: not supported
<!-- end id7  -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/compare_and_select/Squeeze_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_gather_mask_intf.h"`.

Copies the valid elements selected by the mask in the input **srcReg** to **dstReg** in sequence. The valid elements are arranged contiguously from low to high in **dstReg**, and the remaining positions are set to 0, as shown in [Figure 1](#figure1).

**Figure 1**  Squeeze computation diagram<a id="figure1"></a>

![Squeeze computation diagram](../../../../figures/reg_squeeze.png)

In particular, when the template parameter **store** is set to `GatherMaskMode::STORE_REG`, `Squeeze` stores the total number of bytes of the valid elements into the [AR special-purpose register](../../special_register_access/GetSpr.md#table37531617424). In this case, when used together with scenario 3 of [continuous unaligned store](../reg_data_store/StoreUnAlign_continuous.md) (which does not require an explicit offset), `StoreUnAlign` automatically reads the number of valid bytes from the AR register as the address offset. See [Example](#example).

## Function Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, GatherMaskMode store = GatherMaskMode::NO_STORE_REG, typename U>
__simd_callee__ inline void Squeeze(U& dstReg, U& srcReg, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :----- | :--- |
| T | Data type of the target operand and source operand. |
| store | **GatherMaskMode** selects whether to store the total number of bytes of valid elements into the AR register. For the AR register description, see [Table Template parameter description](../../special_register_access/GetSpr.md#table37531617424).<br>• **NO_STORE_REG**: The total number of bytes of valid elements is not stored into the AR register.<br>• **STORE_REG**: The total number of bytes of valid elements is stored into the AR register. |
| U | The `RegTensor` type of the target operand and source operand, for example, `RegTensor<uint32_t>`, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Function parameter description

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dstReg | Output | Target operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| mask | Input | **mask** controls the elements that participate in the computation within each iteration. |

## Return Value

None

## Data Types<a name="zh-cn_topic_0000002568284054_section1962315623118"></a>

The supported data types are: int8\_t, uint8\_t, int16\_t, uint16\_t, half, int32\_t, uint32\_t, float.

## Constraints<a name="section177921451558"></a>

- When the template parameter **store** is set to `GatherMaskMode::STORE_REG`, due to hardware constraints, the `StoreUnAlign` instruction and the `Squeeze` instruction must be used alternately, for example:

  ```cpp
  Squeeze(dstVreg, srcVreg, mask);
  StoreUnAlign(dstAddr, dstVreg, ureg);
  Squeeze(dstVreg, srcVreg, mask);
  StoreUnAlign(dstAddr, dstVreg, ureg);
  ```

- When **store** is set to `GatherMaskMode::NO_STORE_REG`, the AR register is not involved, and `Squeeze` and `StoreUnAlign` are not required to alternate.

## Example<a name="section642mcpsimp"></a>

The following example shows a typical way to use `Squeeze` together with `StoreUnAlign` (the `STORE_REG` mode of `Squeeze`, where `StoreUnAlign` uses the number of valid element bytes in the AR register as the offset):

```cpp
template<typename T>
__simd_vf__ inline void SqueezeVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::UnalignRegForStore ureg;
    AscendC::Reg::MaskReg sqzMask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::H>();
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        AscendC::Reg::LoadAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(srcReg0, srcAddr, oneRepeatSize);
        AscendC::Reg::Squeeze<T, AscendC::Reg::GatherMaskMode::STORE_REG>(srcReg1, srcReg0, sqzMask);
        AscendC::Reg::StoreUnAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(dstAddr, srcReg1, ureg);
     }
     AscendC::Reg::StoreUnAlignPost(dstAddr, ureg);
}
```
