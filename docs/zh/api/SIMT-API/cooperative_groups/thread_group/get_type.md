# get_type

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/thread_group/get_type_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/cooperative_groups.h"`。

获取`thread_group`对象表示的协作组类型。

## 函数原型

```cpp
group_type get_type() const
```

## 参数说明

无

## 返回值说明

返回协作组类型，可能的枚举值如下：

| 枚举值 | 描述 |
| --- | --- |
| `group_type::thread_block_type` | thread_block协作组。 |
| `group_type::tiled_group_type` | thread_block_tile协作组。 |
| `group_type::coalesced_group_type` | coalesced_group协作组。 |
| `group_type::grid_group_type` | grid_group协作组，仅SIMT编程场景支持。 |

## 约束说明

无

## 调用示例

- SIMT编程场景：

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        thread_group tile = tiled_partition(this_thread_block(), 16);
        group_type type = tile.get_type(); // 返回group_type::tiled_group_type
        ...
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(...)
    {
        thread_group tile = tiled_partition(this_thread_block(), 16);
        group_type type = tile.get_type(); // 返回group_type::tiled_group_type
        ...
    }
    ```
