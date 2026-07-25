# Compare（结果存入寄存器）<a name="ZH-CN_TOPIC_0000001835561717"></a>

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
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品 AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品 Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_vec_cmpsel_intf.h"`。

逐元素比较两个tensor大小，如果比较后的结果为真，则输出结果的对应比特位为1，否则为0。计算结果存入CmpMask 128bit寄存器中，可用于后续Select计算，也可使用[GetCmpMask](GetCmpMask(ISASI).md)接口获取寄存器保存的数据。

接口支持多种比较模式：

- LT：小于（less than）

- GT：大于（greater than）

- GE：大于或等于（greater than or equal to）

- EQ：等于（equal to）

- NE：不等于（not equal to）

- LE：小于或等于（less than or equal to）

## 函数原型<a name="section620mcpsimp"></a>

- mask逐bit模式

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Compare(const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode, const uint64_t mask[], const BinaryRepeatParams& repeatParams)
    ```

- mask连续模式

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Compare(const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode, const uint64_t mask, const BinaryRepeatParams& repeatParams)
    ```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 源操作数数据类型。 |
| isSetMask | 是否在接口内部设置mask。<br>&bull; true，表示在接口内部设置mask。<br>&bull; false，表示在接口外部设置mask，开发者需要使用[SetVectorMask](../掩码操作/SetVectorMask.md)接口设置mask值。这种模式下，接口入参中的mask值设置为占位符`MASK_PLACEHOLDER`，用于占位，无实际含义。 |

**表2**  接口参数说明

| 参数名称 | 输入/输出 | 描述 |
| --- | --- | --- |
| src0、src1 | 输入 | 源操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。 |
| cmpMode | 输入 | CMPMODE类型，表示比较模式，包括EQ，NE，GE，LE，GT，LT。<br>&bull; LT：src0小于（less than）src1 <br>&bull; GT：src0大于（greater than）src1 <br>&bull; GE：src0大于或等于（greater than or equal to）src1 <br>&bull; EQ：src0等于（equal to）src1 <br>&bull; NE：src0不等于（not equal to）src1 <br>&bull; LE：src0小于或等于（less than or equal to）src1<br> |
| mask/mask[] | 输入 | mask用于控制每次迭代内参与计算的元素。详细设置参考[掩码](../SIMD计算说明/掩码.md)。 |
| repeatParams | 输入 | 控制操作数地址步长的参数。[BinaryRepeatParams](../../辅助数据结构/BinaryRepeatParams.md)类型，包含操作数相邻迭代间相同DataBlock的地址步长，操作数同一迭代内不同DataBlock的地址步长等参数。<br>相邻迭代间的地址步长参数说明请参考[repeatStride](../SIMD计算说明/高维切分.md)；同一迭代内DataBlock的地址步长参数说明请参考[dataBlockStride](../SIMD计算说明/高维切分.md)。 |

## 数据类型

支持的数据类型为：half、float。

## 返回值说明<a name="section128671456102513"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md)。

- 本接口没有repeat输入，repeat默认为1，即一条指令计算256B的数据。

- 本接口将结果写入128bit的CmpMask寄存器中，可以用[GetCmpMask](GetCmpMask(ISASI).md)接口获取寄存器保存的数据。

<!-- npu="950" id8 -->
- 对UB空间的占用说明。针对Ascend 950PR/Ascend 950DT，该接口占用8KB Unified Buffer临时空间。
<!-- end id8 -->

<!-- npu="A3,910b" id9 -->
- 针对如下型号，当参数count或repeatTime取值为0时，不会执行计算操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作）。
  <!-- npu="A3" id10 -->
  - Atlas A3 训练系列产品/Atlas A3 推理系列产品
  <!-- end id10 -->
  <!-- npu="910b" id11 -->
  - Atlas A2 训练系列产品/Atlas A2 推理系列产品
  <!-- end id11 -->
<!-- end id9 -->

## 调用示例<a name="section642mcpsimp"></a>

本样例中，源操作数src0Local和src1Local各存储了64个float类型的数据。样例实现的功能为，逐元素对src0Local和src1Local中的数据进行比较，如果src0Local中的元素小于src1Local中的元素，dstLocal结果中对应的比特位置1；反之，则置0。dstLocal结果使用uint8\_t类型数据存储。

本样中只展示部分关键代码。完整的调用样例可参考[Compare类样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/compare)场景二。

- mask连续模式

    ```cpp
    uint64_t mask = 256 / sizeof(float); // 256为每个迭代处理的字节数
    AscendC::BinaryRepeatParams repeatParams = { 1, 1, 1, 8, 8, 8 };
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, no gap between blocks in one repeat
    // dstRepStride, src0RepStride, src1RepStride = 8, no gap between repeats
    AscendC::Compare(src0Local, src1Local, AscendC::CMPMODE::LT, mask, repeatParams);
    ```

- mask逐bit模式

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, 0};
    AscendC::BinaryRepeatParams repeatParams = { 1, 1, 1, 8, 8, 8 };
    // srcBlkStride, = 1, no gap between blocks in one repeat
    // dstRepStride, srcRepStride = 8, no gap between repeats
    AscendC::Compare(src0Local, src1Local, AscendC::CMPMODE::LT, mask, repeatParams);
    ```

结果示例如下：

```plain
输入数据(src0Local):
[ 86.72287     9.413112   17.033222  -64.10005   -66.2691    -65.57659
  15.898049   94.61241   -68.920685  -36.16883    15.62852    68.078514
 -59.724575   -9.4302225 -64.770935   66.55523   -84.60122    57.331
  60.42026   -86.78856    37.25265     8.356797  -48.544407   16.73616
  15.28083   -21.889254  -67.93181   -41.01825   -68.79465    20.169441
  44.11346   -27.419518   30.452742  -89.30283   -18.590672   32.45831
   8.392082  -57.198048   98.76846   -81.73067   -38.274437  -83.84363
  64.30617     6.028703  -20.77164    93.71867    54.190437   94.98172
 -47.447758  -65.77461    82.21715    59.953922   23.599781  -77.29708
  26.963976  -63.468987   79.97712   -70.47842    39.00433    52.36555
 -63.94925   -65.77033    26.17237   -71.904884 ]
输入数据(src1Local):
[  2.2989323  51.8879    -81.49718    41.189415    6.4081917  92.566666
  53.205498  -94.47063   -75.38387    36.464787   85.60772   -28.70681
  42.58504   -76.15293    38.723816   10.006577   74.53035   -78.38537
  71.945404   -4.060528  -14.501523   28.229202   96.87876    41.558033
 -92.623215   43.318684   35.387154  -16.029816   61.544827    3.3527017
  55.806778  -93.242096   22.86275   -87.506584   35.29523     8.405956
  91.03445   -85.29485    34.30078    -3.8019252  93.40503    15.459968
 -57.99712   -74.39948   -59.900818  -43.132637  -13.123036   41.246174
 -93.01083    75.476875  -45.437893  -99.19293    13.543604   76.23386
  46.192528  -39.23934    75.9787    -38.38979     9.807722  -60.610104
 -23.062874   48.1669     89.913376   73.78631  ]
输出数据(cmpMask寄存器):
[122  86 237  94 150   3 226 242]
```
