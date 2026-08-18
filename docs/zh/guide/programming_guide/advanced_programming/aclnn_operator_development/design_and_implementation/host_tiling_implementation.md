# 基本流程

本文属于基础内容，介绍Host侧Tiling在aclnn工程化算子开发方式中的基本编写流程。Host侧Tiling承接[算子功能设计](./operator_function_design.md)中的Tiling设计结论，把输入shape、属性和运行场景转换成核函数（Kernel）启动前需要的参数。

## Tiling概念回顾

Tiling是在核函数（Kernel）启动前，根据数据规模、平台资源和核函数（Kernel）实现确定数据切分方式及运行配置的过程。Tiling本身不执行算子的张量计算，而是为核函数（Kernel）准备运行所需的参数。

对于常见的数据并行算子，数据通常按以下两个层次切分：

- Block：核函数（Kernel）的逻辑并行单元。整体数据分配给多个Block并行处理，Block数量通过`numBlocks`指定。
- Tile：单个Block内分批处理的数据单元。当一个Block负责的数据无法一次放入片上存储（例如Unified Buffer（UB））时，核函数（Kernel）按Tile循环完成数据搬入、计算和写回。

TilingFunc运行在Host侧。框架在核函数（Kernel）启动前调用TilingFunc，并通过`TilingContext`提供输入输出shape、属性信息。TilingFunc读取并校验这些信息，计算切分参数、选择核函数（Kernel）分支，最后写回两类结果：

- `TilingData`：核函数（Kernel）执行时需要读取的参数，例如总元素数、单次处理的数据量、循环次数、尾块长度和分支参数。
- 核函数（Kernel）启动配置：例如`numBlocks`、workspace大小、`TilingKey`和调度模式。

Host侧Tiling的输入和输出如下图所示：

![Tiling实现的输入输出](../../../../figures/tiling_io.png "Tiling实现的输入输出")

图中，`TilingContext`是TilingFunc与框架之间的数据载体：TilingFunc从中读取输入输出shape、属性信息，并将`TilingData`、`numBlocks`、`TilingKey`和workspace大小等结果写回其中。

## 核心概念

Host侧Tiling的输出主要包括`TilingData`、`numBlocks`、workspace大小和`TilingKey`；不同运行场景还可能涉及调度模式（schedule mode）等核函数（Kernel）launch配置。写代码前先理解这些输出，后面的流程会更容易对应。

### TilingData

`TilingData`用于承载Host侧传给核函数（Kernel）侧的切分结果，只需定义核函数（Kernel）实际使用的字段。常见字段包括总元素数、单次处理的数据量、循环次数、尾块长度和分支参数，具体字段由核函数（Kernel）实现决定。

AddCustom基础样例只定义`totalLength`和`tileNum`。其中，`totalLength`表示总元素数；`tileNum`表示样例中总数据的分块数量。核函数（Kernel）通过`totalLength / tileNum`计算单次处理的数据量，并与`numBlocks`共同确定每个Block的循环次数。

AscendC推荐使用**标准C++语法**定义Tiling结构体。这种方式便于组织固定大小、可直接复制的字段，且允许不同算子使用同名结构体。字段类型、数组写法和Host/核函数（Kernel）侧一致性约束见[使用约束](#tilingdata-constraints)。

> [!NOTE] 何时使用宏定义方式
> 具体API对Tiling结构体定义方式有特殊要求时，以对应API文档为准。维护使用`BEGIN_TILING_DATA_DEF`开发的旧项目时，也可以继续使用宏定义方式。

本文档以标准C++语法为主线介绍Tiling实现的基本流程。

### numBlocks<a id="numblocks"></a>

`numBlocks`指定核函数（Kernel）启动的SIMD Block数量，应根据数据规模、切分策略和硬件资源确定。可设置范围还受AI处理器型号、核函数（Kernel）执行类型和运行时资源约束，更多信息请参考[核函数（Kernel）配置](../../../language_extension/simd_builtin_keywords.md#section97005415463)。

### workspace大小

workspace是设备侧Global Memory上的临时内存，用于为核函数（Kernel）计算提供辅助存储，例如保存中间结果、作为Ascend C API的临时缓存，或为核函数（Kernel）提供临时空间。Host侧Tiling函数只负责计算并设置所需的workspace大小，不直接使用这块内存。单算子API执行场景下，开发者通过第一段接口获取workspace大小，并申请对应大小的Global Memory；入图场景下，框架会根据Host侧Tiling设置的大小自动申请。申请完成后，核函数（Kernel）可以通过入口参数`workspace`访问这块内存。

Host侧需要设置的workspace内存分为系统workspace和用户workspace两部分，具体设置方法请参考[如何使用workspace](../../../appendix/common_operations/how_to_use_workspace.md)。

### TilingKey（可选）

`TilingKey`用于区分同一个算子的不同核函数（Kernel）实现分支。Host侧Tiling根据shape、dtype、属性或运行场景选择一个数字标识，并通过`context->SetTilingKey(...)`写入上下文。核函数（Kernel）侧和编译工具链会基于这个数字选择对应实现，更多内容可参考[多分支策略](./multi_branch_strategy.md)。

## Tiling实现基本流程

AddCustom的Host侧Tiling实现可以按三个步骤理解：

1. 定义`TilingData`结构体，确定Host侧需要传给核函数（Kernel）侧的字段。
2. 编写`TilingFunc`，读取shape并写入`TilingData`、`numBlocks`和workspace大小。
3. 在算子原型中通过`AICore().SetTiling(...)`关联Tiling函数。

### 定义TilingData结构体<a id="define-tilingdata-structure"></a>

AddCustom样例中的`TilingData`头文件位于：

```text
op_kernel/add_custom_tiling.h
```

代码如下：

```cpp
#ifndef ADD_CUSTOM_TILING_H
#define ADD_CUSTOM_TILING_H
#include <cstdint>

struct AddCustomTilingData {
    uint32_t totalLength;
    uint32_t tileNum;
};

#endif // ADD_CUSTOM_TILING_H
```

字段含义如下：

| 字段 | 含义 | 核函数（Kernel）侧用途 |
|---|---|---|
| `totalLength` | 本次计算的总元素数 | 计算每个Block处理的数据范围 |
| `tileNum` | 样例中总数据的分块数量 | 核函数（Kernel）通过`totalLength / tileNum`计算单次处理的数据量，并与`numBlocks`共同确定每个Block的循环次数 |

结构体头文件放在`op_kernel`目录下，Host侧Tiling函数和核函数（Kernel）侧实现都可以引用这个定义。

### 编写TilingFunc

AddCustom的Host侧实现文件位于：

```text
op_host/add_custom.cpp
```

该文件中定义`optiling::TilingFunc`。算子原型通过`SetTiling(optiling::TilingFunc)`关联这个函数后，框架会在算子执行前调用它，并传入`gert::TilingContext* context`。

AddCustom样例代码如下：

```cpp
#include "../op_kernel/add_custom_tiling.h"
#include "register/op_def_registry.h"

namespace optiling {
const uint32_t NUM_BLOCKS = 8;
const uint32_t TILE_NUM = 8;

static ge::graphStatus TilingFunc(gert::TilingContext* context)
{
    AddCustomTilingData* tiling = context->GetTilingData<AddCustomTilingData>();
    uint32_t totalLength = context->GetInputShape(0)->GetOriginShape().GetShapeSize();
    context->SetSimdNumBlocks(NUM_BLOCKS);
    tiling->totalLength = totalLength;
    tiling->tileNum = TILE_NUM;
    size_t* currentWorkspace = context->GetWorkspaceSizes(1);
    currentWorkspace[0] = 0;
    return ge::GRAPH_SUCCESS;
}
} // namespace optiling
```

> [!NOTE] AddCustom样例的切分前提
> 样例输入shape为`[8, 2048]`，总元素数为16384。Host侧将`numBlocks`和`tileNum`都设置为8；核函数（Kernel）通过`totalLength / GetBlockNum()`计算出每个Block处理2048个元素，通过`totalLength / tileNum`计算出单次也处理2048个元素，因此每个Block只循环1次。

这段代码按以下步骤完成Tiling：

1. 获取`TilingData`指针。
2. 读取输入`x`的shape并计算总元素数。
3. 设置`numBlocks`，并写入`totalLength`和`tileNum`。
4. 将workspace大小设置为0。
5. 返回`ge::GRAPH_SUCCESS`。

标准C++结构体方式下，Host侧通过`GetTilingData<T>()`获取结构体指针并直接写字段，框架负责后续数据传递。核函数（Kernel）侧读取方式会在[核函数（Kernel）侧算子实现](./kernel_operator_implementation.md)中说明。

### 关联算子原型

Tiling函数写好后，需要在算子原型定义中通过`AICore().SetTiling(...)`完成关联。AddCustom样例中的原型代码如下：

```cpp
this->AICore()
    .SetTiling(optiling::TilingFunc);
```

> [!NOTE]说明
>
> 一个算子只能注册一个Host侧Tiling函数。如果不同AI处理器需要采用不同的Tiling配置，应在同一个Tiling函数中区分AI处理器并设置相应参数，不能按AI处理器分别注册多个Tiling函数。

`SetTiling`建立算子原型和Host侧Tiling函数之间的调用关系。本文只说明`TilingFunc`内部如何准备核函数（Kernel）运行参数，完整原型配置见[算子原型定义](./operator_prototype_definition.md)。

## 使用约束<a id="tilingdata-constraints"></a>

使用标准C++语法定义`TilingData`结构体时，可以按代码形态检查。下面几类写法最容易影响Host侧和核函数（Kernel）侧之间的数据传递。

### 避免定义成员函数

`TilingData`会同时被Host侧和核函数（Kernel）侧使用。成员函数容易受到`__aicore__`修饰差异影响。

```cpp
struct AddCustomTilingData {
    uint32_t totalLength;
    uint32_t tileNum;

    __aicore__ void Init() { } // Host侧编译会受到__aicore__修饰影响
    void Reset() { }           // 核函数（Kernel）侧使用时缺少__aicore__修饰
};
```

### 避免使用指针和引用字段

Host侧写入`TilingData`时，写入内容需要能够被序列化并传到核函数（Kernel）侧。指针和引用表达的是Host侧对象关系，传到核函数（Kernel）侧后无法按原对象关系解析。

```cpp
struct AddCustomTilingData {
    uint32_t* totalLength; // 避免使用指针字段
    uint32_t& tileNum;     // 避免使用引用字段
};
```

### 字段类型保持简单

`TilingData`字段应使用大小在编译期确定、可以直接复制的类型，例如`uint32_t`、`int64_t`、`float`、`bool`和定长数组。指针、引用、动态容器以及包含虚函数或虚继承的复杂对象不适合作为`TilingData`字段。

```cpp
struct AddCustomTilingData {
    uint32_t totalLength;
    uint32_t tileNum;
    bool enableExtraPath;
    uint32_t tileSizes[8];
};
```

复杂对象和动态内存相关信息在Host侧Tiling函数中计算完成，再把核函数（Kernel）侧需要的结果写成简单字段。

### 显式给字段赋值

`GetTilingData<T>()`获取到的结构体字段没有默认业务含义。核函数（Kernel）侧后续会读取的字段，都需要在Host侧Tiling函数中显式赋值。

```cpp
AddCustomTilingData* tiling = context->GetTilingData<AddCustomTilingData>();
uint32_t totalLength = context->GetInputShape(0)->GetOriginShape().GetShapeSize();

tiling->totalLength = totalLength;
tiling->tileNum = TILE_NUM;
```

### 多模板参数使用using别名

如果`TilingData`使用多个模板参数，核函数（Kernel）侧注册宏会把模板参数中的逗号识别成宏入参分隔符。此类场景先用`using`定义别名，再把别名传给注册宏。

```cpp
template<int32_t sizeA, int32_t sizeB>
struct AddCustomTilingData {
    uint32_t totalLength;
    uint32_t dataArray[sizeA][sizeB];
};

using AddCustomTiling = AddCustomTilingData<3, 5>;
REGISTER_TILING_DEFAULT(AddCustomTiling);
```

## 相关文档

- [算子原型定义](./operator_prototype_definition.md)：了解如何通过`AICore().SetTiling(...)`把Tiling函数关联到算子原型。
- [通过TilingData传递属性信息](./tiling_data_attributes.md)：了解如何读取算子属性，并通过TilingData传递到核函数（Kernel）侧。
- [使用高阶API时配套的Tiling实现](./tiling_with_advanced_api.md)：了解使用高阶API时如何计算并传递配套Tiling参数。
- [核函数（Kernel）侧算子实现](./kernel_operator_implementation.md)：了解核函数（Kernel）侧如何读取`TilingData`并使用Host侧设置的运行参数。
- [多分支策略](./multi_branch_strategy.md)：了解如何通过`TilingKey`选择不同核函数（Kernel）实现分支。
