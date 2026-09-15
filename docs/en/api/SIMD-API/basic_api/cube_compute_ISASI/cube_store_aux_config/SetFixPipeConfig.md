# SetFixPipeConfig<a name="ZH-CN_TOPIC_0000002538231208"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T15:09:13.606Z -->

## Applicable Products<a name="zh-cn_topic_0000002547729135_section18204144912492"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
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
## Description<a name="zh-cn_topic_0000002547729135_section618mcpsimp"></a>

During in-line quantization in **DataCopy** data movement ([L0C Buffer->GM](../cube_compute_store/DataCopy_L0CToGM.md), [L0C Buffer->L1 Buffer](../cube_compute_store/DataCopy_L0CToL1.md)), call this API to set the quantization parameters.

## Prototype<a name="zh-cn_topic_0000002547729135_section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void SetFixPipeConfig(const LocalTensor<T>& reluPre, const LocalTensor<T>& quantPre, bool isUnitFlag = false)

template <typename T, bool setRelu = false>
__aicore__ inline void SetFixPipeConfig(const LocalTensor<T>& preData, bool isUnitFlag = false)
```

## Parameters<a name="zh-cn_topic_0000002547729135_section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ---------- | ---------- |
| T | Data type of the operand. |
| setRelu | For the case of setting a single tensor, **setRelu** currently supports only **false**, that is, only setting the in-band **quantPre** coefficient is supported. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| reluPre | Input | Source operand, the tensor involved in the computation during the ReLU operation. Its type is **LocalTensor**, and the supported physical memory is Fixpipe Buffer (**TPosition** is **C2PIPE2GM**). It is reserved for future function extension, so pass an empty **LocalTensor**. |
| quantPre | Input | Source operand, the quant tensor involved in the computation during the quantization operation. Its type is **LocalTensor**, and the supported physical memory is Fixpipe Buffer (**TPosition** is **C2PIPE2GM**). |
| isUnitFlag | Input | unitFlag clearing flag. Its type is **bool**. If set to **true**, the unit flags of all memory blocks in the L0C Buffer are set to 0, aiming to eliminate the redundant effects of unitFlag produced by previous operations. |
| preData | Input | Supports setting a single tensor, and controls whether it is a ReLU tensor or a Quant tensor through a switch. The supported physical memory is Fixpipe Buffer (**TPosition** is **C2PIPE2GM**). Currently, only passing a quant tensor is supported. For the scenario of separately setting the unitFlag clearing flag **isUnitFlag**, the supported physical address is L0C Buffer (**TPosition** is set to **CO1**). |

## Data Types

The supported data type is **uint64_t**.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="zh-cn_topic_0000002547729135_section633mcpsimp"></a>

**quantPre** and **reluPre** must be tensors on the **Fixpipe Buffer**.

## Example<a name="section642mcpsimp"></a>

For a complete example, see [data_copy_l0c2gm](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_l0c2gm).

```cpp
__aicore__ inline void SetFPC(const LocalTensor <int32_t>& reluPreTensor, const LocalTensor <int32_t>& quantPreTensor)
{
 
    AscendC::LocalTensor<uint64_t> workA1 = inQueueDeqA1.AllocTensor<uint64_t>();
    uint16_t deqSize = 128; // Size of the deq tensor.
    AscendC::DataCopy(workA1, deqGlobal, deqSize); // deqGlobal is the GM address of the quantization coefficients.
    AscendC::LocalTensor<uint64_t> deqFB = inQueueDeqFB.AllocTensor<uint64_t>(); // Address of the deq tensor on Fix.
    uint16_t fbufBurstLen = deqSize / 128;  // l1->fix, burst_len unit is 128Bytes
    AscendC::DataCopyParams dataCopyParams(1, fbufBurstLen, 0, 0);
    AscendC::DataCopy(deqFB, workA1, dataCopyParams); // Move into C2PIPE2GM through DataCopy.
    AscendC::SetFixPipeConfig(deqFB); // Set the quantization tensor.
    AscendC::PipeBarrier<PIPE_FIX>();
}
```
