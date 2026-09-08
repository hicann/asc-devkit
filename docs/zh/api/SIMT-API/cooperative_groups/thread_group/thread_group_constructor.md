# thread_group构造函数

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/thread_group/thread_group_constructor_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/cooperative_groups.h"`。

`thread_group`不提供默认构造函数。用户可通过非模板版本的`tiled_partition`将已有协作组划分为运行时指定大小的子组，并获取类型擦除后的`thread_group`对象。

## 函数原型

- 划分的父组类型为`thread_block`

    ```cpp
    thread_group tiled_partition(const thread_block& parent, unsigned int tilesz)
    ```

- 划分的父组类型为`thread_group`

    ```cpp
    thread_group tiled_partition(const thread_group& parent, unsigned int tilesz)
    ```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| parent | 输入 | 被划分的父协作组，类型只能是`thread_block`或`thread_group`。 |
| tilesz | 输入 | 划分后的子组大小，必须是1、2、4、8、16或32。 |

## 返回值说明

返回当前线程所属的`thread_group`子组。

## 约束说明

- `tilesz`必须是2的幂且小于等于32（即取值范围为1、2、4、8、16、32）。
- 当父组线程数不能被`tilesz`整除时，最后一个子组包含剩余线程，其组大小小于`tilesz`。

## 调用示例

- SIMT编程场景：

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        thread_block block = this_thread_block();
        thread_group tile = tiled_partition(block, 16);
        unsigned long long rank = tile.thread_rank();
        ...
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(...)
    {
        thread_block block = this_thread_block();
        thread_group tile = tiled_partition(block, 16);
        unsigned long long rank = tile.thread_rank();
        ...
    }
    ```
