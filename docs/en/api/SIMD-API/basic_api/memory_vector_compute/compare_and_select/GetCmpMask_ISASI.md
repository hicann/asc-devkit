# GetCmpMask\(ISASI\)<a name="ZH-CN_TOPIC_0000001836766209"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:54:55.666Z pushedAt=2026-10-28T03:14:39.440Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/compare_and_select/GetCmpMask_ISASI_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_cmpsel_intf.h"`.

This API is used to obtain the comparison result of the [Compare (store result in register)](Compare_store_to_register.md) instruction.

The [Compare (store result in register)](Compare_store_to_register.md) instruction writes the comparison result to the CmpMask register. You can use the GetCmpMask API to obtain the value of the CmpMask register and thus get the Compare result.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template<typename T>
__aicore__ inline void GetCmpMask(const LocalTensor<T>& dst)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Comparison result of the [Compare (store result in register)](Compare_store_to_register.md) instruction.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 16-byte aligned. |

## Data Types

The supported data types are b8, b16, and b32.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

The size of dst must not be less than 128 bytes.

## Example<a name="section837496171220"></a>

The result of the [Compare (store result in register)](Compare_store_to_register.md) instruction is stored using the uint8\_t data type, so dstLocal uses the uint8\_t type. For a complete example, see [Compare examples](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/compare).

```cpp
AscendC::LocalTensor<float> src0Local;
AscendC::LocalTensor<float> src1Local;
AscendC::LocalTensor<uint8_t> dstLocal;
uint64_t mask = 256 / sizeof(float); // 256 is the number of bytes processed per iteration, resulting in 64.
AscendC::BinaryRepeatParams repeatParams = { 1, 1, 1, 8, 8, 8 };
AscendC::Compare(src0Local, src1Local, AscendC::CMPMODE::LT, mask, repeatParams);
AscendC::GetCmpMask(dstLocal); // mask is 0x40. When the comparison data type is float, only the 7th float in each 32B iteration participates in the comparison.
```

```plain
Output example:
src0Local:   [1, 2, 3, 4, 5, 6, 7, 8, 9, ...256]
src1Local:   [2, 3, 4, 5, 6, 7, 8, 9, ...257]
Numbers participating in the comparison after masking
src0Local:   [1, 8, 16, ...256]
src1Local:   [2, 10, 18, ...257]
GetCmpMask result: [256, 256, 256, 256, 256, 256, 256, 256]
```
