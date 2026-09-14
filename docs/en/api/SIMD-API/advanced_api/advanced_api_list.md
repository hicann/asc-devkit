# List of Advanced APIs

<!-- md-trans-meta sourceCommit=e59e1725727133ce76073dba21b022a1b0c9fd56 translatedAt=2026-09-12T06:22:19.509Z -->

## Mathematical Computation

| API | Description |
| --- | --- |
| [Acos](math_compute/Acos_api/Acos.md) | Performs element-wise arccosine computation. |
| [Acosh](math_compute/Acosh_api/Acosh.md) | Performs element-wise inverse hyperbolic cosine computation. |
| [Asin](math_compute/Asin_api/Asin.md) | Performs element-wise arcsine computation. |
| [Asinh](math_compute/Asinh_api/Asinh.md) | Performs element-wise inverse hyperbolic sine computation. |
| [Atan](math_compute/Atan_api/Atan.md) | Performs element-wise arctangent computation. |
| [Atanh](math_compute/Atanh_api/Atanh.md) | Performs element-wise inverse hyperbolic tangent computation. |
| [Axpy](math_compute/Axpy_api/Axpy-80.md) | Multiplies each element of the source operand by a scalar and adds the result to the corresponding element of the destination operand. |
| [Ceil](math_compute/Ceil_api/Ceil.md) | Obtains the smallest integer greater than or equal to x, that is, rounds toward positive infinity. |
| [ClampMax](math_compute/Clamp_api/ClampMax.md) | Replaces values in srcTensor greater than scalar with scalar, keeps values less than or equal to scalar unchanged, and outputs the result as dstTensor. |
| [ClampMin](math_compute/Clamp_api/ClampMin.md) | Replaces values in srcTensor less than scalar with scalar, keeps values greater than or equal to scalar unchanged, and outputs the result as dstTensor. |
| [Cos](math_compute/Cos_api/Cos.md) | Performs element-wise cosine computation. |
| [Cosh](math_compute/Cosh_api/Cosh.md) | Performs element-wise hyperbolic cosine computation. |
| [CumSum](math_compute/CumSum_api/CumSum.md) | Performs cumulative summation row by row or column by column. |
| [Digamma](math_compute/Digamma_api/Digamma.md) | Computes the logarithmic derivative of the gamma function of x element-wise. |
| [Erf](math_compute/Erf_api/Erf.md) | Performs element-wise error function computation, also known as the Gaussian error function. |
| [Erfc](math_compute/Erfc_api/Erfc.md) | Returns the complementary error function result of input x, with the integration interval from x to infinity. |
| [Exp](math_compute/Exp_api/Exp-81.md) | Computes the natural exponential element-wise. |
| [Floor](math_compute/Floor_api/Floor.md) | Obtains the smallest integer less than or equal to x, that is, rounds toward negative infinity. |
| [Fmod](math_compute/Fmod_api/Fmod.md) | Computes the remainder of the division of two floating-point numbers element-wise. |
| [Frac](math_compute/Frac_api/Frac.md) | Performs element-wise fractional part computation. |
| [Hypot](math_compute/Hypot_api/Hypot.md) | Computes the square root of the sum of squares of two floating-point numbers element-wise. |
| [IsFinite](math_compute/IsFinite_api/IsFinite.md) | Determines element-wise whether the input floating-point number is neither NAN nor ±INF. |
| [Lgamma](math_compute/Lgamma_api/Lgamma.md) | Computes the absolute value of the gamma function of x and takes the natural logarithm element-wise. |
| [Log](math_compute/Log_api/Log-79.md) | Performs element-wise logarithm computation with base e, 2, or 10. |
| [Power](math_compute/Power_api/Power.md) | Implements element-wise power computation. |
| [Round](math_compute/Round_api/Round.md) | Rounds the input elements to the nearest integer. |
| [Sign](math_compute/Sign_api/Sign.md) | Performs the Sign operation element-wise, where Sign returns the sign of the input data. |
| [Sin](math_compute/Sin_api/Sin.md) | Performs element-wise sine computation. |
| [Sinh](math_compute/Sinh_api/Sinh.md) | Performs element-wise hyperbolic sine computation. |
| [Tan](math_compute/Tan_api/Tan.md) | Performs element-wise tangent computation. |
| [Tanh](math_compute/Tanh_api/Tanh.md) | Performs element-wise logistic regression Tanh. |
| [Trunc](math_compute/Trunc_api/Trunc.md) | Performs element-wise floating-point truncation, that is, rounds toward zero. |
| [Xor](math_compute/Xor_api/Xor-82.md) | Performs the Xor (exclusive OR) operation element-wise. |
| [Fma](math_compute/Fma_api/fma_api.md) | Computes the result of multiplying two inputs and adding the product to a third input element-wise. |
| [IsNan](math_compute/IsNan_api/isnan_api.md) | Determines element-wise whether the input floating-point number is NaN. |
| [IsInf](math_compute/IsInf_api/isinf_api.md) | Determines element-wise whether the input floating-point number is ±INF. |
| [Rint](math_compute/Rint_api/rint_api.md) | Obtains the integer closest to the input data. |
| [SinCos](math_compute/SinCos_api/sincos_api.md) | Performs sine and cosine computation element-wise, obtaining the sine and cosine results respectively. |
| [LogicalNot](math_compute/LogicalNot_api/LogicalNot.md) | Performs element-wise logical NOT operation. |
| [LogicalAnd](math_compute/LogicalAnd_api/LogicalAnd.md) | Performs element-wise logical AND operation. |
| [LogicalAnds](math_compute/LogicalAnds_api/LogicalAnds.md) | Performs logical AND operation between each element in the input vector and a scalar. |
| [LogicalOr](math_compute/LogicalOr_api/LogicalOr.md) | Performs element-wise logical OR operation. |
| [LogicalOrs](math_compute/LogicalOrs_api/LogicalOrs.md) | Performs logical OR operation between each element in the input vector and a scalar. |
| [LogicalXor](math_compute/LogicalXor_api/LogicalXor.md) | Performs element-wise logical XOR operation. |
| [BitwiseNot](math_compute/BitwiseNot_api/BitwiseNot.md) | Performs bitwise NOT on the input. |
| [BitwiseAnd](math_compute/BitwiseAnd_api/BitwiseAnd.md) | Performs bitwise AND on two inputs. |
| [BitwiseOr](math_compute/BitwiseOr_api/BitwiseOr.md) | Performs bitwise OR on two inputs. |
| [BitwiseXor](math_compute/BitwiseXor_api/BitwiseXor.md) | Performs bitwise XOR on two inputs. |
| [Where](math_compute/Where_api/Where.md) | Selects elements from two source operands based on the specified condition to generate the destination operand. |

## Quantization Operations

| API | Description |
| --- | --- |
| [AntiQuantize](quantization_operations/AntiQuantize.md) | Element-wise pseudo-quantization computation, for example, pseudo-quantizing the int8_t data type to the half data type. |
| [AscendAntiQuant](quantization_operations/AscendAntiQuant.md) | Element-wise pseudo-quantization computation, for example, pseudo-quantizing the int8_t data type to the half data type. |
| [Dequantize](quantization_operations/Dequantize.md) | Element-wise dequantization computation, for example, dequantizing the int32_t data type to data types such as half/float. |
| [AscendDequant](quantization_operations/AscendDequant.md) | Element-wise dequantization computation, for example, dequantizing the int32_t data type to data types such as half/float. |
| [Quantize](quantization_operations/Quantize.md) | Element-wise quantization computation, for example, quantizing the half/float data type to the int8_t data type. |
| [AscendQuant](quantization_operations/AscendQuant.md) | Element-wise quantization computation, for example, quantizing the half/float data type to the int8_t data type. |

## Normalization Operations

| API | Description |
| --- | --- |
| [BatchNorm](normalization/BatchNorm.md) | For each sample in a batch, normalizes each feature of its input along the batch dimension. |
| [DeepNorm](normalization/DeepNorm.md) | A normalization method that can replace LayerNorm during deep neural network training. |
| [GroupNorm](normalization/GroupNorm.md) | Divides the C dimension of the input into groupNum groups and normalizes the data in each group. |
| [LayerNorm](normalization/LayerNorm.md) | A normalization method that converges the input data to the range [0, 1] and can regulate the input and output data distribution of a network layer. |
| [LayerNorm-Tiling](normalization/LayerNorm-Tiling.md) | Tiling parameter configuration API of LayerNorm, used to obtain the tiling parameters required for LayerNorm kernel computation. |
| [LayerNormGrad](normalization/LayerNormGrad.md) | Computes the backpropagation gradient of LayerNorm. |
| [LayerNormGrad-Tiling](normalization/LayerNormGrad-Tiling.md) | Tiling parameter configuration API of LayerNormGrad, used to set the tiling parameters required for backpropagation gradient computation. |
| [LayerNormGradBeta](normalization/LayerNormGradBeta.md) | Obtains the values of the backward beta/gamma, and together with LayerNormGrad outputs pdx, gamma, and beta. |
| [LayerNormGradBeta-Tiling](normalization/LayerNormGradBeta-Tiling.md) | Tiling parameter configuration API of LayerNormGradBeta. |
| [Normalize](normalization/Normalize.md) | In [LayerNorm](normalization/LayerNorm.md), given the mean and variance, computes the reciprocal of the standard deviation rstd and the normalized output y of the input data with shape [A, R]. |
| [RmsNorm](normalization/RmsNorm.md) | Implements RmsNorm normalization on input data with shape [B, S, H]. |
| [WelfordUpdate](normalization/WelfordUpdate.md) | Implements the preprocessing of the Welford algorithm. |
| [WelfordFinalize](normalization/WelfordFinalize.md) | Implements the post-processing of the Welford algorithm. |

## Activation Functions

| API | Description |
| --- | --- |
| [AdjustSoftMaxRes](activation_functions/SoftMax_api/AdjustSoftMaxRes.md) | Performs post-processing on SoftMax-related computation results to adjust the SoftMax computation result to a specified value. |
| [FasterGelu](activation_functions/Gelu_api/FasterGelu.md) | An activation function that is a simplified version of FastGelu. |
| [FasterGeluV2](activation_functions/Gelu_api/FasterGeluV2.md) | An activation function that implements the FastGeluV2 version. |
| [GeGLU](activation_functions/GeGLU_api/GeGLU.md) | A GLU variant that uses GeLU as the activation function. |
| [Gelu](activation_functions/Gelu_api/Gelu.md) | GELU is an important activation function inspired by ReLU and dropout, introducing the idea of stochastic regularization into activation. |
| [LogSoftMax](activation_functions/LogSoftMax_api/LogSoftMax.md) | Performs LogSoftmax computation on the input tensor. |
| [ReGlu](activation_functions/ReGlu_api/ReGlu.md) | A GLU variant that uses ReLU as the activation function. |
| [Sigmoid](activation_functions/Sigmoid_api/Sigmoid.md) | Performs element-wise logistic regression Sigmoid. |
| [Silu](activation_functions/Silu_api/Silu.md) | Performs element-wise SiLU operation. |
| [SimpleSoftMax](activation_functions/SoftMax_api/SimpleSoftMax.md) | Performs softmax computation on the input tensor using the computed sum and max data. |
| [SoftMax](activation_functions/SoftMax_api/SoftMax.md) | Performs Softmax computation on the input tensor row by row. |
| [SoftmaxFlash](activation_functions/SoftMax_api/SoftmaxFlash.md) | An enhanced version of SoftMax. In addition to performing softmaxflash computation on the input tensor, it can also update the current softmax computation result based on the sum and max from the previous softmax computation. |
| [SoftmaxFlashV2](activation_functions/SoftMax_api/SoftmaxFlashV2.md) | An enhanced version of SoftmaxFlash, corresponding to the FlashAttention-2 algorithm. |
| [SoftmaxFlashV3](activation_functions/SoftMax_api/SoftmaxFlashV3.md) | An enhanced version of SoftmaxFlash, corresponding to the Softmax PASA algorithm. |
| [SoftmaxGrad](activation_functions/SoftMax_api/SoftmaxGrad.md) | A method for performing grad backward computation on the input tensor. |
| [SoftmaxGradFront](activation_functions/SoftMax_api/SoftmaxGradFront.md) | A method for performing grad backward computation on the input tensor. |
| [SwiGLU](activation_functions/SwiGLU_api/SwiGLU.md) | A GLU variant that uses Swish as the activation function. |
| [Swish](activation_functions/Swish_api/Swish.md) | The Swish activation function in neural networks. |

## Reduction Operations

| API | Description |
| --- | --- |
| [Sum](reduction_operations/Sum_api/Sum.md) | Computes the sum of elements along the last dimension. |
| [Mean](reduction_operations/Mean_api/Mean.md) | Computes the mean of elements along the last axis. |
| [ReduceXorSum](reduction_operations/ReduceXorSum_api/ReduceXorSum.md) | Performs an element-wise Xor (bitwise XOR) operation and sums the results using ReduceSum. |
| [ReduceSum](reduction_operations/ReduceSum_api/ReduceSum-90.md) | Accumulates data of a multidimensional vector along a specified dimension. |
| [ReduceMean](reduction_operations/ReduceMean_api/ReduceMean.md) | Computes the mean of a multidimensional vector along a specified dimension. |
| [ReduceMax](reduction_operations/ReduceMax_api/ReduceMax-91.md) | Computes the maximum value of a multidimensional vector along a specified dimension. |
| [ReduceMin](reduction_operations/ReduceMin_api/ReduceMin-92.md) | Computes the minimum value of a multidimensional vector along a specified dimension. |
| [ReduceAny](reduction_operations/ReduceAny_api/ReduceAny.md) | Computes the logical OR of a multidimensional vector along a specified dimension. |
| [ReduceAll](reduction_operations/ReduceAll_api/ReduceAll.md) | Computes the logical AND of a multidimensional vector along a specified dimension. |
| [ReduceProd](reduction_operations/ReduceProd_api/ReduceProd.md) | Computes the product of a multidimensional vector along a specified dimension. |

## Sorting Operations

| API | Description |
| --- | --- |
| [TopK](sort_operations/TopK.md) | Obtains the top k largest or smallest values along the last dimension and their corresponding indices. |
| [Concat](sort_operations/Concat.md) | Preprocesses data by merging the source operands srcLocal to be sorted into the target data concatLocal in a one-to-one correspondence. After data preprocessing, Sort can be performed. |
| [Extract](sort_operations/Extract.md) | Processes the result data of Sort and outputs the sorted value and index. |
| [Sort](sort_operations/Sort.md) | Sorting function that sorts in descending order by numerical value. |
| [MrgSort](sort_operations/MrgSort-93.md) | Merges up to four already-sorted queues into a single queue, with the result sorted in descending order by the score field. |

## Data Filtering

| API | Description |
| --- | --- |
| [Select](data_filter/Select-95.md) | Given two source operands src0 and src1, select elements based on the value (non-bit) at the corresponding position of maskTensor to obtain the destination operand dst. |
| [DropOut](data_filter/DropOut.md) | Provides the function of filtering the source operand based on MaskTensor to obtain the destination operand. |

## Tensor Transformation

| API | Description |
| --- | --- |
| [Transpose](tensor_transform/Transpose-96.md) | Performs data layout and Reshape operations on the input data. |
| [TransData](tensor_transform/TransData.md) | Converts the layout format of the input data to the target layout format. |
| [Broadcast](tensor_transform/Broadcast.md) | Broadcasts the input according to the output shape. |
| [Pad](tensor_transform/Pad.md) | Pads a 2D tensor of height * width to 32B alignment in the width direction. |
| [UnPad](tensor_transform/UnPad.md) | Unpads a 2D tensor of height * width in the width direction. |
| [Fill](tensor_transform/Fill-97.md) | Initializes the data in Global Memory to a specified value. |

## Index Computation

| API | Description |
| --- | --- |
| [Arange](index_compute/Arange-94.md) | Given a start value, a step value, and a length, returns an arithmetic sequence. |

## Cube Computation

| API | Description |
| --- | --- |
| [Matmul](matrix_compute/Matmul-Kernel/matmul_kernel_api.md) | Matmul matrix multiplication operation. |

## HCCL Communication

| API | Description |
| --- | --- |
| [HCCL Communication](hccl_communication/hccl_communication.md) | Orchestrates collective communication tasks on the AI Core side. |

## Convolution Computation

| API | Description |
| --- | --- |
| [Conv3D](convolution_compute/Conv3D-Kernel/conv3d_kernel_api.md) | 3D convolution forward matrix operation. |
| [Conv3DBackpropInput](convolution_compute/Conv3DBackpropInput-Kernel/conv3dbackpropinput_kernel_api.md) | Backward convolution operation that computes the backpropagation error of the feature matrix. |
| [Conv3DBackpropFilter](convolution_compute/Conv3DBackpropFilter-Kernel/conv3dbackpropfilter_kernel_api.md) | Backward convolution operation that computes the backpropagation error of the weights. |

## Random Functions

| API  | Description |
| --- | --- |
| [PhiloxRandom](random_functions/PhiloxRandom.md) | Generates a number of random numbers based on the Philox random number generation algorithm with a given random seed. |
