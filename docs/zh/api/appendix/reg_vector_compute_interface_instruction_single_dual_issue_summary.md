# Reg矢量计算接口指令单双发汇总<a name="reg-vector-dual-issue"></a>

本节汇总介绍了Reg矢量计算接口指令单双发的情况，指令单发指1个Cycle能够执行一条指令，指令双发指1个Cycle能够执行两条指令。
<br>**针对寄存器：1个Cycle只能写两个Reg寄存器；针对UB：1个Cycle只能读两个VL（一个VL为256B），或者1个Cycle读一个VL和写一个VL。**

本节内容不包含软仿类接口，仅针对如下型号生效：

Ascend 950PR/Ascend 950DT

**表 1**  接口指令双发汇总<a id="table-dual-issue"></a>

| 接口类型 | 硬件指令名称 | 对应接口名称 |
| --- | --- | --- |
| 寄存器数据类型 | PSET | CreateMask |
| 寄存器数据类型 | PLT | UpdateMask |
| Reg数据搬入 | VLDS/VLD | 连续对齐搬入（LoadAlign）：<br>单搬入模式（除了DIST_DS_B8、DIST_DS_B16分布模式） |
| Reg数据搬入 | VLDAS/VLDA | 连续非对齐搬入初始化（LoadUnAlignPre） |
| Reg数据搬入 | VLDUS/VLDU | 连续非对齐搬入（LoadUnAlign） |
| Reg数据搬入 | PLDS/PLD | MaskReg搬入（LoadAlign） |
| Reg数据搬入 | VMOV | Move（Reg数据搬入） |
| MaskReg计算 | PMOV | Move（MaskReg计算）|
| MaskReg计算 | PSEL | Select |
| MaskReg计算 | MOVP | MoveMask |
| 基础算术 | VABS | Abs |
| 基础算术 | VEXP | Exp |
| 基础算术 | VSQRT | Sqrt |
| 基础算术 | VLN | Ln |
| 基础算术 | VLN | Log |
| 基础算术 | VNEG | Neg |
| 基础算术 | VRELU | Relu |
| 基础算术 | VPRELU | Prelu |
| 基础算术 | VLRELU | LeakyRelu |
| 基础算术 | VADD/VADDC | Add |
| 基础算术 | VADDCS | AddC |
| 基础算术 | VDIV | Div |
| 基础算术 | VMAX | Max |
| 基础算术 | VMIN | Min |
| 基础算术 | VMUL | Mul |
| 基础算术 | VMULL | Mull |
| 基础算术 | VSUB/VSUBC | Sub |
| 基础算术 | VSUBCS | SubC |
| 基础算术 | VADDS | Adds |
| 基础算术 | VMAXS | Maxs |
| 基础算术 | VMINS | Mins |
| 基础算术 | VMULS | Muls |
| 逻辑计算 | VNOT/PNOT | Not |
| 逻辑计算 | VAND/PAND | And |
| 逻辑计算 | VOR/POR | Or |
| 逻辑计算 | VXOR/PXOR | Xor |
| 逻辑计算 | VSHL | ShiftLeft |
| 逻辑计算 | VSHR | ShiftRight |
| 逻辑计算 | VSHLS | ShiftLefts |
| 逻辑计算 | VSHRS | ShiftRights |
| 复合计算 | VABSDIF | AbsSub |
| 复合计算 | VAXPY | Axpy |
| 复合计算 | VEXPDIF | ExpSub |
| 复合计算 | VMADD | MulDstAdd |
| 复合计算 | VMULA | MulAddDst |
| 复合计算 | VMULSCVT | MulsCast |
| 比较与选择 | VCMP | Compare |
| 比较与选择 | VCMPS | Compares |
| 比较与选择 | VSEL/PSEL | Select |
| 类型转换 | VCVTFI/VCVTFF/VCVTFF2/VCVTIF/VCVTII | Cast |
| 类型转换 | VTRC | Truncate |
| 数据填充 | VBR | Duplicate |
| 索引操作 | VCI | Arange |

**表 2**  接口指令单发汇总<a id="table-single-issue"></a>

| 接口类型 | 硬件指令名称 | 对应接口名称 |
| --- | --- | --- |
| Reg数据搬入 | VSLDB | 非连续对齐搬入（LoadAlign） |
| Reg数据搬入 | VLDS/VLD | 连续对齐搬入（LoadAlign）：<br>双搬入模式和单搬入模式的DIST_DS_B8、DIST_DS_B16分布模式 |
| Reg数据搬入 | MOVVP | MaskGenWithRegTensor |
| Reg数据搬入 | VGATHER2 | Gather |
| Reg数据搬入 | VGATHERB | GatherB |
| Reg数据搬出 | VSSTB | 非连续对齐搬出（StoreAlign）|
| Reg数据搬出 | VSTS/VST | 连续对齐搬出（StoreAlign） |
| Reg数据搬出 | VSTUS/VSTU/VSTUR | 连续非对齐搬出（StoreUnAlign） |
| Reg数据搬出 | VSTAS/VSTA/VSTAR | 连续非对齐搬出后处理（StoreUnAlignPost） |
| Reg数据搬出 | PSTS/PST | MaskReg搬出（StoreAlign） |
| Reg数据搬出 | PSTU | MaskReg非对齐搬出（StoreUnAlign） |
| Reg数据搬出 | VSCATTER | Scatter |
| MaskReg计算 | PINTLV | Interleave |
| MaskReg计算 | PDINTLV | DeInterleave |
| MaskReg计算 | PPACK | Pack |
| MaskReg计算 | PUNPACK | UnPack |
| 归约计算 | VCADD | Reduce（ReduceType::SUM） |
| 归约计算 | VCMAX | Reduce（ReduceType::MAX） |
| 归约计算 | VCMIN | Reduce（ReduceType::MIN） |
| 归约计算 | VCGADD | ReduceDataBlock（ReduceType::SUM） |
| 归约计算 | VCCGMAX | ReduceDataBlock（ReduceType::MAX） |
| 归约计算 | VCGMIN | ReduceDataBlock（ReduceType::MIN） |
| 归约计算 | VCPADD | PairReduceElem |
| 比较与选择 | VSQZ | Squeeze |
| 离散操作 | VSELR | Gather |
| 数据重排 | VINTLV | Interleave |
| 数据重排 | VDINTLV | DeInterleave |
| 数据压缩 | VUSQZ | Unsqueeze |
| 数据压缩 | VPACK | Pack |
| 数据压缩 | VUNPACK | UnPack |
| 直方图计算 | DHISTv2 | Histograms |