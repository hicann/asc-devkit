# 基础API迁移指导<a id="ZH-CN_TOPIC_0000002470508258"></a>

本节针对[NPU架构版本3510](../../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)的芯片变更对基础API兼容性产生的影响进行说明，并提供2201架构到3510架构的基础API兼容性适配方案。

**表1**  基础API兼容性问题索引

| 功能分类 | 兼容性问题 | 涉及接口 | 迁移指导 |
|---------|-----------|---------|---------|
| 矢量计算 | Subnormal浮点数在计算中被视为0 | Exp、Ln、Reciprocal、Sqrt、Rsqrt、Div | [配置Subnormal计算模式](#subnormal-migration) |
| 数据搬运 | 不支持从L1 Buffer直接搬运到GM | DataCopy | [适配L1 Buffer到GM搬运通路](#data-copy-l1-to-gm-migration) |
| 数据搬运 | 不支持设置L1 Buffer边界值 | SetLoadDataBoundary、LoadData | [手动拆分LoadData并实现地址绕回](#set-load-data-boundary-migration) |
| 矩阵计算 | Cube计算单元不支持int4b_t数据类型 | LoadData、Mmad、LoadDataWithTranspose | [将int4b_t转换为int8_t后进行矩阵计算](#cube-int4-migration) |
| 矩阵计算 | L0A Buffer分形从Zz变为Nz | LoadData、Mmad、LoadDataWithTranspose | [按Nz分形适配L0A Buffer切分](#l0a-fractal-migration) |
| 矩阵计算 | 不支持4:2结构化稀疏功能 | LoadDataWithSparse、MmadWithSparse | [将稀疏矩阵计算改为稠密矩阵计算](#structured-sparsity-migration) |
| 矩阵计算 | 不支持从GM直接搬运到L0A Buffer或L0B Buffer | LoadData | [通过L1 Buffer中转](#gm-to-l0a-l0b-migration) |
| 矩阵计算 | 不支持直接初始化L0A Buffer或L0B Buffer | Fill、LoadData | [先初始化L1 Buffer再搬运](#fill-l0a-l0b-migration) |
| 矩阵计算 | Fixpipe原生参数结构体及部分参数单位发生变化 | Fixpipe | [切换Fixpipe参数结构体](#fixpipe-params-migration) |
| 系统变量访问 | 不支持监视指定范围内的UB读写 | CheckLocalMemoryIA | [删除CheckLocalMemoryIA调用](#check-local-memory-ia-migration) |

## 矢量计算<a id="vector-compute"></a>

### 删除Subnormal硬件计算功能<a id="subnormal-migration"></a>

**说明**：Subnormal浮点数指的是指数位全为0、尾数不为0的浮点数，用于表示比最小正常数更小的值，避免“下溢为0”。3510版本默认不支持Subnormal，Subnormal浮点数在计算中被视为0。

**兼容方案**：对于支持config参数的基础API，可以通过设置config模板参数来配置Subnormal计算模式。对于不传入config参数的部分基础API及高阶API，可通过编译选项`--cce-ftz`配置Subnormal处理方式，该选项默认为`true`。`--cce-ftz=false`时保留Subnormal，`--cce-ftz=true`时采用FTZ（Flush-To-Zero）模式。软件模拟通过精度扩展等方式处理Subnormal数据，避免其下溢为0。

**表2**  涉及Subnormal的API和config参数说明

| Ascend C基础API | 兼容说明 |
|-----------------|---------|
| Exp、Ln、Reciprocal、Sqrt、Rsqrt、Div | 以Ln接口为例来进行说明。<br>通过LnConfig结构体的参数algo来配置Subnormal计算模式。algo取值如下：<br>- LnAlgo::INTRINSIC，Subnormal处理方式受编译选项--cce-ftz控制，该选项默认为true。<br>- LnAlgo::PRECISION_1ULP_FTZ_TRUE，使用单指令计算得出结果，采用FTZ模式。<br>- LnAlgo::PRECISION_1ULP_FTZ_FALSE，支持Subnormal数据计算。<br>该参数默认值DEFAULT_LN_CONFIG的取值如下：<br>`constexpr LnConfig DEFAULT_LN_CONFIG = { LnAlgo::INTRINSIC };` |

**迁移示例**：

迁移前（2201）：硬件默认支持Subnormal，直接调用即可。

```cpp
// 参考样例代码行：45
AscendC::Ln<half>(dstLocal, srcLocal, count);
```

迁移后（3510）：通过LnConfig模板参数配置algo为PRECISION_1ULP_FTZ_FALSE，软件仿真保留Subnormal。注意config为const引用型模板参数，实参必须具备静态存储期，使用static constexpr。

```cpp
// 参考样例代码行：49-50
static constexpr AscendC::LnConfig CONFIG = { AscendC::LnAlgo::PRECISION_1ULP_FTZ_FALSE };
AscendC::Ln<half, CONFIG>(dstLocal, srcLocal, count);
```

**对应样例**：[Subnormal兼容性样例](../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/subnormal)。

## 数据搬运

### 删除支持L1 Buffer-\>GM硬件通路<a id="data-copy-l1-to-gm-migration"></a>

**说明**：硬件删除L1 Buffer到GM的通路，无法将数据从L1 Buffer直接搬运到GM中。现有接口不支持L1 Buffer到GM的直接搬运。

**兼容方案**：对于纯Cube计算场景：在GM多分配一个单位矩阵，通过Mmad矩阵乘法计算输出到L0C Buffer，再从L0C Buffer通过Fixpipe搬运到GM。对于Vector和Cube计算融合场景，可以通过L1 Buffer搬运到Unified Buffer（UB），再搬运到GM。以下以纯Cube计算场景为例进行说明，介绍算子核心流程。

**迁移示例**：

迁移前（2201）：GM→L1→GM，直接搬运。

```cpp
// 参考样例代码行：123-129
AscendC::LocalTensor<T> leftMatrix(AscendC::TPosition::A1, a1Addr, M * K);
AscendC::DataCopy(leftMatrix, aGlobal, M * K);
AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
AscendC::DataCopy(cGlobal, leftMatrix, M * K);  // L1 -> GM
```

迁移后（3510）：GM→L1→L0A/L0B→L0C→GM，通过矩阵乘法+Fixpipe搬运。

```cpp
// 参考样例代码行：139-150
CopyGmToL1A(a1Local);  // GM -> L1A
CopyGmToL1B(b1Local);  // GM -> L1B（B为单位矩阵）
AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE1>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE1>(EVENT_ID0);
Load2DL1AToL0A(a1Local, a2Local);  // L1A -> L0A
Load2DL1BToL0B(b1Local, b2Local);  // L1B -> L0B
AscendC::SetFlag<AscendC::HardEvent::MTE1_M>(EVENT_ID1);
AscendC::WaitFlag<AscendC::HardEvent::MTE1_M>(EVENT_ID1);
Compute(co1Local, a2Local, b2Local);  // Mmad矩阵乘，A×单位矩阵=A
AscendC::SetFlag<AscendC::HardEvent::M_FIX>(EVENT_ID2);
AscendC::WaitFlag<AscendC::HardEvent::M_FIX>(EVENT_ID2);
CopyL0CToGm(co1Local);  // L0C -> GM via Fixpipe
```

**对应样例**：[L1到GM搬运兼容性样例](../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/data_copy_l1togm)。

### 不支持设置L1 Buffer边界值<a id="set-load-data-boundary-migration"></a>

**说明**：3510架构硬件删除了L1 Buffer的边界值设定相关寄存器，不再支持SetLoadDataBoundary接口。该接口用于设置LoadData（卷积数据搬运）时L1 Buffer的边界值。如果指令在处理源操作数时，源操作数在L1 Buffer上的地址超出设置的边界，则会从L1 Buffer的起始地址开始读取数据。设置为0表示无边界，可以使用整个L1 Buffer。

**兼容方案**：

-   [NPU架构版本2201](../../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)的接口参数boundaryValue设置为0时与3510架构版本等价。
-   如果需要在L1 Buffer上循环读取操作数，需要将对应的LoadData（卷积数据搬运）接口手动拆分成多条指令，手动绕回。

**图1**  手动拆分LoadData实现L1 Buffer地址绕回

![](../../../figures/fig_1_zn.png)

**迁移示例**：

迁移前（2201）：通过SetLoadDataBoundary设置边界，单次LoadData自动绕回。

```cpp
// 参考样例代码行：95-101
AscendC::LoadDataRepeatParam repeatParams;
repeatParams.repeatStride = 0;  // 相邻迭代起始地址的距离，单位为16个元素
repeatParams.repeatTime = 1;    // height方向的迭代次数
repeatParams.repeatMode = 0;    // 迭代方向：0表示height方向
AscendC::SetLoadDataRepeat(repeatParams);
AscendC::SetLoadDataBoundary(1024);  // 设置L1边界为1024B
AscendC::LoadData(a2, leftMatrix, loadData3dParamsPro);
```

迁移后（3510）：删除SetLoadDataBoundary，多次调用LoadData手动绕回。

```cpp
// 参考样例代码行：103-121
uint16_t dstStride = AscendC::DivCeil(M / 2, 16);
AscendC::LoadDataRepeatParamWithStride repeatParams;
repeatParams.repeatStride = 0;  // 相邻迭代起始地址的距离，单位为16个元素
repeatParams.repeatTime = 1;    // height方向的迭代次数
repeatParams.repeatMode = 0;    // 迭代方向：0表示height方向
repeatParams.dstStride = dstStride;  // 输出矩阵K轴偏移，单位为512B分形
AscendC::SetLoadDataRepeatWithStride(repeatParams);
AscendC::LoadData(a2, leftMatrix, loadData3dParamsPro);
AscendC::LocalTensor<T> a3 = a2[256];   // 目的地址偏移256个元素
AscendC::LoadData(a3, leftMatrix, loadData3dParamsPro);
AscendC::LocalTensor<T> a4 = a2[512];   // 目的地址偏移512个元素
AscendC::LoadData(a4, leftMatrix, loadData3dParamsPro);
AscendC::LocalTensor<T> a5 = a2[768];   // 目的地址偏移768个元素
AscendC::LoadData(a5, leftMatrix, loadData3dParamsPro);
```

**对应样例**：[SetLoadDataBoundary兼容性样例](../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/set_loaddata_boundary)。

## 矩阵计算

### Cube计算单元不再支持int4b_t数据类型<a id="cube-int4-migration"></a>

**说明**：相较于2201架构版本，3510架构版本的Cube计算单元不支持int4b_t。相关的基础API有LoadData、Mmad和LoadDataWithTranspose，这些接口不再支持int4b_t。

**兼容方案**：算子侧通过编写CV融合算子在Vector Core进行int4b_t到int8_t的Cast转换，再通过UB搬运到L1后进行Mmad计算。

**迁移示例**：

迁移前（2201）：直接使用int4b_t进行Matmul计算。

```cpp
// 参考样例代码行：128-133
AscendC::Matmul<A_TYPE_S4, B_TYPE_S4, C_TYPE, BIAS_TYPE, CFG_MDL> mm;
mm.SetTensorA(gmA, false);  // gmA为int4b_t类型
mm.SetTensorB(gmB, false);  // gmB为int4b_t类型
mm.IterateAll(gmC);
```

迁移后（3510）：先在Vector Core将int4b_t Cast为int8_t，再进行int8_t矩阵乘。

```cpp
// 参考样例代码行：转换逻辑见140-156，矩阵乘逻辑见128-133
// Step1 - Vector Core上int4b_t -> int8_t转换
AscendC::LocalTensor<AscendC::int4b_t> int4SrcLocalTensor =
    srcLocalTensor.ReinterpretCast<AscendC::int4b_t>();
AscendC::Cast<half, AscendC::int4b_t>(tmpTensor, int4SrcLocalTensor,
    AscendC::RoundMode::CAST_NONE, count * 2);
AscendC::PipeBarrier<PIPE_V>();
AscendC::Cast<int8_t, half>(dstLocalTensor, tmpTensor,
    AscendC::RoundMode::CAST_CEIL, count * 2);
// Step2 - int8_t类型矩阵乘
AscendC::Matmul<A_TYPE_S8, B_TYPE_S8, C_TYPE, BIAS_TYPE, CFG_MDL> mm;
mm.SetTensorA(gmA, false);  // gmA已转换为int8_t类型
mm.SetTensorB(gmB, false);  // gmB已转换为int8_t类型
mm.IterateAll(gmC);
```

**对应样例**：[int4数据类型下Matmul兼容性样例](../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/matmul_s4)。

### L0A Buffer分形改变，从Zz转换为Nz格式<a id="l0a-fractal-migration"></a>

**说明**：涉及的API有LoadData、Mmad和LoadDataWithTranspose。

-   2201架构版本，参与矩阵乘计算（A \* B = C）时，ABC矩阵的数据排布格式分别为Zz，Zn，Nz。A、B、C矩阵分别位于L0A Buffer、L0B Buffer、L0C Buffer。

    矩阵A：每个分形矩阵内部是行主序，分形矩阵之间是行主序。分形Shape为`16 x (32B / sizeof(AType))`，大小为512Byte。

    矩阵B：每个分形矩阵内部是列主序，分形矩阵之间是行主序。分形Shape为`(32B / sizeof(BType)) x 16`，大小为512Byte。

    矩阵C：每个分形矩阵内部是行主序，分形矩阵之间是列主序。分形Shape为16 x 16，大小为256个元素。

    **图2**  2201架构矩阵分形排布

    ![](../../../figures/fig_2_zn.png)

-   3510架构版本，参与矩阵乘计算（A \* B = C）时，ABC矩阵的数据排布格式分别为Nz，Zn，Nz。

    矩阵A：每个分形矩阵内部是行主序，分形矩阵之间是列主序。其Shape为`16 x (32B / sizeof(AType))`，大小为512Byte。

    矩阵B：每个分形矩阵内部是列主序，分形矩阵之间是行主序。其Shape为`(32B / sizeof(BType)) x 16`，大小为512Byte。

    矩阵C：每个分形矩阵内部是行主序，分形矩阵之间是列主序。其Shape为16 x 16，大小为256个元素。

    **图3**  3510架构矩阵分形排布

    ![](../../../figures/fig_3_zn.png)

**兼容方案**：根据L0A Buffer是否复用，分为L0A Buffer不复用和L0A Buffer复用（M轴切分）两种场景进行适配。

**图4**  L1 Buffer到L0A Buffer的分形转换差异

![](../../../figures/fig_4_zn.png)

#### L0A Buffer不复用场景

该场景将完整的A矩阵一次搬入L0A Buffer，并通过一次Mmad完成矩阵计算。

-   2201架构：L1 Buffer中的A矩阵为Nz分形，L0A Buffer要求Zz分形，需要通过LoadData（2D矩阵搬运）完成Nz到Zz的分形转换。
-   3510架构：L1 Buffer和L0A Buffer中的A矩阵均为Nz分形，无需转换分形，需按照Nz分形的数据排布调整LoadData（2D矩阵搬运）的参数。

**图5**  L0A Buffer不复用场景的分形适配

![](../../../figures/fig_5_zn.png)

**迁移示例**：

迁移前（2201）：L1 Buffer中的A矩阵为Nz分形。以M轴方向为外层循环，每次通过LoadData将K轴方向的分形搬入L0A Buffer，并转换为Zz分形。

```cpp
// 参考样例代码行：98-110
constexpr uint32_t mBlocks = M / CUBE_BLOCK;
constexpr uint32_t kBlocks = K * sizeof(T) / C0_SIZE;
int srcOffset = 0;
int dstOffset = 0;
for (uint32_t i = 0; i < mBlocks; ++i) {
    AscendC::LoadData2DParams loadDataParams;
    loadDataParams.repeatTimes = kBlocks;
    loadDataParams.srcStride = mBlocks;
    loadDataParams.ifTranspose = false;
    AscendC::LoadData(a2[dstOffset], a1[srcOffset], loadDataParams);
    srcOffset += CUBE_BLOCK * CUBE_BLOCK;
    dstOffset += K * CUBE_BLOCK;
}
```

迁移后（3510）：L1 Buffer和L0A Buffer中的A矩阵均为Nz分形。使用`LoadData2DParamsV2`描述M轴和K轴的搬运范围，通过一次LoadData完成Nz分形搬运，无需转换分形。

```cpp
// 参考样例代码行：112-124
constexpr uint32_t mBlocks = M / CUBE_BLOCK;
constexpr uint32_t kBlocks = K * sizeof(T) / C0_SIZE;
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStartPosition = 0;  // M轴起始位置，单位为16个元素
loadDataParams.kStartPosition = 0;  // K轴起始位置，单位为32字节
loadDataParams.mStep = mBlocks;     // M轴搬运范围，单位为16个元素
loadDataParams.kStep = kBlocks;     // K轴搬运范围，单位为32字节
loadDataParams.srcStride = mBlocks; // 源相邻K轴分形间隔，单位为512字节
loadDataParams.dstStride = mBlocks; // 目的相邻K轴分形间隔，单位为512字节
loadDataParams.ifTranspose = false;
loadDataParams.sid = 0;
AscendC::LoadData(a2, a1, loadDataParams);
```

#### L0A Buffer复用场景

该场景沿M轴将A矩阵切分为A1和A2，分别与B矩阵进行Mmad计算，得到C1和C2。

-   2201架构：L0A Buffer中的A矩阵为Zz分形，沿M轴切分后A1和A2在L0A Buffer中仍然连续。L1 Buffer到L0A Buffer的搬运方式与不复用场景相同，第二次Mmad计算调整A矩阵的地址偏移即可。
-   3510架构：L0A Buffer中的A矩阵为Nz分形，沿M轴切分后A1和A2不能分别作为连续的Nz子矩阵使用。需要通过两次LoadData（2D矩阵搬运）分别搬运A矩阵的上半部分和下半部分，将其重排为两个连续的Nz子矩阵，无需修改Mmad计算逻辑和流水排布。

**图6**  L0A Buffer复用场景的分形适配

![](../../../figures/fig_6_zn.png)

**迁移示例**：

迁移前（2201）：L1 Buffer到L0A Buffer的搬运方式与不复用场景相同。A1和A2在Zz分形中连续，执行第二次Mmad时将A矩阵和C矩阵的地址偏移到下半部分。

```cpp
// 参考样例代码行：A矩阵搬运见98-110，两次Mmad见180-187
constexpr uint32_t mBlocks = M / CUBE_BLOCK;
constexpr uint32_t kBlocks = K * sizeof(T) / C0_SIZE;
int srcOffset = 0;
int dstOffset = 0;
for (uint32_t i = 0; i < mBlocks; ++i) {
    AscendC::LoadData2DParams loadDataParams;
    loadDataParams.repeatTimes = kBlocks;
    loadDataParams.srcStride = mBlocks;
    loadDataParams.ifTranspose = false;
    AscendC::LoadData(a2[dstOffset], a1[srcOffset], loadDataParams);
    srcOffset += CUBE_BLOCK * CUBE_BLOCK;
    dstOffset += K * CUBE_BLOCK;
}

AscendC::MmadParams mmadParams;
mmadParams.m = M / 2;  // A矩阵沿M轴切分为上下两半
mmadParams.n = N;
mmadParams.k = K;
mmadParams.cmatrixInitVal = true;
mmadParams.isBias = false;
AscendC::Mmad(co1Local, a2, b2, mmadParams);
// A矩阵和C矩阵均偏移到下半部分，计算C2=A2*B
AscendC::Mmad(co1Local[M * N / 2], a2[M * K / 2], b2, mmadParams);
```

迁移后（3510）：沿M轴切分后，A1和A2在原Nz分形中不连续。分别搬运上半部分和下半部分，将其重排为两个连续的Nz子矩阵，再复用与2201相同的Mmad计算逻辑。

```cpp
// 参考样例代码行：A矩阵搬运见112-132，两次Mmad见180-187
constexpr uint32_t mBlocks = M / CUBE_BLOCK;
constexpr uint32_t kBlocks = K * sizeof(T) / C0_SIZE;
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStartPosition = 0;      // 从A矩阵上半部分开始，单位为16个元素
loadDataParams.kStartPosition = 0;      // K轴起始位置，单位为32字节
loadDataParams.mStep = mBlocks / 2;     // 每次搬运一半M轴，单位为16个元素
loadDataParams.kStep = kBlocks;         // K轴搬运范围，单位为32字节
loadDataParams.srcStride = mBlocks;     // 源相邻K轴分形间隔，单位为512字节
loadDataParams.dstStride = mBlocks / 2; // 子矩阵相邻K轴分形间隔，单位为512字节
loadDataParams.ifTranspose = false;
loadDataParams.sid = 0;
// 搬运A矩阵的上半部分
AscendC::LoadData(a2, a1, loadDataParams);
// 搬运A矩阵的下半部分
loadDataParams.mStartPosition = mBlocks / 2;
AscendC::LoadData(a2[M * K / 2], a1, loadDataParams);

AscendC::MmadParams mmadParams;
mmadParams.m = M / 2;  // A矩阵沿M轴切分为上下两半
mmadParams.n = N;
mmadParams.k = K;
mmadParams.cmatrixInitVal = true;
mmadParams.isBias = false;
AscendC::Mmad(co1Local, a2, b2, mmadParams);
// A矩阵和C矩阵均偏移到下半部分，计算C2=A2*B
AscendC::Mmad(co1Local[M * N / 2], a2[M * K / 2], b2, mmadParams);
```

**对应样例**：[pattern_transformation兼容性样例](../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/pattern_transformation)。

### 删除4:2结构化稀疏功能<a id="structured-sparsity-migration"></a>

**说明**：2201架构支持4:2结构化稀疏，通过`LoadDataWithSparse`将L1 Buffer中的512B稠密权重矩阵搬运到L0B Buffer并同时读取128B索引矩阵实现稀疏化，再通过`MmadWithSparse`执行稀疏矩阵乘加。3510架构不支持结构化稀疏功能，`LoadDataWithSparse`和`MmadWithSparse`均不可用。

**兼容方案**：不调用`LoadDataWithSparse`进行矩阵稠密转稀疏操作，直接使用`Mmad`进行正常的稠密矩阵计算。稀疏矩阵相关算法可参考MmadWithSparse中的介绍。

**迁移示例**：

迁移前（2201）：使用LoadDataWithSparse + MmadWithSparse进行稀疏矩阵乘。

```cpp
AscendC::LoadDataWithSparse(b2Local, b1Local, loadDataSparseParams);
AscendC::MmadWithSparse(co1Local, a2Local, b2Local, mmadSparseParams);
```

迁移后（3510）：使用LoadData + Mmad进行稠密矩阵乘。

```cpp
AscendC::LoadData(b2Local, b1Local, loadDataParams);
AscendC::Mmad(co1Local, a2Local, b2Local, mmadParams);
```

### 删除GM-\>L0A Buffer/L0B Buffer通路<a id="gm-to-l0a-l0b-migration"></a>

**说明**：硬件删除GM-\>L0A Buffer/L0B Buffer通路，调用LoadData时，不再支持这些通路。

**兼容方案**：实现GM→L0A Buffer/L0B Buffer搬运需拆分成两步进行，先从GM搬运到L1 Buffer，再从L1 Buffer搬运到L0A Buffer、L0B Buffer。

**迁移示例**：

迁移前（2201）：GM→L0A，单步LoadData直接搬运。

```cpp
// 参考样例代码行：暂无对应样例，本段为2201迁移示意代码
AscendC::LoadData(a2Local, aGlobal, loadDataParams);  // 源为GM
```

迁移后（3510）：GM→L1→L0A，两步搬运。

```cpp
// 参考样例代码行：GM→L1见40-42，L1→L0A见54-64
// Step1 - GM -> L1
AscendC::Nd2NzParams intriParams{1, M, K, 0, K, M, 1, 0};
AscendC::DataCopy(a1Local, aGlobal, intriParams);
// Step2 - L1 -> L0A
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = AscendC::DivCeil(M, CUBE_BLOCK);
loadDataParams.kStep = AscendC::DivCeil(K * sizeof(T), C0_SIZE);
loadDataParams.srcStride = AscendC::DivCeil(M, CUBE_BLOCK);
loadDataParams.dstStride = AscendC::DivCeil(M, CUBE_BLOCK);
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```

**对应样例**：[L1到GM搬运兼容性样例](../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/data_copy_l1togm)（3510分支实现了GM→L1→L0A/L0B通路拆分）。

### 删除L0A Buffer/L0B Buffer初始化的相关硬件指令<a id="fill-l0a-l0b-migration"></a>

**说明**：2201架构支持通过`Fill`接口直接初始化L0A Buffer、L0B Buffer。3510架构删除了L0A Buffer/L0B Buffer初始化的相关硬件指令，`Fill`接口不支持直接初始化L0A/L0B。

**兼容方案**：先通过`Fill`接口初始化L1 Buffer，再通过`LoadData`接口将L1 Buffer上的数据搬运到L0A Buffer、L0B Buffer。

**迁移示例**：

迁移前（2201）：调用Fill初始化L0A Buffer。

```cpp
// 参考样例代码行：32-37
AscendC::Fill(a2Local, {1, static_cast<uint16_t>(M * K * sizeof(T) / 512), 0, 1});
```

迁移后（3510）：调用Fill初始化L1，再调用LoadData将数据搬运到L0A Buffer。

```cpp
// 参考样例代码行：L1初始化见48-51，L1→L0A搬运见59-71
// Step1 - Fill初始化L1
AscendC::Fill(a1Local, {1, static_cast<uint16_t>(M * K * sizeof(T) / 32), 0, 1});
AscendC::PipeBarrier<PIPE_MTE1>();
// Step2 - LoadData从L1搬运到L0A
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = AscendC::DivCeil(M, CUBE_BLOCK);
loadDataParams.kStep = AscendC::DivCeil(K * sizeof(T), C0_SIZE);
loadDataParams.srcStride = AscendC::DivCeil(M, CUBE_BLOCK);
loadDataParams.dstStride = AscendC::DivCeil(M, CUBE_BLOCK);
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```

**对应样例**：[Fill兼容性样例](../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/fill)。

### Fixpipe接口参数结构体切换<a id="fixpipe-params-migration"></a>

**说明**：2201架构的Fixpipe接口使用`FixpipeParamsV220`参数结构体。3510架构引入了原生的`FixpipeParamsArch3510`参数结构体，同时也兼容`FixpipeParamsV220`，但推荐使用原生结构体以获得完整能力。

**FixpipeParamsV220迁移到FixpipeParamsArch3510注意事项**：

- 不开启NZ2ND/NZ2DN时，`dstStride`参数单位变化。该参数用来表示目的NZ矩阵中相邻Z排布的起始地址偏移。
  - 在2201架构下，单位为32B。
  - 在3510架构下，单位为element。
- 在3510架构下，`ndNum`、`srcNdStride`、`dstNdStride`等参数被收敛到`params`结构体参数下。

**兼容方案**：
3510兼容`FixpipeParamsV220`，但更推荐迁移至`FixpipeParamsArch3510`。NZ2ND场景下，对于`ndNum`、`srcNdStride`、`dstNdStride`等参数，包裹在`params`中传递。非NZ2ND场景下，需要按照元素单位重新计算`dstStride`。

**迁移示例（NZ2ND场景）**：

迁移前（2201）：使用FixpipeParamsV220。

```cpp
// 参考样例代码行：155-174
AscendC::FixpipeParamsV220 fixpipeParams;
fixpipeParams.ndNum = 1;
fixpipeParams.srcNdStride = 0;
fixpipeParams.dstNdStride = 0;
fixpipeParams.mSize = baseM;
fixpipeParams.srcStride = CeilAlign(baseM, CUBE_BLOCK);
fixpipeParams.nSize = baseN;
fixpipeParams.dstStride = baseN;
fixpipeParams.quantPre = QuantMode_t::NoQuant;
AscendC::Fixpipe<outputType, l0cType, AscendC::CFG_ROW_MAJOR>(cGM, cL0C, fixpipeParams);
```

迁移后（3510）：使用FixpipeParamsArch3510原生结构体。2201的顶层字段`ndNum`、`srcNdStride`和`dstNdStride`需分别迁移为`params`子结构的对应字段，公共参数字段名一致。

```cpp
// 参考样例代码行：155-174
AscendC::FixpipeParamsArch3510<AscendC::CO2Layout::ROW_MAJOR> fixpipeParams;
fixpipeParams.params.ndNum = 1;        // NZ2ND搬运的NZ矩阵数量
fixpipeParams.params.srcNdStride = 0;  // 单个NZ矩阵无需配置源矩阵间隔
fixpipeParams.params.dstNdStride = 0;  // 单个ND矩阵无需配置目的矩阵间隔
fixpipeParams.mSize = baseM;
fixpipeParams.srcStride = CeilAlign(baseM, CUBE_BLOCK);
fixpipeParams.nSize = baseN;
fixpipeParams.dstStride = baseN;
fixpipeParams.quantPre = QuantMode_t::NoQuant;
AscendC::Fixpipe<outputType, l0cType, AscendC::CFG_ROW_MAJOR>(cGM, cL0C, fixpipeParams);
```

**迁移示例（NZ2NZ场景，dstStride单位不同）**：

迁移前（2201）：dstStride单位为datablock（32字节）。

```cpp
// 参考样例代码行：175-189
AscendC::FixpipeParamsV220 fixpipeParams;
fixpipeParams.dstStride = baseM * c0Size * sizeof(outputType) / AscendC::ONE_BLK_SIZE;
fixpipeParams.nSize = CeilAlign(baseN, c0Size);
fixpipeParams.mSize = baseM;
fixpipeParams.srcStride = CeilAlign(baseM, CUBE_BLOCK);
AscendC::Fixpipe<outputType, l0cType, AscendC::CFG_NZ>(cGM, cL0C, fixpipeParams);
```

迁移后（3510）：dstStride单位为元素个数。

```cpp
// 参考样例代码行：175-189
AscendC::FixpipeParamsArch3510<AscendC::CO2Layout::NZ> fixpipeParams;
fixpipeParams.dstStride = baseM * c0Size;
fixpipeParams.nSize = CeilAlign(baseN, c0Size);
fixpipeParams.mSize = baseM;
fixpipeParams.srcStride = CeilAlign(baseM, CUBE_BLOCK);
AscendC::Fixpipe<outputType, l0cType, AscendC::CFG_NZ>(cGM, cL0C, fixpipeParams);
```

**对应样例**：[Fixpipe参数结构体切换兼容性样例](../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/fixpipe_params_switch)。

## 系统变量访问

### 不支持CheckLocalMemoryIA，硬件删除相关寄存器<a id="check-local-memory-ia-migration"></a>

**说明**：CheckLocalMemoryIA监视设定范围内的UB读写行为，如果监视到有设定范围的读写行为则会出现EXCEPTION报错，未监视到设定范围的读写行为则不会报错。

**兼容方案**：该接口为调测接口，对功能无影响，直接删除调用即可。

**迁移示例**：

迁移前（2201）：调用CheckLocalMemoryIA监视UB读写。

```cpp
// 参考样例代码行：暂无对应样例，本段为迁移示意代码
AscendC::CheckLocalMemoryIA(...);
```

迁移后（3510）：接口删除，直接移除调用。

```cpp
// 参考样例代码行：暂无对应样例，本段为迁移示意代码
// 接口已删除，无需调用
```
