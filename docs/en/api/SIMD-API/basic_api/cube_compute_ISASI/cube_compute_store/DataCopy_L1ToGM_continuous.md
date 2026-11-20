# DataCopy (L1ToGM Continuous Data Transfer)<a name="ZH-CN_TOPIC_0000002563687928"></a>

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-02T03:46:56.283Z pushedAt=2026-11-18T01:25:06.734Z nl=2 -->

## Applicable Products<a name="zh-cn_topic_0000002540198372_section796754519912"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: not supported
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
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90 processor family products: Supported
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030 processor family products: Supported
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/DataCopy_L1ToGM_continuous_res.md#id1 -->

## Description<a name="zh-cn_topic_0000002540198372_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

This API can continuously move the matrix from the L1 Buffer to Global Memory. The format and content remain unchanged during data movement.

## Prototype<a name="zh-cn_topic_0000002540198372_section82039854412"></a>

```cpp
template <typename T>
__aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## Parameter Description<a name="zh-cn_topic_0000002540198372_section16128134420472"></a>

**Table 1** Template parameters

| Parameter | Description |
| ---------- | ---------- |
| T | Data type of the source operand or destination operand. For supported data types, see [Data Types](#zh-cn_topic_0000002540198372_section4219135304818). |

**Table 2** Parameters

| Parameter | Input/Output | Meaning |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of type **GlobalTensor**, stored in Global Memory, with 1-byte alignment required for the address. |
| src | Input | Source operand, of type **LocalTensor**, stored in L1 Buffer, with 32-byte alignment required for the address. |
| count | Input | Number of elements to be copied.<br>**Note: count * sizeof(T) must be 32-byte aligned. If it is not aligned, the copy amount is rounded down to 32-byte alignment.** |

Taking the half data type as an example, the shape of the source operand is 1 \* 128. When count = 128, [Figure 1](#zh-cn_topic_0000002540198372_fig826614454819) continuously copies 128 elements from the source operand to the destination operand.

**Figure 1** Continuous copy diagram<a id="zh-cn_topic_0000002540198372_fig826614454819"></a>  

![](../../../../figures/continuous_data_copy_diagram.png)

## Data Type<a id="zh-cn_topic_0000002540198372_section4219135304818"></a>

The source matrix and destination matrix support the same data types.

<!-- npu="A3" id10 -->
For Atlas A3 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id10 -->

<!-- npu="910b" id11 -->
For Atlas A2 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id11 -->

<!-- npu="x90" id12 -->
For Kirin X90 processor family products, supported data types: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id12 -->

<!-- npu="9030" id13 -->
For Kirin 9030 processor family products, supported data types: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id13 -->

## Return Value

None

## Constraints<a name="zh-cn_topic_0000002540198372_section2045914466492"></a>

- The source address in the L1 Buffer must be 32-byte aligned, and the destination address in Global Memory must be 1-byte aligned.
- When calling the continuous copy API, **count** \* sizeof\(T\) must be 32-byte aligned. If it is not aligned, the copy amount is rounded down to 32-byte alignment.
<!-- npu="A3,910b" id14 -->
- For the following product models:
    <!-- npu="A3" id15 -->
    Atlas A3 products
    <!-- end id15 -->
    <!-- npu="910b" id16 -->
    Atlas A2 products
    <!-- end id16 -->
    In the cross-card communication operator development scenario, the **DataCopy** class APIs support cross-card data copy only over the HCCS physical link, and do not support other paths. During development, pay attention to the physical path involved in inter-card communication. You can run the npu-smi info -t topo command to query the HCCS physical link.
<!-- end id14 -->

## Example<a name="zh-cn_topic_0000002540198372_section088124295117"></a>

For an example of contiguous data copy from L1 to GM, see [L1-to-GM Copy Example](../../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/data_copy_l1togm).
