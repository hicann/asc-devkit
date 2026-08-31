# SoftmaxFlashV3

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
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/SoftMax_interface/SoftmaxFlashV3_res.md#id1 -->

## 功能说明

SoftmaxFlash增强版本，对应Softmax PASA算法。将输入tensor\[m<sub>0</sub>, m<sub>1</sub>, ..., m<sub>t</sub>, n\]（t大于或等于0）的非尾轴长度m<sub>0</sub>, m<sub>1</sub>, ..., m<sub>t</sub>相乘的结果看作m，则输入tensor的shape看作\[m, n\]。对输入tensor x的尾轴进行切分，分块个数为splitMeanCnt，切分后的tensor为x\_cnt<sub>i</sub>。按如下公式进行计算，其中x、inmax、insum、inmean为输入，M、S、E、A均为输出。

-   update为false：

    ![](../../../../figures/zh-cn_formulaimage_0000002466346798.png)

-   update为true：

    ![](../../../../figures/zh-cn_formulaimage_0000002122846977.png)

本接口当前只支持ND格式的输入，内部的reduce过程按last轴处理。

为方便理解，通过Python伪代码实现的方式，表达其计算公式如下。其中，repeatSize为64，elementNumPerBlk/BlkcntPerRepeat为8，splitMeanCnt为8，src、inmean、inmax、 insum、update为输入，dst、x\_mean、x\_sum、x\_max、exp\_max为输出。

```
def softmax_flash_3(src, height, width, loopCnt, alpha, baseK, inmax=None, insum=None, inmean=None, update=False):
    scalar = alpha / (1 - alpha)
    #(m,n)->(m,64)
    tmpbuffer0 = BlockReduceSum(repeatSize, repeatSize, elementNumPerBlk)
    remain = int(width / repeatSize - BlkcntPerRepeat)
    tmpbuffer0 = Add(tmpbuffer0, src, remain, repeatSize * elementNumPerBlk, width)
    #(m,64)->(m,8)
    tmpbuffer0 = BlockReduceSum(1, elementNumPerBlk, elementNumPerBlk)
    #width = baseK * splitMeanCnt
    rowMeanLocal = tmpbuffer0 / baseK
    rowMeanGlobal = np.mean(src, axis=(-1), keepdims=True)
    rowMeanGlobalTmp = (rowMeanGlobal - rowMeanLocal) * scalar
    src = src - rowMeanGlobalTmp

    if update == False:
        x_mean = rowMeanGlobal
        maxTmp = np.max(src, axis=-1, keepdims=True)
        shiftCurr = (rowMeanGlobal - x_mean) * scalar
        x_max = shiftCurr + maxTmp
        maxTmp = x_max - shiftCurr
        x_sub = src - maxTmp
        dst = np.exp(x_sub)
        x_sum = np.sum(dst, axis=-1, keepdims=True)
        exp_max = None
        return dst, x_max, x_sum, x_mean, exp_max
    else:
        x_mean = (rowMeanGlobal + inmean * (loopCnt - 1)) / loopCnt
        maxTmp = np.max(src, axis=-1, keepdims=True)
        shiftCurr = (rowMeanGlobal - x_mean) * scalar
        shiftPrev = (inmean - x_mean) * scalar
	x_max = shiftCurr + maxTmp
        maxTmp = shiftPrev + inmax
        x_max = np.max(np.concatenate((x_max, maxTmp), axis=(-1)), axis=(-1), keepdims=True)
        maxTmp = x_max - shiftCurr
        x_sub = src - maxTmp
        dst = np.exp(x_sub)
        exp_max = np.exp(inmax - x_max + shiftPrev)
        x_sum = np.sum(dst, axis=-1, keepdims=True)
        x_sum = exp_max * insum +  x_sum
        return dst, x_max, x_sum, x_mean, exp_max
```

## 函数原型

-   接口框架申请临时空间

    ```
    template <typename T, typename U, bool isUpdate = false, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
    __aicore__ inline void SoftmaxFlashV3(const LocalTensor<T>& dstTensor, const LocalTensor<U>& meanTensor, const LocalTensor<U>& expSumTensor, const LocalTensor<U>& maxTensor, const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor, const LocalTensor<U>& inMeanTensor, const LocalTensor<U>& inExpSumTensor, const LocalTensor<U>& inMaxTensor, const SoftMaxTiling& tiling, const SoftMaxParams& params)
    ```

-   通过sharedTmpBuffer入参传入临时空间

    ```
    template <typename T, typename U, bool isUpdate = false, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
    __aicore__ inline void SoftmaxFlashV3(const LocalTensor<T>& dstTensor, const LocalTensor<U>& meanTensor,const LocalTensor<U>& expSumTensor, const LocalTensor<U>& maxTensor, const LocalTensor<T>& srcTensor,const LocalTensor<T>& expMaxTensor, const LocalTensor<U>& inMeanTensor, const LocalTensor<U>& inExpSumTensor, const LocalTensor<U>& inMaxTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, const SoftMaxParams& params)
    ```

由于该接口的内部实现中涉及复杂的计算，需要额外的临时空间来存储计算过程中的中间变量。临时空间支持**接口框架申请**和开发者**通过sharedTmpBuffer入参传入**两种方式。

-   接口框架申请临时空间，开发者无需申请，但是需要预留临时空间的大小。

-   通过sharedTmpBuffer入参传入，使用该tensor作为临时空间进行处理，接口框架不再申请。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。

接口框架申请的方式，开发者需要预留临时空间；通过sharedTmpBuffer传入的情况，开发者需要为tensor申请空间。临时空间大小BufferSize的获取方式如下：通过[SoftmaxFlashV3 Tiling接口](SoftmaxFlashV3_Tiling_interface.md)中提供的GetSoftMaxFlashV3MaxMinTmpSize接口获取所需最小和最大临时空间大小，最小空间可以保证功能正确，最大空间用于提升性能。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 输入srcTensor及输出dstTensor、expMaxTensor操作数的数据类型。支持的数据类型为：half。 |
| U | 输入inMeanTensor、inExpSumTensor、inMaxTensor及输出meanTensor、expSumTensor、maxTensor操作数的数据类型。支持的数据类型为：float。 |
| isUpdate | 是否开启update为true的计算。 |
| isReuseSource | 该参数预留，传入默认值false即可。 |
| isBasicBlock | 该参数预留，传入默认值false即可。 |
| isDataFormatNZ | 该参数预留，传入默认值false即可。 |
| config | 该参数预留，传入默认值SOFTMAX_DEFAULT_CFG即可。 |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstTensor | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>dstTensor的shape和源操作数srcTensor一致。 |
| meanTensor | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>用于保存softmax计算过程中平均值的结果。<br>meanTensor的last轴长度固定为32Byte，即一个datablock长度。该datablock中的所有数据为同一个值。比如float数据类型下，该datablock中的8个数均为相同的reducesum求平均后的值。非last轴的长度与dstTensor保持一致。 |
| expSumTensor | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>用于保存softmax计算过程中reducesum的结果。<br>expSumTensor的last轴长度固定为32Byte，即一个datablock长度。该datablock中的所有数据为同一个值。比如float数据类型下，该datablock中的8个数均为相同的reducesum的值。非last轴的长度与dstTensor保持一致。 |
| maxTensor | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>用于保存softmax计算过程中reducemax的结果。<br>maxTensor的last轴长度固定为32Byte，即一个datablock长度。该datablock中的所有数据为同一个值。比如float数据类型下，该datablock中的8个数均为相同的reducemax的值。非last轴的长度与dstTensor保持一致。 |
| srcTensor | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>last轴长度需要32Byte对齐。 |
| expMaxTensor | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>expMaxTensor的last轴长度固定为32Byte，即一个datablock长度。该datablock中的所有数据为同一个值。比如half数据类型下，该datablock中的16个数均为相同的值。非last轴的长度需要与dstTensor保持一致。 |
| inMeanTensor | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>softmax计算所需要的mean值。<br>inMeanTensor的last轴长度固定为32Byte，即一个datablock长度。该datablock中的所有数据为同一个值。比如float数据类型下，该datablock中的8个数均为相同的值。非last轴的长度需要与dstTensor保持一致。 |
| inExpSumTensor | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>softmax计算所需要的sum值。<br>inExpSumTensor的last轴长度固定为32Byte，即一个datablock长度。该datablock中的所有数据为同一个值。比如float数据类型下，该datablock中的8个数均为相同的值。非last轴的长度需要与dstTensor保持一致。 |
| inMaxTensor | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>softmax计算所需要的max值。<br>inMaxTensor的last轴长度固定为32Byte，即一个datablock长度。该datablock中的所有数据为同一个值。比如float数据类型下，该datablock中的8个数均为相同的值。非last轴的长度需要与dstTensor保持一致。 |
| sharedTmpBuffer | 输入 | 临时空间。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>该操作数的数据类型固定uint8_t。<br><br>接口内部复杂计算时用于存储中间变量，由开发者提供。<br><br>临时空间大小BufferSize的获取方式请参考[SoftmaxFlashV3 Tiling接口](SoftmaxFlashV3_Tiling_interface.md)。 |
| tiling | 输入 | SoftmaxFlashV3接口计算所需Tiling信息，Tiling信息的获取请参考[SoftmaxFlashV3 Tiling接口](SoftmaxFlashV3_Tiling_interface.md)。 |
| params | 输入 | srcTensor的shape信息和计算相关参数。SoftMaxParams类型，具体定义如下方代码所示，其中参数的含义为：<br>srcM：非尾轴长度的乘积。<br>srcK：尾轴长度，必须32Byte对齐。<br>oriSrcM：原始非尾轴长度的乘积。<br>oriSrcK：原始尾轴长度。<br>loopCnt：update为true时，公式中的循环次数loopCnt，该参数大于等于1。<br>splitMeanCnt：公式中计算每一行平均值时的分块个数，当前该参数仅支持取值为8。<br>alpha：公式中的计算参数，推荐取值0.9375、0.96889、0.984497。<br><br>注意，当前本接口不支持非对齐场景，因此参数srcM与oriSrcM相等，参数srcK与oriSrcK相等。 |

```
struct SoftMaxParams {
    uint32_t srcM;
    uint32_t srcK;
    uint32_t oriSrcM;
    uint32_t oriSrcK;
    uint32_t loopCnt;
    uint32_t splitMeanCnt;
    float alpha;
};
```

## 返回值说明

无

## 约束说明

-   操作数地址对齐要求请参见[通用地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。
-   对于输入srcTensor需要满足：尾轴长度n大于等于512，同时n是64的倍数；非尾轴长度的乘积m为8的倍数。
-   srcTensor和dstTensor的Tensor的空间可以复用，meanTensor和inMeanTensor的空间可以复用，maxTensor和inMaxTensor的空间可以复用，expSumTensor和inExpSumTensor的空间可以复用。
-   meanTensor、expSumTensor、maxTensor、expMaxTensor、inMeanTensor、inExpSumTensor、inMaxTensor的Tensor空间，last轴长度必须是32字节。
-   不支持sharedTmpBuffer与源操作数和目的操作数地址重叠。

<!-- npu="950" id7 -->
-   针对Ascend 950PR/Ascend 950DT，接口内部计算对Subnormal的处理方式受编译选项`--cce-ftz`控制（默认值为`true`）：
    -   配置为`false`时，计算过程中保留Subnormal，并按照其实际数值参与后续计算。
    -   配置为`true`时，启用FTZ（Flush-To-Zero）模式，计算过程中产生或参与运算的Subnormal将按0处理，可能导致计算结果与保留Subnormal时存在精度差异。
<!-- end id7 -->

## 调用示例

本样例中输入srcTensor和输出dstTensor的shape大小为\[8, 1024\]，输入inMeanTensor、inExpSumTensor、inMaxTensor的shape大小为\[8, 8\]，数据类型为float；输出expMaxTensor的shape大小为\[8, 16\]，数据类型为half；输入和输出的数据排布格式为ND，srcTensor和dstTensor空间不复用，模板参数isUpdate为true。

```
// dstLocal: 存放SoftMax计算结果的Tensor
// meanLocal：存放softmax计算过程中平均值的结果
// expSumLocal：存放softmax计算过程中reducesum的结果
// maxLocal：存放softmax计算过程中reducemax的结果
// srcLocal：存放SoftMax计算的输入Tensor
// expMaxLocal：存放inmax与reducemax差值的e的指数幂的结果
// inMeanLocal：存放softmax计算所需要的mean值
// inExpSumLocal：存放softmax计算所需要的sum值
// inMaxLocal：存放softmax计算所需要的max值
// sharedTmpBuffer: 存放SoftMax计算过程中临时缓存的Tensor
// softmaxTiling：存放SoftMax计算所需Tiling信息，可通过SoftMaxFlashV3TilingFunc接口获取

AscendC::SoftMaxParams params(
    /* 非尾轴长度的乘积          */ srcM,
    /* 尾轴长度，必须32Bytes对齐 */ srcK,
    /* 原始非尾轴长度的乘积      */ oriSrcM,
    /* 原始尾轴长度              */ oriSrcK,
    /* 循环次数，update为true时大于等于1    */ loopCn,
    /* 每一行平均值时的分块个数，仅支持为8  */ splitMeanCnt,
    /* 计算参数，推荐取值0.9375、0.96889、0.984497 */ alpha);

// 通过sharedTmpBuffer入参传入临时空间
AscendC::SoftmaxFlashV3<T, U, true>(
    dstLocal, meanLocal, expSumLocal, maxLocal, srcLocal, expMaxLocal, inMeanLocal, inExpSumLocal, inMaxLocal,
    sharedTmpBuffer, tiling, params);
// 接口框架申请临时空间
AscendC::SoftmaxFlashV3<T, U, true>(
    dstLocal, meanLocal, expSumLocal, maxLocal, srcLocal, expMaxLocal, inMeanLocal, inExpSumLocal, inMaxLocal, tiling,
    params);
```

结果示例如下：

```
输入数据(srcLocal)：
[[ 0.        0.001221  0.002441 ...  2.496     2.498     2.498   ]
 [ 2.5       2.502     2.502    ...  4.996     4.996     5.      ]
 [ 5.        5.        5.004    ...  7.496     7.496     7.5     ]
 ...
 [12.5      12.5      12.5      ... 15.       15.       15.      ]
 [15.       15.       15.       ... 17.5      17.5      17.5     ]
 [17.5      17.5      17.5      ... 20.       20.       20.      ]]
输入数据(inMeanLocal/inExpSumLocal/inMaxLocal)：
[[0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5]
 [0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5]
 ...
 [0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5]
 [0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5]]
输出数据(dstLocal)：
[[0.0049   0.004906 0.004913 ... 0.998    1.       1.      ]
 [0.00488  0.00489  0.00489  ... 0.996    0.996    1.      ]
 [0.004868 0.004868 0.004887 ... 0.996    0.996    1.      ]
 ...
 [0.004894 0.004894 0.004894 ... 1.       1.       1.      ]
 [0.00472  0.00472  0.00472  ... 1.       1.       1.      ]
 [0.004684 0.004684 0.004684 ... 1.       1.       1.      ]]
```
