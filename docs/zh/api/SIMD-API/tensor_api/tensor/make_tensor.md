# make_tensor

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`tensor_api/tensor.h`。

make_tensor负责将内存指针（由[make_mem_ptr](make_mem_ptr.md)生成）与布局对象（由[make_frame_layout](../layout/make_frame_layout.md)/[make_layout](../layout/make_layout.md)生成）绑定，生成带有完整类型信息的Tensor对象。

make_tensor会根据内存指针的物理位置标记，自动推导返回global_tensor或local_tensor，当指针物理位置位于Global Memory（外部存储）时，返回global_tensor；当指针位置位于AI Core上的Local Memory（内部存储）时，返回local_tensor。

生成的Tensor对象可直接传入copy（数据搬运）和mmad（矩阵计算）等接口，编译器会根据其Engine中的物理位置标记和layout中的布局模式，自动选择底层硬件指令。

## 函数原型

```cpp
template <typename Iterator, typename... Args>
__aicore__ inline constexpr auto make_tensor(const Iterator& iter, const Args&... args)
```

## 参数说明

**表1**  参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| iter | 输入 | 迭代器，要求传入view_engine对象，用于创建Tensor。返回类型由该迭代器绑定的内存位置决定。 |
| args... | 输入 | 可变参数，当前支持传入一个或两个参数。<br>&bull;当传入一个入参时：根据传入的layout对象创建Tensor。<br>&bull;当传入两个入参时：根据传入的参数构建layout对象，并基于该layout创建Tensor。 |

## 返回值说明

- 返回`global_tensor<Engine, Layout>`或`local_tensor<Engine, Layout>`类型的张量对象，具体由engine绑定的内存位置决定。

## 调用示例

  ```cpp
  #include "tensor_api/tensor.h"

  using namespace asc::te;

  constexpr uint64_t gm_addr = 128;

  // 示例1：使用GM指针和layout创建global_tensor
  auto gm_ptr = make_mem_ptr<location::gm, half>(gm_addr);
  auto gm_layout = make_layout(make_shape(32, 32), make_stride(32, 1));
  auto gm_tensor = make_tensor(gm_ptr, gm_layout);

  // 示例2：使用Local Memory指针和layout创建local_tensor
  constexpr int tile_num = 4;
  __cbuf__ half data_ptr[tile_num]; // 初始化
  // data_ptr为__cbuf__ half类型数组，其物理位置为L1 Buffer。
  auto ptr = make_mem_ptr(data_ptr);
  auto layout = make_frame_layout<nz_layout_ptn, half>(32, 32);
  auto tensor = make_tensor(ptr, layout);

  // 示例3：使用指针和布局形状创建张量
  auto tensor2 = make_tensor(ptr, make_shape(32, 32), make_stride(32, 32));
  ```
