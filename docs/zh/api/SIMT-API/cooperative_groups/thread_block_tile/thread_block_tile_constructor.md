# thread_block_tile构造函数

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/thread_block_tile/thread_block_tile_constructor_res.md#id1 -->

## 功能说明

`thread_block_tile`不提供默认构造函数，可通过`tiled_partition()`接口从另一个协作组中划分得到。

## 函数原型

```cpp
template <unsigned int Size, typename ParentT>
thread_block_tile<Size, ParentT> tiled_partition(const ParentT& g)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| g | 输入 | 被划分的父组，类型只能是`thread_block`或`thread_block_tile`。 |
| Size | 输入 | 模板参数，指定划分出的`thread_block_tile`组大小。 |

## 返回值说明

返回划分后当前线程所属的`thread_block_tile`组。

## 约束说明

- `Size`必须是$2^n$，当前可选值范围：1、2、4、8、16、32、64、128、256、512、1024、2048。
- 当`Size`大于32时，即`Size`为64、128、256、512、1024、2048的跨Warp协作场景，必须使用[block_tile_memory](../thread_block/thread_block_constructor.md#block_tile_memory说明)创建父`thread_block`。并且用于创建父`thread_block`的`block_tile_memory`对象必须位于Global Memory或Unified Buffer（UB），不能是在栈空间中创建的对象。使用位于UB的对象性能优于位于Global Memory的。
- 对于模板版本的接口，父组中的线程数必须能被`Size`整除。并且如果父组是`thread_block_tile`，则`Size`必须小于父组大小。

## 调用示例

- SIMT编程场景：

    ```cpp
    using namespace cooperative_groups;
    constexpr int THREAD_NUM = 1024;
    __global__ void simt_kernel(...)
    {
        ...
        // 创建Size<=32的thread_block_tile
        thread_block block = this_thread_block();
        thread_block_tile<32> tile32 = tiled_partition<32>(block);              // 按照32个线程为一组划分thread_block
        auto tile32_auto = tiled_partition<32>(block);                          // 建议使用auto管理返回对象
        thread_block_tile<4, thread_block> tile4 = tiled_partition<4>(block);   // 按照4个线程为一组划分thread_block，对象类型中保留父组信息

        // 创建Size>32的thread_block_tile
        __ubuf__ block_tile_memory<THREAD_NUM> scratch;
        thread_block block_with_memory = this_thread_block(scratch);
        auto tile64 = tiled_partition<64>(block_with_memory);                   // 按照64个线程为一组划分thread_block
        ...
    }

    int main()
    {
        ...
        simt_kernel<<<dim3(1), dim3(THREAD_NUM), 0, stream>>>(...);             // 线程块中创建的线程数必须小于等于`block_tile_memory`的模板参数
        ...
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    constexpr int THREAD_NUM = 1024;
    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(...)
    {
        ...
        // 创建Size<=32的thread_block_tile
        thread_block block = this_thread_block();
        thread_block_tile<32> tile32 = tiled_partition<32>(block);              // 按照32个线程为一组划分thread_block
        auto tile32_auto = tiled_partition<32>(block);                          // 建议使用auto管理返回对象
        thread_block_tile<4, thread_block> tile4 = tiled_partition<4>(block);   // 按照4个线程为一组划分thread_block，对象类型中保留父组信息

        // 创建Size>32的thread_block_tile
        __ubuf__ block_tile_memory<THREAD_NUM> scratch = {};                    // 混合编程场景下需使用此方式在UB创建对象
        thread_block block_with_memory = this_thread_block(scratch);
        auto tile64 = tiled_partition<64>(block_with_memory);                   // 使用UB临时存储划分thread_block
        ...
    }

    __global__ __vector__ void global_kernel(...)
    {
        ...
        asc_vf_call<simt_kernel>(dim3(THREAD_NUM), ...);                        // 拉起SIMT VF时配置的线程数必须小于等于 block_tile_memory 的模板参数
        ...
    }
    ```
