# Gemm (Deprecated)

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T13:18:52.693Z pushedAt=2026-10-26T07:41:44.181Z nl=2 -->

## Applicable Products

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
- Atlas 200I/500 A2 inference product: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/Gemm_deprecated_res.md#id1 -->

## Description

**This API is deprecated and will be removed in a later version. Do not use it.**

Performs matrix multiplication on the two given input tensors according to the input tiling rules and outputs the result to the result tensor. Multiplies the two input matrices A and B together to obtain an output matrix C.

## Prototype

- Functional API:

    ```cpp
    template <typename T, typename U, typename S>
    __aicore__ inline void Gemm(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<S>& src1, const uint32_t m, const uint32_t k, const uint32_t n, GemmTiling tiling, bool partialsum = true, int32_t initValue = 0)
    ```

- Tiling scheme calculation API:

    ```cpp
    template <typename T>
    __aicore__ inline GemmTiling GetGemmTiling(uint32_t m, uint32_t k, uint32_t n)
    ```

## Parameters

**Table 1** API parameter description

| Parameter | Type | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br><!-- npu="910" id8 -->Atlas training products support the following TPosition values: CO1, CO2<!-- end id8 --><br><!-- npu="310p" id9 -->Atlas inference products AI Core support the following TPosition values: CO1, CO2<!-- end id9 --> |
| src0 | Input | Source operand, with TPosition set to A1. |
| src1 | Input | Source operand, with TPosition set to B1. |
| m | Input | Valid height of the left matrix Src0Local, in the range [1, 4096].<br>Note: m may not be a multiple of 16. |
| k | Input | Valid width of the left matrix Src0Local and valid height of the right matrix Src1Local.<br>&bull;When the input tensor Src0Local has a data type of float, the range is [1, 8192].<br>&bull;When the input tensor Src0Local has a data type of half, the range is [1, 16384].<br>&bull;When the input tensor Src0Local has a data type of int8_t, the range is [1, 32768].<br><br>Note: k may not be a multiple of 16. |
| n | Input | Valid width of the right matrix Src1Local, in the range [1, 4096].<br>Note: n may not be a multiple of 16. |
| tiling | Input | Tiling rule, of type GemmTiling. The struct is defined as follows:<br><br>struct GemmTiling {<br>    const uint32_t blockSize = 16;<br>    LoopMode loopMode = LoopMode::MODE_NM;<br>    uint32_t mNum = 0;<br>    uint32_t nNum = 0;<br>    uint32_t kNum = 0;<br>    uint32_t roundM = 0;<br>    uint32_t roundN = 0;<br>    uint32_t roundK = 0;<br>    uint32_t c0Size = 32;<br>    uint32_t dtypeSize = 1;<br>    uint32_t mBlockNum = 0;<br>    uint32_t nBlockNum = 0;<br>    uint32_t kBlockNum = 0;<br>    uint32_t mIterNum = 0;<br>    uint32_t nIterNum = 0;<br>    uint32_t kIterNum = 0;<br>    uint32_t mTileBlock = 0;<br>    uint32_t nTileBlock = 0;<br>    uint32_t kTileBlock = 0;<br>    uint32_t kTailBlock = 0;<br>    uint32_t mTailBlock = 0;<br>    uint32_t nTailBlock = 0;<br>    bool kHasTail = false;<br>    bool mHasTail = false;<br>    bool nHasTail = false;<br>    bool kHasTailEle = false;<br>    uint32_t kTailEle = 0;<br>};<br><br>For parameter description, see [Table 3](#table3). |
| partialsum | Input | When the TPosition of the dst parameter is CO2, this parameter controls whether the computation result is moved out.<br>&bull;Value 0: Move out the computation result.<br>&bull;Value 1: Do not move out the computation result, so that subsequent computation can be performed. |
| initValue | Input | Indicates whether dst needs to be initialized.<br>&bull;Value 0: dst needs to be initialized. The initial matrix of dst stores the previous result, and the new computation result is accumulated onto the previous Gemm computation result.<br>&bull;Value 1: dst does not need to be initialized. The data in the initial matrix of dst is meaningless, and the computation result directly overwrites the data in dst. |

**Table 2** Data type combinations of src0, src1, and dst

| src0.dtype | src1.dtype | dst.dtype |
| --- | --- | --- |
| int8_t | int8_t | int32_t |
| half | half | float |
| half | half | half |

**Table 3** Parameter description of the GemmTiling struct<a id="table3"></a>

| Parameter | Type | Description |
| --- | --- | --- |
| blockSize | uint32_t | Fixed value, always 16, indicating the number of elements stored in one dimension. |
| loopMode | LoopMode | Traversal mode. The struct is defined as follows:<br><br><br>enum class LoopMode {<br>    MODE_NM = 0,<br>    MODE_MN = 1,<br>    MODE_KM = 2,<br>    MODE_KN = 3<br>};<br> |
| mNum | uint32_t | M-axis equivalent data length parameter value, in the range [1, 4096]. |
| nNum | uint32_t | N-axis equivalent data length parameter value, in the range [1, 4096]. |
| kNum | uint32_t | K-axis equivalent data length parameter value.<br>&bull;When the input tensor Src0Local has a data type of float, the range is [1, 8192].<br>&bull;When the input tensor Src0Local has a data type of half, the range is [1, 16384].<br>&bull;When the input tensor Src0Local has a data type of int8_t, the range is [1, 32768]. |
| roundM | uint32_t | M-axis equivalent data length parameter value rounded up to a multiple of blockSize, in the range [1, 4096]. |
| roundN | uint32_t | N-axis equivalent data length parameter value rounded up to a multiple of blockSize, in the range [1, 4096]. |
| roundK | uint32_t | K-axis equivalent data length parameter value rounded up to a multiple of c0Size.<br>&bull;When the input tensor Src0Local has a data type of float, the range is [1, 8192].<br>&bull;When the input tensor Src0Local has a data type of half, the range is [1, 16384].<br>&bull;When the input tensor Src0Local has a data type of int8_t, the range is [1, 32768]. |
| c0Size | uint32_t | Byte length of one block, in the range [16 or 32]. |
| dtypeSize | uint32_t | Byte length of the input data type, in the range [1, 2]. |
| mBlockNum | uint32_t | Number of M-axis blocks, mBlockNum = mNum / blockSize. |
| nBlockNum | uint32_t | Number of N-axis blocks, nBlockNum = nNum / blockSize. |
| kBlockNum | uint32_t | Number of K-axis blocks, kBlockNum = kNum / blockSize. |
| mIterNum | uint32_t | Number of M-axis dimensions to traverse, in the range [1, 4096]. |
| nIterNum | uint32_t | Number of N-axis dimensions to traverse, in the range [1, 4096]. |
| kIterNum | uint32_t | Number of K-axis dimensions to traverse, in the range [1, 4096]. |
| mTileBlock | uint32_t | Number of M-axis split blocks, in the range [1, 4096]. |
| nTileBlock | uint32_t | Number of N-axis split blocks, in the range [1, 4096]. |
| kTileBlock | uint32_t | Number of K-axis split blocks, in the range [1, 4096]. |
| kTailBlock | uint32_t | Number of K-axis tail blocks, in the range [1, 4096]. |
| mTailBlock | uint32_t | Number of M-axis tail blocks, in the range [1, 4096]. |
| nTailBlock | uint32_t | Number of N-axis tail blocks, in the range [1, 4096]. |
| kHasTail | bool | Whether a tail block exists on the K axis. |
| mHasTail | bool | Whether a tail block exists on the M axis. |
| nHasTail | bool | Whether a tail block exists on the N axis. |
| kHasTailEle | bool | Whether a tail block element exists. |
| kTailEle | uint32_t | K-axis tail block element, in the range [1, 4096]. |

## Data Types

**Table 4** Data type combinations of src0, src1, and dst

| src0.dtype | src1.dtype | dst.dtype |
| --- | --- | --- |
| int8_t | int8_t | int32_t |
| half | half | float |
| half | half | half |

## Return Value

None

## Constraints

- The parameters **m**, **k**, and **n** may not be aligned to 16. However, due to hardware reasons, the shapes of the operands **dst**, **Src0Local**, and **Src1Local** must meet the alignment requirements, that is, the **m** and **n** dimensions must be rounded up to a multiple of 16, and the **k** dimension must be rounded up to a multiple of 16 or 32 depending on the data type of the operand.
- For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Example

This API is deprecated. Use the **Mmad** API instead.
