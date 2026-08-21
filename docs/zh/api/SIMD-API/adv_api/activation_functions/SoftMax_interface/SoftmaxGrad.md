# SoftmaxGrad

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
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/SoftMax_interface/SoftmaxGrad_res.md#id1 -->

## 功能说明

将输入tensor\[m<sub>0</sub>, m<sub>1</sub>, ...m<sub>t</sub>, n\]（t大于等于0）的非尾轴长度相乘的结果看作m，则输入tensor的shape看作\[m, n\]。对输入tensor\[m,n\]按行做grad反向计算，计算公式如下：

![](../../../../figures/zh-cn_formulaimage_0000001855883134.png)

当输入shape为ND格式时，内部的reduce过程按last轴进行；当输入shape为NZ格式时，内部的reduce过程按照last轴和first轴进行，reduce过程可以参考[SoftMax](SoftMax.md)中的图示说明。

为方便理解，通过Python脚本实现的方式，表达其计算公式如下，其中src、grad、isFront是源操作数（输入），dst为目的操作数（输出）。

```
def softmax_grad(grad, src, isFront = None):
    dst = grad * src
    dst = np.sum(dst, axis=-1, keepdims=True)
    if isFront :
         return dst
    dst = (grad - dst) * src
    return dst
```

## 实现原理

以float类型，ND格式，shape为\[m,k\]的输入Tensor为例，描述SoftmaxGrad高阶API内部算法框图，如下图所示。

**图1**  SoftmaxGrad算法框图  
![](../../../../figures/softmaxgrad_algorithm_diagram.png "SoftmaxGrad算法框图")

计算过程分为如下几步，均在Vector上进行：

1.  mul步骤：对输入x和y所有数据相乘，计算结果会保存到一个临时空间temp中；
2.  reducesum步骤：对temp数据\(\[m, k\]\)每一行求和得到\[m, 1\]，计算结果会保存到临时空间中；
3.  broadcast步骤：对reducesum结果\[m, 1\]的数据做一个按datablock为单位的填充，比如float类型下，把\[m, 1\]扩展成\[m, 8\]；
4.  判断是否isFront模式，如果是，则输出broadcast后的结果，计算结束；如果不是，则继续执行后续步骤；
5.  broadcast步骤：对\[m, 8\]做一个扩维，扩展成\[m, k\]，计算结果会保存到临时空间中；
6.  sub步骤：输入x的所有数据减去上一步broadcast后的结果；
7.  mul步骤：sub后的所有数据和输入y相乘，输出结果z。

## 函数原型

-   接口框架申请临时空间

    ```
    template <typename T, bool isReuseSource = false, bool isDataFormatNZ = false>
    __aicore__ inline void SoftmaxGrad(const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor, const SoftMaxTiling& tiling, bool isFront = false, const SoftMaxShapeInfo& softmaxShapeInfo = {})
    ```

-   通过sharedTmpBuffer入参传入临时空间

    ```
    template <typename T, bool isReuseSource = false, bool isDataFormatNZ = false>
    __aicore__ inline void SoftmaxGrad(const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, bool isFront = false, const SoftMaxShapeInfo& softmaxShapeInfo = {})
    ```

由于该接口的内部实现中涉及复杂的计算，需要额外的临时空间来存储计算过程中的中间变量。临时空间支持**接口框架申请**和开发者**通过sharedTmpBuffer入参传入**两种方式。

-   接口框架申请临时空间，开发者无需申请，但是需要预留临时空间的大小。

-   通过sharedTmpBuffer入参传入，使用该tensor作为临时空间进行处理，接口框架不再申请。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。

接口框架申请的方式，开发者需要预留临时空间；通过sharedTmpBuffer传入的情况，开发者需要为tensor申请空间。临时空间大小BufferSize的获取方式如下：通过[SoftmaxGrad Tiling接口](SoftmaxGrad_Tiling_interface.md)中提供的GetSoftMaxGradMaxTmpSize/GetSoftMaxGradMinTmpSize接口获取所需最大和最小临时空间大小，最小空间可以保证功能正确，最大空间用于提升性能。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。支持的数据类型为：half、float。 |
| isReuseSource | 该参数预留，传入默认值false即可。 |
| isDataFormatNZ | 当前输入输出的数据格式是否为NZ格式，默认数据格式为ND，即默认取值为false。<!-- npu="310b" id7 --><br><br>针对Atlas 200I/500 A2 推理产品，不支持配置为NZ格式。<!-- end id7 --> |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstTensor | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>last轴长度需要32Byte对齐，dstTensor的shape与gradTensor，srcTensor的shape一致。 |
| gradTensor | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>last轴长度需要32Byte对齐，gradTensor的shape与dstTensor，srcTensor的shape一致。 |
| srcTensor | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>last轴长度需要32Byte对齐，srcTensor的shape与dstTensor，gradTensor的shape一致。 |
| sharedTmpBuffer | 输入 | 临时空间。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>该操作数的数据类型固定uint8_t。<br><br>接口内部复杂计算时用于存储中间变量，由开发者提供。<br><br>临时空间大小BufferSize的获取方式请参考[SoftmaxGrad Tiling接口](SoftmaxGrad_Tiling_interface.md)。 |
| softmaxShapeInfo | 输入 | srcTensor的shape信息。SoftMaxShapeInfo类型，具体定义如下方代码所示，其中参数的含义为：<br>srcM：非尾轴长度的乘积。<br>srcK：尾轴长度，必须32Byte对齐。<br>oriSrcM：原始非尾轴长度的乘积。<br>oriSrcK：原始尾轴长度。<br><br>需要注意，当输入输出的数据格式为NZ格式时，尾轴长度为reduce轴长度即[图2](SoftMax.md#fig0172155842215)中的W0\*W1，非尾轴为H0\*H1。 |
| tiling | 输入 | softmaxgrad计算所需tiling信息，Tiling信息的获取请参考[SoftmaxGrad Tiling接口](SoftmaxGrad_Tiling_interface.md)。 |
| isFront | 输入 | 是否开启isFront计算，若为True，dstTensor的last轴长度必须固定32Byte。 |

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

-   srcTensor和dstTensor的Tensor空间可以复用。
-   操作数地址对齐要求请参见[通用地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。
-   不支持sharedTmpBuffer与源操作数和目的操作数地址重叠。
<!-- npu="A3,910b,310p" id8 -->
-   当参数softmaxShapeInfo中srcM != oriSrcM或者srcK != oriSrcK时，开发者需要对GM上的原始输入\(oriSrcM, oriSrcK\)在M或K方向补齐数据到\(srcM, srcK\)，补齐的数据会参与部分运算，在输入输出复用的场景下，API的计算结果会覆盖srcTensor中补齐的原始数据，在输入输出不复用的场景下，API的计算结果会覆盖dstTensor中对应srcTensor补齐位置的数据。
<!-- end id8 -->

## 调用示例

```
// dstLocal: 存放SoftmaxGrad计算结果的Tensor
// gradLocal：存放SoftmaxGrad计算的输入Tensor
// srcLocal：存放SoftmaxGrad计算的输入Tensor
// sharedTmpBuffer: 存放SoftmaxGrad计算过程中临时缓存的Tensor
// softmaxTiling：存放SoftmaxGrad计算所需Tiling信息，可通过SoftMaxGradTilingFunc接口获取

AscendC::SoftMaxShapeInfo softmaxInfo(
    /* 非尾轴长度的乘积          */ srcM,
    /* 尾轴长度，必须32Bytes对齐 */ srcK,
    /* 原始非尾轴长度的乘积      */ oriSrcM,
    /* 原始尾轴长度              */ oriSrcK);
bool isFront = false; // 不开启isFront计算

// 通过sharedTmpBuffer入参传入临时空间
AscendC::SoftmaxGrad<T>(dstLocal, gradLocal, srcLocal, sharedTmpBuffer, softmaxTiling, isFront, softmaxInfo);
// 接口框架申请临时空间
AscendC::SoftmaxGrad<T>(dstLocal, gradLocal, srcLocal, softmaxTiling, isFront, softmaxInfo);
```

结果示例如下：

```
输入数据(gradLocal)：
[[-100.     -80.     -60.     -50.     -30.     -20.     -15.     -10.   ]
 [  -9.      -8.      -7.      -6.      -5.      -4.      -3.      -2.   ]
 [  -1.5     -1.      -0.8     -0.6     -0.5     -0.45    -0.4     -0.35 ]
 [  -0.3     -0.25    -0.2     -0.15    -0.1     -0.05    -0.01    -0.001]
 [   0.       0.001    0.01     0.05     0.1      0.15     0.2      0.25 ]
 [   0.3      0.35     0.4      0.45     0.5      0.6      0.8      1.   ]
 [   1.5      2.       3.       4.       5.       6.       7.       8.   ]
 [   9.      10.      15.      20.      30.      50.      60.      80.   ]]
输入数据(srcLocal)：
[[2. 2. 2. 2. 2. 2. 2. 2.]
 ...
 [2. 2. 2. 2. 2. 2. 2. 2.]]
输出数据(dstLocal)：
[[ 1260.     1300.     1340.     1360.     1400.     1420.     1430.     1440.   ]
 [  158.      160.      162.      164.      166.      168.      170.      172.   ]
 [   19.4      20.4      20.8      21.2      21.4      21.5      21.6      21.7  ]
 [    3.644     3.744     3.844     3.944     4.044     4.144     4.224     4.242]
 [   -3.044    -3.042    -3.024    -2.944    -2.844    -2.744    -2.644    -2.544]
 [  -17.      -16.9     -16.8     -16.7     -16.6     -16.4     -16.      -15.6  ]
 [ -143.     -142.     -140.     -138.     -136.     -134.     -132.     -130.   ]
 [-1078.    -1076.    -1066.    -1056.    -1036.     -996.     -976.     -936.   ]]
```
