# meta_group_size

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/coalesced_group/meta_group_size_res.md#id1 -->

## 功能说明

获取当前线程所在协作组的直接父组被划分时创建的子组数量。

## 函数原型

```cpp
unsigned int meta_group_size() const
```

## 参数说明

无

## 返回值说明

直接父组被划分时创建的子组的数量。

- 如果该组是通过`coalesced_threads`创建的，则`meta_group_size()`的返回值将为1。

## 约束说明

无

## 调用示例

- SIMT编程场景：

    ```cpp
    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        if (threadIdx.x % 2 == 0) {
            coalesced_group active = coalesced_threads();
            unsigned int group_size = active.meta_group_size(); // 返回1
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
            unsigned int group_size = active.meta_group_size(); // 返回1
        }
        ...
    }
    ```
