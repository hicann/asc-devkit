# Mask接口汇总

<!-- npu="A3,910b" id3 -->
以下内容针对如下型号生效：
<!-- npu="A3" id1 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品
<!-- end id1 -->
<!-- npu="910b" id2 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品
<!-- end id2 -->

本节汇总了各接口中模板参数isSetMask的支持情况。

针对Tensor前n个数计算和Tensor高维切分计算两种接口，Mask设置方式分为以下情形：

- isSetMask参数情况：

    - 模板参数有isSetMask，支持[接口外设置Mask](../SIMD-API/基础API/Memory矢量计算/SIMD计算说明/掩码.md#接口外设置Mask)。

    - 模板参数无isSetMask，不支持接口外设置Mask。

    - 模板参数有isSetMask，该参数不起作用。

- Mask设置方式：

    - **接口内已设置Mask**：接口内部调用[SetMaskCount](../SIMD-API/基础API/Memory矢量计算/掩码操作/SetMaskCount.md)、[SetVectorMask](../SIMD-API/基础API/Memory矢量计算/掩码操作/SetVectorMask.md)、[SetMaskNorm](../SIMD-API/基础API/Memory矢量计算/掩码操作/SetMaskNorm.md)、[ResetMask](../SIMD-API/基础API/Memory矢量计算/掩码操作/ResetMask.md)。

    - **接口内通过模板参数控制**：接口内通过isSetMask模板参数控制如何设置Mask。isSetMask为true时表示在接口内设置Mask；isSetMask为false时表示在接口外设置Mask，开发者需使用[SetVectorMask](../SIMD-API/基础API/Memory矢量计算/掩码操作/SetVectorMask.md)接口设置Mask值。

    - **接口不支持Mask或者无需设置Mask**。

更多Mask信息可参考[基于全局掩码复用的计算性能优化](../../guide/算子实践参考/SIMD算子性能优化/矢量计算/基于全局掩码复用的计算性能优化.md)。

注：表中的`-`表示不存在该类型接口。

**表 1**  各接口模板参数isSetMask支持情况汇总表

| 接口类别 | 接口名称 | Tensor前n个数计算 | Tensor高维切分计算 | 
| --- | --- | --- | --- |
| 基础算术 | [Add](../SIMD-API/基础API/Memory矢量计算/基础算术/Add.md)、[Exp](../SIMD-API/基础API/Memory矢量计算/基础算术/Exp.md)、[Ln](../SIMD-API/基础API/Memory矢量计算/基础算术/Ln.md)、[Abs](../SIMD-API/基础API/Memory矢量计算/基础算术/Abs.md)、[Reciprocal](../SIMD-API/基础API/Memory矢量计算/基础算术/Reciprocal.md)、[Sqrt](../SIMD-API/基础API/Memory矢量计算/基础算术/Sqrt.md)、[Rsqrt](../SIMD-API/基础API/Memory矢量计算/基础算术/Rsqrt.md)、[Relu](../SIMD-API/基础API/Memory矢量计算/基础算术/Relu.md)、[Sub](../SIMD-API/基础API/Memory矢量计算/基础算术/Sub.md)、[Mul](../SIMD-API/基础API/Memory矢量计算/基础算术/Mul.md)、[Div](../SIMD-API/基础API/Memory矢量计算/基础算术/Div.md)、[Max](../SIMD-API/基础API/Memory矢量计算/基础算术/Max.md)、[Min](../SIMD-API/基础API/Memory矢量计算/基础算术/Min.md) | &bull; 无isSetMask。<br>&bull; 接口内已设置Mask。 | &bull; 有isSetMask。<br>&bull; 接口内通过模板参数控制。 |
| 基础算术 | [BilinearInterpolation](../SIMD-API/基础API/Memory矢量计算/基础算术/BilinearInterpolation(ISASI).md) | - | &bull; 无isSetMask。<br>&bull; 接口内已设置Mask。 |
| 基础算术 | [Adds](../SIMD-API/基础API/Memory矢量计算/基础算术/Adds.md)、[Muls](../SIMD-API/基础API/Memory矢量计算/基础算术/Muls.md)、[Maxs](../SIMD-API/基础API/Memory矢量计算/基础算术/Maxs.md)、[Mins](../SIMD-API/基础API/Memory矢量计算/基础算术/Mins.md)、[LeakyRelu](../SIMD-API/基础API/Memory矢量计算/基础算术/LeakyRelu.md) | &bull; 有isSetMask。<br>&bull; 接口内已设置Mask。 | &bull; 有isSetMask。<br>&bull; 接口内通过模板参数控制。 |
| 逻辑计算 | [Not](../SIMD-API/基础API/Memory矢量计算/逻辑计算/Not.md)、[And](../SIMD-API/基础API/Memory矢量计算/逻辑计算/And.md)、[Or](../SIMD-API/基础API/Memory矢量计算/逻辑计算/Or.md) | &bull; 无isSetMask。<br>&bull; 接口内已设置Mask。 | &bull; 有isSetMask。<br>&bull; 接口内通过模板参数控制。 |
| 逻辑计算 | [ShiftLeft](../SIMD-API/基础API/Memory矢量计算/逻辑计算/ShiftLeft.md)、[ShiftRight](../SIMD-API/基础API/Memory矢量计算/逻辑计算/ShiftRight.md) | &bull; 有isSetMask。<br>&bull; 接口内通过模板参数控制。 | &bull; 有isSetMask。<br>&bull; 接口内通过模板参数控制。 |
| 复合计算 | [Axpy](../SIMD-API/基础API/Memory矢量计算/复合计算/Axpy.md)、[MulAddDst](../SIMD-API/基础API/Memory矢量计算/复合计算/MulAddDst.md)、[MulCast](../SIMD-API/基础API/Memory矢量计算/复合计算/MulCast.md)、[AddRelu](../SIMD-API/基础API/Memory矢量计算/复合计算/AddRelu.md)、[AddReluCast](../SIMD-API/基础API/Memory矢量计算/复合计算/AddReluCast.md)、[SubRelu](../SIMD-API/基础API/Memory矢量计算/复合计算/SubRelu.md)、[SubReluCast](../SIMD-API/基础API/Memory矢量计算/复合计算/SubReluCast.md)、[FusedMulAdd](../SIMD-API/基础API/Memory矢量计算/复合计算/FusedMulAdd.md)、[MulAddRelu](../SIMD-API/基础API/Memory矢量计算/复合计算/MulAddRelu.md)、[CastDequant](../SIMD-API/基础API/Memory矢量计算/复合计算/CastDequant.md)、[AddDeqRelu](../SIMD-API/基础API/Memory矢量计算/复合计算/AddDeqRelu.md) | &bull; 无isSetMask。<br>&bull; 接口内已设置Mask。 | &bull; 有isSetMask。<br>&bull; 接口内通过模板参数控制。 |
| 比较与选择 | [Compare](../SIMD-API/基础API/Memory矢量计算/比较与选择/Compare.md) | &bull; 无isSetMask。<br>&bull; 接口不支持Mask。 | &bull; 有isSetMask，不起作用。<br>&bull; 接口无需设置Mask。 |
| 比较与选择 | [Compare（结果存入寄存器）](../SIMD-API/基础API/Memory矢量计算/比较与选择/Compare（结果存入寄存器）.md) | - | &bull; 有isSetMask。<br>&bull; 接口内通过模板参数控制。 |
| 比较与选择 | [Compares](../SIMD-API/基础API/Memory矢量计算/比较与选择/Compares.md) | &bull; 无isSetMask。<br>&bull; 接口不支持Mask。 | &bull; 有isSetMask。<br>&bull; 接口无需设置Mask。 |
| 比较与选择 | [Select](../SIMD-API/基础API/Memory矢量计算/比较与选择/Select.md) | &bull; 无isSetMask。<br>&bull; 接口内已设置Mask。 | &bull; 有isSetMask，不起作用。<br>&bull; 如需使用在接口外部设置Mask的功能，可以调用[不传入mask参数的接口](../SIMD-API/基础API/Memory矢量计算/比较与选择/Select.md#函数原型)来实现。 |
| 比较与选择 | [GatherMask](../SIMD-API/基础API/Memory矢量计算/比较与选择/GatherMask.md) | - | &bull; 无isSetMask。<br>&bull; 接口内已设置Mask。 |
| 类型转换 | [Cast](../SIMD-API/基础API/Memory矢量计算/类型转换/Cast.md) | &bull; 无isSetMask。<br>&bull; 接口内已设置Mask。 | &bull; 有isSetMask。<br>&bull; 接口内通过模板参数控制。 |
| 归约计算 | [ReduceDataBlock](../SIMD-API/基础API/Memory矢量计算/归约计算/ReduceDataBlock.md)、[ReduceRepeat](../SIMD-API/基础API/Memory矢量计算/归约计算/ReduceRepeat.md)、[ReducePairElem](../SIMD-API/基础API/Memory矢量计算/归约计算/ReducePairElem.md) | - | &bull; 有isSetMask。<br>&bull; 接口内通过模板参数控制。 |
| 归约计算 | [ReduceSum](../SIMD-API/基础API/Memory矢量计算/归约计算/ReduceSum.md) | &bull; 有isSetMask。<br>&bull; 接口内通过模板参数控制。 | &bull; 无isSetMask。<br>&bull; 接口内已设置Mask。 |
| 归约计算 | [ReduceMax](../SIMD-API/基础API/Memory矢量计算/归约计算/ReduceMax.md)、[ReduceMin](../SIMD-API/基础API/Memory矢量计算/归约计算/ReduceMin.md) | &bull; 无isSetMask。<br>&bull; 接口内已设置Mask。 | &bull; 无isSetMask。<br>&bull; 接口内已设置Mask。 |
| 数据排布转换 | [Transpose](../SIMD-API/基础API/Memory矢量计算/数据排布转换/Transpose.md)、[TransDataTo5HD](../SIMD-API/基础API/Memory矢量计算/数据排布转换/TransDataTo5HD.md) | &bull; 无isSetMask。<br>&bull; 接口不支持Mask。 | &bull; 无isSetMask。<br>&bull; 接口不支持Mask。 |
| 数据填充 | [Brcb](../SIMD-API/基础API/Memory矢量计算/数据填充/Brcb.md) | - | &bull; 无isSetMask。<br>&bull; 接口不支持Mask。 |
| 数据填充 | [Duplicate](../SIMD-API/基础API/Memory矢量计算/数据填充/Duplicate.md) | &bull; 无isSetMask。<br>&bull; 接口内已设置Mask。 | &bull; 有isSetMask。<br>&bull; 接口内通过模板参数控制。 |
| 数据填充 | [CreateVecIndex](../SIMD-API/基础API/Memory矢量计算/数据填充/CreateVecIndex.md) | &bull; 无isSetMask。<br>&bull; 接口不支持Mask。 | &bull; 无isSetMask。<br>&bull; 接口不支持Mask。 |
| 排序组合（ISASI） | [Sort32](../SIMD-API/基础API/Memory矢量计算/排序组合（ISASI）/Sort32.md)、[MrgSort](../SIMD-API/基础API/Memory矢量计算/排序组合（ISASI）/MrgSort.md) | - | &bull; 无isSetMask。<br>&bull; 接口不支持Mask。 |
| 离散与聚合 | [Gather](../SIMD-API/基础API/Memory矢量计算/离散与聚合/Gather.md)、[Gatherb](../SIMD-API/基础API/Memory矢量计算/离散与聚合/Gatherb(ISASI).md) | &bull; 无isSetMask。<br>&bull; 接口不支持Mask。 | &bull; 无isSetMask。<br>&bull; 接口不支持Mask。 |
| 掩码操作 | [SetMaskCount](../SIMD-API/基础API/Memory矢量计算/掩码操作/SetMaskCount.md)、[SetMaskNorm](../SIMD-API/基础API/Memory矢量计算/掩码操作/SetMaskNorm.md)、[SetVectorMask](../SIMD-API/基础API/Memory矢量计算/掩码操作/SetVectorMask.md)、[ResetMask](../SIMD-API/基础API/Memory矢量计算/掩码操作/ResetMask.md) | - | - |

<!-- end id3 -->
