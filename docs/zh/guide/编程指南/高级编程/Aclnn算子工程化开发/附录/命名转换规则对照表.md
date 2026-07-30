# 命名转换规则对照表

本文属于基础内容，介绍自定义算子工程中的命名转换规则。算子类型（OpType）通常采用大驼峰命名方式，配置中的实现文件名和Kernel入口名通常采用小写下划线命名方式。默认情况下，框架会根据OpType生成这些名称。

## 转换规则概述

转换发生在构建阶段。构建工具会根据已注册的OpType生成配置和相关代码；算子运行时使用已经生成的配置和符号，不会再次转换名称。

构建工具从左向右按以下规则处理字符：

1. **首字符大写转小写**：首字符直接转换为小写，不插入下划线。
2. **前一个字符不是大写字母时插入下划线**：当前字符是大写字母，且前一个字符是小写字母或数字时，在当前字符前插入下划线，并将当前字符转换为小写。
3. **连续大写与后续单词之间插入下划线**：当前字符和前一个字符都是大写字母，但下一个字符是小写字母时，在当前字符前插入下划线，并将当前字符转换为小写。
4. **其他字符直接转换或保留**：其他大写字母直接转换为小写；小写字母和数字保持不变。

构建工具还会检查OpType是否符合推荐的命名规范：首字符为大写字母，后续字符只能是字母或数字。不符合规范时会输出告警，建议使用大驼峰命名。

## 对照表

下表列出了常见算子名称的转换结果。结果以当前构建工具的实际转换结果为准。

| 算子类型（大驼峰） | 文件名/Kernel入口名（小写下划线） | 转换说明 |
|---|---|---|
| `AddCustom` | `add_custom` | 首字符转小写，`Custom`前插入下划线 |
| `ReduceMaxCustom` | `reduce_max_custom` | `Max`和`Custom`前分别插入下划线 |
| `MatmulCustom` | `matmul_custom` | `Custom`前插入下划线 |
| `BatchNormCustom` | `batch_norm_custom` | `Norm`和`Custom`前分别插入下划线 |
| `Conv2DCustom` | `conv2_d_custom` | `D`前是数字`2`，因此在`D`前插入下划线 |
| `SoftmaxCustom` | `softmax_custom` | `Custom`前插入下划线 |
| `ReluCustom` | `relu_custom` | `Custom`前插入下划线 |
| `LayerNormCustom` | `layer_norm_custom` | `Norm`和`Custom`前分别插入下划线 |
| `SliceCustom` | `slice_custom` | `Custom`前插入下划线 |
| `GatherV2Custom` | `gather_v2_custom` | `V`前是小写字母，`Custom`前是数字`2` |
| `SplitVCustom` | `split_v_custom` | `V`和`Custom`前分别插入下划线 |
| `TransposeCustom` | `transpose_custom` | `Custom`前插入下划线 |
| `OneHotCustom` | `one_hot_custom` | `Hot`和`Custom`前分别插入下划线 |
| `TopKCustom` | `top_k_custom` | `K`和`Custom`前分别插入下划线 |

## 特殊情况说明

### 连续大写字母

连续大写字母通常表示缩写。转换工具不会在缩写的每个字母前都插入下划线，而是在缩写和后续单词的边界插入下划线。

- `AbcAAc`转换为`abc_a_ac`：第一个`A`前是小写字母`c`，第二个`A`后是小写字母`c`，两处都会插入下划线。
- `GRUCell`转换为`gru_cell`：`C`前是大写字母`U`，后是小写字母`e`，因此在`C`前插入下划线。
- `HTTPServerCustom`转换为`http_server_custom`：在`Server`和`Custom`的单词边界插入下划线。

### 数字与字母相邻

数字本身保持不变。数字后的字符如果是大写字母，由于数字不是大写字母，会触发规则2。

```text
GatherV2Custom -> gather_v2_custom
Conv2DCustom   -> conv2_d_custom
```

因此，不能仅凭常见缩写习惯把`Conv2D`写成`conv2d`；当前实现的实际转换结果是`conv2_d`。

### 首字母大写

首字母直接转换为小写，不插入下划线。

```text
Add    -> add
Reduce -> reduce
```

## 工程中的实际影响

默认情况下，开发者需要保证以下对应关系：

| 项目 | 命名方式 | `AddCustom`示例 |
|---|---|---|
| 算子类型、`OpDef`类名、构造函数名、`OP_ADD`参数 | 大驼峰 | `AddCustom` |
| Kernel源码文件名 | 建议使用小写下划线，可由CMake显式指定 | `add_custom.cpp` |
| Kernel入口函数名 | 默认使用小写下划线，需与`opInterface.value`一致 | `add_custom` |

使用msOpGen创建算子工程时，工具通常会按上述规则生成初始文件名和Kernel入口名。构建后可以通过生成的`ops-info.ini`和aclnn头文件确认实际转换结果，应用程序也应以这些构建产物为准。

如果修改了aclnn工程名前缀或直调接口前缀，生成文件和接口的前缀也会变化。重命名Host或Kernel源文件后，还需要同步修改CMake中的源文件列表。

## 相关文档

- [算子原型定义](../设计与实现/算子原型定义.md)：了解OpType、`OpDef`类和`OP_ADD`注册方法。
- [Kernel侧算子实现](../设计与实现/Kernel侧算子实现.md)：了解Kernel入口格式和参数顺序。
- [单算子API调用](../调用/单算子API调用.md)：了解两段式aclnn接口及其参数命名规则。
