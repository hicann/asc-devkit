# 算子功能设计

本文属于基础内容，介绍在编写自定义算子代码前需要完成的功能设计，包括将计算语义转换为工程实现，以及明确需要编写的工程文件。

本文以AddCustom算子作为示例。AddCustom的计算语义如下：

```text
z = x + y
```

本文只给出功能设计阶段需要明确的内容。具体接口和代码写法会在后续章节展开。

## 前置理解：aclnn工程化算子开发方式由哪几部分组成

一个aclnn自定义算子工程通常需要完成三类代码。

第一类代码：**算子原型定义**，声明算子的接口信息，包括输入、输出和属性，以及支持的dtype、format。

第二类代码：**Host侧Tiling实现**，在Kernel执行前准备运行参数。Kernel在AI Core上运行前，需要知道本次输入一共有多少数据、如何切核并启动多少个Block、每个Block内部如何继续切分、是否需要额外workspace，以及是否需要设置调度模式等launch配置。

第三类代码：**Kernel侧算子实现**，在AI Core上执行的实际计算。Kernel侧根据Tiling传入的参数，从GM搬运数据到UB，在UB上完成计算，再把结果写回GM。

本文的功能设计，就是在写这三类代码前，先确定每一类代码需要表达什么。

## 围绕算子原型做功能设计

算子原型设计从算子语义开始。算子语义描述算子接收什么数据、执行什么计算、输出什么结果。这个步骤决定OpDef中的输入输出声明，也决定Tiling和Kernel需要围绕哪些数据设计。

AddCustom是逐元素加法算子。输入`x`和`y`在相同位置上的元素相加，结果写入输出`z`。每个元素的计算互相独立，适合按连续数据段切分到多个Block上并行处理。

```text
z[i] = x[i] + y[i]
```

根据这条语义，可以先确定算子对外暴露的原型边界。AddCustom对外暴露两个输入`x`、`y`和一个输出`z`。这三个Tensor已经可以完整表达`z = x + y`的计算语义，每次调用都需要提供，因此在原型边界上都属于必选参数。AddCustom没有额外属性，调用方只需要准备两个输入Tensor和一个输出Tensor。

如果算子带有标量、维度、阈值、开关等配置项，需要在功能设计阶段确定它们的承载方式。值较小且用于描述算子行为的配置项适合作为属性；需要按Tensor传入、参与动态图数据依赖或数据量较大的内容适合作为输入。属性识别和传递的完整示例请参考[通过TilingData传递属性信息](./tiling_data_attributes.md)。

除此之外，还需要确定算子支持什么场景：

- 数据类型和数据格式。
- 确定性：算子在相同输入下是否产生相同输出。

将前两步确定的输入输出、dtype/format列表等合并，即得到完整的算子原型定义。完整的算子原型定义编写方法和API说明请参考[算子原型定义](./operator_prototype_definition.md)。

完成算子原型设计后，功能设计可以继续拆解Host侧Tiling需要准备的运行参数，以及Kernel侧如何完成实际计算。

## 围绕Host侧Tiling做功能设计

Host侧Tiling设计的重点，是确定Kernel启动前需要准备哪些运行参数。Host侧Tiling负责把输入shape、属性等信息转换成Kernel运行时需要的参数，包括切核策略、Block内切分、workspace大小、TilingKey和必要的调度模式等。AddCustom的Kernel只需要知道总数据量和总数据的分块数量，因此TilingData设计为两个字段。

第一个字段是`totalLength`，表示本次计算的总元素数量。Kernel侧会基于这个值计算每个Block处理的数据量。第二个字段是`tileNum`，表示样例中总数据的分块数量。Kernel通过`totalLength / tileNum`计算单次处理的数据量，并与`numBlocks`共同确定每个Block的循环次数。

编写Tiling函数时，需要把这些设计结论转换成Host侧计算步骤。AddCustom的Tiling函数需要读取输入`x`的shape并计算`totalLength`，设置Kernel启动使用的`numBlocks`，写入`tileNum`，并给出workspace大小；更复杂的算子还可能需要根据场景设置TilingKey或调度模式。

TilingData结构体、Tiling函数写法、workspace设置、`numBlocks`配置和相关launch参数会在[Host侧Tiling实现](./host_tiling_implementation.md)章节详细展开。

## 围绕Kernel侧实现做功能设计

Kernel侧实现设计的重点，是确定AI Core上用什么计算API完成算子语义。AddCustom的语义是逐元素加法，对应到Ascend C基础API就是`Add`。

```text
z[i] = x[i] + y[i]
        ↓
AscendC::Add(zLocal, xLocal, yLocal, length)
```

这里的`xLocal`和`yLocal`表示已经搬入UB的输入数据，`zLocal`表示UB中的输出数据。`length`表示本轮参与计算的元素个数，由Kernel侧结合Host侧传入的Tiling信息确定。

从数据流上看，AddCustom采用基础的搬运、计算、写回流程：

```text
GM(x, y) -> UB(xLocal, yLocal) -> Add -> UB(zLocal) -> GM(z)
```

功能设计阶段只需要明确这条计算映射和基础数据流。Kernel入口参数、TilingData读取等代码组织细节，会在[Kernel侧算子实现](./kernel_operator_implementation.md)章节展开。

## 针对不同场景扩展Kernel实现

完成基础Kernel设计后，还需要判断当前实现能覆盖哪些场景。后续若需要支持更多数据类型、更大shape范围或运行时配置时，需要在基础实现上增加扩展策略。功能设计阶段需要先判断扩展维度和触发条件，后续再通过[多分支策略](./multi_branch_strategy.md)说明TilingKey、Kernel模板或差异化编译配置的具体写法。

### 按数据类型扩展Kernel实现

当不同dtype的计算路径、精度要求或中间缓存不同，需要按数据类型评估Kernel实现范围。以AddCustom为例，如果后续同时支持`float32`和`float16`，需要分别评估指令支持情况、数据搬运对齐、UB占用和Tile大小。

```text
float32: 元素占用空间较大，相同UB容量下单次可处理的元素数较少
float16: 元素占用空间较小，但仍需满足数据搬运和计算API的对齐要求
```

归约、累加或乘加等存在较长中间计算链的算子，可能需要使用`float32`保存中间结果以改善精度。但如果最终输出仍为`float16`，最终结果仍然必须处于`float16`的可表示范围内；升精度不能扩大最终输出的数据范围。

这种设计的重点，是在功能设计阶段提前识别dtype差异是否会改变Kernel计算路径。后续如果需要根据dtype选择不同路径，可以在Host侧Tiling中设置对应标识，再由Kernel侧选择匹配实现。

### 按shape大小扩展Kernel实现

当数据量差异较大时，不同shape适合的切分策略也可能不同。小shape可能一次搬入UB完成计算，大shape通常需要按Block和Tile分块循环处理。

以AddCustom为例，可以在功能设计阶段先给出shape分界思路：总元素数较小时使用较少Tile，减少循环开销；总元素数较大时增加Tile数量，让每个Block按多轮搬运、计算和写回处理数据。具体阈值需要结合UB容量、对齐要求、尾块处理方式和性能验证结果确定。配套基础样例的输入shape为`[8, 2048]`，固定使用8个Block，`tileNum`取8。此时每个Block处理2048个元素，单次也处理2048个元素，因此每个Block只循环1次。该样例只覆盖固定规格；扩展shape范围时需要重新计算切分参数，并补充不能整除时的尾核、尾Tile处理。

### 按运行时配置扩展Kernel实现

运行时配置也可能影响Kernel实现路径。典型场景是确定性配置：相同输入下要求输出完全稳定时，Kernel需要固定计算顺序；追求性能时，某些归约类算子可能采用更灵活的并行顺序。

AddCustom是逐元素加法，元素之间没有依赖关系，基础实现采用固定的数据分片和计算顺序，属于确定性实现。对于归约、排序等存在跨元素关系的算子，需要在功能设计阶段记录确定性要求，并判断是否需要准备不同Kernel路径。

### 分支策略选择总结

关于TilingKey和Kernel模板的选择准则，请参考[多分支策略](./multi_branch_strategy.md)。

## 下一步

  - [算子原型定义](./operator_prototype_definition.md)：输入、输出、属性、dtype/format。
  - [Host侧Tiling实现](./host_tiling_implementation.md)：TilingData定义和Tiling函数编写。
  - [Kernel侧算子实现](./kernel_operator_implementation.md)：数据搬运、计算接口和Kernel编程流程。
  - [多分支策略](./multi_branch_strategy.md)：TilingKey和不同Kernel分支的选择方法。
  - [通过TilingData传递属性信息](./tiling_data_attributes.md)：属性读取和传递介绍。
