# make_mem_ptr

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

make_mem_ptr用于构造带物理存储位置标记和数据类型信息的内存指针适配器`hardware_mem_ptr`，支持的物理位置如下表所示。

make_mem_ptr提供三种调用形式：

1. 指定物理位置和数据类型，根据原始地址构造指针。
2. 指定物理位置，从已有迭代器或指针适配器构造新指针。
3. 从已有迭代器或指针适配器中自动推导物理位置。

## 函数原型

- 指定物理位置和数据类型，根据原始地址构造：

  ```cpp
  template <typename PtrPattern, typename DataType, typename Addr>
  __aicore__ inline auto make_mem_ptr(Addr address)
  ```

- 指定物理位置，从已有迭代器构造：

  ```cpp
  template <typename PtrPattern, typename Iterator>
  __aicore__ inline constexpr auto make_mem_ptr(Iterator iterator)
  ```

- 从已有迭代器中自动推导物理位置：

  ```cpp
  template <typename Iterator>
  __aicore__ inline constexpr auto make_mem_ptr(Iterator iterator)
  ```

## 参数说明

**表1**  支持的物理位置

| location | 说明 | 用途 |
| :--- | :--- | :--- |
| location::gm | Global Memory | 全局内存，张量输入输出的主要存储区域 |
| location::l1 | L1 Buffer | 一级缓存，矩阵搬运和计算前的数据暂存区 |
| location::l0a | L0A Buffer | 矩阵计算左输入缓存 |
| location::l0b | L0B Buffer | 矩阵计算右输入缓存 |
| location::l0scalea | L0 ScaleA Buffer | MX矩阵计算ScaleA缓存 |
| location::l0scaleb | L0 ScaleB Buffer | MX矩阵计算ScaleB缓存 |
| location::l0c | L0C Buffer | 矩阵计算结果缓存 |
| location::ub | UB | 统一缓存，矢量计算的数据存储区 |
| location::bias | BiasTable Buffer | 偏置表缓存，带偏置矩阵计算的偏置存放区 |
| location::fixbuf | Fixpipe Buffer | Fixpipe输出缓存，L0C到GM/UB的中转区 |

**表2**  模板参数说明

| 参数名 | 类型 | 描述 |
| :--- | :---: | :--- |
| PtrPattern | 输入 | 物理存储位置类型。 |
| DataType | 输入 | 数据类型。 |
| Addr | 输入 | 偏移量数值的类型。 |
| Iterator | 输入 | 迭代器类型或已有指针适配器类型。 |

**表3**  参数说明

| 参数名 | 类型 | 描述 |
| :--- | :---: | :--- |
| address | 输入 | 地址偏移量，即相对于基地址的偏移大小。 |
| iterator | 输入 | 已有的硬件地址迭代器或指针适配器对象。 |

## 返回值说明

返回`hardware_mem_ptr<PtrPattern, Pointer>`类型的指针适配器对象，其中：

- PtrPattern表示指定或推导得到的物理位置；
- Pointer表示对应地址空间下的指针类型，例如__gm__ float*、\_\_cbuf_\_ half*等。

## 约束说明

- 指定PtrPattern时，必须是受支持的物理位置类型。
- Iterator需要满足对应构造路径的模板约束。
- 自动推导形式要求输入对象本身已经带有可识别的硬件位置信息。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

constexpr uint64_t gm_offset = 128;
constexpr uint64_t l1_offset = 128;
constexpr uint64_t l0a_offset = 128;
constexpr uint64_t ub_offset = 128;
// 1. 从原始地址构造：指定物理位置和数据类型
auto gm_ptr = make_mem_ptr<location::gm, float>(gm_offset);
auto l1_ptr = make_mem_ptr<location::l1, float>(l1_offset);
auto l0a_ptr = make_mem_ptr<location::l0a, half>(l0a_offset);
auto ub_ptr = make_mem_ptr<location::ub, bfloat16_t>(ub_offset);

// 2. 配合make_tensor构造张量
auto gm_a = make_tensor(gm_ptr, make_frame_layout<nd_ext_layout_ptn, layout_trait_default<float>>(m, n));
auto l1_a = make_tensor(l1_ptr, make_frame_layout<nz_layout_ptn, layout_trait_default<float>>(m, n));

// 3. 从已有指针适配器继承位置（自动推导）
auto another_ptr = make_mem_ptr(gm_ptr);

// 4. 从已有迭代器指定新位置
auto l1_from_iter = make_mem_ptr<location::l1>(some_iterator);
```
