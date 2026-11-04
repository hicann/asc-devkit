# BroadCastVecToMM\(ISASI\)<a name="ZH-CN_TOPIC_0000001860143713"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T11:23:15.218Z pushedAt=2026-10-26T07:41:44.056Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 Inference Product: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/BroadCastVecToMM_ISASI_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Broadcasts vector data into a matrix, where every 16 elements in each data block are consecutively copied 16 times. Currently supported data transfer path: UB-\>L0C Buffer (VECIN/VECCALC/VECOUT-\>CO1).

**Figure 1** Example<a name="fig1730933122314"></a>  

![](../../../../figures/function_example.png "Example")

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T, typename U>
__aicore__ inline void BroadCastVecToMM(const LocalTensor<T> &dst, const LocalTensor<U> &src, const int32_t blockCount, const uint8_t blockLen, const uint8_t srcGap, const uint8_t dstGap)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description<a name="table4835205712588"></a>

| Parameter | Description |
| ---------- | ---------- |
| T | Data type of **dst**. |
| U | Data type of **src**. |

**Table 2** Parameter description<a name="table284254116544"></a>

| Parameter | Type | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, the result matrix, of type **LocalTensor**, with the supported physical address being L0C Buffer (**TPosition**: CO1).<br>The start address of **LocalTensor** must be aligned to 256 elements.<br>Supported data types: **half**, **int32_t**, **float**. |
| src | Input | Source operand, the input vector, of type **LocalTensor**, with the supported physical address being UB (**TPosition**: VECIN/VECCALC/VECOUT).<br>The supported data type must be the same as that of **dst**. |
| blockCount | Input | Specifies the number of consecutive broadcast data blocks contained in this instruction. Value range: blockCount ∈ [1, 255]. |
| blockLen | Input | Specifies the length of each consecutive broadcast data block in this instruction. Unit: 16 elements. Value range: blockLen ∈ [1, 255]. |
| srcGap | Input | Source operand, the gap between adjacent contiguous data blocks (the gap between the end of the preceding data block and the start of the following data block). Unit: datablock (32 bytes). |
| dstGap | Input | Destination operand, the gap between adjacent contiguous data blocks (the gap between the end of the preceding data block and the start of the following data block). Unit: 256 elements. |

## Data Types

Atlas inference products AI Core: the supported data types are half, int32_t, and float.

## Constraints<a name="section633mcpsimp"></a>

For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Example<a name="section642mcpsimp"></a>

In this example, the input bias has a shape of \[1, 32\], and the output c has a shape of \[32, 32\], in Nz format. The example diagram is as follows:

**Figure 2** Call example diagram<a name="fig496292825418"></a>  

![](../../../../figures/call_example.png "Example diagram")

This example shows only part of the code. To run it, see [BroadCastVecToMM example](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/broadcast_ub2l0c) for the complete code.

```cpp
// brcLocal is a float LocalTensor at TPosition::CO1, and srcLocal is a float LocalTensor at TPosition::VECIN.
// blockCount = 1, blockLen = 1. The number of consecutive broadcast data blocks is 1, each data block contains 16 elements, and a total of 256 elements are output.
// srcGap = 0, dstGap = 1. The source operand and the destination operand are contiguous.
AscendC::BroadCastVecToMM(brcLocal, srcLocal, 1, 1, 0, 1);
```
