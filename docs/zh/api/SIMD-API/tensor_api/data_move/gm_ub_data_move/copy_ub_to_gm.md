# copy（Unified Buffer到Global Memory数据搬运）

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3训练系列产品/Atlas A3推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2训练系列产品/Atlas A2推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`tensor_api/tensor.h`。

本接口将源Tensor的数据从Unified Buffer搬运到Global Memory。接口支持完整Tensor搬运，也支持通过`dst_coord`、`src_coord`和`copy_shape`指定目的Tensor中的起始坐标、源Tensor中的起始坐标和搬运区域的形状，执行区域搬运。

## 函数原型

- 执行Unified Buffer到Global Memory搬运。

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor>
    __aicore__ inline void copy(const copy_atom<Atom>& atom,
        const DstTensor& dst, const SrcTensor& src)
    ```

- 按指定源坐标、目的坐标和搬运形状执行Unified Buffer到Global Memory搬运。

    ```cpp
    template <typename Atom, typename DstTensor, typename SrcTensor, typename DstCoord,
        typename SrcCoord, typename CopyShape>
    __aicore__ inline void copy(const copy_atom<Atom>& atom, const DstTensor& dst,
        const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape)
    ```

- 使用默认trait构造搬运原子对象。

    ```cpp
    template <typename CopyOperation>
    __aicore__ inline constexpr auto make_copy(const CopyOperation& operation)
    ```

- 使用指定trait构造搬运原子对象。

    ```cpp
    template <typename CopyOperation, typename CopyTrait>
    __aicore__ inline constexpr auto make_copy(const CopyOperation& operation,
        const CopyTrait& trait)
    ```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| Atom | `copy_atom`的模板参数，由`make_copy`接口推导得到。 |
| DstTensor | 目的张量类型，需为Tensor API Tensor类型。 |
| SrcTensor | 源张量类型，需为Tensor API Tensor类型。 |
| DstCoord | 目的张量起始坐标类型，也可为`zero_coord`。 |
| SrcCoord | 源张量起始坐标类型，也可为`zero_coord`。 |
| CopyShape | 搬运区域形状的类型。 |
| CopyOperation | 搬运操作对象类型。 |
| CopyTrait | 搬运trait对象类型。 |

**表2**  `copy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| atom | 输入 | 搬运原子对象，可由`make_copy`接口构造。 |
| dst | 输出 | 目的Tensor，必须位于Global Memory。 |
| src | 输入 | 源Tensor，必须位于Unified Buffer。 |
| dst_coord | 输入 | 搬运区域在目的Tensor中的起始坐标。 |
| src_coord | 输入 | 搬运区域在源Tensor中的起始坐标。 |
| copy_shape | 输入 | 搬运区域的形状，用于指定搬运区域的大小。 |

**表3**  `make_copy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| operation | 输入 | 搬运操作对象。Unified Buffer到Global Memory搬运取`copy_ub_to_gm{}`。 |
| trait | 输入 | 搬运trait对象。Unified Buffer到Global Memory默认取`ub_to_gm_trait_default{}`。 |

### copy_ub_to_gm说明

`copy_ub_to_gm`用于标识Unified Buffer到Global Memory的数据搬运通路，其静态`copy`成员负责分发该通路的数据搬运。

```cpp
struct copy_ub_to_gm {
    template <typename Trait, const Trait& trait, typename... Args>
    __aicore__ inline static void copy(const Args&... args);
};
```

### ub_to_gm_trait说明

`ub_to_gm_trait`表示Unified Buffer到Global Memory搬运的编译期特性，当前不包含可配置成员。`ub_to_gm_trait_default`提供该通路的默认Trait类型和默认值。

```cpp
struct ub_to_gm_trait {};

struct ub_to_gm_trait_default {
    using trait_type = ub_to_gm_trait;
    static constexpr const trait_type value = {};
};
```

## 约束说明

- 源和目的Tensor的元素类型必须一致。
- 支持相同格式间搬运：`ND`、`NDExt`、`DN`、`DNExt`和`NZ`。
- 搬运长度、块数和Stride需要满足Unified Buffer到Global Memory搬运指令约束。
- 区域搬运的坐标、搬运区域大小及范围必须合法。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto atom = make_copy(copy_ub_to_gm{}, ub_to_gm_trait_default{});
copy(atom, gm_tensor, ub_tensor);
```
