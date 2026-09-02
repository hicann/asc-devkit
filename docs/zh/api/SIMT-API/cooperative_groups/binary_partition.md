# binary_partition

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/binary_partition_res.md#id1 -->

## 功能说明

`binary_partition` API用于根据一个标签（0或1）将父组划分为两个子组，标签相同的线程会被分配到同一组中。

## 函数原型

```cpp
coalesced_group binary_partition(const coalesced_group& g, bool pred)
```

```cpp
template <unsigned int Size, typename ParentT>
coalesced_group binary_partition(const thread_block_tile<Size, ParentT>& g, bool pred)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| g | 输入 | 被划分的父组，类型可以是`coalesced_group`或`thread_block_tile`。 |
| pred | 输入 | 标签，用于划分子组。 |

## 返回值说明

返回包含当前调用线程的子组。当前线程及所有与其具有相同`pred`标签的线程会被划分到同一子组。返回类型为`coalesced_group`。

## 约束说明

- 组内所有线程必须共同调用该接口，否则行为未定义。

## 调用示例

- SIMT编程场景：

    ```cpp
    using namespace cooperative_groups;
    __global__ void simt_kernel(int *inputArr, ...)
    {
        auto block = this_thread_block();
        auto tile32 = tiled_partition<32>(block);

        // inputArr中是随机的整数
        int elem = inputArr[block.thread_rank()];
        // 根据elem&1是否为true将tile32划分为两个子组
        auto subtile = binary_partition(tile32, (elem & 1));
        ...
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(__gm__ int *inputArr, ...)
    {
        ...
        auto block = this_thread_block();
        auto tile32 = tiled_partition<32>(block);

        // inputArr中是随机的整数
        int elem = inputArr[block.thread_rank()];
        // 根据elem&1是否为true将tile32划分为两个子组
        auto subtile = binary_partition(tile32, (elem & 1));
        ...
    }
    ```
