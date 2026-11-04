# ClearSpr<a name="ZH-CN_TOPIC_0000002537397560"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:25:36.973Z pushedAt=2026-10-26T07:41:44.769Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/special_register_access/ClearSpr_res.md#id1 -->

## Description<a name="section11971757181915"></a>

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

Clears the specified special-purpose register. Currently supported registers are listed in [SpecialPurposeReg Template Parameter Description](GetSpr.md#table37531617424).

## Prototype<a name="section113251712205"></a>

```cpp
template <SpecialPurposeReg spr>
__aicore__ inline void ClearSpr()
```

## Parameters<a name="section1116018170208"></a>

**Table 1** Template parameter description

| Parameter | Description |
|--------|------|
| spr | Special-purpose register, of the **SpecialPurposeReg** enum class. For details about the values, see [Table SpecialPurposeReg template parameter description](GetSpr.md#table37531617424). |

## Data Types<a name="section3853852112218"></a>

None

## Return Value<a name="section16895132314202"></a>

None

## Constraints<a name="section162221734202016"></a>

This API can only be called outside VF functions, with the namespace **AscendC** and the function marker \_\_aicore\_\_. If it needs to be called inside a VF, the namespace is **AscendC::Reg** and the function marker is \_\_simd\_callee\_\_. For details, see [ClearSpr](../reg_vector_compute/system_variable_access/ClearSpr.md).

## Example<a name="section849174212202"></a>

None
