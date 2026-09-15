# GetDataPtr<a name="ZH-CN_TOPIC_0000002306338962"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T11:50:21.556Z -->

## Applicable Products<a name="section73648168211"></a>

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
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->


## Description<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section36583473819"></a>

Obtains the address where the **Tensor** data is stored.

## Prototype<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section13230182415108"></a>

```cpp
T* GetDataPtr()
```

## Parameters<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section75395119104"></a>

None

## Return Value<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section25791320141317"></a>

Returns the address where the Tensor data is stored. The data type is **T**.

## Constraints<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section320753512363"></a>

The memory layout of srcGm to be parsed in the example is shown in the following figure:

![](../../../../figures/zh-cn_image_0000002340320301.png)

```cpp
AscendC::ListTensorDesc listTensorDesc(reinterpret_cast<__gm__ void *>(srcGm)); // srcGm is the GM address to be parsed.
uint32_t size = listTensorDesc.GetSize();                                       // size = 2
auto dataPtr0 = listTensorDesc.GetDataPtr<int32_t>(0);                          // Obtain ptr0.
auto dataPtr1 = listTensorDesc.GetDataPtr<int32_t>(1);                          // Obtain ptr1.

uint64_t buf[100] = {0}; // In the example, the dim of the Tensor is 3, and 100 here indicates reserving a sufficiently large space.
AscendC::TensorDesc<int32_t> desc;
desc.SetShapeAddr(buf);          // Specify the address for desc to store shape information.
listTensorDesc.GetDesc(desc, 0); // Obtain the shape information at index 0.

uint64_t dim = desc.GetDim();   // dim = 3
uint64_t idx = desc.GetIndex(); // idx = 0
uint64_t shape[3] = {0};
for (uint32_t i = 0; i < desc.GetDim(); i++)
{
    shape[i] = desc.GetShape(i); // GetShape(0) = 1, GetShape(1) = 2, GetShape(2) = 3
}
auto ptr = desc.GetDataPtr();
```
