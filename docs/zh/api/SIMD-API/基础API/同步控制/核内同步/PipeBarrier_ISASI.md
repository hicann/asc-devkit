# PipeBarrier\(ISASI\)<a name="ZH-CN_TOPIC_0000001835308877"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：支持
<!-- end id7 -->


## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_block_sync_intf.h"`。

如下图1所示，完成同一流水线内的同步控制，用于在同一流水线内部约束执行顺序。其作用是，保证前序指令中所有数据的读写工作全部完成，后序指令才能执行。

**图1**  PipeBarrier接口功能示意图<a name="zh-cn_topic_0000002549838971_fig7681194617307"></a>  
![](../../../../figures/pipebarrier_function_diagram.png "PipeBarrier_接口功能示意图")

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <pipe_t pipe>
__aicore__ inline void PipeBarrier()
```

## 参数说明<a name="section622mcpsimp"></a>

**表1** 模板参数说明

| 参数名 | 描述 |
| -------- | -------- |
| pipe | 模板参数，表示阻塞的流水类别。<br>支持的流水参考[硬件流水类型](核内同步能力概述.md#zh-cn_topic_0000002542725361_section1272612276459)，其中不支持PIPE_S。<br>如果不关注流水类别，希望阻塞所有流水，可以传入PIPE_ALL。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- Scalar流水之间的同步由硬件自动保证，PipeBarrier接口不支持PIPE\_S单流水的同步。

- `PipeBarrier<PIPE_ALL>()`会等待所有流水线中所有先前提交的接口完成，这会对性能产生影响。若仅阻塞单条流水线即可解决问题，应避免随意调用`PipeBarrier<PIPE_ALL>()`。

- PIPE\_MTE2/PIPE\_MTE3在搬运地址有重叠的情况下需要开发者插入同步。例如，当需要执行多个DataCopy指令，且DataCopy的目的地址存在重叠时，需要通过调用来插入PipeBarrier同步指令，保证多个DataCopy指令的串行化，防止出现异常数据。如下图左侧示意图，执行两个DataCopy指令，搬运的目的GM地址存在重叠，两条搬运指令之间需要通过调用`PipeBarrier<PIPE_MTE3>()`添加MTE3搬出流水的同步；如下图右侧示意图所示，搬运的目的地址UB存在重叠，两条搬运指令之间需要调用`PipeBarrier<PIPE_MTE2>()`添加MTE2搬入流水的同步。

![](../../../../figures/datacopy_address_overlap_pipebarrier.png "DataCopy_地址重叠_PipeBarrier_同步示意图")

## 调用示例<a name="section837496171220"></a>

如下示例，Mul指令的输入dst0Local是Add指令的输出，两个矢量运算指令产生依赖，需要插入PipeBarrier保证两条指令的执行顺序。

注：仅作为示例参考，开启自动同步（Kernel直调算子工程和自定义算子开发工程已默认开启）的情况下，编译器自动插入PIPE\_V同步，无需开发者手动插入。

**图2**  Mul指令和Add指令是串行关系，必须等待Add指令执行完成后，才能执行Mul指令。<a name="fig1359216580459"></a>  
![](../../../../figures/mul_add_pipebarrier_sync_diagram.png "Mul_Add_指令_串行依赖_PipeBarrier_同步示意图")

```cpp
AscendC::LocalTensor<half> src0Local;
AscendC::LocalTensor<half> src1Local;
AscendC::LocalTensor<half> src2Local;
AscendC::LocalTensor<half> dst0Local;
AscendC::LocalTensor<half> dst1Local;

AscendC::Add(dst0Local, src0Local, src1Local, 512);
AscendC::PipeBarrier<PIPE_V>();
AscendC::Mul(dst1Local, dst0Local, src2Local, 512);
```
