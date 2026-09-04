# TConv3DApiTiling结构体

TConv3DApiTiling结构体包含Conv3D算子规格信息及Tiling切分算法的相关参数，被传递给Conv3D核函数（Kernel）侧，用于数据切分、数据搬运和计算等。TConv3DApiTiling结构体的参数说明见[表1](#table18244199192620)。

用户通过调用[GetTiling](GetTiling.md)接口获取TConv3DApiTiling结构体，具体流程请参考[Conv3D Tiling使用说明](Conv3D_Tiling_usage.md)。当前暂不支持用户自定义配置TConv3DApiTiling结构体中的参数。

**表1**  TConv3DApiTiling结构说明

<a name="table18244199192620"></a>

| 参数名称 | 数据类型 | 说明 |
| --- | --- | --- |
| groups | uint32_t | 预留参数，当前仅支持为1。 |
| singleCoreDo | uint64_t | 单核上处理的Dout大小。 |
| singleCoreCo | uint32_t | 单核上处理的Cout大小。 |
| singleCoreM | uint64_t | 单核上处理的M大小。 |
| orgDo | uint64_t | Conv3D计算中原始Dout大小。 |
| orgCo | uint32_t | Conv3D计算中原始Cout大小。 |
| orgHo | uint64_t | Conv3D计算中原始Hout大小。 |
| orgWo | uint64_t | Conv3D计算中原始Wout大小。 |
| orgCi | uint32_t | Conv3D计算中原始Cin大小。 |
| orgDi | uint64_t | Conv3D计算中原始Din大小。 |
| orgHi | uint64_t | Conv3D计算中原始Hin大小。 |
| orgWi | uint64_t | Conv3D计算中原始Win大小。 |
| kernelD | uint32_t | Conv3D计算中卷积核原始kernel D维度大小。 |
| kernelH | uint32_t | Conv3D计算中卷积核原始kernel H维度大小。 |
| kernelW | uint32_t | Conv3D计算中卷积核原始kernel W维度大小。 |
| strideD | uint32_t | Conv3D计算中Stride D维度大小。 |
| strideH | uint32_t | Conv3D计算中Stride H维度大小。 |
| strideW | uint32_t | Conv3D计算中Stride W维度大小。 |
| dilationD | uint32_t | Conv3D计算中Dilation D维度大小。 |
| dilationH | uint32_t | Conv3D计算中Dilation H维度大小。 |
| dilationW | uint32_t | Conv3D计算中Dilation W维度大小。 |
| padHead | uint32_t | Conv3D计算中Padding D维度Head方向大小。 |
| padTail | uint32_t | Conv3D计算中Padding D维度Tail方向大小。 |
| padUp | uint32_t | Conv3D计算中Padding H维度Up方向大小。 |
| padDown | uint32_t | Conv3D计算中Padding H维度Down方向大小。 |
| padLeft | uint32_t | Conv3D计算中Padding W维度Left方向大小。 |
| padRight | uint32_t | Conv3D计算中Padding W维度Right方向大小。 |
| mL0 | uint32_t | L0上单次处理的M大小。 |
| kL0 | uint32_t | L0上单次处理的K大小。 |
| nL0 | uint32_t | L0上单次处理的N大小。 |
| kAL1 | uint32_t | L1上Input K的实际大小，等于Cin1InL1 * KH *  KW * C0，Cin1InL1是KD * Cin1合轴之后Tiling切分的大小。 |
| kBL1 | uint32_t | L1上Weight K的实际大小，等于Cin1InL1 * KH *  KW * C0，Cin1InL1是KD * Cin1合轴之后Tiling切分的大小。 |
| nBL1 | uint32_t | L1上Weight载入Cout维度的实际数据大小。 |
| mAL1 | uint32_t | L1上Input载入M的实际数据大小。 |
| al1FullLoad | uint8_t | Input数据在L1 Buffer是否全载。<br><br>0：Input数据在L1 Buffer上不全载。<br><br>1：Input数据在L1 Buffer上全载。 |
| bl1FullLoad | uint8_t | Weight数据在L1 Buffer是否全载。<br><br>0：Weight数据在L1 Buffer上不全载。<br><br>1：Weight数据在L1 Buffer上全载。 |
| iterateMNOrder | uint8_t | 输出结果矩阵Output时，M轴和N轴的输出顺序。<br><br>0：优先输出M方向。先输出M方向，再输出N方向，请见图2。<br><br>1：优先输出N方向。先输出N方向，再输出M方向，请见图3。<br><br>M由Hout和Wout组成，M方向的输出顺序为，先输出Wout方向，再输出Hout方向。 |
| biasFullLoadFlag | uint8_t | Bias是否全载进L1 Buffer。<br><br>0：否，单核内单次载入Bias大小等于单次矩阵乘N方向的大小nL0。<br><br>1：是，单核内的Bias一次全载。 |

注：上述的M轴为卷积正向操作过程中的输入Input在img2col展开后的纵轴，数值上等于Hout \* Wout；K为输入Input在img2col展开后的横轴，数值上等于KD\*C1\*KH\*KW\*C0；KD/KH/KW为Weight的Depth、Height、Width，即kernelD/kernelH/kernelW的简写；N为Weight的Cout，具体请见图1。

**图1**  卷积3D正向MKN示意图  
![](../../../../figures/conv3d_mkn.png "卷积3D正向MKN示意图")

**图2**  卷积3D正向MFirst示意图  
![](../../../../figures/conv3d_mfirst.png "卷积3D正向MFirst示意图")

**图3**  卷积3D正向NFirst示意图  
![](../../../../figures/conv3d_nfirst.png "卷积3D正向NFirst示意图")
