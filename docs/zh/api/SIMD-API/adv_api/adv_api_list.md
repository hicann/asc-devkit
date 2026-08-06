# 高阶API列表

## 数据结构

| 类型 | 功能描述 |
| --- | --- |
| [TensorShape](data_structures/TensorShape.md) | 描述高阶API的Host侧Tiling接口使用的Tensor形状。 |
| [TensorDataType](data_structures/TensorDataType.md) | 描述高阶API的Host侧Tiling接口使用的Tensor数据类型。 |

## 数学计算

| 接口名 | 功能描述 |
| --- | --- |
| [Acos](math_compute/Acos_interface/Acos.md) | 按元素做反余弦函数计算。 |
| [Acosh](math_compute/Acosh_interface/Acosh.md) | 按元素做双曲反余弦函数计算。 |
| [Asin](math_compute/Asin_interface/Asin.md) | 按元素做反正弦函数计算。 |
| [Asinh](math_compute/Asinh_interface/Asinh.md) | 按元素做反双曲正弦函数计算。 |
| [Atan](math_compute/Atan_interface/Atan.md) | 按元素做三角函数反正切运算。 |
| [Atanh](math_compute/Atanh_interface/Atanh.md) | 按元素做反双曲正切余弦函数计算。 |
| [Axpy](math_compute/Axpy_interface/Axpy.md) | 源操作数中每个元素与标量求积后和目的操作数中的对应元素相加。 |
| [Ceil](math_compute/Ceil_interface/Ceil.md) | 获取大于或等于x的最小的整数值，即向正无穷取整操作。 |
| [ClampMax](math_compute/Clamp_interface/ClampMax.md) | 将srcTensor中大于scalar的数替换为scalar，小于等于scalar的数保持不变，作为dstTensor输出。 |
| [ClampMin](math_compute/Clamp_interface/ClampMin.md) | 将srcTensor中小于scalar的数替换为scalar，大于等于scalar的数保持不变，作为dstTensor输出。 |
| [Cos](math_compute/Cos_interface/Cos.md) | 按元素做三角函数余弦运算。 |
| [Cosh](math_compute/Cosh_interface/Cosh.md) | 按元素做双曲余弦函数计算。 |
| [CumSum](math_compute/CumSum_interface/CumSum.md) | 对数据按行依次累加或按列依次累加。 |
| [Digamma](math_compute/Digamma_interface/Digamma.md) | 按元素计算x的gamma函数的对数导数。 |
| [Erf](math_compute/Erf_interface/Erf.md) | 按元素做误差函数计算，也称为高斯误差函数。 |
| [Erfc](math_compute/Erfc_interface/Erfc.md) | 返回输入x的互补误差函数结果，积分区间为x到无穷大。 |
| [Exp](math_compute/Exp_interface/Exp.md) | 按元素取自然指数。 |
| [Floor](math_compute/Floor_interface/Floor.md) | 获取小于或等于x的最小的整数值，即向负无穷取整操作。 |
| [Fmod](math_compute/Fmod_interface/Fmod.md) | 按元素计算两个浮点数相除后的余数。 |
| [Frac](math_compute/Frac_interface/Frac.md) | 按元素做取小数计算。 |
| [Hypot](math_compute/Hypot_interface/Hypot.md) | 按元素计算两个浮点数平方和的平方根。 |
| [IsFinite](math_compute/IsFinite_interface/IsFinite.md) | 按元素判断输入的浮点数是否非NAN、非±INF。 |
| [Lgamma](math_compute/Lgamma_interface/Lgamma.md) | 按元素计算x的gamma函数的绝对值并求自然对数。 |
| [Log](math_compute/Log_interface/Log.md) | 按元素以e、2、10为底做对数运算。 |
| [Power](math_compute/Power_interface/Power.md) | 实现按元素做幂运算功能。 |
| [Round](math_compute/Round_interface/Round.md) | 将输入的元素四舍五入到最接近的整数。 |
| [Sign](math_compute/Sign_interface/Sign.md) | 按元素执行Sign操作，Sign是指返回输入数据的符号。 |
| [Sin](math_compute/Sin_interface/Sin.md) | 按元素做正弦函数计算。 |
| [Sinh](math_compute/Sinh_interface/Sinh.md) | 按元素做双曲正弦函数计算。 |
| [Tan](math_compute/Tan_interface/Tan.md) | 按元素做正切函数计算。 |
| [Tanh](math_compute/Tanh_interface/Tanh.md) | 按元素做逻辑回归Tanh。 |
| [Trunc](math_compute/Trunc_interface/Trunc.md) | 按元素做浮点数截断操作，即向零取整操作。 |
| [Xor](math_compute/Xor_interface/Xor.md) | 按元素执行Xor（异或）运算。 |
| [Fma](math_compute/Fma_interface/Fma_interface.md) | 按元素计算两个输入相乘后与第三个输入相加的结果。 |
| [IsNan](math_compute/IsNan_interface/IsNan_interface.md) | 按元素判断输入的浮点数是否为nan。 |
| [IsInf](math_compute/IsInf_interface/IsInf_interface.md) | 按元素判断输入的浮点数是否为±INF。 |
| [Rint](math_compute/Rint_interface/Rint_interface.md) | 获取与输入数据最接近的整数。 |
| [SinCos](math_compute/SinCos_interface/SinCos_interface.md) | 按元素进行正弦计算和余弦计算，分别获得正弦和余弦的结果。 |
| [LogicalNot](math_compute/LogicalNot_interface/LogicalNot.md) | 按元素进行取反操作。 |
| [LogicalAnd](math_compute/LogicalAnd_interface/LogicalAnd.md) | 按元素进行与操作。 |
| [LogicalAnds](math_compute/LogicalAnds_interface/LogicalAnds.md) | 输入矢量内的每个元素与标量进行与操作。 |
| [LogicalOr](math_compute/LogicalOr_interface/LogicalOr.md) | 按元素进行或操作。 |
| [LogicalOrs](math_compute/LogicalOrs_interface/LogicalOrs.md) | 输入矢量内的每个元素与标量进行或操作。 |
| [LogicalXor](math_compute/LogicalXor_interface/LogicalXor.md) | 按元素进行逻辑异或操作。 |
| [BitwiseNot](math_compute/BitwiseNot_interface/BitwiseNot.md) | 逐比特对输入进行取反。 |
| [BitwiseAnd](math_compute/BitwiseAnd_interface/BitwiseAnd.md) | 逐比特对两个输入进行与操作。 |
| [BitwiseOr](math_compute/BitwiseOr_interface/BitwiseOr.md) | 逐比特对两个输入进行或操作。 |
| [BitwiseXor](math_compute/BitwiseXor_interface/BitwiseXor.md) | 逐比特对两个输入进行异或操作。 |
| [Where](math_compute/Where_interface/Where.md) | 根据指定的条件，从两个源操作数中选择元素，生成目标操作数。 |

## 量化操作

| 接口名 | 功能描述 |
| --- | --- |
| [AntiQuantize](quantization/AntiQuantize.md) | 按元素做伪量化计算，比如将int8_t数据类型伪量化为half数据类型。 |
| [AscendAntiQuant](quantization/AscendAntiQuant.md) | 按元素做伪量化计算，比如将int8_t数据类型伪量化为half数据类型。 |
| [Dequantize](quantization/Dequantize.md) | 按元素做反量化计算，比如将int32_t数据类型反量化为half/float等数据类型。 |
| [AscendDequant](quantization/AscendDequant.md) | 按元素做反量化计算，比如将int32_t数据类型反量化为half/float等数据类型。 |
| [Quantize](quantization/Quantize.md) | 按元素做量化计算，比如将half/float数据类型量化为int8_t数据类型。 |
| [AscendQuant](quantization/AscendQuant.md) | 按元素做量化计算，比如将half/float数据类型量化为int8_t数据类型。 |

##  归一化操作

| 接口名 | 功能描述 |
| --- | --- |
| [BatchNorm](normalization/BatchNorm.md) | 对于每个batch中的样本，对其输入的每个特征在batch的维度上进行归一化。 |
| [DeepNorm](normalization/DeepNorm.md) | 在深层神经网络训练过程中，可以替代LayerNorm的一种归一化方法。 |
| [GroupNorm](normalization/GroupNorm.md) | 将输入的C维度分为groupNum组，对每一组数据进行标准化。 |
| [LayerNorm](normalization/LayerNorm.md) | 将输入数据收敛到[0, 1]之间，可以规范网络层输入输出数据分布的一种归一化方法。 |
| [LayerNorm-Tiling](normalization/LayerNorm-Tiling.md) | LayerNorm的Tiling参数配置接口，用于获取LayerNorm kernel计算时所需的Tiling参数。 |
| [LayerNormGrad](normalization/LayerNormGrad.md) | 用于计算LayerNorm的反向传播梯度。 |
| [LayerNormGrad-Tiling](normalization/LayerNormGrad-Tiling.md) | LayerNormGrad的Tiling参数配置接口，用于设置反向传播梯度计算所需的分片参数。 |
| [LayerNormGradBeta](normalization/LayerNormGradBeta.md) | 用于获取反向beta/gmma的数值，和LayerNormGrad共同输出pdx, gmma和beta。 |
| [LayerNormGradBeta-Tiling](normalization/LayerNormGradBeta-Tiling.md) | LayerNormGradBeta的Tiling参数配置接口。 |
| [Normalize](normalization/Normalize.md) | [LayerNorm](normalization/LayerNorm.md)中，已知均值和方差，计算shape为[A，R]的输入数据的标准差的倒数rstd和归一化输出y。 |
| [RmsNorm](normalization/RmsNorm.md) | 实现对shape大小为[B，S，H]的输入数据的RmsNorm归一化。 |
| [WelfordUpdate](normalization/WelfordUpdate.md) | 实现Welford算法的前处理。 |
| [WelfordFinalize](normalization/WelfordFinalize.md) | 实现Welford算法的后处理。 |

##  激活函数

| 接口名 | 功能描述 |
| --- | --- |
| [AdjustSoftMaxRes](activation_functions/SoftMax_interface/AdjustSoftMaxRes.md) | 用于对SoftMax相关计算结果做后处理，调整SoftMax的计算结果为指定的值。 |
| [FasterGelu](activation_functions/Gelu_interface/FasterGelu.md) | FastGelu化简版本的一种激活函数。 |
| [FasterGeluV2](activation_functions/Gelu_interface/FasterGeluV2.md) | 实现FastGeluV2版本的一种激活函数。 |
| [GeGLU](activation_functions/GeGLU_interface/GeGLU.md) | 采用GeLU作为激活函数的GLU变体。 |
| [Gelu](activation_functions/Gelu_interface/Gelu.md) | GELU是一个重要的激活函数，其灵感来源于relu和dropout，在激活中引入了随机正则的思想。 |
| [LogSoftMax](activation_functions/LogSoftMax_interface/LogSoftMax.md) | 对输入tensor做LogSoftmax计算。 |
| [ReGlu](activation_functions/ReGlu_interface/ReGlu.md) | 一种GLU变体，使用Relu作为激活函数。 |
| [Sigmoid](activation_functions/Sigmoid_interface/Sigmoid.md) | 按元素做逻辑回归Sigmoid。 |
| [Silu](activation_functions/Silu_interface/Silu.md) | 按元素做Silu运算。 |
| [SimpleSoftMax](activation_functions/SoftMax_interface/SimpleSoftMax.md) | 使用计算好的sum和max数据对输入tensor做softmax计算。 |
| [SoftMax](activation_functions/SoftMax_interface/SoftMax.md) | 对输入tensor按行做Softmax计算。 |
| [SoftmaxFlash](activation_functions/SoftMax_interface/SoftmaxFlash.md) | SoftMax增强版本，除了可以对输入tensor做softmaxflash计算，还可以根据上一次softmax计算的sum和max来更新本次的softmax计算结果。 |
| [SoftmaxFlashV2](activation_functions/SoftMax_interface/SoftmaxFlashV2.md) | SoftmaxFlash增强版本，对应FlashAttention-2算法。 |
| [SoftmaxFlashV3](activation_functions/SoftMax_interface/SoftmaxFlashV3.md) | SoftmaxFlash增强版本，对应Softmax PASA算法。 |
| [SoftmaxGrad](activation_functions/SoftMax_interface/SoftmaxGrad.md) | 对输入tensor做grad反向计算的一种方法。 |
| [SoftmaxGradFront](activation_functions/SoftMax_interface/SoftmaxGradFront.md) | 对输入tensor做grad反向计算的一种方法。 |
| [SwiGLU](activation_functions/SwiGLU_interface/SwiGLU.md) | 采用Swish作为激活函数的GLU变体。 |
| [Swish](activation_functions/Swish_interface/Swish.md) | 神经网络中的Swish激活函数。 |

##  归约操作

| 接口名 | 功能描述 |
| --- | --- |
| [Sum](reduction_operations/Sum_interface/Sum.md) | 获取最后一个维度的元素总和。 |
| [Mean](reduction_operations/Mean_interface/Mean.md) | 根据最后一轴的方向对各元素求平均值。 |
| [ReduceXorSum](reduction_operations/ReduceXorSum_interface/ReduceXorSum.md) | 按照元素执行Xor（按位异或）运算，并将计算结果ReduceSum求和。 |
| [ReduceSum](reduction_operations/ReduceSum_interface/ReduceSum.md) | 对一个多维向量按照指定的维度进行数据累加。 |
| [ReduceMean](reduction_operations/ReduceMean_interface/ReduceMean.md) | 对一个多维向量按照指定的维度求平均值。 |
| [ReduceMax](reduction_operations/ReduceMax_interface/ReduceMax.md) | 对一个多维向量在指定的维度求最大值。 |
| [ReduceMin](reduction_operations/ReduceMin_interface/ReduceMin.md) | 对一个多维向量在指定的维度求最小值。 |
| [ReduceAny](reduction_operations/ReduceAny_interface/ReduceAny.md) | 对一个多维向量在指定的维度求逻辑或。 |
| [ReduceAll](reduction_operations/ReduceAll_interface/ReduceAll.md) | 对一个多维向量在指定的维度求逻辑与。 |
| [ReduceProd](reduction_operations/ReduceProd_interface/ReduceProd.md) | 对一个多维向量在指定的维度求积。 |

##  排序操作

| 接口名 | 功能描述 |
| --- | --- |
| [TopK](sort_operations/TopK.md) | 获取最后一个维度的前k个最大值或最小值及其对应的索引。 |
| [Concat](sort_operations/Concat.md) | 对数据进行预处理，将要排序的源操作数srcLocal一一对应的合入目标数据concatLocal中，数据预处理完后，可以进行Sort。 |
| [Extract](sort_operations/Extract.md) | 处理Sort的结果数据，输出排序后的value和index。 |
| [Sort](sort_operations/Sort.md) | 排序函数，按照数值大小进行降序排序。 |
| [MrgSort](sort_operations/MrgSort.md) | 将已经排好序的最多4条队列，合并排列成1条队列，结果按照score域由大到小排序。 |

##  数据过滤

| 接口名 | 功能描述 |
| --- | --- |
| [Select](data_filter/Select.md) | 给定两个源操作数src0和src1，根据maskTensor相应位置的值（非bit位）选取元素，得到目的操作数dst。 |
| [DropOut](data_filter/DropOut.md) | 提供根据MaskTensor对源操作数进行过滤的功能，得到目的操作数。 |

##  张量变换

| 接口名 | 功能描述 |
| --- | --- |
| [Transpose](tensor_transform/Transpose.md) | 对输入数据进行数据排布及Reshape操作。 |
| [TransData](tensor_transform/TransData.md) | 将输入数据的排布格式转换为目标排布格式。 |
| [Broadcast](tensor_transform/Broadcast.md) | 将输入按照输出shape进行广播。 |
| [Pad](tensor_transform/Pad.md) | 对height * width的二维Tensor在width方向上pad到32B对齐。 |
| [UnPad](tensor_transform/UnPad.md) | 对height * width的二维Tensor在width方向上进行unpad。 |
| [Fill](tensor_transform/Fill.md) | 将Global Memory上的数据初始化为指定值。 |

##  索引计算

| 接口名 | 功能描述 |
| --- | --- |
| [Arange](index_compute/Arange.md) | 给定起始值，等差值和长度，返回一个等差数列。 |

##  矩阵计算

| 接口名 | 功能描述 |
| --- | --- |
| [Matmul](cube_compute/Matmul_Kernel/Matmul_Kernel.md) | Matmul矩阵乘法的运算。 |

##  HCCL通信类

| 接口名 | 功能描述 |
| --- | --- |
| [HCCL通信类](HCCL_communication/HCCL_communication.md) | 在AI Core侧编排集合通信任务。 |

##  卷积计算

| 接口名 | 功能描述 |
| --- | --- |
| [Conv3D](convolution_compute/Conv3D_Kernel/Conv3d_Kernel.md) | 3维卷积正向矩阵运算。 |
| [Conv3DBackpropInput](convolution_compute/Conv3DBackpropInput_Kernel/Conv3DBackpropInput_Kernel.md) | 卷积的反向运算，求解特征矩阵的反向传播误差。 |
| [Conv3DBackpropFilter](convolution_compute/Conv3DBackpropFilter_Kernel/Conv3DBackpropFilter_Kernel.md) | 卷积的反向运算，求解权重的反向传播误差。 |

##  随机函数

| 接口名 | 功能描述 |
| --- | --- |
| [PhiloxRandom](random_functions/PhiloxRandom.md) | 基于Philox随机数生成算法，给定随机数种子，生成若干的随机数。 |
