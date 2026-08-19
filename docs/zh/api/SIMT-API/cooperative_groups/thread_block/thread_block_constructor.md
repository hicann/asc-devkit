# thread_block构造函数

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/thread_block/thread_block_constructor_res.md#id1 -->

## 功能说明

`thread_block`不提供默认的构造函数，用户使用`this_thread_block`函数获取当前线程所属的`thread_block`对象。

## 函数原型

```cpp
// 默认场景使用此接口创建thread_block
thread_block this_thread_block()
```

```cpp
// 后续需要创建跨warp的thread_block_tile（size > 32）时使用此接口创建thread_block
template <unsigned int MaxBlockSize>
thread_block this_thread_block(block_tile_memory<MaxBlockSize>& scratch)
```

## 参数说明

### 接口参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| scratch | 输入 | Global Memory或UB上的[block_tile_memory](#block_tile_memory说明)对象，用于为`thread_block`携带跨Warp临时存储。 |

### block_tile_memory说明

`block_tile_memory`用于为跨Warp的`thread_block_tile`提供组内同步和跨Warp通信所需的临时存储。结构体大小可通过`sizeof()`接口获取。

```cpp
template <unsigned int MaxBlockSize = 1024>
struct block_tile_memory
```

**表2**  `block_tile_memory`模板参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| MaxBlockSize | 输入 | 模板参数，指定当前线程块中需要支持的最大线程数，必须大于0且小于等于2048，必须是Warp大小32的整数倍，默认值为1024。 |

> [!CAUTION]注意 
> 线程块实际启动线程数不能超过`MaxBlockSize`，否则是未定义行为。

## 返回值说明

返回当前线程所属的`thread_block`对象。

## 约束说明

- 线程块实际启动线程数不能超过`MaxBlockSize`。
- `scratch`对应的[block_tile_memory](#block_tile_memory说明)对象必须位于Global Memory或Unified Buffer（UB），不能是在栈空间中创建的对象。

## 调用示例

参考[调用示例](sync.md#调用示例)
