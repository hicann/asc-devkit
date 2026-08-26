# thread_rank

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/coalesced_group/thread_rank_res.md#id1 -->

## 功能说明

获取当前线程在`coalesced_group`组内的排名，排名从0开始。

## 函数原型

```cpp
unsigned int thread_rank() const
```

## 参数说明

无

## 返回值说明

当前线程在`coalesced_group`组内的排名。

## 约束说明

无

## 调用示例

示例代码中的条件分支将一个warp中所有线程id是偶数的线程组成`coalesced_group`协作组，组内各线程`thread_rank`接口返回结果如下图所示。

**图1**  coalesced_group组内各线程rank  
![](../../../figures/coalesced_group_rank.png "coalesced_group_rank")

- SIMT编程场景：

    ```cpp
    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        if (threadIdx.x % 2 == 0) {
            coalesced_group active = coalesced_threads();
            unsigned int rank = active.thread_rank();
        }
        ...
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(...)
    {
        ...
        if (threadIdx.x % 2 == 0) {
            coalesced_group active = coalesced_threads();
            unsigned int rank = active.thread_rank();
        }
        ...
    }
    ```
