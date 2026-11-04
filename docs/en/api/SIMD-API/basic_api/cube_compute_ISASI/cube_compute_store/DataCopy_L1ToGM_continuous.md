# DataCopy (L1ToGM Continuous Data Transfer)<a name="ZH-CN_TOPIC_0000002563687928"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T12:24:15.203Z pushedAt=2026-10-26T07:41:44.114Z nl=2 -->

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
- Atlas 200I/500 A2 inference product: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/DataCopy_L1ToGM_continuous_res.md#id1 -->

## Description<a name="zh-cn_topic_0000002540198372_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

This API copies a matrix from the L1 Buffer to Global Memory continuously, keeping the format and content unchanged during data transfer.

## Prototype<a name="zh-cn_topic_0000002540198372_section82039854412"></a>

```cpp
template <typename T>
__aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## Parameter Description<a name="zh-cn_topic_0000002540198372_section16128134420472"></a>

**Table 1** Template parameter description

| Parameter Name | Description |
| ---------- | ---------- |
| T | Data type of the source operand or destination operand. For supported data types, see [Data Types](#zh-cn_topic_0000002540198372_section4219135304818). |

**Table 2** Parameter description

| Parameter Name | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, whose type is GlobalTensor, stored in Global Memory, and requires 1-byte alignment. |
| src | Input | Source operand, whose type is LocalTensor, stored in L1 Buffer, and requires 32-byte alignment. |
| count | Input | Number of elements to be copied.<br>**Note: count * sizeof(T) requires 32-byte alignment. If it is not aligned, the copy amount is rounded down to 32-byte alignment.** |

Taking the half data type as an example, the shape of the source operand is 1 \* 128. When count = 128, [Figure 1](#zh-cn_topic_0000002540198372_fig826614454819) continuously copies 128 elements from the source operand to the destination operand.

**Figure 1** Continuous copy diagram<a id="zh-cn_topic_0000002540198372_fig826614454819"></a>  

![](../../../../figures/continuous_data_copy_diagram.png)

## Data Type<a id="zh-cn_topic_0000002540198372_section4219135304818"></a>

The source matrix and the destination matrix support the same data types.

<!-- npu="A3" id10 -->
For the Atlas A3 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id10 -->

<!-- npu="910b" id11 -->
For the Atlas A2 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id11 -->

## Return Value

None

## Constraints<a name="zh-cn_topic_0000002540198372_section2045914466492"></a>

- The source address in the L1 Buffer must be 32-byte aligned, and the destination address in the Global Memory must be 1-byte aligned.
- When calling the continuous copy API, **count** \* sizeof\(T\) must be 32-byte aligned. If it is not aligned, the copy amount is rounded down to 32-byte alignment.
<!-- npu="A3,910b" id14 -->
- For the following product models:
    <!-- npu="A3" id15 -->
    Atlas A3 products
    <!-- end id15 -->
    <!-- npu="910b" id16 -->
    Atlas A2 products
    <!-- end id16 -->
    In cross-card communication operator development scenarios, the **DataCopy** class APIs support cross-card data copy. Only the HCCS physical link is supported, and other paths are not supported. During development, developers need to pay attention to the physical paths involved in inter-card communication. The HCCS physical link can be queried using the npu-smi info -t topo command.
<!-- end id14 -->

## Example<a name="zh-cn_topic_0000002540198372_section088124295117"></a>

For an example of contiguous data copy from L1 to GM, see [L1-to-GM Copy Example](../../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/data_copy_l1togm).
