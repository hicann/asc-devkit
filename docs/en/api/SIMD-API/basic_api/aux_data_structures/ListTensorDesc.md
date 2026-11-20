# ListTensorDesc<a name="ZH-CN_TOPIC_0000001714160421"></a>

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-11-02T02:41:33.645Z pushedAt=2026-11-18T01:25:06.654Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
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
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/aux_data_structures/ListTensorDesc_res.md#id1 -->


## Description<a name="section195171847105215"></a>

**ListTensorDesc** is used to parse data that conforms to the following memory layout format, and to obtain the address and shape information of the corresponding stored data by index on the kernel side.

![](../../../figures/ListTensorDesc.png)

## Header File to Include<a name="section12341115212912"></a>

```cpp
#include "kernel_operator_list_tensor_intf.h"
```

## Prototype<a name="zh-cn_topic_0000001441184464_section620mcpsimp"></a>

```cpp
class ListTensorDesc {
    ListTensorDesc();
    ListTensorDesc(__gm__ void* data, uint32_t length = 0xffffffff, uint32_t shapeSize = 0xffffffff);
    void Init(__gm__ void* data, uint32_t length = 0xffffffff, uint32_t shapeSize = 0xffffffff);
    template<class T> void GetDesc(TensorDesc<T>& desc, uint32_t index);
    template<class T> T* GetDataPtr(uint32_t index);
    uint32_t GetSize();
}
```

## Function Description<a name="section396516531098"></a>

**Table 1** Template parameter description

| Parameter | Description |
|---|---|
| T | Data type of the elements in the tensor. |

**Table 2**  Function and parameter description

| Function Name | Input Parameter | Meaning |
|---|---|---|
| ListTensorDesc | - | Default constructor, which must be used together with the Init function. |
| ListTensorDesc | data: start address of the data to be parsed<br>length: length of the memory to be parsed<br>shapeSize: number of data pointers<br>length and shapeSize are used only for validation. If they are not specified, no validation is performed. | Constructor of the ListTensorDesc class, used to parse the corresponding memory layout. |
| Init | data: start address of the data to be parsed<br>length: length of the memory to be parsed<br>shapeSize: number of data pointers<br>length and shapeSize are used only for validation. If they are not specified, no validation is performed. | Initialization function, used to parse the corresponding memory layout. |
| GetDesc | desc: output parameter, parsed tensor description information<br>index: index value | Obtains the TensorDesc information corresponding to the index in the function description diagram.<br>Before calling GetDesc, call TensorDesc.SetShapeAddr to specify the address for storing the shape information for desc. After GetDesc is called, the shape information is written to this address. |
| GetDataPtr | index: index value | Obtains the address of the corresponding stored data by index. |
| GetSize | - | Obtains the number of data pointers contained in ListTensor. |

## Example<a name="section1742652412511"></a>

The memory layout of srcGm to be parsed in this example is shown in the following figure:

![](../../../figures/image_0000001866617737.png)

```cpp
AscendC::ListTensorDesc listTensorDesc(reinterpret_cast<__gm__ void *>(srcGm)); // srcGm is the GM address to be parsed.
uint32_t size = listTensorDesc.GetSize();                                       // size = 2
auto dataPtr0 = listTensorDesc.GetDataPtr<int32_t>(0);                          // Obtain ptr0.
auto dataPtr1 = listTensorDesc.GetDataPtr<int32_t>(1);                          // Obtain ptr1.

uint64_t buf[100] = {0}; // In this example, the tensor has 3 dims. 100 here reserves enough space.
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
