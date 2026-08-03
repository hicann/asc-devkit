# LoadData（2D矩阵搬运）

## 产品支持情况

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
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->

## 功能说明

头文件路径为：`"basic_api/kernel_operator_mm_intf.h"`。

负责完成普通矩阵计算所需的2D格式数据的搬运，以大小为512字节的数据分形为单位进行搬运，支持如下数据通路的搬运：

- 支持GM->L0A Buffer、GM->L0B Buffer、L1 Buffer->L0A Buffer、L1 Buffer->L0B Buffer。
<!-- npu="950" id10 -->
- 特别针对Ascend 950PR/Ascend 950DT：仅支持L1 Buffer->L0A Buffer、L1 Buffer->L0B Buffer。
<!-- end id10 -->

对于不同的数据类型，每个数据分形对应的矩阵如下：

- 对于b8数据类型，每个数据分形在L0A Buffer中为一个16×32的矩阵，在L0B Buffer中为一个32×16的矩阵。
- 对于b16数据类型，每个数据分形为一个16×16的矩阵。
- 对于b32数据类型，每个数据分形在L0A Buffer中为一个16×8的矩阵，在L0B Buffer中为一个8×16的矩阵。

实现原理可参考伪代码：[LoadData（2D矩阵搬运）伪代码](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0/scripts/load2d.py)。

## 函数原型

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParams& loadDataParams)

template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParams& loadDataParams)
```

<!-- npu="950" id11 -->
针对Ascend 950PR/Ascend 950DT：

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParams& loadDataParams)
```
<!-- end id11 -->

## 参数说明

**表1** 通用参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ----------- | ------ |
| dst | 输出 | 目的操作数，类型为LocalTensor。<br>分形约束参考[矩阵计算输入搬运约束](矩阵计算输入搬运约束.md)。<br>起始地址对齐约束参考[对齐约束](矩阵计算输入搬运约束.md)。<br>数据类型和src的数据类型保持一致。<br>支持的物理存储位置为L0A Buffer（TPosition: A2）/L0B Buffer（TPosition: B2）。<br>数据连续排列顺序由目的操作数所在TPosition决定：A2对应ZZ格式/NZ格式，分形大小为16×(32字节/sizeof(T))；B2对应ZN格式，分形大小为(32字节/sizeof(T))×16。 |
| src | 输入 | 源操作数，类型为LocalTensor或GlobalTensor。<br>分形约束参考[矩阵计算输入搬运约束](矩阵计算输入搬运约束.md)。<br>起始地址对齐约束参考[对齐约束](矩阵计算输入搬运约束.md)。<br>数据类型和dst的数据类型保持一致。<br>位于L1 Buffer（TPosition: A1/B1）时无格式要求，一般情况下为NZ格式，分形大小为16×(32字节/sizeof(T))。 |
| loadDataParams | 输入 | LoadData参数结构体，类型为LoadData2DParams，具体参考[表2](#table_load2d_params)。 |

**表2** LoadData2DParams结构体内参数说明<a id="table_load2d_params"></a>

| 参数名称 | 含义 |
| ---------- | ------ |
| startIndex | 分形矩阵ID，说明搬运起始位置为源操作数中第几个分形（0为源操作数中第1个分形矩阵）。取值范围：startIndex∈[0, 65535]。单位：512字节。默认为0。<br>特性细节可参考：[设置搬运起始位置](#设置搬运起始位置)。 |
| repeatTimes | 迭代次数，每个迭代可以处理512字节数据。取值范围：repeatTimes∈[0, 255]。<br>**注：repeatTimes=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| srcStride | 相邻迭代间，源操作数前一个分形与后一个分形起始地址的间隔，单位：512字节。取值范围：srcStride∈[0, 65535]。默认为0。<br>特性细节可参考：[非连续搬入](#非连续搬入)。<br>**注：srcStride=0表示在连续的重复执行周期之间，重复获取相同的分形矩阵。** |
| sid | 此参数用户无需关注，设置为0即可。<br>**注：预留的扩展参数，当前因后续架构升级，该参数已废弃，不对其进行业务处理。** |
| dstGap | 相邻迭代间，目的操作数前一个分形结束地址与后一个分形起始地址的间隔，单位：512字节。取值范围：dstGap∈[0, 65535]。默认为0。<br>特性细节可参考：[非连续搬入](#非连续搬入)。<br>**注：dstGap=0表示相邻repeat目的操作数起始地址间隔1个数据分形即连续存放。** |
| ifTranspose | 是否启用转置功能，对每个分形矩阵进行转置，默认为false：<br>&nbsp;&nbsp;&bull; true：启用<br>&nbsp;&nbsp;&bull; false：不启用<br>特性细节可参考：[分形转置](#分形转置)。<br>**注：只有L1 Buffer->L0A Buffer和L1 Buffer->L0B Buffer通路才能开启转置，开启转置功能时，源操作数、目的操作数仅支持b16数据类型。** |
| addrMode | 用于控制多次迭代场景下，源操作数中每一次迭代的分形矩阵索引ID是递增还是递减：<br>&nbsp;&nbsp;&bull; 0（默认）：递增，下一次repeat index = startIndex + srcStride \* repeatTimes；<br>&nbsp;&nbsp;&bull; 1：递减，下一次repeat index = startIndex - srcStride \* repeatTimes。<br>特性细节可参考：[控制地址更新方式](#控制地址更新方式)。<br>**注：目前仅GM->L1 Buffer通路支持配置，保持默认值0即可，该参数不涉及性能。** |

## 数据类型

<!-- npu="950" id12 -->
Ascend 950PR/Ascend 950DT，支持数据类型为：uint8_t、int8_t、uint16_t、int16_t、half、bfloat16_t、uint32_t、int32_t、float。
<!-- end id12 -->

<!-- npu="A3" id13 -->
Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持数据类型为：int4b_t、uint8_t、int8_t、uint16_t、int16_t、half、bfloat16_t、uint32_t、int32_t、float；**注：int4b_t仅支持L1 Buffer->L0A Buffer、L1 Buffer->L0B Buffer通路。**
<!-- end id13 -->

<!-- npu="910b" id14 -->
Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持数据类型为：int4b_t、uint8_t、int8_t、uint16_t、int16_t、half、bfloat16_t、uint32_t、int32_t、float；**注：int4b_t仅支持L1 Buffer->L0A Buffer、L1 Buffer->L0B Buffer通路。**
<!-- end id14 -->

<!-- npu="310b" id15 -->
Atlas 200I/500 A2 推理产品，支持数据类型为：uint8_t、int8_t、uint16_t、int16_t、half、bfloat16_t、uint32_t、int32_t、float。
<!-- end id15 -->

<!-- npu="910" id16 -->
Atlas 训练系列产品，支持数据类型为：uint8_t、int8_t、uint16_t、int16_t、half。
<!-- end id16 -->

<!-- npu="310p" id17 -->
Atlas 推理系列产品AI Core，支持数据类型为：int4b_t、uint8_t、int8_t、uint16_t、int16_t、half；**注：int4b_t仅支持L1 Buffer->L0A Buffer、L1 Buffer->L0B Buffer通路。**
<!-- end id17 -->

<!-- npu="x90" id18 -->
Kirin X90，支持数据类型为：int8_t、half。
<!-- end id18 -->

<!-- npu="9030" id19 -->
Kirin 9030，支持数据类型为：half。
<!-- end id19 -->

## 返回值说明

无

## 约束说明

- repeatTimes为0时，表示不执行搬运，该接口将被视为NOP（空操作）。
- 只有L1 Buffer->L0A Buffer/L0B Buffer通路才能开启转置，开启转置功能时，源操作数、目的操作数仅支持b16数据类型。
- 当目的地址位于L0A Buffer/L0B Buffer时，地址必须512字节对齐。当源地址或目的地址位于L1 Buffer时，地址必须32字节对齐。当源地址位于GM时，地址必须1字节对齐。
- 当源地址位于GM时，指令执行占用的流水为PIPE_MTE2；当源地址位于L1 Buffer时，指令执行占用的流水为PIPE_MTE1。
- 当srcStride=0时，表示连续的repeat之间读取源操作数中的同一块数据分形。
<!-- npu="310p" id20 -->
- 对于Atlas 推理系列产品AI Core，在配合Mmad接口使用、B矩阵数据类型为S4场景下，如果通过ifTranspose参数启用转置，只支持64×64的分形。
<!-- end id20 -->
- 不同型号的物理存储位置不同，开发者可以参考如下：
    - 支持的物理存储位置为Global Memory（TPosition: GM）/L1 Buffer（TPosition: A1/B1）。
    <!-- npu="950" id21 -->
    - 特别针对Ascend 950PR/Ascend 950DT，支持的物理存储位置为L1 Buffer（TPosition: A1/B1）。
    <!-- end id21 -->

<!-- npu="950" id22 -->
- 特别针对Ascend 950PR/Ascend 950DT，设置dstGap参数无效。
<!-- end id22 -->

## 关键特性说明

### 非连续搬入

- 利用srcStride和dstGap参数进行跳读跳写，从L1 Buffer搬运四个float数据类型的分形到L0A Buffer。

    startIndex为0：说明搬运起始位置为源操作数中第1个分形（0为源操作数中第1个分形矩阵）。

    repeatTimes为4：表示每条指令搬运4个数据分形。

    srcStride为2：表示源操作数上，前一个数据分形的首地址与后一个数据分形的首地址之间间隔2个数据分形。

    dstGap为2：表示目的操作数上，前一个数据分形的尾地址与后一个数据分形的首地址之间间隔2个数据分形。

    ![](../../../../figures/load2d_l12l0a_noncontinuous.png)

- 从L1 Buffer搬运数据类型为float的6个数据分形到L0A Buffer，不开启转置

    需要循环调用搬运指令3次，每次调用搬运指令，源操作数地址需要偏移2个数据分形，目的操作数地址需要偏移1个数据分形。

    repeatTimes为2：表示每条指令搬运2个数据分形。

    srcStride为1：表示源操作数上，前一个数据分形的首地址与后一个数据分形的首地址之间间隔1个数据分形，表示数据分形在物理上相邻。

    dstGap为2：表示目的操作数上，前一个数据分形的尾地址与后一个数据分形的首地址之间间隔2个数据分形。

    ![](../../../../figures/load2d_l12l0a_noncontinuous_float.png)

- 从L1 Buffer搬运数据类型为float的8个数据分形到L0B Buffer，不开启转置

    需要循环调用搬运指令2次，每次调用搬运指令，源操作数地址需要偏移1个数据分形，目的操作数地址需要偏移1个数据分形。

    repeatTimes为4：表示每条指令搬运4个数据分形。

    srcStride为2：表示源操作数上，前一个数据分形的首地址与后一个数据分形的首地址之间间隔2个数据分形。

    dstGap为1：表示目的操作数上，前一个数据分形的尾地址与后一个数据分形的首地址之间间隔1个数据分形。

    ![](../../../../figures/load2d_l12l0b_noncontinuous_float.png)

### 设置搬运起始位置

从L1 Buffer搬运4个float数据类型的分形到L0A Buffer，通过设置startIndex为1跳过第1个分形。

startIndex为1：说明搬运起始位置为源操作数中第2个分形。

repeatTimes为4：表示每条指令搬运4个数据分形。

srcStride为2：表示源操作数上，前一个数据分形的首地址与后一个数据分形的首地址之间间隔2个数据分形。

dstGap为2：表示目的操作数上，前一个数据分形的尾地址与后一个数据分形的首地址之间间隔2个数据分形。

![](../../../../figures/load2d_l12l0a_set_start_pos.png)

### 分形转置

- 数据类型为half时，通过设置ifTranspose为1开启转置。

    startIndex为0：说明搬运起始位置为源操作数中第1个分形。

    repeatTimes为2：表示每条指令搬运2个数据分形。

    srcStride为2：表示源操作数上，前一个数据分形的首地址与后一个数据分形的首地址之间间隔2个数据分形。

    dstGap为2：表示目的操作数上，前一个数据分形的尾地址与后一个数据分形的首地址之间间隔2个数据分形。

    ifTranspose=1：表示开启转置。

    ![](../../../../figures/load2d_l12l0a_set_frac_trans_demo1.png)

- 从L1 Buffer搬运数据类型为half的6个数据分形到L0A Buffer，开启转置。

    需要循环调用搬运指令2次，每次调用搬运指令，源操作数地址需要偏移1个数据分形，目的操作数地址需要偏移1个数据分形。

    repeatTimes为3：表示每条指令搬运3个数据分形。

    srcStride为2：表示源操作数上，前一个数据分形的首地址与后一个数据分形的首地址之间间隔2个数据分形。

    dstGap为1：表示目的操作数上，前一个数据分形的尾地址与后一个数据分形的首地址之间间隔1个数据分形。

    ifTranspose=1：表示开启转置。

    ![](../../../../figures/load2d_l12l0a_set_frac_trans_demo2.png)

### 控制地址更新方式

从GM搬运四个float数据类型的分形到L1 Buffer，设置addrMode为1改变源操作数地址的更新方式。

addrMode为1：源操作数地址每次迭代在前一个地址的基础上减去srcStride。

startIndex为6：说明搬运起始位置为源操作数中第7个分形。

repeatTimes为4：表示每条指令搬运4个数据分形。

srcStride为2：表示源操作数上，前一个数据分形的首地址与后一个数据分形的首地址之间间隔2个数据分形。

dstGap为2：表示目的操作数上，前一个数据分形的尾地址与后一个数据分形的首地址之间间隔2个数据分形。

![](../../../../figures/load2d_l12l0a_control_update_addr.png)

## 调用示例

一、如下示例中：在A矩阵不转置，数据类型为half的场景下，可以直接调用LoadData（2D矩阵搬运）接口，无需开启转置相关参数。

搬运过程的数据排布变化示意图如下：

![](../../../../figures/load2d_l12l0a_nontrans.png)

示例代码片段如下，仅展示样例中的部分代码，完整示例请参考：[load_data_l12l0样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0)。

```cpp
uint32_t dstOffset = CeilDivision(k, fractalShape[1]) * fractalSize;
uint32_t srcOffset = fractalSize;
// Nz -> Zz
AscendC::LoadData2DParams loadDataParams;
loadDataParams.repeatTimes = CeilDivision(k, fractalShape[1]);
loadDataParams.srcStride = CeilDivision(m, fractalShape[0]);
loadDataParams.dstGap = 0;
loadDataParams.ifTranspose = false;
for (int i = 0; i < CeilDivision(m, fractalShape[0]); ++i) {
    AscendC::LoadData(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
}
```

二、如下示例中：在A矩阵转置，数据类型为half的场景下，调用LoadData（2D矩阵搬运）接口需开启转置相关参数。

搬运过程的数据排布变化示意图如下：

![](../../../../figures/load2d_l12l0a_trans.png)

示例代码片段如下，仅展示样例中的部分代码，完整示例请参考：[load_data_l12l0样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0)。

```cpp
uint32_t dstOffset = CeilDivision(k, fractalShape[0]) * fractalSize;
uint32_t srcOffset = CeilDivision(k, fractalShape[0]) * fractalSize;
// Nz -> Zz
AscendC::LoadData2DParams loadDataParams;
loadDataParams.repeatTimes = CeilDivision(k, fractalShape[0]);
// 源操作数，内轴，相邻迭代间
loadDataParams.srcStride = 1;
loadDataParams.dstGap = 0;
loadDataParams.ifTranspose = true;
for (int i = 0; i < CeilDivision(m, fractalShape[1]); ++i) {
    AscendC::LoadData(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
}
```
