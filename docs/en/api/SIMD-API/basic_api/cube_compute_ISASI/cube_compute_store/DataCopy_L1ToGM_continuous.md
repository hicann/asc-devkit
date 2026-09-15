# DataCopy (L1ToGM Continuous Data Copy)<a name="ZH-CN_TOPIC_0000002563687928"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T15:39:19.482Z -->

## Applicable Products<a name="zh-cn_topic_0000002540198372_section796754519912"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
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
## Description<a name="zh-cn_topic_0000002540198372_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

This API continuously copies a cube from the L1 Buffer to Global Memory, keeping the format and content unchanged during data transfer.

## Prototype<a name="zh-cn_topic_0000002540198372_section82039854412"></a>

```cpp
template <typename T>
__aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## Parameters<a name="zh-cn_topic_0000002540198372_section16128134420472"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ---------- | ---------- |
| **T** | Data type of the source operand or destination operand. For supported data types, see [Data Types](#zh-cn_topic_0000002540198372_section4219135304818). |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| **dst** | Output | Destination operand, type is **GlobalTensor**, stored in **Global Memory**, and the address requires 1-byte alignment. |
| **src** | Input | Source operand, type is **LocalTensor**, stored in **L1 Buffer**, and the address requires 32-byte alignment. |
| **count** | Input | Number of elements involved in the transfer. **count** \* sizeof(T) requires 32-byte alignment. If not aligned, the transfer amount is rounded down to 32-byte alignment. |

Taking the **half** data type as an example, the shape of the source operand is 1 \* 128. When **count** = 128, [Figure 1](#zh-cn_topic_0000002540198372_fig826614454819) continuously transfers the 128 elements in the source operand to the destination operand.

**Figure 1** Continuous data transfer diagram<a id="zh-cn_topic_0000002540198372_fig826614454819"></a>  

![](../../../../figures/L12GM_Continuous_Transporting.png)

## Data Types<a id="zh-cn_topic_0000002540198372_section4219135304818"></a>

The source cube and the destination cube support the same data types.

<!-- npu="A3" id10 -->
For Atlas A3 training products/Atlas A3 inference products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id10 -->

<!-- npu="910b" id11 -->
For Atlas A2 training products/Atlas A2 inference products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id11 -->

## Return Value

None

## Constraints<a name="zh-cn_topic_0000002540198372_section2045914466492"></a>

- The source address in the L1 Buffer must be 32-byte aligned, and the destination address in Global Memory must be 1-byte aligned.
- When calling the continuous copy API, count \* sizeof\(T\) must be 32-byte aligned. If it is not aligned, the copy amount is rounded down to 32-byte alignment.
<!-- npu="A3,910b" id14 -->
- For the following product models:
    <!-- npu="A3" id15 -->
    Atlas A3 training products/Atlas A3 inference products
    <!-- end id15 -->
    <!-- npu="910b" id16 -->
    Atlas A2 training products/Atlas A2 inference products
    <!-- end id16 -->
    In cross-card communication operator development scenarios, the **DataCopy** class APIs support cross-card data copy only over the HCCS physical link, and do not support other paths. During development, developers need to pay attention to the physical paths involved in inter-card communication. The HCCS physical link can be queried by running the npu-smi info -t topo command.
<!-- end id14 -->

## Example<a name="zh-cn_topic_0000002540198372_section088124295117"></a>

```cpp
// dstLocal is a LocalTensor of the half type, and dstGlobal is a GlobalTensor of the half type.
// Use the copy API with the count parameter to complete continuous copy.
AscendC::DataCopy(dstGlobal, dstLocal, 512);
```
