# ListTensorDesc<a name="ZH-CN_TOPIC_0000001714160421"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：不支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/aux_data_structures/ListTensorDesc_res.md#id1 -->


## 功能说明<a name="section195171847105215"></a>

ListTensorDesc用来解析符合以下内存排布格式的数据，并在核函数（Kernel）侧根据索引获取储存对应数据的地址及shape信息。

![](../../../figures/ListTensorDesc.png)

## 需要包含的头文件<a name="section12341115212912"></a>

```cpp
#include "kernel_operator_list_tensor_intf.h"
```

## 函数原型<a name="zh-cn_topic_0000001441184464_section620mcpsimp"></a>

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

## 函数说明<a name="section396516531098"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
|---|---|
| T | Tensor中元素的数据类型。 |

**表2**  函数及参数说明

| 函数名称 | 入参说明 | 含义 |
|---|---|---|
| ListTensorDesc | - | 默认构造函数，需配合Init函数使用。 |
| ListTensorDesc | data：待解析数据的首地址<br>length：待解析内存的长度<br>shapeSize：数据指针的个数<br>length和shapeSize仅用于校验，不填写时不进行校验 | ListTensorDesc类的构造函数，用于解析对应的内存排布。 |
| Init | data：待解析数据的首地址<br>length：待解析内存的长度<br>shapeSize：数据指针的个数<br>length和shapeSize仅用于校验，不填写时不进行校验 | 初始化函数，用于解析对应的内存排布。 |
| GetDesc | desc：出参，解析后的Tensor描述信息<br>index：索引值 | 根据index获得功能说明图中对应的TensorDesc信息。<br>使用GetDesc前需要先调用TensorDesc.SetShapeAddr为desc指定用于储存shape信息的地址，调用GetDesc后会将shape信息写入该地址。<br>Atlas推理系列产品AI Core支持该功能<br>Atlas训练系列产品不支持该功能<br>Atlas A2系列产品支持该功能<br>Atlas A3系列产品支持该功能<br>Atlas 200I/500 A2推理产品不支持该功能 |
| GetDataPtr | index：索引值 | 根据index获取储存对应数据的地址。 |
| GetSize | - | 获取ListTensor中包含的数据指针的个数。 |

## 调用示例<a name="section1742652412511"></a>

示例中待解析的srcGm内存排布如下图所示：

![](../../../figures/zh-cn_image_0000001866617737.png)

```cpp
AscendC::ListTensorDesc listTensorDesc(reinterpret_cast<__gm__ void *>(srcGm)); // srcGm为待解析的gm地址
uint32_t size = listTensorDesc.GetSize();                                       // size = 2
auto dataPtr0 = listTensorDesc.GetDataPtr<int32_t>(0);                          // 获取ptr0
auto dataPtr1 = listTensorDesc.GetDataPtr<int32_t>(1);                          // 获取ptr1

uint64_t buf[100] = {0}; // 示例中Tensor的dim为3,此处的100表示预留足够大的空间
AscendC::TensorDesc<int32_t> desc;
desc.SetShapeAddr(buf);          // 为desc指定用于储存shape信息的地址
listTensorDesc.GetDesc(desc, 0); // 获取索引0的shape信息

uint64_t dim = desc.GetDim();   // dim = 3
uint64_t idx = desc.GetIndex(); // idx = 0
uint64_t shape[3] = {0};
for (uint32_t i = 0; i < desc.GetDim(); i++)
{
    shape[i] = desc.GetShape(i); // GetShape(0) = 1, GetShape(1) = 2, GetShape(2) = 3
}
auto ptr = desc.GetDataPtr();
```
