# LogSoftMax

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
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/LogSoftMax_interface/LogSoftMax_res.md#id1 -->

## 功能说明

对输入tensor做LogSoftmax计算。计算公式如下：

![](../../../../figures/zh-cn_formulaimage_0000002315199128.png)

![](../../../../figures/zh-cn_formulaimage_0000001773726766.png)

为方便理解，通过Python脚本实现的方式表达计算公式如下，其中src是源操作数（输入），dst、sum、max为目的操作数（输出）。

```
def log_softmax(src):
    #基于last轴进行rowmax(按行取最大值)处理
    max = np.max(src, axis=-1, keepdims=True)
    sub = src - max
    exp = np.exp(sub)
    #基于last轴进行rowsum(按行求和)处理
    sum = np.sum(exp, axis=-1, keepdims=True)
    dst = exp / sum
    dst = np.log10(dst)
    return dst, max, sum
```

## 实现原理

以float类型，ND格式，shape为\[m, k\]的输入Tensor为例，描述LogSoftMax高阶API内部算法框图，如下图所示。

**图1**  LogSoftMax算法框图  
![](../../../../figures/logsoftmax_diagram.png "LogSoftMax算法框图")

计算过程分为如下几步，均在Vector上进行：

1.  reducemax步骤：对输入x的每一行数据求最大值得到\[m, 1\]，计算结果会保存到一个临时空间temp中；
2.  broadcast步骤：对temp中的数据\(\[m, 1\]\)做一个按datablock为单位的填充，比如float类型下，把\[m, 1\]扩展成\[m, 8\]，同时输出max；
3.  sub步骤：对输入x的所有数据按行减去max；
4.  exp步骤：对sub之后的所有数据求exp；
5.  reducesum步骤：对exp后的结果的每一行数据求和得到\[m, 1\]，计算结果会保存到临时空间temp中；
6.  broadcast步骤：对temp\(\[m, 1\]\)做一个按datablock为单位的填充，比如float类型下，把\[m, 1\]扩展成\[m, 8\]，同时输出sum；
7.  div步骤：对exp结果的所有数据按行除以sum；
8.  log步骤：对div后的所有数据按行做log10计算，输出y。

## 函数原型

```
template <typename T, bool isReuseSource = false, bool isDataFormatNZ = false>
__aicore__ inline void LogSoftMax(const LocalTensor<T>& dst, const LocalTensor<T>& sum, const LocalTensor<T>& max, const LocalTensor<T>& src, const LocalTensor<uint8_t>& sharedTmpBuffer, const LogSoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
```

由于该接口的内部实现中涉及复杂的数学计算，需要额外的临时空间来存储计算过程中的中间变量。临时空间支持开发者通过sharedTmpBuffer入参传入。临时空间大小BufferSize的获取方式如下：通过[LogSoftMax Tiling](LogSoftMax-Tiling.md)中提供的接口获取空间范围的大小。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。支持的数据类型为：half、float。 |
| isReuseSource | 是否允许修改源操作数。该参数预留，传入默认值false即可。 |
| isDataFormatNZ | 源操作数是否为NZ格式。默认值为false。 |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>last轴长度需要32Byte对齐。 |
| sum | 输出 | reduceSum操作数。<br><br>reduceSum操作数的数据类型需要与目的操作数保持一致。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>sum的last轴长度固定为32Byte，即一个datablock长度。该datablock中的所有数据为同一个值，比如half数据类型下，该datablock中的16个数均为相同的reducesum的值。<br>非last轴的长度与目的操作数保持一致。 |
| max | 输出 | reduceMax操作数。<br><br>reduceMax操作数的数据类型需要与目的操作数保持一致。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>max的last轴长度固定为32Byte，即一个datablock长度。该datablock中的所有数据为同一个值。比如half数据类型下，该datablock中的16个数均为相同的reducemax的值。<br>非last轴的长度与目的操作数保持一致。 |
| src | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>源操作数的数据类型需要与目的操作数保持一致。 |
| sharedTmpBuffer | 输入 | 临时缓存。临时空间大小BufferSize的获取方式请参考[LogSoftMax Tiling](LogSoftMax-Tiling.md)。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| tiling | 输入 | LogSoftMax计算所需Tiling信息，Tiling信息的获取请参考[LogSoftMax Tiling](LogSoftMax-Tiling.md)。 |
| softmaxShapeInfo | 输入 | src的shape信息。SoftMaxShapeInfo类型，具体定义如下方代码所示，其中参数的含义为：<br>srcM：非尾轴长度的乘积。<br>srcK：尾轴长度，必须32Byte对齐。<br>oriSrcM：原始非尾轴长度的乘积。<br>oriSrcK：原始尾轴长度。<br><br>注意，当输入输出的数据格式为NZ（FRACTAL_NZ）格式时，尾轴长度为reduce轴长度，即[图2](../SoftMax_interface/SoftMax.md#fig0172155842215)中的W0\*W1，非尾轴为H0\*H1。 |

```
struct SoftMaxShapeInfo {
    uint32_t srcM;
    uint32_t srcK;
    uint32_t oriSrcM;
    uint32_t oriSrcK;
};
```

## 返回值说明

无

## 约束说明

-   输入源数据需保持值域在\[-2147483647.0, 2147483647.0\]。若输入不在范围内，输出结果无效。
-   **不支持源操作数与目的操作数地址重叠。**
-   不支持sharedTmpBuffer与源操作数和目的操作数地址重叠。
-   操作数地址对齐要求请参见[通用地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。
<!-- npu="A3,910b,310p" id7 -->
-   当参数softmaxShapeInfo中srcM != oriSrcM或者srcK != oriSrcK时，开发者需要对GM上的原始输入\(oriSrcM, oriSrcK\)在M或K方向补齐数据到\(srcM, srcK\)，补齐的数据会参与部分运算，在输入输出复用的场景下，API的计算结果会覆盖srcTensor中补齐的原始数据，在输入输出不复用的场景下，API的计算结果会覆盖dstTensor中对应srcTensor补齐位置的数据。
<!-- end id7 -->

<!-- npu="950" id8 -->
-   针对Ascend 950PR/Ascend 950DT，接口内部计算对Subnormal的处理方式受编译选项`--cce-ftz`控制（默认值为`true`）：
    -   配置为`false`时，计算过程中保留Subnormal，并按照其实际数值参与后续计算。
    -   配置为`true`时，启用FTZ（Flush-To-Zero）模式，计算过程中产生或参与运算的Subnormal将按0处理，可能导致计算结果与保留Subnormal时存在精度差异。
<!-- end id8 -->

## 调用示例

```
// dstLocal: 存放LogSoftMax计算结果的Tensor
// sumLocal：存放LogSoftMax计算过程中reducesum结果的Tensor
// maxLocal：存放LogSoftMax计算过程中reduceMax结果的Tensor
// srcLocal：存放LogSoftMax计算的输入Tensor
// sharedTmpBuffer: 存放LogSoftMax计算过程中临时缓存的Tensor
// softmaxTiling：存放LogSoftMax计算所需Tiling信息，可通过LogSoftMaxTilingFunc接口获取

AscendC::SoftMaxShapeInfo softmaxInfo(
    /* 非尾轴长度的乘积          */ srcM,
    /* 尾轴长度，必须32Bytes对齐 */ srcK,
    /* 原始非尾轴长度的乘积      */ oriSrcM,
    /* 原始尾轴长度              */ oriSrcK);
AscendC::LogSoftMax<DTYPE_X, false>(
    dstLocal, sumLocal, maxLocal, srcLocal, sharedTmpBuffer, softmaxTiling, softmaxInfo);
```

结果示例如下：

```
输入数据(srcLocal):
[[-100.     -80.     -60.     -50.     -30.     -20.     -15.     -10.   ]
 [  -9.      -8.      -7.      -6.      -5.      -4.      -3.      -2.   ]
 [  -1.5     -1.      -0.8     -0.6     -0.5     -0.45    -0.4     -0.35 ]
 [  -0.3     -0.25    -0.2     -0.15    -0.1     -0.05    -0.01    -0.001]
 [   0.       0.001    0.01     0.05     0.1      0.15     0.2      0.25 ]
 [   0.3      0.35     0.4      0.45     0.5      0.6      0.8      1.   ]
 [   1.5      2.       3.       4.       5.       6.       7.       8.   ]
 [   9.      10.      15.      20.      30.      50.      60.      80.   ]]
输出数据(maxLocal)：
[[-10.    -10.    -10.    -10.    -10.    -10.    -10.    -10.   ]
 [ -2.     -2.     -2.     -2.     -2.     -2.     -2.     -2.   ]
 [ -0.35   -0.35   -0.35   -0.35   -0.35   -0.35   -0.35   -0.35 ]
 [ -0.001  -0.001  -0.001  -0.001  -0.001  -0.001  -0.001  -0.001]
 [  0.25    0.25    0.25    0.25    0.25    0.25    0.25    0.25 ]
 [  1.      1.      1.      1.      1.      1.      1.      1.   ]
 [  8.      8.      8.      8.      8.      8.      8.      8.   ]
 [ 80.     80.     80.     80.     80.     80.     80.     80.   ]]
输出数据(sumLocal)：
[[1.0067834 1.0067834 1.0067834 1.0067834 1.0067834 1.0067834 1.0067834 1.0067834]
 [1.5814459 1.5814459 1.5814459 1.5814459 1.5814459 1.5814459 1.5814459 1.5814459]
 [5.971886  5.971886  5.971886  5.971886  5.971886  5.971886  5.971886  5.971886 ]
 [7.051223  7.051223  7.051223  7.051223  7.051223  7.051223  7.051223  7.051223 ]
 [6.880514  6.880514  6.880514  6.880514  6.880514  6.880514  6.880514  6.880514 ]
 [5.239974  5.239974  5.239974  5.239974  5.239974  5.239974  5.239974  5.239974 ]
 [1.5820376 1.5820376 1.5820376 1.5820376 1.5820376 1.5820376 1.5820376 1.5820376]
 [1.        1.        1.        1.        1.        1.        1.        1.       ]]
输出数据(dstLocal):
[[-39.08944    -30.40355    -21.71766    -17.374716    -8.688826    -4.345881    -2.1744084   -0.00293603]
 [ -3.2391157   -2.8048213   -2.3705268   -1.9362322   -1.5019379   -1.0676433   -0.6333489   -0.19905435]
 [ -1.2755501   -1.0584029   -0.971544    -0.88468516  -0.8412557   -0.8195409   -0.7978263   -0.77611154]
 [ -0.97811854  -0.9564038   -0.93468904  -0.9129743   -0.89125955  -0.86954486  -0.85217315  -0.84826446]
 [ -0.9461945   -0.94576025  -0.94185156  -0.92447984  -0.90276515  -0.88105035  -0.8593356   -0.8376209 ]
 [ -1.0233353   -1.0016205   -0.97990584  -0.95819116  -0.93647635  -0.89304686  -0.806188    -0.7193291 ]
 [ -3.022131    -2.8049836   -2.3706892   -1.9363947   -1.5021002   -1.0678058   -0.6335113   -0.1992168 ]
 [-30.834908   -30.400614   -28.229141   -26.057669   -21.714724   -13.028834    -8.685889     0.        ]]
```
