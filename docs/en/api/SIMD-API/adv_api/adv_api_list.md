# Advanced API List

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-10T06:38:49.962Z pushedAt=2026-11-18T11:07:26.933Z nl=2 -->

## Data Structures

| Type | Description |
| --- | --- |
| [TensorShape](data_structures/TensorShape.md) | Describes the tensor shape used by the Host-side Tiling API of the high-level API. |
| [TensorDataType](data_structures/TensorDataType.md) | Describes the tensor data type used by the Host-side Tiling API of the high-level API. |

## Mathematical Computation

| API | Description |
| --- | --- |
| [Acos](math_compute/Acos_interface/Acos.md) | Element-wise operation: computes the inverse cosine function. |
| [Acosh](math_compute/Acosh_interface/Acosh.md) | Element-wise operation: computes the inverse hyperbolic cosine function. |
| [Asin](math_compute/Asin_interface/Asin.md) | Element-wise operation: computes the inverse sine function. |
| [Asinh](math_compute/Asinh_interface/Asinh.md) | Element-wise operation: computes the inverse hyperbolic sine function. |
| [Atan](math_compute/Atan_interface/Atan.md) | Element-wise operation: computes the arctangent function. |
| [Atanh](math_compute/Atanh_interface/Atanh.md) | Element-wise operation: computes the inverse hyperbolic tangent function. |
| [Axpy](math_compute/Axpy_interface/Axpy.md) | Multiplies each element in the source operand by a scalar and adds the result to the corresponding element in the destination operand. |
| [Ceil](math_compute/Ceil_interface/Ceil.md) | Obtains the smallest integer greater than or equal to x, that is, rounds toward positive infinity. |
| [ClampMax](math_compute/Clamp_interface/ClampMax.md) | Replaces the elements in srcTensor that are greater than scalar with scalar, and keeps the elements that are less than or equal to scalar unchanged, outputting the result as dstTensor. |
| [ClampMin](math_compute/Clamp_interface/ClampMin.md) | Replaces the elements in srcTensor that are smaller than scalar with scalar, and keeps the elements that are greater than or equal to scalar unchanged, outputting the result as dstTensor. |
| [Cos](math_compute/Cos_interface/Cos.md) | Element-wise operation: computes the cosine function. |
| [Cosh](math_compute/Cosh_interface/Cosh.md) | Element-wise operation: computes the hyperbolic cosine function. |
| [CumSum](math_compute/CumSum_interface/CumSum.md) | Performs cumulative summation on data row by row or column by column. |
| [Digamma](math_compute/Digamma_interface/Digamma.md) | Element-wise operation: computes the logarithmic derivative of the gamma function of x. |
| [Erf](math_compute/Erf_interface/Erf.md) | Element-wise operation: computes the error function, also known as the Gaussian error function. |
| [Erfc](math_compute/Erfc_interface/Erfc.md) | Returns the complementary error function of the input x, with the integration interval from x to positive infinity. |
| [Exp](math_compute/Exp_interface/Exp.md) | Element-wise operation: computes the natural exponential. |
| [Floor](math_compute/Floor_interface/Floor.md) | Obtains the smallest integer less than or equal to x, that is, rounds toward negative infinity. |
| [Fmod](math_compute/Fmod_interface/Fmod.md) | Element-wise operation: computes the remainder after dividing two floating-point numbers. |
| [Frac](math_compute/Frac_interface/Frac.md) | Element-wise operation: computes the fractional part. |
| [Hypot](math_compute/Hypot_interface/Hypot.md) | Element-wise operation: computes the square root of the sum of squares of two floating-point numbers. |
| [IsFinite](math_compute/IsFinite_interface/IsFinite.md) | Element-wise comparison: determines whether the input floating-point number is neither NAN nor ±INF. |
| [Lgamma](math_compute/Lgamma_interface/Lgamma.md) | Element-wise operation: computes the absolute value of the gamma function of x and takes the natural logarithm. |
| [Log](math_compute/Log_interface/Log.md) | Element-wise operation: computes the logarithm with base e, 2, or 10. |
| [Power](math_compute/Power_interface/Power.md) | Implements element-wise exponentiation. |
| [Round](math_compute/Round_interface/Round.md) | Rounds the input elements to the nearest integer. |
| [Sign](math_compute/Sign_interface/Sign.md) | Element-wise operation: performs the Sign operation, which returns the sign of the input data. |
| [Sin](math_compute/Sin_interface/Sin.md) | Element-wise operation: computes the sine function. |
| [Sinh](math_compute/Sinh_interface/Sinh.md) | Element-wise operation: computes the hyperbolic sine function. |
| [Tan](math_compute/Tan_interface/Tan.md) | Element-wise operation: computes the tangent function. |
| [Tanh](math_compute/Tanh_interface/Tanh.md) | Element-wise operation: computes the Tanh function. |
| [Trunc](math_compute/Trunc_interface/Trunc.md) | Element-wise operation: truncates the floating-point number, that is, rounds toward zero. |
| [Xor](math_compute/Xor_interface/Xor.md) | Element-wise operation: performs the Xor (exclusive OR) operation. |
| [Fma](math_compute/Fma_interface/Fma_interface.md) | Element-wise operation: computes the result of multiplying two inputs and then adding a third input. |
| [IsNan](math_compute/IsNan_interface/IsNan_interface.md) | Element-wise comparison: determines whether the input floating-point number is nan. |
| [IsInf](math_compute/IsInf_interface/IsInf_interface.md) | Element-wise comparison: determines whether the input floating-point number is ±INF. |
| [Rint](math_compute/Rint_interface/Rint_interface.md) | Obtains the integer closest to the input data. |
| [SinCos](math_compute/SinCos_interface/SinCos_interface.md) | Performs sine and cosine computation element-wise to obtain the sine and cosine results respectively. |
| [LogicalNot](math_compute/LogicalNot_interface/LogicalNot.md) | Element-wise operation: performs the logical NOT operation. |
| [LogicalAnd](math_compute/LogicalAnd_interface/LogicalAnd.md) | Element-wise operation: performs the logical AND operation. |
| [LogicalAnds](math_compute/LogicalAnds_interface/LogicalAnds.md) | Performs the logical AND operation between each element in the input vector and a scalar. |
| [LogicalOr](math_compute/LogicalOr_interface/LogicalOr.md) | Element-wise operation: performs the logical OR operation. |
| [LogicalOrs](math_compute/LogicalOrs_interface/LogicalOrs.md) | Performs the logical OR operation between each element in the input vector and a scalar. |
| [LogicalXor](math_compute/LogicalXor_interface/LogicalXor.md) | Element-wise operation: performs the logical XOR operation. |
| [BitwiseNot](math_compute/BitwiseNot_interface/BitwiseNot.md) | Performs a bitwise NOT operation on the input. |
| [BitwiseAnd](math_compute/BitwiseAnd_interface/BitwiseAnd.md) | Performs a bitwise AND operation on two inputs. |
| [BitwiseOr](math_compute/BitwiseOr_interface/BitwiseOr.md) | Performs a bitwise OR operation on two inputs. |
| [BitwiseXor](math_compute/BitwiseXor_interface/BitwiseXor.md) | Performs a bitwise XOR operation on two inputs. |
| [Where](math_compute/Where_interface/Where.md) | Selects elements from two source operands based on the specified condition to generate the destination operand. |

## Quantization Operations

| API | Description |
| --- | --- |
| [AntiQuantize](quantization/AntiQuantize.md) | Element-wise pseudo-quantization, for example, pseudo-quantizing the int8_t data type to the half data type. |
| [AscendAntiQuant](quantization/AscendAntiQuant.md) | Element-wise pseudo-quantization, for example, pseudo-quantizing the int8_t data type to the half data type. |
| [Dequantize](quantization/Dequantize.md) | Element-wise dequantization, for example, dequantizing the int32_t data type to data types such as half/float. |
| [AscendDequant](quantization/AscendDequant.md) | Element-wise dequantization, for example, dequantizing the int32_t data type to data types such as half/float. |
| [Quantize](quantization/Quantize.md) | Element-wise quantization, for example, quantizing the half/float data type to the int8_t data type. |
| [AscendQuant](quantization/AscendQuant.md) | Element-wise quantization, for example, quantizing the half/float data type to the int8_t data type. |

##  Normalization Operations

| API | Description |
| --- | --- |
| [BatchNorm](normalization/BatchNorm.md) | For each sample in a batch, normalizes each feature of its input along the batch dimension. |
| [DeepNorm](normalization/DeepNorm.md) | A normalization method that can replace LayerNorm during deep neural network training. |
| [GroupNorm](normalization/GroupNorm.md) | Divides the C dimension of the input into groupNum groups and standardizes the data in each group. |
| [LayerNorm](normalization/LayerNorm.md) | A normalization method that converges the input data to the range [0, 1] and normalizes the input and output data distribution of a network layer. |
| [LayerNorm-Tiling](normalization/LayerNorm-Tiling.md) | Tiling parameter configuration API for LayerNorm, used to obtain the Tiling parameters required for LayerNorm kernel-side computation. |
| [LayerNormGrad](normalization/LayerNormGrad.md) | Computes the backpropagation gradient of LayerNorm. |
| [LayerNormGrad-Tiling](normalization/LayerNormGrad-Tiling.md) | Tiling parameter configuration API for LayerNormGrad, used to set the tiling parameters required for backpropagation gradient computation. |
| [LayerNormGradBeta](normalization/LayerNormGradBeta.md) | Obtains the values of the backward beta/gmma and, together with LayerNormGrad, outputs pdx, gmma, and beta. |
| [LayerNormGradBeta-Tiling](normalization/LayerNormGradBeta-Tiling.md) | Tiling parameter configuration API for LayerNormGradBeta. |
| [Normalize](normalization/Normalize.md) | In [LayerNorm](normalization/LayerNorm.md), given the mean and variance, computes the reciprocal of the standard deviation rstd and the normalized output y for input data of shape [A, R]. |
| [RmsNorm](normalization/RmsNorm.md) | Implements RmsNorm normalization on input data of shape [B, S, H]. |
| [WelfordUpdate](normalization/WelfordUpdate.md) | Implements the preprocessing of the Welford algorithm. |
| [WelfordFinalize](normalization/WelfordFinalize.md) | Implements the postprocessing of the Welford algorithm. |

##  Activation Functions

| API | Description |
| --- | --- |
| [AdjustSoftMaxRes](activation_functions/SoftMax_interface/AdjustSoftMaxRes.md) | Post-processes the SoftMax computation result and adjusts it to a specified value. |
| [FasterGelu](activation_functions/Gelu_interface/FasterGelu.md) | An activation function that is a simplified variant of FastGelu. |
| [FasterGeluV2](activation_functions/Gelu_interface/FasterGeluV2.md) | An activation function that implements the FastGeluV2 version. |
| [GeGLU](activation_functions/GeGLU_interface/GeGLU.md) | A GLU variant that uses GeLU as the activation function. |
| [Gelu](activation_functions/Gelu_interface/Gelu.md) | GELU is an important activation function inspired by ReLU and dropout, which introduces the idea of stochastic regularization into activation. |
| [LogSoftMax](activation_functions/LogSoftMax_interface/LogSoftMax.md) | Performs LogSoftmax computation on the input tensor. |
| [ReGlu](activation_functions/ReGlu_interface/ReGlu.md) | A GLU variant that uses ReLU as the activation function. |
| [Sigmoid](activation_functions/Sigmoid_interface/Sigmoid.md) | Performs element-wise logistic regression Sigmoid. |
| [Silu](activation_functions/Silu_interface/Silu.md) | Performs element-wise SiLU operation. |
| [SimpleSoftMax](activation_functions/SoftMax_interface/SimpleSoftMax.md) | Performs softmax computation on the input tensor using the precomputed sum and max data. |
| [SoftMax](activation_functions/SoftMax_interface/SoftMax.md) | Performs Softmax computation on the input tensor row by row. |
| [SoftmaxFlash](activation_functions/SoftMax_interface/SoftmaxFlash.md) | An enhanced version of SoftMax. In addition to performing softmaxflash computation on the input tensor, it can update the current softmax result based on the sum and max from the previous softmax computation. |
| [SoftmaxFlashV2](activation_functions/SoftMax_interface/SoftmaxFlashV2.md) | An enhanced version of SoftmaxFlash, corresponding to the FlashAttention-2 algorithm. |
| [SoftmaxFlashV3](activation_functions/SoftMax_interface/SoftmaxFlashV3.md) | An enhanced version of SoftmaxFlash, corresponding to the Softmax PASA algorithm. |
| [SoftmaxGrad](activation_functions/SoftMax_interface/SoftmaxGrad.md) | A method for performing backward gradient computation on the input tensor. |
| [SoftmaxGradFront](activation_functions/SoftMax_interface/SoftmaxGradFront.md) | A method for performing backward gradient computation on the input tensor. |
| [SwiGLU](activation_functions/SwiGLU_interface/SwiGLU.md) | A GLU variant that uses Swish as the activation function. |
| [Swish](activation_functions/Swish_interface/Swish.md) | The Swish activation function in neural networks. |

##  Reduction Operations

| API | Description |
| --- | --- |
| [Sum](reduction_operations/Sum_interface/Sum.md) | Obtains the sum of elements in the last dimension. |
| [Mean](reduction_operations/Mean_interface/Mean.md) | Calculates the average of elements along the last axis. |
| [ReduceXorSum](reduction_operations/ReduceXorSum_interface/ReduceXorSum.md) | Performs an Xor (bitwise exclusive OR) operation element-wise, and then performs ReduceSum on the computed results. |
| [ReduceSum](reduction_operations/ReduceSum_interface/ReduceSum.md) | Accumulates data of a multidimensional vector along a specified dimension. |
| [ReduceMean](reduction_operations/ReduceMean_interface/ReduceMean.md) | Computes the mean of a multidimensional vector along a specified dimension. |
| [ReduceMax](reduction_operations/ReduceMax_interface/ReduceMax.md) | Computes the maximum value of a multidimensional vector along a specified dimension. |
| [ReduceMin](reduction_operations/ReduceMin_interface/ReduceMin.md) | Computes the minimum value of a multidimensional vector along a specified dimension. |
| [ReduceAny](reduction_operations/ReduceAny_interface/ReduceAny.md) | Performs a logical OR reduction on a multidimensional vector along a specified dimension. |
| [ReduceAll](reduction_operations/ReduceAll_interface/ReduceAll.md) | Performs a logical AND reduction on a multidimensional vector along a specified dimension. |
| [ReduceProd](reduction_operations/ReduceProd_interface/ReduceProd.md) | Computes the product of a multidimensional vector along a specified dimension. |

##  Sorting Operations

| API | Description |
| --- | --- |
| [TopK](sort_operations/TopK.md) | Obtains the top k largest or smallest values in the last dimension and their corresponding indices. |
| [Concat](sort_operations/Concat.md) | Preprocesses data by merging the source operands srcLocal to be sorted into the target data concatLocal in a one-to-one correspondence. After data preprocessing, Sort can be performed. |
| [Extract](sort_operations/Extract.md) | Processes the result data of Sort and outputs the sorted values and indices. |
| [Sort](sort_operations/Sort.md) | Sorting function that sorts values in descending order. |
| [MrgSort](sort_operations/MrgSort.md) | Merges up to four sorted queues into one queue, with the result sorted by the score field in descending order. |

##  Data Filtering

| API | Description |
| --- | --- |
| [Select](data_filter/Select.md) | Given two source operands src0 and src1, selects elements based on the value (non-bit) at the corresponding position of maskTensor to obtain the destination operand dst. |
| [DropOut](data_filter/DropOut.md) | Filters the source operand based on MaskTensor to obtain the destination operand. |

##  Tensor Transformation

| API | Description |
| --- | --- |
| [Transpose](tensor_transform/Transpose.md) | Performs data layout and reshape operations on the input data. |
| [TransData](tensor_transform/TransData.md) | Converts the layout format of the input data to the target layout format. |
| [Broadcast](tensor_transform/Broadcast.md) | Broadcasts the input according to the output shape. |
| [Pad](tensor_transform/Pad.md) | Pads a two-dimensional tensor of height * width along the width dimension to 32B alignment. |
| [UnPad](tensor_transform/UnPad.md) | Unpads a two-dimensional tensor of height * width along the width dimension. |
| [Fill](tensor_transform/Fill.md) | Initializes the data in Global Memory to a specified value. |

##  Index Computation

| API | Description |
| --- | --- |
| [Arange](index_compute/Arange.md) | Returns an arithmetic sequence given the start value, common difference, and length. |

##  Cube Computation

| API | Description |
| --- | --- |
| [Matmul](cube_compute/Matmul_Kernel/Matmul_Kernel.md) | Matmul cube multiplication operation. |

##  HCCL Communication

| API | Description |
| --- | --- |
| [HCCL Communication](HCCL_communication/HCCL_communication.md) | Orchestrates collective communication tasks on the AI Core side. |

##  Convolution Computation

| API | Description |
| --- | --- |
| [Conv3D](convolution_compute/Conv3D_Kernel/Conv3d_Kernel.md) | 3D convolution forward matrix operation. |
| [Conv3DBackpropInput](convolution_compute/Conv3DBackpropInput_Kernel/Conv3DBackpropInput_Kernel.md) | Backward operation of convolution, computing the backpropagation error of the feature matrix. |
| [Conv3DBackpropFilter](convolution_compute/Conv3DBackpropFilter_Kernel/Conv3DBackpropFilter_Kernel.md) | Backward operation of convolution, computing the backpropagation error of the weights. |

##  Random Functions

| API | Description |
| --- | --- |
| [PhiloxRandom](random_functions/PhiloxRandom.md) | Generates a specified number of random numbers based on the Philox random number generation algorithm, given a random seed. |
