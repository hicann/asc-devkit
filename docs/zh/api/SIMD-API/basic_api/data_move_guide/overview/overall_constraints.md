# 总体约束说明<a name="ZH-CN_TOPIC_0000002565840545"></a>

所有数据搬运接口共享若干通用约束规则，在进行任何搬运操作前都应确保这些约束被满足，否则可能导致未定义行为或数据异常。

## 地址对齐约束<a name="section136711917101710"></a>

AI Core上的各类存储单元有不同的地址对齐要求。Ascend C API操作数的起始地址必须满足对应存储单元的对齐约束。DataBlock（32字节）是搬运操作的基本寻址粒度，大部分搬运接口的blockLen参数均以DataBlock为单位计算。各存储单元的对齐要求如下表所示。

**表 1** **各存储单元地址对齐要求**<a name="table16278354141117"></a>

| 存储单元 | 对齐要求 | 说明 |
|----------|----------|------|
| Global Memory | 1字节对齐 | 最宽松的对齐要求，任意字节地址均可 |
| UB | 32字节对齐 | 与DataBlock大小一致，地址必须为32的整数倍 |
| L1 Buffer | 32字节对齐 | 同UB，地址必须为32的整数倍 |
| L0A Buffer/L0B Buffer | 512字节对齐 | 矩阵计算输入的高对齐要求 |
| L0C Buffer | 64字节对齐 | 矩阵计算结果缓冲区 |
| BiasTable Buffer | 64字节对齐 | 偏置数据缓冲区 |
| Fixpipe Buffer | 128字节对齐 | 量化参数暂存区 |

> [!NOTE]说明
  > - 需要注意的是：如果某个具体接口的约束说明中明确指定了与上表不同的对齐要求，则应以该接口自身的说明为准。
  > - 对于不满足对齐要求的场景，应使用DataCopyPad接口的非对齐搬运能力。

## 多指令同步<a name="section711085812111"></a>

如果需要执行多个DataCopy指令，且DataCopy的目的地址存在重叠，需要调用[PipeBarrier](../../sync_control/intra_core_sync/PipeBarrier_ISASI.md)插入同步指令，保证多个DataCopy指令的串行化，防止出现异常数据。如下图左侧示意图，执行两个DataCopy指令，搬运的目的GM地址存在重叠，两条搬运指令之间需要通过调用PipeBarrier<PIPE\_MTE3>\(\)进行MTE3流水的同步；如下图右侧示意图所示，搬运的目的地址Unified Buffer（UB）存在重叠，两条搬运指令之间需要调用PipeBarrier<PIPE\_MTE2>\(\)进行MTE2流水的同步。多指令同步的核心代码示例如下：

```cpp
AscendC::DataCopy(src2Local, src2Global, srcDataSize);
// 两条搬运指令之间需要调用PipeBarrier<PIPE_MTE2>()添加MTE2搬入流水的同步。
AscendC::PipeBarrier<PIPE_MTE2>();
AscendC::DataCopy(src1Local, src1Global, srcDataSize);
// 输入src1和src2在UB上分配的地址区间有重叠。
// 为了保证src2的数据不会覆盖src1在重叠区间的值，必须保证先搬入src2，后搬入src1。
```

**图 1**  DataCopy的目的地址存在重叠<a name="fig333145263318"></a>  

![](../../../../figures/datacopy_address_overlap_sync_diagram.png "DataCopy的目的地址存在重叠")
