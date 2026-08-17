# 输出shape依赖计算

本文属于扩展内容，介绍输出shape依赖Kernel计算结果时的实现方法。NonZero等算子在Kernel计算完成前无法确定实际输出shape。此类输出需要在原型定义中调用`OutputShapeDependOnCompute()`标记，并由Kernel按照固定格式把实际shape写入框架追加的shape输出地址。

## 在算子原型中标记输出

`OutputShapeDependOnCompute()`只能用于输出。以下示例表示输出`y`的shape需要等待Kernel计算结果：

```cpp
this->Output("y")
    .ParamType(REQUIRED)
    .DataType({ge::DT_UINT32})
    .Format({ge::FORMAT_ND})
    .OutputShapeDependOnCompute();
```

算子有多个输出时，只在实际依赖计算的输出上增加该标记。输入输出声明方法见[算子原型定义](./operator_prototype_definition.md)。

## Kernel入口增加shape输出参数

框架会在所有普通输出之后、`workspace`和`tiling`之前追加一个`GM_ADDR`参数，用于保存依赖计算的输出shape。Kernel入口保持以下顺序：

```text
输入 -> 普通输出 -> shape输出 -> workspace -> tiling
```

示例：

```cpp
extern "C" __global__ __aicore__ void non_zero_custom(
    GM_ADDR x, GM_ADDR y, GM_ADDR shapeOut,
    GM_ADDR workspace, GM_ADDR tiling)
{
    // 计算y，并把y的实际shape写入shapeOut。
}
```

即使有多个输出依赖计算，也只增加一个shape输出地址。各输出的shape信息在该地址中按原型输出顺序连续排列。

## shape信息排布格式

shape输出按`uint64_t`数组组织。每个待刷新输出固定占9个`uint64_t`元素：

| 偏移 | 内容 |
|---|---|
| `0` | rank，即实际维度数 |
| `1`至`8` | 各维度大小，最多8维 |

如果有`n`个输出依赖计算，总空间为`n * 9`个`uint64_t`元素。第`i`个输出从`i * 9`开始，顺序与算子原型中的输出声明顺序一致。

![shape信息排布示意图](../../../../figures/pipe_task_run_33.png)

对于输出Tensor数据类型为`uint64_t`的场景，需要把rank值的第31位设置为1，表示后续维度按`uint64_t`解析：

```cpp
constexpr uint64_t UINT64_SHAPE_FLAG = 0x0000000080000000ULL;
uint64_t encodedRank = UINT64_SHAPE_FLAG | rank;
```

输出Tensor为`uint32_t`时不设置该标记，直接写入rank。

## 写入单个输出shape

假设输出`y`的数据类型为`uint32_t`，Kernel计算后得到实际shape为`[32, 64]`：

```cpp
extern "C" __global__ __aicore__ void non_zero_custom(
    GM_ADDR x, GM_ADDR y, GM_ADDR shapeOut,
    GM_ADDR workspace, GM_ADDR tiling)
{
    // 算子计算逻辑，此处省略。

    constexpr uint32_t SHAPE_SLOT_SIZE = 9;
    AscendC::GlobalTensor<uint64_t> shapeGlobal;
    shapeGlobal.SetGlobalBuffer(
        reinterpret_cast<__gm__ uint64_t*>(shapeOut), SHAPE_SLOT_SIZE);

    shapeGlobal.SetValue(0, 2);   // rank
    shapeGlobal.SetValue(1, 32);  // dim[0]
    shapeGlobal.SetValue(2, 64);  // dim[1]
}
```

索引3至8是该输出的保留槽位，不会保存其他输出的数据。

输出Tensor的数据类型为`uint64_t`，实际shape为`[1, 64, 128, 128]`时，需要设置类型标记：

```cpp
constexpr uint32_t SHAPE_SLOT_SIZE = 9;
constexpr uint64_t UINT64_SHAPE_FLAG = 0x0000000080000000ULL;
AscendC::GlobalTensor<uint64_t> shapeGlobal;
shapeGlobal.SetGlobalBuffer(
    reinterpret_cast<__gm__ uint64_t*>(shapeOut), SHAPE_SLOT_SIZE);

shapeGlobal.SetValue(0, UINT64_SHAPE_FLAG | 4ULL);
shapeGlobal.SetValue(1, 1);
shapeGlobal.SetValue(2, 64);
shapeGlobal.SetValue(3, 128);
shapeGlobal.SetValue(4, 128);
```

## 写入多个输出shape

假设两个输出都依赖计算，数据类型均为`uint64_t`，实际shape分别为`[16, 32]`和`[1, 16, 16, 32]`：

```cpp
constexpr uint32_t SHAPE_SLOT_SIZE = 9;
constexpr uint32_t OUTPUT_COUNT = 2;
constexpr uint32_t SHAPEOUT_SIZE = SHAPE_SLOT_SIZE * OUTPUT_COUNT;
constexpr uint64_t UINT64_SHAPE_FLAG = 0x0000000080000000ULL;

AscendC::GlobalTensor<uint64_t> shapeGlobal;
shapeGlobal.SetGlobalBuffer(
    reinterpret_cast<__gm__ uint64_t*>(shapeOut), SHAPEOUT_SIZE);

// 第0个输出：[16, 32]
shapeGlobal.SetValue(0, UINT64_SHAPE_FLAG | 2ULL);
shapeGlobal.SetValue(1, 16);
shapeGlobal.SetValue(2, 32);

// 第1个输出：[1, 16, 16, 32]，起始偏移为9。
shapeGlobal.SetValue(9, UINT64_SHAPE_FLAG | 4ULL);
shapeGlobal.SetValue(10, 1);
shapeGlobal.SetValue(11, 16);
shapeGlobal.SetValue(12, 16);
shapeGlobal.SetValue(13, 32);
```

当算子还包含不依赖计算的普通输出时，shape输出区域只为调用了`OutputShapeDependOnCompute()`的输出分配槽位，但槽位顺序仍按这些输出在原型中的先后顺序排列。

## 相关文档

- [Kernel侧算子实现](./kernel_operator_implementation.md)：Kernel入口格式和参数顺序。
- [算子原型定义](./operator_prototype_definition.md)：输入输出声明和shape推导关系。
- [OutputShapeDependOnCompute](../../../../../api/Utils-API/prototype_register_management/OpParamDef/OutputShapeDependOnCompute.md)：接口定义和工程版本约束。
