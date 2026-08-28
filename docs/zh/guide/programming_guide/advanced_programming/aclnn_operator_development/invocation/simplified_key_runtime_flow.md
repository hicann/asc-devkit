# 基于simplified key的运行时选择流程

本文介绍配置`GenSimplifiedKey`后的运行时流程。该机制用于从同一算子的多份预编译核函数（Kernel）binary中选择与本次调用条件匹配的一份。算子开发者负责提供回调和编译配置JSON；运行时框架负责调用回调、组装完整simplified key、查找并选择binary。

回调的实现见[GenSimplifiedKey回调实现](../design_and_implementation/gen_simplified_key.md)，JSON的提供和编译产物见[simplified key二进制配置JSON](../compilation_and_deployment/simplified_key_json_configuration.md)。

## 运行时流程

在`aclnnXxxGetWorkspaceSize`阶段，运行时按以下顺序处理：

1. 加载算子包中的Host侧算子实现和`binary_info_config.json`，取得该OpType的`binaryList`及`GenSimplifiedKey`回调。
2. 根据本次调用的输入、输出、属性和目标AI处理器创建或复用`TilingContext`。
3. 调用算子注册的`GenSimplifiedKey`回调。回调读取`TilingContext`并写出自定义simplified key片段，例如`custom_fp16`。
4. 运行时框架补齐算子类型、确定性和实现模式等公共维度，生成完整simplified key，例如`AddCustomTemplate/d=0,p=1/custom_fp16`。
5. 运行时使用完整Key在`binary_info_config.json`的`binaryList`中查找，得到匹配的`.o`和配套`.json`路径，并加载对应binary。
6. 运行时调用已注册的`TilingFunc`，计算TilingData、workspace、BlockDim和TilingKey；随后创建executor并返回给调用方。
7. 在`aclnnXxx`执行阶段，运行时使用已选择的binary和前一阶段准备的Tiling结果下发核函数（Kernel）。

simplified key选择发生在Tiling之前的binary匹配阶段。`GenSimplifiedKey`不替代`TilingFunc`：前者用于选择哪份预编译binary，后者用于计算该binary本次执行需要的运行参数。

## AddCustomTemplate示例

`AddCustomTemplate`预编译了FP16和FP32两份binary。其回调根据输入`x`的dtype生成片段，运行时完成后续选择：

**表1**  AddCustomTemplate运行时选择示例

| 本次调用 | 回调结果 | 运行时完整Key示例 | 选中的binary |
|---|---|---|---|
| `x`为FP16，`y`和`z`为FP16 | `custom_fp16` | `AddCustomTemplate/d=0,p=1/custom_fp16` | `AddCustomTemplate_FP16.o` |
| `x`为FP32，`y`和`z`为FP32 | `custom_fp32` | `AddCustomTemplate/d=0,p=1/custom_fp32` | `AddCustomTemplate_FP32.o` |

这里的`d=...`和`p=...`由运行时按调用公共条件处理；算子开发者只维护最后的`custom_fp16`或`custom_fp32`片段。实际配置中同一份binary可以对应多个完整Key，以覆盖不同的确定性或实现模式组合。

## 开发者需要保证的对应关系

运行时能够选择正确binary的前提是两端使用同一套规则：

**表2**  编译侧与运行时回调侧的对应关系

| 编译侧 | 运行时回调侧 |
|---|---|
| JSON为每个支持组合声明唯一的`bin_filename`和`simplified_key`。 | `GenSimplifiedKey`对相同调用组合生成相同的`simplified_key`。 |
| 输入输出dtype、format、shape和属性组合与原型、核函数（Kernel）实现一致。 | 读取所有会影响binary选择的输入、输出、shape和属性，不遗漏组合维度。 |
| 编译产物将完整Key、`.o`路径和`.json`路径写入`binary_info_config.json`。 | 不拼接完整Key前缀，不加载文件，不选择binary。 |

例如，如果binary由两个输入的dtype共同决定，回调必须同时读取两个输入；只根据第一个输入生成Key会使不同组合落入同一个片段，导致查找错误或找不到binary。

## 失败处理

以下情况会导致本次调用无法通过自定义simplified key匹配到预编译binary：

- 未注册`GenSimplifiedKey`回调，或回调为空。
- 回调返回失败，例如输入描述为空或遇到未支持的dtype。
- 回调生成的片段为空、超长或与JSON中的`simplified_key`不一致。
- JSON未编译出该调用组合，或部署时缺少对应的`.o`、`.json`和`binary_info_config.json`。

因此，新增一种binary组合时必须同步完成三项工作：新增JSON项、更新回调分支、重新编译并部署算子包。只改其中一端都会破坏编译产物与运行时选择规则的一致性。

## 与两段式调用的关系

simplified key选择属于`aclnnXxxGetWorkspaceSize`阶段的准备工作。参数缓存命中时，运行时可以复用已保存的binary和Tiling信息；缓存未命中时，才执行上述回调、查找和Tiling流程。`aclnnXxx`执行阶段不重新生成Key，而是使用executor中已准备好的binary和Tiling结果。

## 相关文档

- [运行时加载机制](./runtime_loading_mechanism.md)：了解单算子两段式调用、缓存和通用binary匹配行为。
- [GenSimplifiedKey回调实现](../design_and_implementation/gen_simplified_key.md)：了解回调签名、注册方法和编写要求。
- [simplified key二进制配置JSON](../compilation_and_deployment/simplified_key_json_configuration.md)：了解完整Key和binary路径如何写入编译产物。
