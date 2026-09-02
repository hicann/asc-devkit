# sync

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/sync_res.md#id1 -->

## 功能说明

同步指定协作组内的所有线程，功能等价于调用该协作组对象的成员函数`sync()`。

## 函数原型

```cpp
template <typename GroupType>
void sync(const GroupType& g)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| GroupType | 协作组对象的类型。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| g | 输入 | 待同步的协作组对象。 |

## 返回值说明

无

## 约束说明

- 对于`grid_group`，该接口仅支持SIMT编程场景，不支持SIMD与SIMT混合编程场景。
- 必须保证协作组内所有线程都能执行到同一个`sync(g)`调用，否则是未定义行为。
- 使用`grid_group`执行跨线程块同步时，需确保Grid中的线程块总数不超过设备的AIV物理核数，以避免程序卡死。
- `grid_group`的同步仅保证线程指令执行的同步，不保证各线程块的Data Cache和Global Memory间的缓存一致性。

## 调用示例

- SIMT编程场景：

    ```cpp
    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        thread_block block = this_thread_block();
        sync(block); // 等价于block.sync()

        grid_group grid = this_grid();
        sync(grid); // 等价于grid.sync()
        ...
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(...)
    {
        ...
        thread_block block = this_thread_block();
        sync(block); // 等价于block.sync()
        ...
    }
    ```
