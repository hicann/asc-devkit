# ClearSpr<a name="ZH-CN_TOPIC_0000002537397560"></a>

<!-- md-trans-meta sourceCommit=b817dec23ab1f7e681e8aef4846330c1cf74e443 translatedAt=2026-08-27T13:18:10.005Z -->

## Applicable Products<a name="section364964311912"></a>

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

## Description<a name="section11971757181915"></a>

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

Clears the specified special-purpose register. Currently, [Table SpecialPurposeReg template parameter description](GetSpr.md#table37531617424) is supported.

## Prototype<a name="section113251712205"></a>

```cpp
template <SpecialPurposeReg spr>
__aicore__ inline void ClearSpr()
```

## Parameters<a name="section1116018170208"></a>

**Table 1** Template parameter description

| Parameter | Description |
|--------|------|
| spr | Special register, of the **SpecialPurposeReg** enumeration class. For details about the values, see [SpecialPurposeReg template parameter description](GetSpr.md#table37531617424). |

## Data Types<a name="section3853852112218"></a>

None

## Return Value<a name="section16895132314202"></a>

None

## Constraints<a name="section162221734202016"></a>

This API can only be called outside a VF function, with the namespace **AscendC** and the function marker **__aicore__**. If it needs to be called inside a VF, the namespace is **AscendC::Reg** and the function marker is **__simd_callee__**. For details, see [ClearSpr](../reg_vector_compute/system_variable_access/ClearSpr.md).

## Example<a name="section849174212202"></a>

None
