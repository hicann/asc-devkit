# tiled_partition

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
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/tiled_partition_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/cooperative_groups.h"`。

`tiled_partition` API用于将一个线程组划分为多个更小、固定大小的子组，以便线程在以更精细的粒度上进行协作。提供模板和非模板两个版本的接口，分别用于编译时确定划分大小以及运行时确定划分大小的场景。模板版本支持创建跨Warp的`thread_block_tile`。

## 函数原型

```cpp
template <unsigned int Size, typename ParentT>
thread_block_tile<Size, ParentT> tiled_partition(const ParentT& g)
```

```cpp
thread_group tiled_partition(const thread_group& parent, unsigned int tilesz)
```

```cpp
thread_group tiled_partition(const thread_block& parent, unsigned int tilesz)
```

```cpp
coalesced_group tiled_partition(const coalesced_group& parent, unsigned int tilesz)
```

## 参数说明

**表1**  模板版本参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| g | 输入 | 被划分的父组，类型只能是`thread_block`或`thread_block_tile`。 |
| Size | 输入 | 模板参数，指定划分出的`thread_block_tile`组大小。 |

**表2**  非模板版本参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| parent | 输入 | 被划分的父组，类型只能是`thread_block`或`coalesced_group`。 |
| tilesz | 输入 | 指定划分出的子组大小。 |

## 返回值说明

返回划分出的子组对象。

## 约束说明

- 模板版本中，`Size`必须是$2^n$，当前可选值范围：1、2、4、8、16、32、64、128、256、512、1024、2048。
- 非模板版本仅支持创建`tilesz <= 32`的子组，且`tilesz`必须是2的幂（即取值范围为1、2、4、8、16、32）。
- 当要创建跨Warp的`thread_block_tile`时，用户需创建[block_tile_memory](./thread_block/thread_block_constructor.md#block_tile_memory说明)作为临时存储，并通过带`scratch`参数的`this_thread_block`创建父`thread_block`。传入的`block_tile_memory`对象必须位于Global Memory或Unified Buffer（UB），不能传入栈空间中创建的对象。使用位于UB的对象性能优于位于Global Memory的。
- 对于模板版本的接口，父组中的线程数必须能被`Size`整除。并且如果父组是`thread_block_tile`，则`Size`必须小于父组大小。

## 调用示例

- SIMT编程场景：

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        // 无需使用跨warp的thread_block_tile
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);

        // 需要使用跨warp的thread_block_tile
        __ubuf__ block_tile_memory<1024> scratch;
        thread_block block_with_memory = this_thread_block(scratch);
        auto tile64 = tiled_partition<64>(block_with_memory);
        ...
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(...)
    {
        ...
        // 无需使用跨warp的thread_block_tile
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);

        // 需要使用跨warp的thread_block_tile
        __ubuf__ block_tile_memory<1024> scratch;
        thread_block block_with_memory = this_thread_block(scratch);
        auto tile64 = tiled_partition<64>(block_with_memory);
        ...
    }
    ```
