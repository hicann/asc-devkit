# BroadCastVecToMM\(ISASI\)<a name="ZH-CN_TOPIC_0000001860143713"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T15:55:36.911Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not Supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not Supported
<!-- end id7 -->

## Description<a name="section618mcpsimp"></a>

Broadcasts vector data to a cube. Every 16 elements in each data block are consecutively copied 16 times. Currently supported data transfer paths: Unified Buffer->L0C Buffer (VECIN/VECCALC/VECOUT->CO1).

**Figure 1** Example<a name="fig1730933122314"></a>  

![](../../../../figures/functional_example.png "Example")

## Function Prototype<a name="section620mcpsimp"></a>

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
| dst | Output | Destination operand, the result cube, of type **LocalTensor**, with the supported physical address being L0C Buffer (**TPosition**: CO1).<br>The start address of **LocalTensor** must be aligned to 256 elements.<br>Supported data types: half, int32_t, float. |
| src | Input | Source operand, the input vector, of type **LocalTensor**, with the supported physical address being Unified Buffer (**TPosition**: VECIN/VECCALC/VECOUT).<br>The supported data type must be consistent with that of **dst**. |
| blockCount | Input | Number of consecutive broadcast data blocks contained in this instruction. Value range: blockCount ∈ [1, 255]. |
| blockLen | Input | Length of each consecutive broadcast data block in this instruction, in units of 16 elements. Value range: blockLen ∈ [1, 255]. |
| srcGap | Input | Source operand, the gap between adjacent consecutive data blocks (the gap between the tail of the preceding data block and the head of the following data block), in units of datablock (32 bytes). |
| dstGap | Input | Destination operand, the gap between adjacent consecutive data blocks (the gap between the tail of the preceding data block and the head of the following data block), in units of 256 elements. |

## Data Types

Atlas Inference Series products AI Core: Supported data types are half, int32_t, and float.

## Constraints<a name="section633mcpsimp"></a>

For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Example<a name="section642mcpsimp"></a>

In this example, the input **bias** has a shape of \[1, 32\], and the output **c** has a shape of \[32, 32\] in the Nz format. The example diagram is as follows:

**Figure 2** Example diagram<a name="fig496292825418"></a>  

![](../../../../figures/call_example_diagram.png "Example diagram")

This example shows only part of the code in the sample. To run it, refer to the [BroadCastVecToMM sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/00_data_movement/broadcast_ub2l0c) for the complete code.

```cpp
// brcLocal is a LocalTensor of type float on TPosition::CO1, and srcLocal is a LocalTensor of type float on TPosition::VECIN.
// blockCount = 1, blockLen = 1. The number of consecutively broadcast data blocks is 1, each data block contains 16 elements, and a total of 256 elements are output.
// srcGap = 0, dstGap = 1. The source operand and the destination operand are contiguous.
AscendC::BroadCastVecToMM(brcLocal, srcLocal, 1, 1, 0, 1)
```
