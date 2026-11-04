# SetPadValue\(ISASI\)<a name="ZH-CN_TOPIC_0000001786582454"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:56:09.876Z pushedAt=2026-10-28T03:14:39.509Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id11 -->
- Ascend 950PR&950DT products: supported
<!-- end id11 -->
<!-- npu="A3" id12 -->
- Atlas A3 products: supported
<!-- end id12 -->
<!-- npu="910b" id13 -->
- Atlas A2 products: supported
<!-- end id13 -->
<!-- npu="310b" id14 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products AI Core: not supported
<!-- end id15 -->
<!-- npu="310p" id16 -->
- Atlas inference products Vector Core: not supported
<!-- end id16 -->
<!-- npu="910" id17 -->
- Atlas training products: not supported
<!-- end id17 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_move_aux_config/SetPadValue_ISASI_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

[DataCopyPad (GMToUB unaligned data migration)](../data_move/DataCopyPad_GMToUB.md) migrates data from Global Memory to Unified Buffer (UB) in an unaligned manner, and you can fill the data as needed. SetPadValue is used to set the value to be filled by DataCopyPad.

The specific supported data paths are (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Global Memory -> UB
    - GM -> VECIN
    - GM -> VECOUT

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T, TPosition pos = TPosition::MAX>
__aicore__ inline void SetPadValue(T paddingValue)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| T | Input | Data type of the fill value, which is the same as the data type migrated by the DataCopyPad API. |
| pos | Input | Destination address to which data is migrated from Global Memory during the migration performed by the DataCopyPad API. The destination address is expressed by a logical position. The default value is TPosition::MAX, which is equivalent to TPosition::VECIN or TPosition::VECOUT.<br>Supported values:<br>&bull; TPosition::VECIN, TPosition::VECOUT, TPosition::MAX |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| paddingValue | Input | Value filled by the DataCopyPad API, whose data type is the same as the data type migrated by the DataCopyPad API. |

## Data Type

<!-- npu="950" id1 -->
- For the Ascend 950PR&950DT products, the supported data types are: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, complex64.
<!-- end id1 -->

<!-- npu="A3" id2 -->
- For the Atlas A3 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id2 -->

<!-- npu="910b" id3 -->
- For the Atlas A2 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id3 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Examples<a name="section1227835243314"></a>

```cpp
// Scenario 1: Use SetPadValue for custom padding.
// DataCopyExtParams parameters: blockCount=1, blockLen=20*sizeof(half), srcStride=0, dstStride=0, rsv=0.
// DataCopyPadExtParams parameters: isPad=false, leftPadding=0, rightPadding=12.
// SetPadValue sets the padding value to 1, used together with isPad=false.
AscendC::DataCopyExtParams copyParams{1, srcCols * sizeof(T), 0, 0, 0};
AscendC::DataCopyPadExtParams<T> padParams;
padParams.isPad = false;
padParams.leftPadding = 0;
padParams.rightPadding = dstCols - srcCols;

AscendC::SetPadValue((T)1);
AscendC::DataCopyPad(srcLocal, srcGlobal, copyParams, padParams);
```

For the complete example, see [DataCopyPad example scenario 1](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_pad_gm2ub_ub2gm).
