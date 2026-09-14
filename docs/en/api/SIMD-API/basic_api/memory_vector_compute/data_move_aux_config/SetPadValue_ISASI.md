# SetPadValue \(ISASI\)<a name="ZH-CN_TOPIC_0000001786582454"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-09-09T14:08:29.685Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id11 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id11 -->
<!-- npu="A3" id12 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id12 -->
<!-- npu="910b" id13 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id13 -->
<!-- npu="310b" id14 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products AI Core: Not supported
<!-- end id15 -->
<!-- npu="310p" id16 -->
- Atlas inference products Vector Core: Not supported
<!-- end id16 -->
<!-- npu="910" id17 -->
- Atlas training products: Not supported
<!-- end id17 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_data_copy_intf.h"`.

When [DataCopyPad](../data_move/DataCopyPad_GMToUB.md) migrates data from Global Memory to Unified Buffer in a non-aligned manner, you can fill the data as needed. SetPadValue is used to set the value to be filled by DataCopyPad.

The supported data paths are as follows (expressed by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Global Memory -> Unified Buffer
    - GM -> VECIN
    - GM -> VECOUT

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T, TPosition pos = TPosition::MAX>
__aicore__ inline void SetPadValue(T paddingValue)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| T | Input | Data type of the padding value, which is the same as the data type transferred by the DataCopyPad API. |
| pos | Input | Specifies the destination address to which data is transferred from Global Memory during the DataCopyPad API process. The destination address is expressed by a logical position. The default value is TPosition::MAX, which is equivalent to TPosition::VECIN or TPosition::VECOUT.<br>Supported values:<br>&bull; TPosition::VECIN, TPosition::VECOUT, TPosition::MAX, |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| paddingValue | Input | Value filled by the DataCopyPad API. The data type is the same as the data type transferred by the DataCopyPad API. |

## Data Type

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT, the supported data types are: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, complex64.
<!-- end id1 -->

<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id2 -->

<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id3 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section1227835243314"></a>

```cpp
// Scenario 1: Use SetPadValue for custom padding.
// DataCopyExtParams parameters: blockCount=1, blockLen=20*sizeof(half), srcBlkStride=0, srcRepStride=0, reserved=0.
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

For the complete sample, see [DataCopyPad sample scenario 1](https://gitcode.com/cann/asc-devkit/blob/9.1.0/examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_pad_gm2ub_ub2gm).
