# ASC_IS_AIC/ASC_IS_AIV

## 功能说明

头文件路径为：`"c_api/defs/macro.h"`。

`ASC_IS_AIC`和`ASC_IS_AIV`用于[分离模式](../../../../../guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md)中对Cube Core（AIC）和Vector Core（AIV）的代码路径进行条件编译和运行，可作为`if`语句的条件。编译或者运行AIC代码路径时，`ASC_IS_AIC`条件成立；编译或者运行AIV代码路径时，`ASC_IS_AIV`条件成立。

这两个宏分别是`ASCEND_IS_AIC`和`ASCEND_IS_AIV`的C API别名，对应宏的功能相同。基于AIC和AIV分离模式开发`__mix__`融合算子时，可通过这两个宏隔离仅由AIC执行的矩阵计算、矩阵搬运等代码，以及仅由AIV执行的矢量计算、矢量数据搬运等代码。

## 宏定义

```cpp
#define ASC_IS_AIC ASCEND_IS_AIC
#define ASC_IS_AIV ASCEND_IS_AIV
```

## 取值说明

| 当前代码路径 | `ASC_IS_AIC`条件结果 | `ASC_IS_AIV`条件结果 |
| --- | --- | --- |
| AIC | `true` | `false` |
| AIV | `false` | `true` |

## 约束说明

- 这两个宏仅适用[分离模式](../../../../../guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md)。
- 在AIC和AIV分离模式的融合算子中，AIC和AIV执行不同类型的指令。仅支持在AIC上执行的代码必须置于`ASC_IS_AIC`条件分支内；仅支持在AIV上执行的代码必须置于`ASC_IS_AIV`条件分支内。
