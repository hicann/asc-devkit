# Basic Process

<!-- md-trans-meta sourceCommit=ea8897b388b9bae7364d26841da4bdbbcadcfb65 translatedAt=2026-10-23T13:49:59.893Z pushedAt=2026-11-03T07:05:13.403Z nl=2 -->

This document covers the basics and describes the basic process of writing Host-side Tiling in the aclnn engineering-based operator development approach. Host-side Tiling takes the Tiling design conclusions from [Operator Function Design](./operator_function_design.md), and converts the input shape, attributes, and running scenarios into the parameters required before the kernel function is launched.

## Tiling Concept Review

Tiling is the process of determining the data partitioning method and runtime configuration based on the data scale, platform resources, and kernel function implementation before the kernel function is launched. Tiling itself does not perform the tensor computation of the operator; instead, it prepares the parameters required for the kernel function to run.

For common data-parallel operators, data is typically partitioned at the following two levels:

- Block: the logical parallel unit of the kernel function. The overall data is distributed to multiple Blocks for parallel processing, and the number of Blocks is specified by `numBlocks`.
- Tile: the data unit processed in batches within a single Block. When the data assigned to a Block cannot fit into on-chip storage at once (for example, the Unified Buffer (UB)), the kernel function loops over Tiles to complete data loading, computation, and write-back.

TilingFunc runs on the Host side. The framework invokes TilingFunc before the kernel function is launched and provides the input/output shapes and attribute information through `TilingContext`. TilingFunc reads and validates this information, computes the partitioning parameters, selects the kernel function branch, and finally writes back two types of results:

- `TilingData`: the parameters that the kernel function needs to read during execution, such as the total number of elements, the amount of data processed at a time, the number of loop iterations, the tail block length, and branch parameters.
- Kernel function launch configuration: for example, `numBlocks`, the workspace size, `TilingKey`, and the scheduling mode.

The inputs and outputs of Host-side Tiling are shown in the following figure:

![Tiling inputs and outputs](../../../../figures/tiling_io.png "Tiling inputs and outputs")

In the figure, `TilingContext` is the data carrier between TilingFunc and the framework: TilingFunc reads the input and output shapes and attribute information from it, and writes back the results such as `TilingData`, `numBlocks`, `TilingKey`, and the workspace size.

## Core Concepts

The outputs of Host-side Tiling mainly include `TilingData`, `numBlocks`, workspace size, and `TilingKey`. Different runtime scenarios may also involve kernel function launch configurations such as the schedule mode. Understanding these outputs before writing code makes the subsequent process easier to follow.

### TilingData

`TilingData` carries the tiling results passed from the Host side to the kernel function side. You only need to define the fields actually used by the kernel function. Common fields include the total number of elements, the amount of data processed at a time, the number of loops, the tail block length, and branch parameters. The specific fields are determined by the kernel function implementation.

The AddCustom basic sample defines only `totalLength` and `tileNum`. `totalLength` indicates the total number of elements, and `tileNum` indicates the number of blocks into which the total data is divided in the sample. The kernel function calculates the amount of data processed at a time through `totalLength / tileNum`, and together with `numBlocks` determines the number of loops for each Block.

AscendC recommends using **standard C++ syntax** to define the Tiling structure. This approach makes it easy to organize fixed-size, directly copyable fields, and allows different operators to use structures with the same name. For constraints on field types, array syntax, and Host/kernel function side consistency, see [Usage Constraints](#tilingdata-constraints).

> [!NOTE] When to Use the Macro Definition Approach
> When a specific API has special requirements on how the Tiling structure is defined, follow the corresponding API documentation. When maintaining legacy projects developed with `BEGIN_TILING_DATA_DEF`, you can also continue to use the macro definition approach.

This document uses standard C++ syntax as the main line to introduce the basic process of Tiling implementation.

### numBlocks<a id="numblocks"></a>

`numBlocks` specifies the number of SIMD Blocks launched by the kernel function. It should be determined based on the data scale, tiling strategy, and hardware resources. The configurable range is also subject to the AI processor model, the kernel function execution type, and runtime resource constraints. For details, see [Kernel Configuration](../../../language_extension/simd_builtin_keywords.md#section97005415463).

### workspace Size

The workspace is temporary memory in the Global Memory on the device side. It provides auxiliary storage for kernel function computation, for example, saving intermediate results, serving as a temporary cache for Ascend C APIs, or providing temporary space for the kernel function. The Tiling function on the host side only calculates and sets the required workspace size and does not directly use this memory. In the single-operator API execution scenario, the developer obtains the workspace size through the first-stage API and applies for Global Memory of the corresponding size. In the graph mode scenario, the framework automatically applies for memory based on the size set by the host-side Tiling. After the memory is allocated, the kernel function can access this memory through the entry parameter `workspace`.

The workspace memory to be set on the host side consists of the system workspace and the user workspace. For details about how to set them, see [How to Use Workspace](../../../appendix/common_operations/how_to_use_workspace.md).

### TilingKey (Optional)

`TilingKey` is used to distinguish different kernel function implementation branches of the same operator. Host-side Tiling selects a numeric identifier based on shape, dtype, attributes, or the running scenario, and writes it into the context through `context->SetTilingKey(...)`. The Kernel side and the compilation toolchain select the corresponding implementation based on this number. For more information, see [Multi-Branch Strategy](./multi_branch_strategy.md).

## Tiling Implementation Process

The host-side tiling implementation of AddCustom can be understood in three steps:

1. Define the `TilingData` structure to determine the fields that the Host side needs to pass to the kernel function side.
2. Write `TilingFunc` to read the shape and write `TilingData`, `numBlocks`, and the workspace size.
3. Associate the Tiling function with the operator prototype through `AICore().SetTiling(...)`.

### Defining the TilingData Structure<a id="define-tilingdata-structure"></a>

The `TilingData` header file in the AddCustom sample is located at:

```text
op_kernel/add_custom_tiling.h
```

The code is as follows:

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

The fields are described as follows:

| Field | Meaning | Purpose on the Kernel Side |
|---|---|---|
| `totalLength` | Total number of elements to be computed this time | Calculates the data range processed by each Block |
| `tileNum` | Number of tiles into which the total data is divided in the sample | The Kernel calculates the amount of data processed at a time through `totalLength / tileNum`, and determines the number of loops for each Block together with `numBlocks` |

The structure header file is placed in the `op_kernel` directory so that both the Host-side Tiling function and the Kernel-side implementation can reference this definition.

### Writing TilingFunc

The Host-side implementation file of AddCustom is located at:

```text
op_host/add_custom.cpp
```

This file defines `optiling::TilingFunc`. After the operator prototype associates this function through `SetTiling(optiling::TilingFunc)`, the framework calls it before the operator is executed and passes in `gert::TilingContext* context`.

The AddCustom sample code is as follows:

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

> [!NOTE] Tiling Premise of the AddCustom Sample
> The sample input shape is `[8, 2048]`, with a total of 16384 elements. The Host side sets both `numBlocks` and `tileNum` to 8. The kernel function calculates that each Block processes 2048 elements through `totalLength / GetBlockNum()`, and calculates that a single pass also processes 2048 elements through `totalLength / tileNum`. Therefore, each Block loops only once.

This code completes Tiling through the following steps:

1. Obtain the `TilingData` pointer.
2. Read the shape of input `x` and calculate the total number of elements.
3. Set `numBlocks`, and write `totalLength` and `tileNum`.
4. Set the workspace size to 0.
5. Return `ge::GRAPH_SUCCESS`.

In the standard C++ structure mode, the Host side obtains the structure pointer through `GetTilingData<T>()` and directly writes fields, and the framework handles subsequent data transfer. The way the kernel function side reads the data is described in [Kernel Side Operator Implementation](./kernel_operator_implementation.md).

<!-- npu="950" id1 -->
> [!NOTE] Dynamic UB Configuration in SIMD and SIMT Hybrid Scenarios
>
> When SIMD and SIMT hybrid programming is enabled, at least 32 KB Data Cache must be reserved for SIMT. The Host side Tiling needs to determine the dynamic UB size based on the static UB used by the kernel function, the reserved space, and the Data Cache size, and call [`SetDynUBufSize`](https://gitcode.com/cann/metadef/blob/9.2.0/docs/en/api/gert_namespace/tilingcontext/SetDynUBufSize.md) to set it. You can call [`GetDynUBufSize`](https://gitcode.com/cann/metadef/blob/9.2.0/docs/en/api/gert_namespace/tilingcontext/GetDynUBufSize.md) to query the dynamic UB size that has been set.
>
> Assume that TilingFunc has calculated `dynUBufSize` based on the actual requirements of the kernel function. The call example is as follows:
>
> ```cpp
> ge::graphStatus ret = context->SetDynUBufSize(dynUBufSize);
> if (ret != ge::GRAPH_SUCCESS) {
>     return ret;
> }
> ```
>
> For details about the dynamic allocation method, size limits, and Data Cache layout of shared memory, see [Shared Memory](../../../programming_model/ai_core_simt_programming/memory_hierarchy.md#section66329146410).
<!-- end id1 -->

### Associating the Operator Prototype

After the Tiling function is written, you need to associate it in the operator prototype definition through `AICore().SetTiling(...)`. The prototype code in the AddCustom sample is as follows:

```cpp
this->AICore()
    .SetTiling(optiling::TilingFunc);
```

> [!NOTE] Note
>
> An operator can register only one Host-side Tiling function. If different AI processors require different Tiling configurations, distinguish the AI processors and set the corresponding parameters within the same Tiling function. Do not register multiple Tiling functions for different AI processors.

`SetTiling` establishes the call relationship between the operator prototype and the Host-side Tiling function. This document only describes how to prepare the kernel function running parameters inside `TilingFunc`. For the complete prototype configuration, see [Operator Prototype Definition](./operator_prototype_definition.md).

## Usage Constraints<a id="tilingdata-constraints"></a>

When defining the `TilingData` structure using standard C++ syntax, you can inspect it by code form. The following types of writing are most likely to affect data transfer between the Host side and the kernel function side.

### Avoiding Member Function Definitions

`TilingData` is used by both the Host side and the Kernel side. Member functions are easily affected by the difference in the `__aicore__` modifier.

```cpp
struct AddCustomTilingData {
    uint32_t totalLength;
    uint32_t tileNum;

    __aicore__ void Init() { } // Host-side compilation is affected by the __aicore__ modifier
    void Reset() { }           // The Kernel side lacks the __aicore__ modifier when used
};
```

### Avoiding Pointer and Reference Fields

When the Host side writes `TilingData`, the written content must be serializable and transferable to the kernel function side. Pointers and references express object relationships on the Host side, which cannot be resolved according to the original object relationships after being transferred to the kernel function side.

```cpp
struct AddCustomTilingData {
    uint32_t* totalLength; // Avoid using pointer fields.
    uint32_t& tileNum;     // Avoid using reference fields.
};
```

### Keep Field Types Simple

`TilingData` fields should use types whose size is determined at compile time and that can be copied directly, such as `uint32_t`, `int64_t`, `float`, `bool`, and fixed-length arrays. Pointers, references, dynamic containers, and complex objects containing virtual functions or virtual inheritance are not suitable as `TilingData` fields.

```cpp
struct AddCustomTilingData {
    uint32_t totalLength;
    uint32_t tileNum;
    bool enableExtraPath;
    uint32_t tileSizes[8];
};
```

Complex objects and dynamic memory-related information are computed in the Host-side Tiling function, and then the results required by the kernel function are written as simple fields.

### Explicitly Assigning Values to Fields

The fields of the structure obtained by `GetTilingData<T>()` have no default business meaning. Any field that the Kernel will subsequently read must be explicitly assigned in the Host-side Tiling function.

```cpp
AddCustomTilingData* tiling = context->GetTilingData<AddCustomTilingData>();
uint32_t totalLength = context->GetInputShape(0)->GetOriginShape().GetShapeSize();

tiling->totalLength = totalLength;
tiling->tileNum = TILE_NUM;
```

### Using a using Alias for Multiple Template Parameters

If `TilingData` uses multiple template parameters, the registration macro on the kernel function side will treat the commas in the template parameters as macro argument separators. In such scenarios, first define an alias with `using`, and then pass the alias to the registration macro.

```cpp
template<int32_t sizeA, int32_t sizeB>
struct AddCustomTilingData {
    uint32_t totalLength;
    uint32_t dataArray[sizeA][sizeB];
};

using AddCustomTiling = AddCustomTilingData<3, 5>;
REGISTER_TILING_DEFAULT(AddCustomTiling);
```

## Related Documents

- [Operator Prototype Definition](./operator_prototype_definition.md): Learn how to associate the Tiling function with the operator prototype through `AICore().SetTiling(...)`.
- [Passing Attribute Information Through TilingData](./tiling_data_attributes.md): Learn how to read operator attributes and pass them to the Kernel side through TilingData.
- [Tiling Implementation for Advanced APIs](./tiling_with_advanced_api.md): Learn how to compute and pass the corresponding Tiling parameters when using advanced APIs.
- [Kernel-Side Operator Implementation](./kernel_operator_implementation.md): Learn how the Kernel side reads `TilingData` and uses the runtime parameters set on the Host side.
- [Multi-Branch Strategy](./multi_branch_strategy.md): Learn how to select different Kernel implementation branches through `TilingKey`.
