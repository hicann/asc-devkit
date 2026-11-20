# Basic API List

<!-- md-trans-meta sourceCommit=d1c6d885aaa504b2fae9a963830336c6fe119808 translatedAt=2026-11-19T03:15:41.337Z pushedAt=2026-11-19T05:58:49.130Z nl=2 -->

## Basic Data Structure

| API | Function Description |
| --- | --- |
| [LocalTensor](data_structures/LocalTensor/LocalTensor.md) | LocalTensor is used to store data in the Local Memory (internal storage) of the AI Core, and supports the logical positions TPosition of VECIN, VECOUT, VECCALC, A1, A2, B1, B2, CO1, and CO2. |
| [GlobalTensor](data_structures/GlobalTensor/GlobalTensor.md) | GlobalTensor is used to store global data in the Global Memory (external storage). |
| [Coordinate](aux_data_structures/Coordinate/Coordinate.md) | Coordinate is essentially a tuple used to represent the position information of a tensor in different dimensions, that is, the coordinate values. |
| [Layout](aux_data_structures/Layout/Layout.md) | The Layout data structure is a basic template class that describes the memory layout of a multi-dimensional tensor. It maps the logical coordinate space to the one-dimensional memory address space through the compile-time shape and stride information. |
| [TensorTrait](aux_data_structures/TensorTrait/TensorTrait.md) | The TensorTrait data structure is a basic template class that describes tensor-related information, including the data type, logical position, and Layout memory layout of the tensor. |
| [ShapeInfo](aux_data_structures/ShapeInfo.md) | Stores the shape information of LocalTensor or GlobalTensor. |
| [ListTensorDesc](aux_data_structures/ListTensorDesc.md) | Parses data that conforms to a specified memory layout format, and obtains the address and shape information of the corresponding stored data by index on the kernel side. |
| [TensorDesc](aux_data_structures/TensorDesc/TensorDesc.md) | Stores the tensor description information obtained by index in ListTensorDesc.GetDesc(). |
| [UnaryRepeatParams](aux_data_structures/UnaryRepeatParams.md) | A data structure that controls the address stride of a single operand, including the address stride parameters of DataBlock between adjacent iterations and within the same iteration. |
| [BinaryRepeatParams](aux_data_structures/BinaryRepeatParams.md) | A data structure that controls the address stride of dual operands, including the address stride parameters of DataBlock between adjacent iterations and within the same iteration. |
| [complex32/complex64](aux_data_structures/complex32-complex64.md) | Complex number types. For complex32, both the real part and the imaginary part are of the half type with a bit width of 32 bits; for complex64, both the real part and the imaginary part are of the float type with a bit width of 64 bits. |
| [TPosition](aux_data_structures/TPosition.md) | Uses an abstract logical position (TPosition) to represent storage at various levels, replacing the concept of on-chip physical storage. The main types include VECIN, VECOUT, VECCALC, A1, A2, B1, B2, CO1, and CO2. |

## Matrix Computation (ISASI)

### Move Matrix Data Into L0-Buffer

| API | Function Description |
| --- | --- |
| [LoadData (2D Matrix Movement)](cube_compute_ISASI/cube_compute_load/LoadData_2D.md) | LoadData (2D Matrix Movement) is responsible for completing the movement of 2D formatted data required for ordinary matrix computation, moving data from L1 Buffer to L0A Buffer/L0B Buffer in units of 512-byte data fractals. |
| [LoadData (2D Matrix Movement V2)](cube_compute_ISASI/cube_compute_load/LoadData_2D_V2.md) | LoadData (2D Matrix Movement V2) is responsible for completing the movement of 2D formatted data required for ordinary matrix computation, using the V2 parameter structure and supporting more data types. |
| [LoadData (MX Matrix Movement)](cube_compute_ISASI/cube_compute_load/LoadData_2D_MX.md) | LoadData (MX Matrix Movement) is used to move MX formatted matrix data, supporting moving matrix data from L1 Buffer to L0A Buffer/L0B Buffer and simultaneously moving the quantization coefficient matrix to L0A_MX Buffer/L0B_MX Buffer. |
| [LoadData (BitMode 2D Matrix Movement)](cube_compute_ISASI/cube_compute_load/LoadData_2D_BitMode.md) | LoadData (BitMode 2D Matrix Movement) is the bit-mode variant of LoadData (2D Matrix Movement V2). It passes parameters through a union structure and supports bitwise data movement. |
| [LoadData (Convolution Data Movement)](cube_compute_ISASI/cube_compute_load/LoadData_3D.md) | LoadData (Convolution Data Movement) essentially performs Image to Column expansion on the Feature Map in NC1HWC0 format, and then selects the specified data block from the expanded two-dimensional matrix and moves it into the corresponding memory location. |
| [LoadData (BitMode Convolution Data Movement)](cube_compute_ISASI/cube_compute_load/LoadData_3D_BitMode.md) | LoadData (BitMode Convolution Data Movement) is used to complete the image to column operation, converting a multi-dimensional feature map into a two-dimensional matrix. It is the bit-mode variant of LoadData (Convolution Data Movement). |
| [LoadDataWithStride](cube_compute_ISASI/cube_compute_load/LoadDataWithStride.md) | LoadDataWithStride essentially performs Image to Column expansion on the Feature Map in NC1HWC0 format, and then selects the specified data block from the expanded two-dimensional matrix and moves it into the corresponding memory location, supporting configuration of the output matrix K-axis direction offset. |
| [LoadDataWithTranspose](cube_compute_ISASI/cube_compute_load/LoadDataWithTranspose.md) | LoadDataWithTranspose is responsible for completing the movement of 2D formatted data required for ordinary matrix computation. A transpose operation accompanies the movement process, referring to the feature fractal transpose. |
| [LoadDataWithSparse](cube_compute_ISASI/cube_compute_load/LoadDataWithSparse.md) | Used to move the dense weight matrix stored in units of 512 bytes from L1 Buffer to L0B Buffer, while moving the index matrix in units of 128 bytes to the built-in dedicated buffer space (for subsequent reading by the MmadWithSparse API). |
| [BroadCastVecToMM(ISASI)](cube_compute_ISASI/cube_compute_load/BroadCastVecToMM_ISASI.md) | Broadcasts vector data into a matrix. Every 16 elements in each data block are continuously copied 16 times, supporting the data transfer path from UB to L0C Buffer. |
| [DataCopy (L1ToBiasTable-Buffer Data Movement)](cube_compute_ISASI/cube_compute_load/DataCopy_L1ToBiasTable.md) | DataCopy data movement supports moving the Bias parameters used in matrix computation from L1 Buffer to BiasTable Buffer. |
| [DataCopy (L1ToFixpipe-Buffer Data Movement)](cube_compute_ISASI/cube_compute_load/DataCopy_L1ToFixpipe.md) | DataCopy data movement supports moving the on-the-fly quantization parameters from L1 Buffer to Fixpipe Buffer. |

### Move Matrix Data Into L1 Buffer

| API | Function Description |
| --- | --- |
| [DataCopy (GMToL1 Continuous Data Movement)](cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_continuous.md) | This API moves the matrix from Global Memory to L1 Buffer continuously (TPosition is A1/B1), and the format and content remain unchanged during data movement. |
| [DataCopy (GMToL1 High-Dimensional Splitting Data Movement)](cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_highdim_split.md) | This API mainly moves the matrix from Global Memory to L1 Buffer (TPosition is A1/B1), and the format and content remain unchanged during data movement. |
| [DataCopy (GMToL1 On-the-fly Conversion - ND2NZ Movement)](cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_ND2NZ.md) | This API mainly moves the matrix from Global Memory to L1 Buffer (TPosition is A1/B1), and supports ND-to-NZ format conversion during data movement. |
| [DataCopy (GMToL1 On-the-fly Conversion - DN2NZ Movement)](cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_DN2NZ.md) | This API mainly moves the matrix from Global Memory to L1 Buffer (TPosition is A1/B1), and supports DN-to-NZ format conversion during data movement. |
| [DataCopyPad (GMToL1 Unaligned Data Movement)](cube_compute_ISASI/cube_compute_load/DataCopyPad_GMToL1.md) | This API provides unaligned data movement from Global Memory to L1 Buffer, and allows developers to fill data as needed. |
| [LoadData (GMToL1-2D Matrix Movement)](cube_compute_ISASI/cube_compute_load/LoadData_GMToL1_2D.md) | Responsible for completing the movement of 2D formatted data required for ordinary matrix computation, moving data from Global Memory to L1 Buffer (TPosition is A1/B1) in units of 512-byte data fractals. |
| [LoadData (GMToL1-2D Matrix Movement V2)](cube_compute_ISASI/cube_compute_load/LoadData_GMToL1_2DV2.md) | Responsible for completing the movement of 2D formatted data required for ordinary matrix computation, moving data from Global Memory to L1 Buffer (TPosition is A1/B1) in units of 512-byte data fractals. |
| [DataCopy (UBToL1 Continuous Data Movement)](cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_continuous.md) | This API moves the matrix from UB (TPosition is VECIN/VECCALC/VECOUT) to L1 Buffer in a continuous movement manner, and the format and content remain unchanged during data movement. |
| [DataCopy (UBToL1 High-Dimensional Splitting Data Movement)](cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_highdim_split.md) | This API moves the matrix from UB (TPosition is VECIN/VECCALC/VECOUT) to L1 Buffer, supports non-continuous and continuous movement, and the format and content remain unchanged during data movement. |
| [DataCopyPad (UBToL1 Unaligned Data Movement)](cube_compute_ISASI/cube_compute_load/DataCopyPad_UBToL1.md) | This API provides unaligned data movement from UB to L1 Buffer. |
| [DataCopy (UBToL1 On-the-fly Conversion - ND2NZ Movement)](cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_ND2NZ.md) | Supports ND-to-NZ format conversion during data movement. Data is moved from UB (TPosition is VECIN/VECCALC/VECOUT) to L1 Buffer, and ND->NZ format conversion is completed during the movement process. |

### Auxiliary Configuration APIs

| API | Function Description |
| --- | --- |
| [Fill](cube_compute_ISASI/cube_load_aux_config/Fill.md) | Initializes the LocalTensor at a specific physical storage location to a specific value. Only LocalTensor initialization on L1 Buffer/L0A Buffer/L0B Buffer is supported. |
| [SetFmatrix](cube_compute_ISASI/cube_load_aux_config/SetFmatrix.md) | Used to set the attribute description of FeatureMap when calling LoadData (convolution data movement). When the template parameter isSetFMatrix of LoadData (convolution data movement) is set to false, the attributes of the FeatureMap passed to LoadData (convolution data movement) do not take effect, and developers need to set them through this API. |
| [SetLoadDataBoundary](cube_compute_ISASI/cube_load_aux_config/SetLoadDataBoundary.md) | Sets the boundary value of the L1 Buffer (TPosition: A1/B1) required by the LoadData (convolution data movement) API. |
| [SetLoadDataRepeat](cube_compute_ISASI/cube_load_aux_config/SetLoadDataRepeat.md) | Used to set the repeat parameter of the LoadData (convolution data movement) API. After the repeat parameter is set, data movement for multiple iterations can be completed by calling the LoadData (convolution data movement) API once. |
| [SetLoadDataRepeatWithStride](cube_compute_ISASI/cube_load_aux_config/SetLoadDataRepeatWithStride.md) | Used to set the repeat parameter of the LoadDataWithStride API. After the repeat parameter is set, data movement for multiple iterations can be completed by calling the LoadData (convolution data movement) API once, and the output matrix K-axis direction offset parameter dstStride must be configured. |
| [SetLoadDataPaddingValue](cube_compute_ISASI/cube_load_aux_config/SetLoadDataPaddingValue.md) | Used to set the Pad fill value when calling the LoadData (convolution data movement) API. When the template parameter isSetPadding of LoadData (convolution data movement) is set to false, users need to set the Pad fill value through this API; when it is set to true, the fill value set by this API does not take effect. |
| [LoadDataUnzip](cube_compute_ISASI/cube_load_aux_config/LoadDataUnzip.md) | Decompresses the data on GM and moves it to L1 Buffer (A1/B1) or L0B Buffer (B2). Before executing this API, LoadUnzipIndex must be executed to load the compression index table. |
| [LoadImageToLocal](cube_compute_ISASI/cube_load_aux_config/LoadImageToLocal.md) | Moves image data from Global Memory to Local Memory. During the movement process, image preprocessing operations can be completed, including image flipping, changing image size (cropping, edge trimming, scaling, stretching), as well as color space conversion and type conversion. The related parameters of image preprocessing are configured through SetAippFunctions. |
| [LoadUnzipIndex](cube_compute_ISASI/cube_load_aux_config/LoadUnzipIndex.md) | Loads the compression index table on GM into internal registers. |
| [SetAippFunctions](cube_compute_ISASI/cube_load_aux_config/SetAippFunctions.md) | Sets the parameters related to image preprocessing (AIPP, AI Core pre-process). It is used in coordination with the LoadImageToLocal API. After setting, calling the LoadImageToLocal API can complete image preprocessing operations during the movement process, including data fill, channel swap, single-row read, data type conversion, channel fill, and color space conversion. When calling the SetAippFunctions API, the matrix of the source image in Global Memory and the image format of the source image must be passed in. |

### Mmad Computation

| API | Function Description |
| --- | --- |
| [Mmad](cube_compute_ISASI/mmad_compute/Mmad.md) | The Mmad API is the core matrix multiply-accumulate computation API of Ascend C for Ascend AI chips. It is designed for high-performance operator development, encapsulates the matrix multiply-accumulate computation capability of Ascend NPU hardware, and is widely used in the development of neural network layers (such as fully connected layers and convolution layers) and numerical computation operators. |
| [MmadMx](cube_compute_ISASI/mmad_compute/MmadMx.md) | MmadMx (full name Microscaling Mmad) is matrix multiplication with quantization coefficients, that is, both the left matrix and the right matrix have corresponding quantization coefficient matrices, namely the left quantization coefficient matrix scaleA and the right quantization coefficient matrix scaleB. In the MmadMx scenario, the left quantization coefficient matrix is multiplied by the left matrix, the right quantization coefficient matrix is multiplied by the right matrix, and matrix multiplication is performed on the results of the two products. |
| [MmadBitMode](cube_compute_ISASI/mmad_compute/MmadBitMode.md) | MmadBitMode optimizes the construction of the **MmadParams** structure. This API is suitable for scenarios where the scalar pipeline becomes a performance optimization bottleneck, and supports the basic Mmad/MmadMx computation functions. The difference between this API and the Mmad/MmadMx APIs lies in the way parameters are passed in. This API passes in the union structure **MmadBitModeParams**. |
| [MmadWithSparse](cube_compute_ISASI/mmad_compute/MmadWithSparse.md) | The MmadWithSparse API is responsible for completing special sparse matrix multiply-accumulate operations. A sparse matrix is a special type of matrix that contains a large number of zero elements. 4:2 structured sparsity requires that in a group of four consecutive weights or activation values (usually a row or a column in a tensor), at most two values are non-zero, and the remaining two are forced to be zero. |

### Mmad Register Configuration Description

| API | Function Description |
| --- | --- |
| [SetHF32Mode](cube_compute_ISASI/mmad_compute_aux_config/SetHF32Mode.md) | Used to set whether HF32 mode is enabled for Mmad computation. After this mode is enabled, the FP32 data in L0A Buffer/L0B Buffer is rounded to HF32 before participating in Mmad computation. |
| [SetHF32TransMode](cube_compute_ISASI/mmad_compute_aux_config/SetHF32TransMode.md) | Sets the specific rounding method of HF32 mode. SetHF32Mode must be used first to enable the HF32 rounding mode. |
| [SetMMColumnMajor/SetMMRowMajor](cube_compute_ISASI/mmad_compute_aux_config/SetMMColumnMajor-SetMMRowMajor.md) | Controls which direction of M/N is prioritized by Mmad/MmadWithSparse. |

### Matrix Computation Store-Out

| API | Function Description |
| --- | --- |
| [DataCopy (L0C to GM Data Movement)](cube_compute_ISASI/cube_compute_store/DataCopy_L0CToGM.md) | The result of matrix computation is stored in the L0C Buffer. The DataCopy API is used to move the result to Global Memory (GM), and supports on-the-fly format conversion and other operations during the movement process. |
| [Fixpipe (L0C to GM Data Movement)](cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md) | The result of matrix computation is stored in the L0C Buffer. The Fixpipe API is used to move the result to Global Memory (GM), and supports on-the-fly format conversion and other operations during the movement process. |
| [DataCopy (L0C to L1 Data Movement)](cube_compute_ISASI/cube_compute_store/DataCopy_L0CToL1.md) | The result of matrix computation is stored in the L0C Buffer. The DataCopy API is used to move the result to the L1 Buffer, and supports on-the-fly format conversion and other operations during the movement process. |
| [Fixpipe (L0C to L1 Data Movement)](cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToL1.md) | The result of matrix computation is stored in the L0C Buffer. The Fixpipe API is used to move the result to the L1 Buffer, and supports on-the-fly format conversion and other operations during the movement process. |
| [Fixpipe (L0C Buffer to UB Data Movement)](cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToUB.md) | The result of matrix computation is stored in the L0C Buffer. The Fixpipe API is used to move the result to UB, and supports on-the-fly format conversion and other operations during the movement process. |
| [DataCopy (L0C to UB data movement)](cube_compute_ISASI/cube_compute_store/DataCopy_L0CToUB.md) | The result of cube computation is stored in the L0C Buffer. The DataCopy API is used to move the result to the Unified Buffer (UB), and it supports operations such as inline quantization during data movement. |

### L1 to GM Data Movement

| API | Function Description |
| --- | --- |
| [DataCopy (L1ToGM Continuous Data Movement)](cube_compute_ISASI/cube_compute_store/DataCopy_L1ToGM_continuous.md) | This API can continuously move the matrix from the L1 Buffer to Global Memory. The format and content remain unchanged during data movement. |
| [DataCopy (L1ToGM High-Dimensional Splitting Data Movement)](cube_compute_ISASI/cube_compute_store/DataCopy_L1ToGM_highdim_split.md) | This API mainly moves the matrix from the L1 Buffer to Global Memory. The format and content remain unchanged during data movement. |

### Store Register Configuration Description

| API | Function Description |
| --- | --- |
| [SetFixPipeConfig](cube_compute_ISASI/cube_store_aux_config/SetFixPipeConfig.md) | During on-the-fly quantization in the DataCopy data movement (L0C-\>GM, L0C-\>L1) process, call this API to set the quantization parameters. |
| [SetFixpipePreQuantFlag](cube_compute_ISASI/cube_store_aux_config/SetFixpipePreQuantFlag.md) | During on-the-fly quantization in the DataCopy data movement (L0C-\>GM, L0C-\>L1) process, call this API to set the scalar quantization parameters. |
| [SetFixpipeNz2ndFlag](cube_compute_ISASI/cube_store_aux_config/SetFixpipeNz2ndFlag.md) | During on-the-fly format conversion (converting NZ format to ND format) in the DataCopy data movement (L0C-\>GM) process, call this API to set the related configuration for format conversion. |
| [SetFixpipeNz2dnFlag](cube_compute_ISASI/cube_store_aux_config/SetFixpipeNz2dnFlag.md) | During the DataCopy data movement (L0C Buffer -> UB), when performing inline format conversion (converting Nz format to DN format), call this API to set the related configuration for format conversion. |
| [SetFixPipeClipRelu](cube_compute_ISASI/cube_store_aux_config/SetFixPipeClipRelu.md) | After on-the-fly quantization in the DataCopy data movement (L0C-\>GM) process, call this API to set the maximum value for the ClipReLU operation. |
| [SetFixPipeAddr](cube_compute_ISASI/cube_store_aux_config/SetFixPipeAddr.md) | After on-the-fly quantization in the DataCopy data movement (L0C-\>GM) process, call this API to set the address of the LocalTensor for the Elementwise operation. |

## Memory Vector Computation

### Data Movement

| API | Function Description |
| --- | --- |
| [DataCopy (GM and UB continuous data movement)](memory_vector_compute/data_move/DataCopy_GMAndUB_continuous.md) | Supports continuous data movement between Global Memory and UB, with the original format and content of the data preserved during transmission. |
| [DataCopy (GM and UB high-dimensional splitting data movement)](memory_vector_compute/data_move/DataCopy_GMAndUB_highdim_split.md) | Supports high-dimensional splitting data movement between Global Memory and UB, with the original format and content of the data preserved during transmission. |
| [DataCopy (GM and UB slice data movement)](memory_vector_compute/data_move/DataCopy_GMAndUB_slice.md) | This API is a software simulation API designed for ease of use. It supports slice data movement and extracts a subset of a multi-dimensional Tensor for movement. |
| [DataCopy (GMToUB on-the-fly ND2NZ movement)](memory_vector_compute/data_move/DataCopy_GMToUB_ND2NZ.md) | This API is a software simulation API designed for ease of use. It supports ND-to-NZ format conversion during data movement from Global Memory to UB. |
| [DataCopy (UBToGM on-the-fly NZ2ND movement)](memory_vector_compute/data_move/DataCopy_UBToGM_NZ2ND.md) | This API is a software simulation API designed for ease of use. It supports NZ-to-ND format conversion during data movement from UB to Global Memory. |
| [DataCopy (GMToUB multi-dimensional data movement NDDMA)](memory_vector_compute/data_move/DataCopy_GMToUB_NDDMA.md) | A multi-dimensional data movement API. Compared with the basic data movement API, it allows more flexible configuration of the dimension information to be moved in and the corresponding Stride. |
| [DataCopyPad (GMToUB unaligned data movement)](memory_vector_compute/data_move/DataCopyPad_GMToUB.md) | This API provides the function of moving data from Global Memory to UB in an unaligned manner, and allows developers to fill data as needed. |
| [DataCopyPad (UBToGM unaligned data movement)](memory_vector_compute/data_move/DataCopyPad_UBToGM.md) | This API provides the function of moving data from UB to Global Memory in an unaligned manner. |
| [SetPadValue(ISASI)](memory_vector_compute/data_move_aux_config/SetPadValue_ISASI.md) | When moving data from Global Memory to UB in an unaligned manner, developers can fill data as needed. SetPadValue is used to set the value to be filled by DataCopyPad. |
| [SetLoopModePara](memory_vector_compute/data_move_aux_config/SetLoopModePara.md) | During DataCopy and DataCopyPad, this API enables loop mode and sets the loop mode parameters. After data movement is complete, reset the loop mode parameters through ResetLoopModePara. |
| [ResetLoopModePara](memory_vector_compute/data_move_aux_config/ResetLoopModePara.md) | Resets the loop mode parameters. It is used together with SetLoopModePara. In data movement scenarios where loop mode is enabled and loop mode parameters are set, this function must be called to reset the loop mode parameters after data movement is complete. |
| [DataCopy (UBToUB continuous data movement)](memory_vector_compute/data_move/DataCopy_UBToUB_continuous.md) | Supports continuous data movement between UB and UB, with the original format and content of the data preserved during transmission. |
| [DataCopy (UBToUB high-dimensional splitting data movement)](memory_vector_compute/data_move/DataCopy_UBToUB_highdim_split.md) | Supports high-dimensional splitting data movement between UB and UB, with the original format and content of the data preserved during transmission. |
| [Copy (UBToUB continuous data movement)](memory_vector_compute/data_move/Copy_UBToUB_continuous.md) | Supports continuous data movement between UB and UB, with the format and content of the data preserved during movement. |
| [Copy (UBToUB masked high-dimensional data movement)](memory_vector_compute/data_move/Copy_UBToUB_mask_highdim_split.md) | Supports data movement between UB and UB, with the format and content of the data preserved during movement. It supports mask operations and DataBlock interval operations. |

### Basic Arithmetic

| API | Function Description |
| --- | --- |
| [Exp](memory_vector_compute/basic_arithmetic/Exp.md) | Exp is a unary vector computation API that is responsible for taking the natural exponential of the input tensor element-wise. |
| [Ln](memory_vector_compute/basic_arithmetic/Ln.md) | Ln is a unary vector computation API that is responsible for taking the natural logarithm of the input tensor element-wise. |
| [Abs](memory_vector_compute/basic_arithmetic/Abs.md) | Abs is a unary vector computation API that is responsible for taking the absolute value of the input tensor element-wise. |
| [Reciprocal](memory_vector_compute/basic_arithmetic/Reciprocal.md) | Reciprocal is a unary vector computation API that is responsible for taking the reciprocal of the input tensor element-wise. |
| [Sqrt](memory_vector_compute/basic_arithmetic/Sqrt.md) | Sqrt is a unary vector computation API that is responsible for performing the square root operation on the input tensor element-wise. |
| [Rsqrt](memory_vector_compute/basic_arithmetic/Rsqrt.md) | Rsqrt is a unary vector computation API that is responsible for performing the square root operation and then taking the reciprocal on the input tensor element-wise. |
| [Relu](memory_vector_compute/basic_arithmetic/Relu.md) | Relu is a unary vector computation API that is responsible for performing ReLU (Rectified Linear Unit) computation on the input tensor element-wise. |
| [Neg(ISASI)](memory_vector_compute/basic_arithmetic/Neg_ISASI.md) | Performs element-wise negation. |
| [Add](memory_vector_compute/basic_arithmetic/Add.md) | Add is a binary vector computation API that is responsible for computing the element-wise sum of the two input tensors. |
| [Sub](memory_vector_compute/basic_arithmetic/Sub.md) | Sub is a binary vector computation API that is responsible for computing the element-wise difference of the two input tensors. |
| [Mul](memory_vector_compute/basic_arithmetic/Mul.md) | Mul is a binary vector computation API that is responsible for computing the element-wise product of the two input tensors. |
| [Div](memory_vector_compute/basic_arithmetic/Div.md) | Div is a binary vector computation API that is responsible for computing the element-wise quotient of the two input tensors. |
| [Max](memory_vector_compute/basic_arithmetic/Max.md) | Max is a binary vector computation API that is responsible for computing the element-wise maximum of the two input tensors. |
| [Min](memory_vector_compute/basic_arithmetic/Min.md) | Min is a binary vector computation API that is responsible for computing the element-wise minimum of the two input tensors. |
| [BilinearInterpolation(ISASI)](memory_vector_compute/basic_arithmetic/BilinearInterpolation_ISASI.md) | BilinearInterpolation is a binary vector computation API. Its core performs multiply-accumulate operations through a two-level loop of horizontal iteration and vertical iteration, and is applicable to vector operation scenarios with irregular data access and weighted multiply-add (data Gather, Multiply, and Reduce). |
| [Prelu(ISASI)](memory_vector_compute/basic_arithmetic/Prelu_ISASI.md) | When source operand src0 is greater than 0, src0 is written directly to destination operand dst; otherwise, the result of src0 * src1 is written to dst. |
| [Mull(ISASI)](memory_vector_compute/basic_arithmetic/Mull_ISASI.md) | Performs element-wise multiplication on the first count input data src0 and src1, writes the result to dst0Local, and writes the overflow part to dst1Local. |
| [Adds](memory_vector_compute/basic_arithmetic/Adds.md) | Adds is a binary scalar computation API. Its inputs are a vector tensor and a scalar. Adds is responsible for computing the sum of each element in the vector tensor with the scalar. |
| [Adds (Flexible Scalar Position)](memory_vector_compute/basic_arithmetic/Adds_flexible_scalar.md) | Provides an API with flexible scalar position, supporting two scenarios: scalar first and scalar last. The scalar input supports configuring a single-point element of LocalTensor. |
| [Muls](memory_vector_compute/basic_arithmetic/Muls.md) | Muls is a binary scalar computation API. Its inputs are a vector tensor and a scalar. Muls is responsible for computing the product of each element in the vector tensor with the scalar. |
| [Muls (Flexible Scalar Position)](memory_vector_compute/basic_arithmetic/Muls_flexible_scalar.md) | Provides an API with flexible scalar position, supporting two scenarios: scalar first and scalar last. The scalar input supports configuring a single-point element of LocalTensor. |
| [Maxs](memory_vector_compute/basic_arithmetic/Maxs.md) | Maxs is a binary scalar computation API. Its inputs are a vector tensor and a scalar. Maxs is responsible for computing the maximum of each element in the vector tensor with the scalar. |
| [Maxs (Flexible Scalar Position)](memory_vector_compute/basic_arithmetic/Maxs_flexible_scalar.md) | Provides an API with flexible scalar position, supporting two scenarios: scalar first and scalar last. The scalar input supports configuring a single-point element of LocalTensor. |
| [Mins](memory_vector_compute/basic_arithmetic/Mins.md) | Mins is a binary scalar computation API. Its inputs are a vector tensor and a scalar. Mins is responsible for computing the minimum of each element in the vector tensor with the scalar. |
| [Mins (Flexible Scalar Position)](memory_vector_compute/basic_arithmetic/Mins_flexible_scalar.md) | Provides an API with flexible scalar position, supporting two scenarios: scalar first and scalar last. The scalar input supports configuring a single-point element of LocalTensor. |
| [Subs](memory_vector_compute/basic_arithmetic/Subs_flexible_scalar.md) | Performs subtraction between each element in the vector and the scalar, supporting two scenarios: scalar first and scalar last. The scalar input supports configuring a single-point element of LocalTensor. |
| [Divs](memory_vector_compute/basic_arithmetic/Divs_flexible_scalar.md) | Performs division between each element in the vector and the scalar, supporting two scenarios: scalar first and scalar last. The scalar input supports configuring a single-point element of LocalTensor. |
| [LeakyRelu](memory_vector_compute/basic_arithmetic/LeakyRelu.md) | LeakyRelu is a binary scalar computation API. Its inputs are a vector tensor and a scalar. LeakyRelu is responsible for performing the Leaky ReLU (Leaky Rectified Linear Unit) operation on each element in the vector tensor. |

### Logical Computation

| API | Function Description |
| --- | --- |
| [Not](memory_vector_compute/logical_compute/Not.md) | Performs bitwise NOT on each element. |
| [And](memory_vector_compute/logical_compute/And.md) | Performs bitwise AND on each element. |
| [Or](memory_vector_compute/logical_compute/Or.md) | Performs bitwise OR on each pair of elements. |
| [Ands](memory_vector_compute/logical_compute/Ands.md) | Performs an AND operation between each element in the vector and a scalar, supporting both scalar-first and scalar-last scenarios, where the scalar input supports configuring a single-point element of a LocalTensor. |
| [Ors](memory_vector_compute/logical_compute/Ors.md) | Performs an OR operation between each element in the vector and a scalar, supporting both scalar-first and scalar-last scenarios, where the scalar input supports configuring a single-point element of a LocalTensor. |
| [ShiftLeft](memory_vector_compute/logical_compute/ShiftLeft.md) | Performs a left shift operation on each element of the source operand, where the number of bits to shift is determined by the scalar scalarValue. |
| [ShiftLeft (Shift Amount as Tensor)](memory_vector_compute/logical_compute/ShiftLeft_shift_amount_tensor.md) | Performs a left shift operation on each element of the source operand. |
| [ShiftRight](memory_vector_compute/logical_compute/ShiftRight.md) | Performs a right shift operation on each element of the source operand, where the number of bits to shift is determined by the scalar scalarValue. |
| [ShiftRight (Shift Amount as Tensor)](memory_vector_compute/logical_compute/ShiftRight_shift_amount_tensor.md) | Performs a right shift on each element of the source operand. |

### Compound Computation

| API | Function Description |
| --- | --- |
| [Axpy](memory_vector_compute/composite_compute/Axpy.md) | Computes the product of a vector and a scalar, and adds the product element-wise to the output vector. |
| [CastDequant](memory_vector_compute/composite_compute/CastDequant.md) | Performs quantization and precision conversion on the input, and applies saturation processing to the overflow of the output result. This API must be used together with the SetDeqScale API. |
| [AddRelu](memory_vector_compute/composite_compute/AddRelu.md) | Performs element-wise summation, and then performs ReLU computation (compares the result with 0 and takes the larger value). |
| [AddReluCast](memory_vector_compute/composite_compute/AddReluCast.md) | Performs element-wise summation, compares the result with 0 and takes the larger value, and performs precision conversion based on the data types of the source and destination operand Tensors. |
| [AddDeqRelu](memory_vector_compute/composite_compute/AddDeqRelu.md) | Sequentially performs element-wise summation, applies dequantization to the result, and then performs ReLU computation (compares the result with 0 and takes the larger value). |
| [SubRelu](memory_vector_compute/composite_compute/SubRelu.md) | Performs element-wise subtraction, and then performs ReLU computation (compares the result with 0 and takes the larger value). |
| [SubReluCast](memory_vector_compute/composite_compute/SubReluCast.md) | Performs element-wise subtraction, then performs ReLU computation (compares the result with 0 and takes the larger value), and performs precision conversion based on the data types of the source and destination operand Tensors. |
| [MulAddDst](memory_vector_compute/composite_compute/MulAddDst.md) | Multiplies src0 and src1 element-wise and adds the result to dst, storing the final result into dst. |
| [MulCast](memory_vector_compute/composite_compute/MulCast.md) | Performs element-wise multiplication, and performs precision conversion based on the data types of the source and destination operand Tensors. |
| [FusedMulAdd](memory_vector_compute/composite_compute/FusedMulAdd.md) | Multiplies src0 and dst element-wise and adds src1, storing the final result into dst. |
| [MulAddRelu](memory_vector_compute/composite_compute/MulAddRelu.md) | Multiplies src0 and dst element-wise and adds src1, then performs ReLU computation (compares the result with 0 and takes the larger value), storing the final result into dst. |
| [AbsSub(ISASI)](memory_vector_compute/composite_compute/AbsSub_ISASI.md) | Subtracts src1 from src0Local and computes the absolute value, writing the computation result into dst. |
| [ExpSub(ISASI)](memory_vector_compute/composite_compute/ExpSub_ISASI.md) | Subtracts src1 from src0, computes the power of the natural constant e with the difference as the exponent, and writes the computation result into dst. |
| [MulsCast(ISASI)](memory_vector_compute/composite_compute/MulsCast_ISASI.md) | Multiplies the first count data of the vector source operand by a scalar, converts the result to the half type in CAST_ROUND mode, and writes the computation result into dst. This API supports both scalar-first and scalar-last scenarios. |

### Comparison and Selection

| API | Function Description |
| --- | --- |
| [Compare](memory_vector_compute/compare_and_select/Compare.md) | Compares two tensors element by element. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0. |
| [Compare (Result Stored in Register)](memory_vector_compute/compare_and_select/Compare_store_to_register.md) | Compares two tensors element by element. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0. The computation result is stored in the CmpMask 128-bit register, which can be used for subsequent Select computation. The data stored in the register can also be obtained using the GetCmpMask API. |
| [Compares](memory_vector_compute/compare_and_select/Compares.md) | Compares the elements of a tensor with another Scalar element by element. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0. |
| [Compares (Flexible Scalar Position)](memory_vector_compute/compare_and_select/Compares_flexible_scalar.md) | Provides an API with flexible scalar position, supporting two scenarios: scalar first and scalar last. The scalar input supports configuring a single-point element of LocalTensor. |
| [GetCmpMask(ISASI)](memory_vector_compute/compare_and_select/GetCmpMask_ISASI.md) | This API is used to obtain the comparison result of the Compare (Result Stored in Register) instruction. |
| [SetCmpMask(ISASI)](memory_vector_compute/compare_and_select/SetCmpMask_ISASI.md) | Sets the value of the comparison register. It is used together with the Select API that does not pass the mask parameter, and passes different data according to different selMode values. |
| [Select](memory_vector_compute/compare_and_select/Select.md) | Given two source operands src0 and src1, selects elements according to the bit values of selMask (the mask used for selection) to obtain the destination operand dst. The selection rule is: when the bit of selMask is 1, the element is selected from src0; when the bit is 0, it is selected from src1. |
| [Select (Flexible Scalar Position)](memory_vector_compute/compare_and_select/Select_flexible_scalar.md) | Given two source operands src0 and src1, selects elements according to the bit values of selMask (the mask used for selection) to obtain the destination operand dst. The selection rule is: when the bit of selMask is 1, the element is selected from src0; when the bit is 0, it is selected from src1. |
| [GatherMask](memory_vector_compute/compare_and_select/GatherMask.md) | Uses the binary representation corresponding to the **built-in fixed pattern** or the **user-defined input Tensor** values as the gather mask (the mask for data collection), and selects elements from the source operand to write into the destination operand. 1 means selected, and 0 means not selected. |

### Type Conversion

| API | Function Description |
| --- | --- |
| [SetDeqScale](memory_vector_compute/type_conversion_aux_config/SetDeqScale.md) | This API is used to set the value of the DEQSCALE register. The DEQSCALE register is 64 bits wide and is used for quantization computation on the Vector computation unit. The parameters stored in the register have different meanings in different scenarios. |
| [Cast](memory_vector_compute/type_conversion/Cast.md) | Performs precision conversion based on the data types of the source operand and destination operand tensors. |
| [Truncate(ISASI)](memory_vector_compute/type_conversion/Truncate_ISASI.md) | Truncates the floating-point elements of the source operand to integer bits, while the data type of the source operand remains unchanged. |

### Reduction Computation

| API | Function Description |
| --- | --- |
| [ReduceDataBlock](memory_vector_compute/reduction_compute/ReduceDataBlock.md) | The `ReduceDataBlock` API performs a reduction operation on the input data in units of DataBlock. Based on the template parameter `reduceType`, it computes the sum/maximum/minimum of the data within each DataBlock. |
| [ReduceRepeat](memory_vector_compute/reduction_compute/ReduceRepeat.md) | The `ReduceRepeat` API is used to perform a reduction operation on all data within each repeat. Based on the template parameter reduceType, it performs the sum/maximum/minimum operation, and writes the results to the destination address in order. |
| [ReducePairElem](memory_vector_compute/reduction_compute/ReducePairElem.md) | The `ReducePairElem` API performs a reduction operation on two adjacent (odd-even) elements based on the template parameter `reduceType`, and writes the results to the destination address in order. **Currently only the sum operation is supported.** |
| [ReduceMax](memory_vector_compute/reduction_compute/ReduceMax.md) | The `ReduceMax` API is used to find the maximum value and its index from all input data. |
| [ReduceMin](memory_vector_compute/reduction_compute/ReduceMin.md) | The `ReduceMin` API is used to find the minimum value and its index from all input data. |
| [ReduceSum](memory_vector_compute/reduction_compute/ReduceSum.md) | The `ReduceSum` API sums all input data. |
| [GetReduceRepeatSumSpr(ISASI)](memory_vector_compute/reduction_compute_aux_config/GetReduceRepeatSumSpr_ISASI.md) | Obtains the computation result of the ReduceSum API. It is only supported for use with APIs that continuously compute the first n data of a tensor. The computation result is stored as a global variable and can be retrieved at any time. |
| [GetReduceRepeatMaxMinSpr(ISASI)](memory_vector_compute/reduction_compute_aux_config/GetReduceRepeatMaxMinSpr_ISASI.md) | This API is used to obtain the extreme values and their indices within all repeats when ReduceRepeat is called, or to obtain the extreme values obtained by calling ReduceMax/ReduceMin. The computation result is stored as a global variable and can be retrieved at any time. |

### Data Layout Conversion

| API | Function Description |
| --- | --- |
| [Transpose](memory_vector_compute/data_layout_conversion/Transpose.md) | The **Transpose** API is used to transpose a 16*16 two-dimensional matrix data block or to convert between the [N,C,H,W] and [N,H,W,C] data formats. |
| [TransDataTo5HD](memory_vector_compute/data_layout_conversion/TransDataTo5HD.md) | The **TransDataTo5HD** API performs data format conversion, generally used to convert the NCHW format to the NC1HWC0 format. In particular, it can also be used to transpose two-dimensional matrix data blocks. When performing transposition, compared with the **Transpose** API, **Transpose** only supports matrix transposition of 16\*16 size; this API can process 512 bytes of data (16 DataBlocks) within a single repeat, supports matrix transposition of different shapes depending on the data type, and can also support multiple repeat operations. |

### Data Fill

| API | Function Description |
| --- | --- |
| [Duplicate](memory_vector_compute/data_padding/Duplicate.md) | The Duplicate API copies a variable or an immediate value multiple times and fills it into a vector. |
| [Brcb](memory_vector_compute/data_padding/Brcb.md) | The Brcb API performs a padding operation on the input data by taking 8 numbers from the input tensor each time and filling each number into one DataBlock (32 bytes) of the result tensor. |
| [CreateVecIndex](memory_vector_compute/data_padding/CreateVecIndex.md) | The CreateVecIndex API creates a vector index with a specified start value. |
| [VectorPadding(ISASI)](memory_vector_compute/data_padding/VectorPadding_ISASI.md) | Performs a padding operation on the source operand by datablock according to padMode (pad mode) and padSide (pad side). |

### Sort and Merge (ISASI)

| API | Function Description |
| --- | --- |
| [ProposalConcat](memory_vector_compute/sort_and_merge_ISASI/ProposalConcat.md) | Merges continuous elements into the corresponding positions within Region Proposals. Each iteration merges 16 continuous elements into the corresponding positions of 16 Region Proposals. |
| [ProposalExtract](memory_vector_compute/sort_and_merge_ISASI/ProposalExtract.md) | Performs the opposite of ProposalConcat. It extracts a single element from the corresponding position within each Region Proposal and rearranges them. Each iteration processes 16 Region Proposals, extracting 16 elements and arranging them contiguously. |
| [RpSort16](memory_vector_compute/sort_and_merge_ISASI/RpSort16.md) | Sorts Region Proposals based on their score field (larger scores come first), processing 16 Region Proposals at a time. |
| [MrgSort4](memory_vector_compute/sort_and_merge_ISASI/MrgSort4.md) | Arranges and merges up to 4 already-sorted Region Proposal queues into a single queue, with the result sorted by the score field from largest to smallest. |
| [Sort32](memory_vector_compute/sort_and_merge_ISASI/Sort32.md) | The Sort32 API performs descending-order sorting of 32 numbers within a single iteration. |
| [MrgSort](memory_vector_compute/sort_and_merge_ISASI/MrgSort.md) | Merges and arranges up to 4 already-sorted queues into a single queue, with the result sorted by the score field from largest to smallest. |
| [GetMrgSortResult](memory_vector_compute/sort_and_merge_ISASI/GetMrgSortResult.md) | The GetMrgSortResult API must be used together with the MrgSort API. When the `ifExhaustedSuspension` parameter in the MrgSort API is set to true, if the data in an input queue is exhausted, MrgSort stops executing. In this case, the GetMrgSortResult API can be called to obtain the number of already-processed data items in the 4 queues. |

### Scatter and Gather

| API | Function Description |
| --- | --- |
| [Gather](memory_vector_compute/scatter_gather/Gather.md) | The Gather API accepts an input tensor (src), an address offset tensor (srcOffset), and a base address (srcBaseAddr). It determines the index of the input tensor based on the base address and the address offset, and collects the corresponding elements from the input tensor into the result tensor (dst). |
| [Gatherb(ISASI)](memory_vector_compute/scatter_gather/Gatherb_ISASI.md) | The Gatherb API accepts an input tensor (src) and a DataBlock offset tensor (offset). Based on the index position, it collects the input tensor into the result tensor (dst) by DataBlock (32 bytes). |
| [Scatter(ISASI)](memory_vector_compute/scatter_gather/Scatter_ISASI.md) | Given a continuous input tensor and a destination address offset tensor, the Scatter instruction generates a new result tensor based on the offset address and then scatters the input tensor into the result tensor. |

### Mask Operations

| API | Function Description |
| --- | --- |
| [SetMaskCount](memory_vector_compute/mask_operations/SetMaskCount.md) | Sets the mask mode to Counter mode. In this mode, developers do not need to be aware of the iteration count or handle operations such as unaligned tail blocks. The amount of data to be computed can be passed directly, and the actual iteration count is automatically inferred by the Vector computation unit. This API is recommended to be used together with the **isSetMask** template parameter in the API. When **isSetMask** is **false**, users can call this API to manually manage the Counter mode and use **SetVectorMask** to set the number of elements participating in the computation in Counter mode. |
| [SetMaskNorm](memory_vector_compute/mask_operations/SetMaskNorm.md) | Sets the mask mode to Normal mode. This mode is the system default mode and supports developers in configuring the iteration count. This API is recommended to be used together with the **isSetMask** template parameter in the API. When **isSetMask** is **false**, users can call this API to manually manage the Normal mode and use **SetVectorMask** to set the mask in Normal mode. |
| [SetVectorMask](memory_vector_compute/mask_operations/SetVectorMask.md) | This API is used to set the mask during vector computation. |
| [ResetMask](memory_vector_compute/mask_operations/ResetMask.md) | Restores the mask value to the default value (all 1s), indicating that all elements in each iteration of the vector computation will participate in the operation. |

### Data Rearrangement (ISASI)

| API | Function Description |
| --- | --- |
| [Interleave](memory_vector_compute/data_rearrange_ISASI/Interleave.md) | Given source operands src0 and src1, interleaves the elements of src0 and src1 into destination operands dst0 and dst1. |
| [DeInterleave](memory_vector_compute/data_rearrange_ISASI/DeInterleave.md) | Given source operands src0 and src1, de-interleaves the elements of src0 and src1 into destination operands dst0 and dst1. |

## Reg Vector Computation

### Register Data Type

| API | Function Description |
| --- | --- |
| [RegTensor](reg_vector_compute/register_data_types/RegTensor.md) | The basic unit of Reg vector computation. The bit width of RegTensor is VL (Vector Length), and the specific value may vary depending on the AI processor model. |
| [MaskReg](reg_vector_compute/register_data_types/MaskReg.md) | MaskReg is used to indicate which elements participate in the computation, with a width of one-eighth of RegTensor (VL/8). |
| [UnalignRegForLoad & UnalignRegForStore](reg_vector_compute/register_data_types/UnalignRegForLoad-UnalignRegForStore.md) | UnalignRegForLoad and UnalignRegForStore are used as buffers to optimize the overhead of contiguous unaligned address access between UB and RegTensor. Before reading an unaligned address, UnalignRegForLoad and UnalignRegForStore should be initialized through the LoadUnalignPre API, and then the LoadUnalign API is used. When writing an unaligned address, the StoreUnalign API is used first, |
| [AddrReg](reg_vector_compute/register_data_types/AddrReg.md) | AddrReg is the Address Register, a register used to store address offsets. AddrReg should be initialized through the CreateAddrReg API, and then used in a loop to store address offsets. AddrReg increments in each loop level according to the configured stride. |

### Reg Data Movement

| API | Function Description |
| --- | --- |
| [Continuous Aligned Load](reg_vector_compute/reg_data_load/LoadAlign_continuous.md) | Reg vector computation data movement API, used for continuous aligned load from UB into **RegTensor**. In single-load mode, data can be moved from UB to one destination register; in dual-load mode, data can be moved from UB to two destination registers. |
| [Continuous Aligned Store](reg_vector_compute/reg_data_store/StoreAlign_continuous.md) | Reg vector computation data movement API, used for continuous aligned store from **RegTensor** to UB. |
| [Non-continuous Aligned Load](reg_vector_compute/reg_data_load/LoadAlign_noncontinuous.md) | Reg vector computation data movement API, used for non-continuous aligned load from UB into **RegTensor** (in units of **DataBlock**). |
| [Non-continuous Aligned Store](reg_vector_compute/reg_data_store/StoreAlign_noncontinuous.md) | Reg vector computation data movement API, used for non-continuous aligned store from **RegTensor** to UB (in units of **DataBlock**). |
| [Contiguous Unaligned Move In](reg_vector_compute/reg_data_load/LoadUnAlign_continuous.md) | Reg vector computation data movement API, applicable to moving data contiguously into a RegTensor from a UB address that is not 32B aligned. |
| [Contiguous Unaligned Move Out](reg_vector_compute/reg_data_store/StoreUnAlign_continuous.md) | Reg vector computation data movement API, applicable to moving data contiguously and unaligned from a RegTensor out to UB. |
| [MaskReg Load](reg_vector_compute/reg_data_load/LoadAlign_MaskReg.md) | Reg vector computation data movement API, used for loading from UB or **RegTensor** into **MaskReg**. |
| [MaskReg Store](reg_vector_compute/reg_data_store/StoreAlign_MaskReg.md) | Reg vector computation data movement API, used for storing from **MaskReg** to UB. |
| [Move](reg_vector_compute/reg_data_load/Move.md) | Copies each valid element in **srcReg** to the corresponding position in **dstReg** one by one. |

### MaskReg Computation

| API | Function Description |
| --- | --- |
| [Move](reg_vector_compute/MaskReg_compute/Move.md) | Copies the elements in src to the corresponding positions in dst. If an input mask is provided, only the valid elements selected by the mask are copied, and invalid elements are filled with 0. |
| [Interleave](reg_vector_compute/MaskReg_compute/Interleave.md) | Interleaves the elements of source operands src0 and src1 and stores them into destination operands dst0 and dst1. |
| [DeInterleave](reg_vector_compute/MaskReg_compute/DeInterleave.md) | De-interleaves the elements of source operands src0 and src1 and stores them into destination operands dst0 and dst1. |
| [Select](reg_vector_compute/MaskReg_compute/Select.md) | Given two source operands src0 and src1, selects elements based on the bit values of mask to obtain the destination operand dst. The selection rule is: when a mask bit is 1, the element at the corresponding position is selected from src0; when the bit is 0, the element is selected from src1. |
| [Pack](reg_vector_compute/MaskReg_compute/Pack.md) | Based on the selected low-bit mode or high-bit mode, extracts the even-numbered bits of the source operand src into the lower half or upper half of the destination operand dst. |
| [UnPack](reg_vector_compute/MaskReg_compute/UnPack.md) | Based on the selected low-bit mode or high-bit mode, expands the lower half or upper half of the source operand src into the destination operand dst. The expansion method is: each bit is expanded to 2 bits, with the high bit set to zero. |
| [MoveMask](reg_vector_compute/MaskReg_compute/MoveMask.md) | Reads the Mask value from the mask registers {MASK1, MASK0} set by SetVectorMask, converts it according to the data format corresponding to the template parameter T, and writes it into the return value MaskReg. |

### Basic Arithmetic

| API | Function Description |
| --- | --- |
| [Abs](reg_vector_compute/basic_arithmetic/Abs.md) | Computes the absolute value of each valid element in **srcReg** and writes the result to the corresponding position in **dstReg**. |
| [Relu](reg_vector_compute/basic_arithmetic/Relu.md) | This API performs a ReLU (Rectified Linear Unit) operation on the input data **srcReg** according to the mask and writes the result to **dstReg**. |
| [Exp](reg_vector_compute/basic_arithmetic/Exp.md) | This API performs an element-wise exponential operation on the source operand **srcReg** according to the mask and writes the result to the destination operand **dstReg**. |
| [Sqrt](reg_vector_compute/basic_arithmetic/Sqrt.md) | This API performs an element-wise square root operation on the source operand **srcReg** according to the mask and writes the result to the destination operand **dstReg**. |
| [Ln](reg_vector_compute/basic_arithmetic/Ln.md) | This API computes the natural logarithm of the source operand element-wise according to the mask and writes the result to the destination operand. |
| [Log](reg_vector_compute/basic_arithmetic/Log.md) | This API computes the natural logarithm of the source operand element-wise according to the mask and writes the result to the destination operand. |
| [Log2](reg_vector_compute/basic_arithmetic/Log2.md) | This API is used to compute the base-2 logarithm of the input data **srcReg** according to the mask and write the result to **dstReg**. |
| [Log10](reg_vector_compute/basic_arithmetic/Log10.md) | This API computes the base-10 logarithm of the source operand element-wise according to the mask and writes the result to the destination operand. |
| [Neg](reg_vector_compute/basic_arithmetic/Neg.md) | This API is used to negate the input data **srcReg** according to the mask and write the result to **dstReg**. |
| [Add](reg_vector_compute/basic_arithmetic/Add.md) | This API performs an element-wise addition operation on the source operands **srcReg0** and **srcReg1** according to the mask and writes the result to the destination operand **dstReg**. |
| [Sub](reg_vector_compute/basic_arithmetic/Sub.md) | This API performs an element-wise subtraction operation on the source operands **srcReg0** and **srcReg1** according to the mask and writes the result to the destination operand **dstReg**. |
| [Mul](reg_vector_compute/basic_arithmetic/Mul.md) | This API performs an element-wise multiplication operation on the input data **srcReg0** and **srcReg1** according to the mask and writes the result to **dstReg**. |
| [Div](reg_vector_compute/basic_arithmetic/Div.md) | This API performs an element-wise division operation on the source operands **srcReg0** and **srcReg1** according to the mask and writes the result to the destination operand **dstReg**. |
| [Max](reg_vector_compute/basic_arithmetic/Max.md) | This API performs an element-wise maximum operation on the source operands **srcReg0** and **srcReg1** according to the mask and writes the result to the destination operand **dstReg**. |
| [Min](reg_vector_compute/basic_arithmetic/Min.md) | This API performs an element-wise minimum operation on the source operands **srcReg0** and **srcReg1** according to the mask and writes the result to the destination operand **dstReg**. |
| [Mull](reg_vector_compute/basic_arithmetic/Mull.md) | This API performs an element-wise multiplication operation on the input data **srcReg0** and **srcReg1** according to the mask, writes the low-order part of the multiplication result to **dstReg0**, and writes the overflow (high-order) part to **dstReg1**. |
| [AddC](reg_vector_compute/basic_arithmetic/AddC.md) | This API performs an element-wise addition operation on the source operands **srcReg0**, **srcReg1**, and the input carry **carrySrc** according to the mask, writes the result to the destination operand **dstReg**, and writes the carry result of each element to **carry**. |
| [SubC](reg_vector_compute/basic_arithmetic/SubC.md) | This API performs an element-wise subtraction operation on the source operands **srcReg0**, **srcReg1**, and the input carry **carrySrc** according to the mask, writes the result to the destination operand **dstReg**, and writes the carry result of each element to **carry**. |
| [Prelu](reg_vector_compute/basic_arithmetic/Prelu.md) | This API is used to implement the PReLU (Parametric ReLU) activation function: when the corresponding element in the source operand **srcReg0** is greater than 0, the element is written directly to the destination operand **dstReg**; otherwise, the product of the corresponding elements in **srcReg0** and **srcReg1** is written to **dstReg**. |
| [Adds](reg_vector_compute/basic_arithmetic/Adds.md) | This API performs an element-wise addition operation on the source operand **srcReg** and the scalar **scalarValue** according to the mask and writes the result to the destination operand **dstReg**. |
| [Muls](reg_vector_compute/basic_arithmetic/Muls.md) | Computes the product of each element in the vector and a scalar. |
| [Maxs](reg_vector_compute/basic_arithmetic/Maxs.md) | This API compares each element of the source operand **srcReg** with a scalar according to the mask: if the element is greater than the scalar, the source operand value is taken; if the element is smaller than the scalar, the scalar value is taken. The result is written to the destination operand **dstReg**. |
| [Mins](reg_vector_compute/basic_arithmetic/Mins.md) | This API compares each element of the source operand **srcReg** with a scalar according to the mask: if the element is greater than the scalar, the scalar value is taken; if the element is smaller than the scalar, the source operand value is taken. The result is written to the destination operand **dstReg**. |
| [LeakyRelu](reg_vector_compute/basic_arithmetic/LeakyRelu.md) | This API is used to perform an element-wise Leaky ReLU (Leaky Rectified Linear Unit) operation: when an element in the source operand is greater than 0, the element is written directly to the destination operand; otherwise, the element is multiplied by the scalar value **scalarValue** and then written to the destination operand. |

### Logical Computation

| API | Function Description |
| --- | --- |
| [Not](reg_vector_compute/logical_compute/Not.md) | This section introduces two APIs, which are used to perform a bitwise NOT operation on the valid bits of RegTensor and MaskReg respectively, and save the result. |
| [And](reg_vector_compute/logical_compute/And.md) | This section introduces two APIs, which are used to perform a logical AND operation on the valid bits of RegTensor and MaskReg respectively, and save the result. |
| [Or](reg_vector_compute/logical_compute/Or.md) | This section introduces two APIs, which are used to perform a logical OR operation on the valid bits of RegTensor and MaskReg respectively, and save the result. |
| [Xor](reg_vector_compute/logical_compute/Xor.md) | This section introduces two APIs, which are used to perform a bitwise XOR operation on the valid bits of RegTensor and MaskReg respectively, and save the result. |
| [ShiftLeft](reg_vector_compute/logical_compute/ShiftLeft.md) | Based on mask, performs a left shift operation on the input data srcReg0 according to the corresponding elements of srcReg1, and writes the result to dstReg. |
| [ShiftRight](reg_vector_compute/logical_compute/ShiftRight.md) | Based on mask, performs a right shift operation on the input data srcReg0 according to the corresponding elements of srcReg1, and writes the result to dstReg. |
| [ShiftLefts](reg_vector_compute/logical_compute/ShiftLefts.md) | Performs a logical left shift on each element in the source operand, where the number of shift bits is determined by the input parameter scalarValue. |
| [ShiftRights](reg_vector_compute/logical_compute/ShiftRights.md) | Performs a right shift on each element in the source operand, where the number of shift bits is determined by the input parameter scalarValue. |

### Compound Computation

| API | Function Description |
| --- | --- |
| [Axpy](reg_vector_compute/composite_compute/Axpy.md) | Performs an element-wise multiply-accumulate operation on the input data **dstReg**, **srcReg**, and **scalarValue** based on **mask**, and writes the result to **dstReg**. |
| [AbsSub](reg_vector_compute/composite_compute/AbsSub.md) | Subtracts **srcReg1** from **srcReg0** and then computes the absolute value, writing the result to **dstReg** based on **mask**. |
| [ExpSub](reg_vector_compute/composite_compute/ExpSub.md) | Subtracts **srcReg1** from **srcReg0** and uses the difference as the exponent of e, writing the result to **dstReg** based on **mask**. |
| [MulDstAdd](reg_vector_compute/composite_compute/MulDstAdd.md) | Multiplies **dstReg** by **srcReg0** and then adds **srcReg1**, writing the result to **dstReg** based on **mask**. |
| [MulAddDst](reg_vector_compute/composite_compute/MulAddDst.md) | Multiplies **srcReg0** by **srcReg1** and then adds the value of **dstReg**, writing the result to **dstReg** based on **mask**. |
| [MulsCast](reg_vector_compute/composite_compute/MulsCast.md) | Multiplies **src** by **scalar** and then converts the result to the **half** type according to the **CAST_ROUND** mode, writing the result to **dst** based on **mask**. |

### Comparison and Selection

| API | Function Description |
| --- | --- |
| [Compare](reg_vector_compute/compare_and_select/Compare.md) | Compares two RegTensors element by element. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0. |
| [Compares](reg_vector_compute/compare_and_select/Compares.md) | Compares each element in a Tensor with a scalar element by element. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0. |
| [Select](reg_vector_compute/compare_and_select/Select.md) | Given two source operands srcReg0 and srcReg1, selects elements based on the bit values of the mask to obtain the destination operand dstReg. The selection rule is as follows: when the mask bit is 1, the element at the corresponding position is selected from srcReg0; when the bit is 0, the element at the corresponding position is selected from srcReg1. |
| [Squeeze](reg_vector_compute/compare_and_select/Squeeze.md) | Copies the valid elements selected by the mask from the input srcReg to dstReg in sequence. The valid elements are arranged contiguously from low to high in dstReg. The remaining positions in dstReg are set to 0. |

### Type Conversion

| API | Function Description |
| --- | --- |
| [Cast](reg_vector_compute/type_conversion/Cast.md) | Cast is used for data type precision conversion. It converts the source operand data type into the destination operand data type, supporting float-to-integer, float-to-float, integer-to-float, and integer-to-integer data type conversions. |
| [Truncate](reg_vector_compute/type_conversion/Truncate.md) | Truncates the floating-point elements in the source operand to integer values (retaining the original data type) and stores them in the destination operand. |

### Reduction Computation

| API | Function Description |
| --- | --- |
| [Reduce](reg_vector_compute/reduction_compute/Reduce.md) | The reduction instruction simplifies a data set into a single value or a smaller set based on ReduceType. |
| [ReduceDataBlock](reg_vector_compute/reduction_compute/ReduceDataBlock.md) | The reduction instruction simplifies a data set into a single value within each DataBlock (32B) based on ReduceType. |
| [PairReduceElem](reg_vector_compute/reduction_compute/PairReduceElem.md) | Adds two adjacent values in the input srcReg and stores the result in the lower bits of dstReg. |

### Data Fill

| API | Function Description |
| --- | --- |
| [Duplicate](reg_vector_compute/data_padding/Duplicate.md) | Supports both Scalar and Tensor modes. |

### Scatter Operations

| API | Function Description |
| --- | --- |
| [Gather](reg_vector_compute/scatter_operations/Gather.md) | This instruction collects the source operand **srcReg** into the destination operand **dstReg** element by element according to the index value **indexReg**. |

### Data Rearrangement

| API | Function Description |
| --- | --- |
| [Interleave](reg_vector_compute/data_reorder/Interleave.md) | Given source operand registers srcReg0 and srcReg1, interleaves the elements in srcReg0 and srcReg1 into the destination operand registers dstReg0 and dstReg1. |
| [DeInterleave](reg_vector_compute/data_reorder/DeInterleave.md) | Given source operand registers srcReg0 and srcReg1, de-interleaves the elements in srcReg0 and srcReg1 into the destination operand registers dstReg0 and dstReg1. |

### Data Compression

| API | Function Description |
| --- | --- |
| [Unsqueeze](reg_vector_compute/data_compression/Unsqueeze.md) | Decompresses the data in **dstReg** according to **mask**. Decompression method: the 0th element in **dstReg** is set to 0, and the i-th element in **dstReg** equals the number of 1s from the 0th to the \(i-1\)-th element in **mask**. The highest bit of **mask** is ignored and not counted. |
| [Pack](reg_vector_compute/data_compression/Pack.md) | Selects the lower 8 bits (for the b16 type), lower 16 bits (for the b32 type), or lower 32 bits (for the b64 type) of the elements in the source operand **srcReg** and writes them into the lower half or upper half of **dstReg**. |
| [UnPack](reg_vector_compute/data_compression/UnPack.md) | For unsigned integers, writes the elements in the lower half or upper half of the source operand **srcReg** into **dstReg** by extending the bit width with zero-padding of the high bits. For signed integers, writes the elements in the lower half or upper half of the source operand **srcReg** into **dstReg** by extending the bit width while preserving the sign bit. |

### Histogram Computation

| API | Function Description |
| --- | --- |
| [Histograms](reg_vector_compute/histogram_compute/Histograms.md) | Performs statistics on histogram data, adding the statistical results of the source operand srcReg data, including frequency statistics and cumulative statistics, to the base data of the destination operand dstReg. |

### Index Operations

| API | Function Description |
| --- | --- |
| [Arange](reg_vector_compute/index_operations/Arange.md) | This function uses the value of the passed **scalar** as the start value, generates incrementing/decrementing indexes, and stores the indexes in **dstReg**. |

### Synchronization Control

| API | Function Description |
| --- | --- |
| [LocalMemBar](reg_vector_compute/sync_control/LocalMemBar.md) | A synchronization instruction between different pipelines within a Reg vector computation macro function. This synchronization instruction specifies the source pipeline and the destination pipeline. The destination pipeline waits until all instructions on the source pipeline are complete before executing. In read/write scenarios, when the register used by a read instruction is the same as the register used by a write instruction, register ordering can be triggered, and the instructions are executed in code order without inserting a synchronization instruction. When the registers used are different, a synchronization instruction must be inserted to ensure that read and write instructions are executed in order. The same applies to write/write scenarios. |

### System Variable Access

| API | Function Description |
| --- | --- |
| [ClearSpr](reg_vector_compute/system_variable_access/ClearSpr.md) | Clears the specified special-purpose register. |

### Auxiliary Data Types

| API | Function Description |
| --- | --- |
| [PostLiteral](reg_vector_compute/aux_data_types/PostLiteral.md) | enum class PostLiteral  |
| [RegLayout](reg_vector_compute/aux_data_types/RegLayout.md) | enum class RegLayout  |
| [SatMode](reg_vector_compute/aux_data_types/SatMode.md) | Sets the saturation and non-saturation modes for Cast type conversion. |
| [MaskMergeMode](reg_vector_compute/aux_data_types/MaskMergeMode.md) | Determines whether the elements not selected by the mask are set to 0 for the dst element. |
| [RoundMode](reg_vector_compute/aux_data_types/RoundMode.md) | Controls the rounding mode. |

## Scalar Computation

| API | Function Description |
| --- | --- |
| [GetBitCount](scalar_compute/GetBitCount.md) | Counts the number of occurrences of a specified bit value (0 or 1) in the binary representation of a uint64_t value. |
| [CountLeadingZero](scalar_compute/CountLeadingZero.md) | Counts the number of leading zeros in the binary representation of a uint64_t value (the number of 0s from the most significant bit to the first '1'; returns 64 when the input is 0). |
| [CountBitsCntSameAsSignBit](scalar_compute/CountBitsCntSameAsSignBit.md) | Counts the number of consecutive bits, starting from the most significant value bit, that are the same as the sign bit in the binary representation of an int64_t value. |
| [GetSFFValue](scalar_compute/GetSFFValue.md) | Finds the position of the first specified bit value (0 or 1) starting from the least significant bit in the binary representation of a uint64_t value, and returns -1 if not found. |
| [Cast (float to half/int32_t)](scalar_compute/Cast_float_to_half_int32.md) | Converts the data type of a scalar. |
| [Cast (float to bfloat16_t)](scalar_compute/Cast_float_to_bfloat16.md) | Converts float scalar data to bfloat16_t scalar data. |
| [Cast (multiple types to float)](scalar_compute/Cast_multi_type_to_float.md) | This API converts the input data to the float type. |
| [Nop](scalar_compute/Nop.md) | The user inputs the number of delay cycles to implement a delay of several cycles. |
| [WriteGmBypassDCache(ISASI)](scalar_compute/WriteGmBypassDCache_ISASI.md) | Writes data to a GM address without going through the DCache. |
| [ReadGmBypassDCache(ISASI)](scalar_compute/ReadGmBypassDCache_ISASI.md) | Reads data from a GM address without going through the DCache. |

## Resource Management

| API | Function Description |
| --- | --- |
| [TPipe](resource_management/TPipe/TPipe.md) | TPipe is a framework used to manage resources such as global memory. Through the APIs provided by the TPipe class, allocation and management of resources such as memory can be completed. |
| [GetTPipePtr](resource_management/GetTPipePtr.md) | When a TPipe object is created, object initialization sets a globally unique TPipe pointer. This API is used to obtain this pointer. After obtaining the pointer, TPipe-related operations can be performed. |
| [TBufPool](resource_management/TBufPool/TBufPool.md) | TPipe can manage global memory resources, while TBufPool can manually manage or reuse UB/L1 Buffer physical memory. It is mainly used in scenarios where UB/L1 Buffer physical memory is insufficient during multi-stage computation. |
| [TQue](resource_management/TQue/TQue.md) | Provides APIs such as enqueue and dequeue, and completes synchronization between tasks through queues. |
| [TQueBind](resource_management/TQueBind/TQueBind.md) | TQueBind binds the source logical position and the destination logical position. Based on the source position and the destination position, it determines the location of memory allocation and inserts corresponding synchronization events, helping developers solve problems such as memory allocation and management and synchronization. |
| [TBuf](resource_management/TBuf/TBuf.md) | During Ascend C programming, some temporary variables may be used. The memory occupied by these temporary variables can be managed using the TBuf data structure. |
| [InitSpmBuffer](resource_management/TPipe/InitSpmBuffer.md) | Initializes the SPM Buffer. |
| [WriteSpmBuffer](resource_management/TPipe/WriteSpmBuffer.md) | Copies the data that needs to be spilled and temporarily stored into the SPM Buffer. |
| [ReadSpmBuffer](resource_management/TPipe/ReadSpmBuffer.md) | Reads data back from the SPM Buffer into local data. |
| [GetUserWorkspace](resource_management/workspace/GetUserWorkspace.md) | Obtains the workspace pointer used by the user. |
| [SetSysWorkSpace](resource_management/workspace/SetSysWorkSpace.md) | When performing fused operator programming, the framework communication mechanism requires the use of workspace, that is, the system workspace. Therefore, in this scenario, developers need to call this API to set the system workspace pointer. |
| [GetSysWorkSpacePtr](resource_management/workspace/GetSysWorkSpacePtr.md) | Obtains the system workspace pointer. |
| [PopStackBuffer](resource_management/PopStackBuffer.md) | Applies for temporary space at the specified position (logical position), with the space size being all the remaining space at the specified position. |

## Synchronization Control

### Intra-Core Synchronization

| API | Function Description |
| --- | --- |
| [TQueSync](sync_control/intra_core_sync/TQueSync.md) | The TQueSync class provides synchronization control APIs. Developers can use these APIs to perform synchronization control on their own. |
| [AllocMutexID (ISASI)](sync_control/intra_core_sync/AllocMutexID_ISASI.md) | Obtains and occupies a MutexID from the framework. It is used together with ReleaseMutexID to manage the acquisition and release of MutexIDs. The obtained MutexID can be passed in to the Mutex API, in which case the Mutex can be used together with other APIs such as TQue. |
| [DataSyncBarrier(ISASI)](sync_control/intra_core_sync/DataSyncBarrier_ISASI.md) | Blocks the execution of subsequent instructions until all preceding memory access instructions (the memory positions to wait for can be controlled by parameters) have completed execution. It is used to resolve Scalar memory dependencies. |
| [PipeBarrier(ISASI)](sync_control/intra_core_sync/PipeBarrier_ISASI.md) | Completes synchronization control within the same pipeline and is used to constrain the execution order within the same pipeline. Its function is to ensure that the read and write of all data in the preceding instructions are fully completed before the subsequent instructions can execute. |
| [ReleaseMutexID (ISASI)](sync_control/intra_core_sync/ReleaseMutexID_ISASI.md) | Releases a MutexID from the framework. It is used together with AllocMutexID. |
| [SetFlag/WaitFlag(ISASI)](sync_control/intra_core_sync/SetFlag_WaitFlag_ISASI.md) | The SetFlag/WaitFlag API is used for synchronization between multiple pipelines within a core. |

### Inter-Core Synchronization

| API | Description |
| --- | --- |
| [IBSet](sync_control/inter_core_sync/IBSet.md) | Call IBSet to set the flag of a certain core. It is used in pairs with IBWait and represents the synchronous wait instruction between cores: the waited core calls IBSet to notify the waiting core by writing the value 1 to a certain address block in global memory; the waiting core calls IBWait to continuously read the value of the corresponding address block in global memory until its value becomes 1, thereby implementing inter-core synchronization. |
| [IBWait](sync_control/inter_core_sync/IBWait.md) | Call IBSet to set the flag of a certain core. It is used in pairs with IBWait and represents the synchronous wait instruction between cores: the waited core calls IBSet to notify the waiting core by writing the value 1 to a certain address block in global memory; the waiting core calls IBWait to continuously read the value of the corresponding address block in global memory until its value becomes 1, thereby implementing inter-core synchronization. |
| [SyncAll](sync_control/inter_core_sync/SyncAll.md) | SyncAll is an inter-core synchronization control API. |
| [InitDetermineComputeWorkspace](sync_control/inter_core_sync/InitDetermineComputeWorkspace.md) | InitDetermineComputeWorkspace is an initialization configuration API for deterministic computation based on sequential execution between cores. It can initialize the value of GM shared memory. Only after the initialization is complete can WaitPreBlock and NotifyNextBlock be called. The above three APIs jointly implement deterministic computation based on sequential execution between cores. For the specific meaning of deterministic computation, see deterministic computation. |
| [NotifyNextBlock](sync_control/inter_core_sync/NotifyNextBlock.md) | WaitPreBlock and NotifyNextBlock are inter-core synchronization control APIs. NotifyNextBlock notifies other cores that the current core has completed execution by writing to global memory, so that other cores can continue execution; WaitPreBlock determines whether the current core can continue execution by reading global memory. |
| [WaitPreBlock](sync_control/inter_core_sync/WaitPreBlock.md) | WaitPreBlock and NotifyNextBlock are inter-core synchronization control APIs. NotifyNextBlock notifies other cores that the current core has completed execution by writing to global memory, so that other cores can continue execution; WaitPreBlock determines whether the current core can continue execution by reading global memory. |
| [CrossCoreSetFlag(ISASI)](sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md) | In the inter-core synchronization scenario, the CrossCoreSetFlag API and the CrossCoreWaitFlag API work in pairs. When used, the inter-core synchronization flag ID (flagId) must be passed in, and each ID corresponds to a counter used to control synchronization. The CrossCoreSetFlag API is used to notify the scheduling module that "the pipe pipeline task of this core has been completed", and the CrossCoreWaitFlag API is used to block the subsequent instruction dispatch until all related cores have completed synchronization, after which the blocking can be released. |
| [CrossCoreWaitFlag(ISASI)](sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md) | In the inter-core synchronization scenario, the CrossCoreSetFlag API and the CrossCoreWaitFlag API work in pairs. For specific functions, see [CrossCoreSetFlag](sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md). |
| [Arrive](sync_control/inter_core_sync/GroupBarrier/Arrive.md) | Notify other waiting AIVs that this AIV has completed the tasks it depends on. |
| [Wait](sync_control/inter_core_sync/GroupBarrier/Wait.md) | Block the current AIV until all AIVs in the Arrive group have completed their dependent tasks. |
| [GetWorkspaceLen](sync_control/inter_core_sync/GroupBarrier/GetWorkspaceLen.md) | Return the size of the Global Memory message space occupied by the current GroupBarrier. |

### Inter-Core Communication

| API | Description |
| --- | --- |
| [CreateCubeResGroup](sync_control/inter_core_communication/CubeResGroupHandle/CreateCubeResGroup.md) | Quickly create a CubeResGroupHandle object, and internally complete the allocation of the message queue space and synchronization events. It is recommended to use this API and avoid using the constructor of CubeResGroupHandle to create objects, which may cause cases such as message queue space conflicts and synchronization event errors between different objects. |
| [AssignQueue](sync_control/inter_core_communication/CubeResGroupHandle/AssignQueue.md) | Used for an AIV to bind the sequence number of a certain message queue in CubeResGroupHandle. |
| [AllocMessage](sync_control/inter_core_communication/CubeResGroupHandle/AllocMessage.md) | An AIV applies for message space from the message queue to store the message structure, and returns the address of the currently applied message space. The depth of the message queue is fixed at 4, and the order of applying for message space is top-down and then cyclic. When the message space pointed to by the message queue pointer is in the FREE state, AllocMessage returns the address of the space; otherwise, it waits in a loop until the current space is in the FREE state. |
| [PostMessage](sync_control/inter_core_communication/CubeResGroupHandle/PostMessage.md) | After obtaining the message space address msg through the AllocMessage API, construct the message structure CubeMsgType and send the message. |
| [PostFakeMsg](sync_control/inter_core_communication/CubeResGroupHandle/PostFakeMsg.md) | After obtaining the message space address through the AllocMessage API, the AIV sends a fake message and refreshes the message state msgState to FAKE. |
| [SetQuit](sync_control/inter_core_communication/CubeResGroupHandle/SetQuit.md) | After obtaining the message space address through the AllocMessage API, send a quit message to inform the AIC corresponding to the message queue that it no longer needs to process the messages of this queue. After the AIV corresponding to Queue5 sends the quit message, Block1 will no longer process any message of Queue5. |
| [Wait](sync_control/inter_core_communication/CubeResGroupHandle/Wait.md) | After calling PostMessage or PostFakeMessage, query whether the message has been processed by the AIC. |
| [FreeMessage](sync_control/inter_core_communication/CubeResGroupHandle/FreeMessage.md) | In the custom callback function logic, after the message processing is complete, call this API to refresh the message state to FREE; or refresh the message state to FREE when the message state is the specified state waitState. |
| [SetSkipMsg](sync_control/inter_core_communication/CubeResGroupHandle/SetSkipMsg.md) | The AIC skips the processing of a specified number of fake messages, and it is called only in the callback function. In the following figure, Block0 skips three fake messages by calling SetSkipMsg. |
| [UpdateKfcWorkspace](sync_control/inter_core_communication/KfcWorkspace/UpdateKfcWorkspace.md) | Update the memory address used for the CubeResGroupHandle message communication area. When the user uses the CubeResGroupHandle API, this API must be used to manage the space address independently. |
| [GetKfcWorkspace](sync_control/inter_core_communication/KfcWorkspace/GetKfcWorkspace.md) | Obtain the memory address used for the CubeResGroupHandle message communication area. When the user uses the CubeResGroupHandle API, this API must be used to manage the space address independently. |

### Inter-Task Synchronization

| API | Description |
| --- | --- |
| [SetNextTaskStart](sync_control/inter_task_sync/SetNextTaskStart.md) | Called in the sub-kernel function (Kernel) of SuperKernel. After this API is called, the subsequent instructions can be executed in parallel with other subsequent sub-kernel functions (Kernel), improving overall performance. SuperKernel calls sub-kernel functions (Kernel) in order. To ensure that data between sub-kernel functions (Kernel) does not interfere with each other, inter-operator synchronization is inserted between sub-kernel functions (Kernel) to preserve order. After sub-KernelN-1 calls this API, the subsequent instructions are executed in parallel with the subsequent sub-KernelN. |
| [WaitPreTaskEnd](sync_control/inter_task_sync/WaitPreTaskEnd.md) | Called in the sub-kernel function (Kernel) of SuperKernel. Before this API is called, the preceding instructions can be executed in parallel with other preceding sub-kernel functions (Kernel), improving overall performance. SuperKernel calls sub-kernel functions (Kernel) in order. To ensure that data between sub-kernel functions (Kernel) does not interfere with each other, inter-operator synchronization is inserted between sub-kernel functions (Kernel) to preserve order. The instructions before sub-KernelN+1 calls this API are executed in parallel with the preceding sub-KernelN. |

## Cache Control

| API | Function Description |
| --- | --- |
| [DataCachePreload](cache_control/DataCachePreload.md) | Preloads data from the specific GM address where the source address resides into the DCache. Each call can preload only one Cache Line of data. |
| [DataCacheCleanAndInvalid](cache_control/DataCacheCleanAndInvalid.md) | Within the AI Core, both the Scalar unit and the DMA unit may access the GM. |
| [ICachePreLoad(ISASI)](cache_control/ICachePreLoad_ISASI.md) | Developers can manually call the ICachePreLoad API to preload instructions from the GM address where the instructions reside into the ICache. |
| [PRELOAD](cache_control/PRELOAD.md) | Starting from the GM address where the current instruction is located, preloads subsequent instructions into the ICache. |
| [GetICachePreloadStatus(ISASI)](cache_control/GetICachePreloadStatus_ISASI.md) | GetICachePreloadStatus is a debug API, called after ICachePreLoad, used to obtain the PreLoad status of the ICache: when the return value is 0, it indicates that the ICache PreLoad is complete; when the return value is 1, it indicates that the ICache PreLoad is not complete. |

## Atomic Operations

| API | Function Description |
| --- | --- |
| [SetAtomicAdd](atomic_operations/SetAtomicAdd.md) | Enables atomic accumulation for subsequent data movement whose destination address is GM. Atomic accumulation process: sums the content to be copied with the existing content in GM, and then writes the sum result to GM. The SetAtomicAdd API can set different accumulation data types through template parameters. |
| [SetAtomicType](atomic_operations/SetAtomicType.md) | Sets different data types for atomic operations by configuring template parameters. |
| [DisableDmaAtomic](atomic_operations/DisableDmaAtomic.md) | Disables the on-the-fly atomic operation function of data movement. When data movement is subsequently executed, the original data in GM will be completely overwritten by the newly moved data. |
| [SetAtomicMax(ISASI)](atomic_operations/SetAtomicMax_ISASI.md) | Sets whether the data subsequently moved to GM performs atomic comparison: compares the content to be copied with the existing content in GM, and then writes the maximum value to GM. The SetAtomicMax API can set different data types by configuring template parameters. |
| [SetAtomicMin(ISASI)](atomic_operations/SetAtomicMin_ISASI.md) | Sets whether the data subsequently moved to GM performs atomic comparison: compares the content to be copied with the existing content in GM, and then writes the minimum value to GM. The SetAtomicMin API can set different data types by configuring template parameters. |
| [SetStoreAtomicConfig(ISASI)](atomic_operations/SetStoreAtomicConfig_ISASI.md) | Sets the atomic operation enable bit and the atomic operation type. |
| [GetStoreAtomicConfig(ISASI)](atomic_operations/GetStoreAtomicConfig_ISASI.md) | Obtains the values of the atomic operation enable bit and the atomic operation type. |
| [AtomicAdd](atomic_operations/AtomicAdd.md) | After this API is called, an atomic addition operation can be performed on the specified GM address. |
| [AtomicMin](atomic_operations/AtomicMin.md) | After this API is called, an atomic comparison-and-minimum operation can be performed on the specified GM address. |
| [AtomicMax](atomic_operations/AtomicMax.md) | After this API is called, an atomic maximum operation can be performed on the specified GM address. |
| [AtomicCas](atomic_operations/AtomicCas.md) | After this API is called, an atomic comparison can be performed on the specified GM address. If the value equals value1, the value of value2 is assigned to GM; if the value does not equal value1, the value in GM remains unchanged. |
| [AtomicExch](atomic_operations/AtomicExch.md) | Performs an atomic exchange operation in GM memory. Specifically, it reads the data at the specified GM address and stores a new value back to the same address. The function returns the old value. |

## Debug API

| API | Function Description |
| --- | --- |
| [DumpTensor](debug_interface/onboard_print/DumpTensor.md) | This API prints the content of a Tensor and also supports printing custom labels (only information of the uint32_t data type), such as the current line number. |
| [DumpAccChkPoint](debug_interface/onboard_print/DumpAccChkPoint.md) | This API dumps the content of a specified Tensor. It also supports printing custom labels (only information of the uint32_t data type), such as the current line number. Unlike DumpTensor, this API supports printing a Tensor at a specified offset position. |
| [PrintTimeStamp](debug_interface/onboard_print/PrintTimeStamp.md) | Provides a timestamp marking function, used to mark key execution points in the operator kernel function (Kernel) code. |
| [Trap](debug_interface/exception_detection/Trap.md) | Called on the kernel function (Kernel) side. In NPU mode, it interrupts the execution of the AI Core; in CPU mode, it is equivalent to assert. It can be used for debugging abnormal scenarios on the kernel function (Kernel) side. |
| [CheckLocalMemoryIA(ISASI)](debug_interface/exception_detection/CheckLocalMemoryIA_ISASI.md) | Checks the read and write behavior of the Unified Buffer (UB) within the specified range. If there is read or write behavior within the specified range, an EXCEPTION error occurs; if there is no read or write behavior within the specified range, no error is reported. |
| [GmAlloc](debug_interface/cpu_twin_debug/GmAlloc.md) | Used to create shared memory when performing CPU-side execution verification of the kernel function (Kernel): creates a shared file in the /tmp directory and returns the mapping pointer of the file. |
| [ICPU_RUN_KF](debug_interface/cpu_twin_debug/ICPU_RUN_KF.md) | The overall entry point for CPU debugging when performing CPU-side execution verification of the kernel function (Kernel), which completes the operator program invocation on the CPU side. |
| [ICPU_SET_TILING_KEY](debug_interface/cpu_twin_debug/ICPU_SET_TILING_KEY.md) | Used to specify the tilingKey for this CPU debugging session. During debugging execution, only the branch corresponding to this tilingKey in the operator kernel function (Kernel) is executed. |
| [GmFree](debug_interface/cpu_twin_debug/GmFree.md) | Used to release the shared memory allocated through GmAlloc when performing CPU-side execution verification of the kernel function (Kernel). |
| [SetKernelMode](debug_interface/cpu_twin_debug/SetKernelMode.md) | For the separate mode, during CPU debugging, sets the kernel mode to single-AIV mode, single-AIC mode, or MIX mode to support CPU debugging of single-AIV vector operators, single-AIC matrix operators, and MIX hybrid operators, respectively. If this API is not called, the default is MIX mode. To ensure compatibility of operator code across multiple hardware platforms, it can also be called in coupled mode; in this scenario, the API does not take effect and does not affect normal debugging. |
| [MetricsProfStart](debug_interface/performance_stats/MetricsProfStart.md) | Used to set the start signal for performance data collection, and is used together with MetricsProfStop. When using the msOpProf tool for on-board operator tuning, MetricsProfStart and MetricsProfStop can be called before and after the kernel-side code segment to specify the range of the code segment to be tuned. |
| [MetricsProfStop](debug_interface/performance_stats/MetricsProfStop.md) | Sets the stop signal for performance data collection, and is used together with MetricsProfStart. When using the msOpProf tool for on-board operator tuning, MetricsProfStart and MetricsProfStop can be called before and after the kernel-side code segment to specify the range of the code segment to be tuned. |
| [MarkStamp](debug_interface/performance_stats/MarkStamp.md) | The user calls this API to mark a specific position during operator execution, facilitating later analysis of the code execution path and performance hotspots through the pipeline graph. |

## Tool API

| API | Function Description |
| --- | --- |
| [Async](tool_interface/execution_mode/Async.md) | **Async** encapsulates this isolation mode through a template function and provides a unified API for executing a specific function on different execution units (AIC or AIV), thereby avoiding the use of hardware conditional branches in the code. |
| [InitSocState](tool_interface/system_init/InitSocState.md) | This API initializes the global state of the AI Core, including the common state and the respective dedicated states of the AIC (Cube Core) and AIV (Vector Core). Because implementations differ across products, the initialization items actually executed also differ. |
| [Max](tool_interface/NumericLimits_utils/Max.md) | Returns the maximum finite value of the specified data type. |
| [Lowest](tool_interface/NumericLimits_utils/Lowest.md) | Returns the lowest finite value of the specified data type, that is, the finite value x for which there is no other finite value y satisfying y < x. |
| [Min](tool_interface/NumericLimits_utils/Min.md) | Returns the minimum finite value of the specified data type. |
| [Infinity](tool_interface/NumericLimits_utils/Infinity.md) | Returns the positive infinity value of the specified data type. |
| [NegativeInfinity](tool_interface/NumericLimits_utils/NegativeInfinity.md) | Returns the negative infinity value of the specified data type. |
| [QuietNaN](tool_interface/NumericLimits_utils/QuietNaN.md) | Returns the quiet NaN value of the specified data type (the highest bit of the floating-point mantissa is 1). |
| [SignalingNaN](tool_interface/NumericLimits_utils/SignalingNaN.md) | Returns the signaling NaN value of the specified data type (the highest bit of the floating-point mantissa is 0). |
| [DeNormMin](tool_interface/NumericLimits_utils/DeNormMin.md) | Returns the smallest positive denormal value of the specified data type. |
| [GetTaskRatio](tool_interface/system_resources_and_variables/GetTaskRatio.md) | In separate mode, obtains the ratio of the number of Cube Cores (AIC) or Vector Cores (AIV) started for a task to the number of logical AI Cores. |
| [GetUBSizeInBytes](tool_interface/system_resources_and_variables/GetUBSizeInBytes.md) | Obtains the size of the UB space, in bytes. Developers calculate parameter values such as the number of loop iterations based on the UB size. |
| [GetRuntimeUBSize](tool_interface/system_resources_and_variables/GetRuntimeUBSize.md) | Obtains the size of the runtime UB space, in bytes. Developers calculate parameter values such as the number of loop iterations based on the UB size. |
| [GetVecLen](tool_interface/system_resources_and_variables/GetVecLen.md) | Obtains the size of the RegTensor bit width VL (Vector Length). |
| [GetSsbufBaseAddr](tool_interface/system_resources_and_variables/GetSsbufBaseAddr.md) | This API is used to obtain the base address of the SSBuffer. |
| [GetSystemCycle(ISASI)](tool_interface/system_resources_and_variables/GetSystemCycle_ISASI.md) | Obtains the current cycle value of the system counter. By calculating the difference between the cycle values before and after a code segment, the number of system cycles consumed by the code segment can be obtained, and the execution time of the code segment can then be derived through conversion. |
| [GetBlockNum](tool_interface/system_resources_and_variables/GetBlockNum.md) | Obtains the number of logical AI Cores configured for the current task, which is used for multi-core logic control within the code. |
| [GetBlockIdx](tool_interface/system_resources_and_variables/GetBlockIdx.md) | Obtains the index of the current logical AI Core, which is used for multi-core logic control and multi-core offset calculation within the code. |
| [GetDataBlockSizeInBytes](tool_interface/system_resources_and_variables/GetDataBlockSizeInBytes.md) | Obtains the size of one DataBlock on the current chip version, in bytes. |
| [GetArchVersion](tool_interface/system_resources_and_variables/GetArchVersion.md) | Obtains the architecture version number of the current AI processor. |
| [GetProgramCounter(ISASI)](tool_interface/system_resources_and_variables/GetProgramCounter_ISASI.md) | Obtains the value of the program counter to track the current execution position of the program. |
| [GetSubBlockNum(ISASI)](tool_interface/system_resources_and_variables/GetSubBlockNum_ISASI.md) | In the separate mode architecture, obtains the number of Cube Cores (AIC) or Vector Cores (AIV) on one logical AI Core in the current configuration. |
| [GetSubBlockIdx(ISASI)](tool_interface/system_resources_and_variables/GetSubBlockIdx_ISASI.md) | In the separate mode architecture, obtains the logical index of the Cube Core (AIC) or Vector Core (AIV) on the logical AI Core. |
| [GET_TILING_DATA](tool_interface/tiling_data_acquisition/GET_TILING_DATA.md) | Used to obtain the Tiling information passed into the operator kernel function (Kernel) entry function and fill it into the registered TilingData structure. This function is compiled by macro expansion. The corresponding operator host implementation needs to define the TilingData structure, and implement and register the Tiling function that computes the TilingData. If the user registers multiple TilingData structures through TilingData structure registration, this API returns the structure registered by default. |
| [GET_TILING_DATA_WITH_STRUCT](tool_interface/tiling_data_acquisition/GET_TILING_DATA_WITH_STRUCT.md) | Use this API to specify the structure name, obtain the specified tiling information, and fill it into the corresponding Tiling structure. This function is compiled by macro expansion. The difference from GET\_TILING\_DATA is that GET\_TILING\_DATA can only obtain the structure registered by default, whereas this API can obtain the corresponding structure according to the specified structure name. It is commonly used when different structures are registered for different TilingKeys. |
| [GET_TILING_DATA_MEMBER](tool_interface/tiling_data_acquisition/GET_TILING_DATA_MEMBER.md) | Used to obtain the member variable of the tiling structure. |
| [GET_TILING_DATA_PTR_WITH_STRUCT](tool_interface/tiling_data_acquisition/GET_TILING_DATA_PTR_WITH_STRUCT.md) | When using this macro, the developer can obtain the corresponding Tiling information by specifying the structure name and fill it into the corresponding Tiling structure. After the filling is complete, this macro returns a pointer to the Tiling structure and modifies the pointer with the \_\_tiling\_data\_ptr\_\_ modifier. This modification method ensures the uniformity and compatibility of the code in dynamic and static Shape scenarios. |
| [COPY_TILING_WITH_STRUCT](tool_interface/tiling_data_acquisition/COPY_TILING_WITH_STRUCT.md) | Copies the Tiling structure and returns a pointer to the copied Tiling structure. This macro is applicable to nested structure scenarios and can copy the sub-structure member variables of a structure. This macro copies the specified structure to the stack, and is applicable to scenarios where Tiling data is accessed frequently, which can accelerate data access. |
| [COPY_TILING_WITH_ARRAY](tool_interface/tiling_data_acquisition/COPY_TILING_WITH_ARRAY.md) | Copies the array content of the specified size to the target array and returns a pointer to the copied array. It is applicable to the scenario of copying the array member variable of a structure. This macro copies the specified array to the stack, and is applicable to scenarios where Tiling data is accessed frequently, which can accelerate data access. |
| [TILING_KEY_IS](tool_interface/tiling_key_management/TILING_KEY_IS.md) | In the kernel function (Kernel), determines whether the tiling_key of the current execution is equal to a certain key set on the host side at runtime, thereby identifying a kernel function (Kernel) branch where tiling_key==key. |
| [TILING_KEY_LIST](tool_interface/tiling_key_management/TILING_KEY_LIST.md) | The TILING_KEY_LIST function is used in the kernel function (Kernel) to determine whether the TilingKey of the current execution matches the specified TilingKey configured on the Host side, thereby identifying the branch logic that satisfies the condition TilingKey == key1 or TilingKey == key2. |
| [REGISTER_TILING_DEFAULT](tool_interface/tiling_key_management/REGISTER_TILING_DEFAULT.md) | Used to register, on the kernel function (Kernel) side, the default TilingData structure customized by the user using standard C++ syntax. |
| [REGISTER_TILING_FOR_TILINGKEY](tool_interface/tiling_key_management/REGISTER_TILING_FOR_TILINGKEY.md) | Used to register, on the kernel function (Kernel) side, the custom TilingData structure that matches the TilingKey. This API requires a logical expression, in which the string "TILING_KEY_VAR" represents the actual TilingKey and expresses the range satisfied by the TilingKey. |
| [REGISTER_NONE_TILING](tool_interface/tiling_key_management/REGISTER_NONE_TILING.md) | When using a TilingData structure customized with standard C++ syntax on the kernel function (Kernel) side, if the user is uncertain about which structures need to be registered, this API can be used to inform the framework side that the TilingData should be defined using unregistered standard C++ syntax, together with GET\_TILING\_DATA\_WITH\_STRUCT, GET\_TILING\_DATA\_MEMBER, and GET\_TILING\_DATA\_PTR\_WITH\_STRUCT to obtain the corresponding TilingData. |
| [Set the kernel function (Kernel) type](tool_interface/execution_mode/set_Kernel_type.md) | Used for the user to customize the kernel function (Kernel) type, so that when the operator is executed, only kernels of this type are started, avoiding starting kernels that do not need to work and reducing the kernel startup overhead. |

## Special-Purpose Register Access

| API | Function Description |
| --- | --- |
| [SetCtrlSpr(ISASI)](special_register_access/SetCtrlSpr_ISASI.md) | Sets specific bits of the CTRL register (control register). |
| [GetCtrlSpr(ISASI)](special_register_access/GetCtrlSpr_ISASI.md) | Reads the value of specific bits of the CTRL register (control register). |
| [ResetCtrlSpr(ISASI)](special_register_access/ResetCtrlSpr_ISASI.md) | Resets specific bits of the CTRL register (control register). |
| [GetSaturationFlag(ISASI)](special_register_access/GetSaturationFlag_ISASI.md) | Obtains the enabled state of the specified saturation mode during computation or precision conversion. |
| [SetSaturationFlag(ISASI)](special_register_access/SetSaturationFlag_ISASI.md) | Sets the enabled state of the specified saturation mode during computation or precision conversion. |
| [GetSaturationStrategy](special_register_access/GetSaturationStrategy.md) | Obtains the currently used saturation mode configuration strategy. |
| [SetSaturationStrategy](special_register_access/SetSaturationStrategy.md) | Sets the saturation mode configuration strategy, selecting whether to use the saturation configuration of the specific computation API itself or the saturation mode set by SetSaturationFlag. |
