# Basic API List

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-09-01T07:24:12.651Z -->

## Basic Data Structure

| API Name | Description |
| --- | --- |
| [LocalTensor](data_structures/LocalTensor/LocalTensor.md) | LocalTensor is used to store data in the Local Memory (internal storage) of the AI Core, supporting logical positions TPosition of VECIN, VECOUT, VECCALC, A1, A2, B1, B2, CO1, and CO2. |
| [GlobalTensor](data_structures/GlobalTensor/GlobalTensor.md) | GlobalTensor is used to store global data in Global Memory (external storage). |
| [layout_structure](data_structure_TensorAPI/data_structure_TensorAPI.md) | Layout-related basic data structures in the Tensor API basic data structures. |
| [tensor_structure](data_structure_TensorAPI/data_structure_TensorAPI.md) | Tensor-related data structures in the Tensor API basic data structures. |
| [GetLayoutPattern](data_structure_TensorAPI/utils/GetLayoutPattern.md) | GetLayoutPattern is used to extract the LayoutPattern type information from a Layout type. |
| [Coordinate](aux_data_structures/Coordinate/Coordinate.md) | Coordinate is essentially a tuple used to represent the position information, i.e., coordinate values, of a tensor in different dimensions. |
| [Layout](aux_data_structures/Layout/Layout.md) | The Layout data structure is a basic template class that describes the memory layout of a multi-dimensional tensor. It maps the logical coordinate space to the one-dimensional memory address space through compile-time shape and stride information. |
| [TensorTrait](aux_data_structures/TensorTrait/TensorTrait.md) | The TensorTrait data structure is a basic template class that describes Tensor-related information, including the data type, logical position, and Layout memory layout of the Tensor. |
| [ShapeInfo](aux_data_structures/ShapeInfo.md) | Stores the shape information of LocalTensor or GlobalTensor. |
| [ListTensorDesc](aux_data_structures/ListTensorDesc.md) | Parses data that conforms to a specified memory layout format, and obtains the address and shape information of the corresponding data by index on the kernel side. |
| [TensorDesc](aux_data_structures/TensorDesc/TensorDesc.md) | Stores the Tensor description information obtained by index in ListTensorDesc.GetDesc(). |
| [UnaryRepeatParams](aux_data_structures/UnaryRepeatParams.md) | A data structure that controls the address stride of a single operand, including the address stride parameters of DataBlock between adjacent iterations and within the same iteration. |
| [BinaryRepeatParams](aux_data_structures/BinaryRepeatParams.md) | A data structure that controls the address stride of dual operands, including the address stride parameters of DataBlock between adjacent iterations and within the same iteration. |
| [complex32/complex64](aux_data_structures/complex32-complex64.md) | Complex number types. For complex32, both the real and imaginary parts are of the half type with a bit width of 32 bits. For complex64, both the real and imaginary parts are of the float type with a bit width of 64 bits. |
| [TPosition](aux_data_structures/TPosition.md) | Uses an abstract logical position (TPosition) to represent storage at various levels, replacing the concept of on-chip physical storage. The main types include: VECIN, VECOUT, VECCALC, A1, A2, B1, B2, CO1, and CO2. |

## Cube Computation (ISASI)

### Cube Data Load to L0-Buffer

| API Name | Description |
| --- | --- |
| [LoadData (Convolution Data Movement)](cube_compute_ISASI/cube_compute_load/Load3D.md) | Load3D is essentially used to perform Image to Column expansion on the Feature Map in NC1HWC0 format, and then select the specified data block from the expanded two-dimensional cube and load it into the corresponding memory location. |
| [LoadDataWithTranspose](cube_compute_ISASI/cube_compute_load/LoadDataWithTranspose.md) | LoadDataWithTranspose is responsible for moving 2D-format data required by ordinary cube computation. The movement process is accompanied by a transpose operation. For details, see the fractal transpose feature. |
| [LoadDataWithSparse](cube_compute_ISASI/cube_compute_load/LoadDataWithSparse.md) | Used to move the dense weight cube stored in units of 512 bytes from the L1 Buffer to the L0B Buffer, and simultaneously move the index cube in units of 128 bytes to the built-in dedicated buffer space (for subsequent reading by the MmadWithSparse API). |

### Cube Data Load to L1 Buffer

| API Name | Description |
| --- | --- |
| [DataCopy (GMToL1 Continuous Data Movement)](cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_continuous.md) | This API moves a cube from Global Memory to L1 Buffer (TPosition is A1/B1) continuously, with the format and content preserved during data movement. |
| [DataCopy (GMToL1 High-Dimensional Splitting Data Movement)](cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_highdim_split.md) | This API mainly moves a cube from Global Memory to L1 Buffer (TPosition is A1/B1), with the format and content preserved during data movement. |
| [DataCopy (GMToL1 Inline Conversion - ND2NZ Movement)](cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_ND2NZ.md) | This API mainly moves a cube from Global Memory to L1 Buffer (TPosition is A1/B1), and supports ND-to-NZ format conversion during data movement. |
| [LoadData (GMToL1-2D Cube Movement)](cube_compute_ISASI/cube_compute_load/gmtol1_load2d_instruction_movement.md) | This API moves 2D format data required for ordinary cube computation, moving data from Global Memory to L1 Buffer (TPosition is A1/B1) in units of 512-byte data fractals. |
| [DataCopy (UBToL1 Continuous Data Movement)](cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_continuous.md) | This API moves a cube from Unified Buffer (UB, TPosition is VECIN/VECCALC/VECOUT) to L1 Buffer in a continuous manner, with the format and content preserved during data movement. |
| [DataCopy (UBToL1 High-Dimensional Splitting Data Movement)](cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_highdim_split.md) | This API moves a cube from Unified Buffer (UB, TPosition is VECIN/VECCALC/VECOUT) to L1 Buffer, supporting both non-continuous and continuous movement, with the format and content preserved during data movement. |
| [DataCopyPad (UBToL1 Unaligned Data Movement)](cube_compute_ISASI/cube_compute_load/DataCopyPad_UBToL1.md) | This API provides unaligned data movement from Unified Buffer to L1 Buffer. |
| [DataCopy (UBToL1 Inline Conversion - ND2NZ Movement)](cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_ND2NZ.md) | Supports ND-to-NZ format conversion during data movement. Data is moved from Unified Buffer (UB, TPosition is VECIN/VECCALC/VECOUT) to L1 Buffer, with ND->NZ format conversion completed during the movement process. |

### Auxiliary Configuration API

| API Name | Description |
| --- | --- |
| [Fill](cube_compute_ISASI/cube_load_aux_config/Fill.md) | Initializes the LocalTensor at a specific physical storage location to a specific value. Only LocalTensor initialization on L1 Buffer/L0A Buffer/L0B Buffer is supported. |
| [SetFcube](cube_compute_ISASI/cube_load_aux_config/SetFmatrix.md) | Used to set the attribute description of FeatureMap when calling Load3D. When the template parameter isSetFCube of Load3D is set to false, the attributes of the FeatureMap passed to Load3D will not take effect, and developers need to set them through this API. |
| [SetLoadDataBoundary](cube_compute_ISASI/cube_load_aux_config/SetLoadDataBoundary.md) | Sets the boundary value of the L1 Buffer (TPosition: A1/B1) required by the Load3D API. |
| [SetLoadDataRepeat](cube_compute_ISASI/cube_load_aux_config/SetLoadDataRepeat.md) | Used to set the repeat parameter of the Load3D API. After setting the repeat parameter, data movement for multiple iterations can be completed by calling the Load3D API once. |
| [SetLoadDataPaddingValue](cube_compute_ISASI/cube_load_aux_config/SetLoadDataPaddingValue.md) | Used to set the padding value when calling the Load3D API. When the template parameter isSetPadding of Load3D is set to true, users need to set the padding value through this API. When it is set to false, the padding value set by this API does not take effect. |
| [LoadDataUnzip](cube_compute_ISASI/cube_load_aux_config/LoadDataUnzip.md) | Decompresses data on GM and moves it to A1/B1/B2. LoadUnzipIndex must be executed to load the compressed index table before executing this API. |
| [LoadImageToLocal](cube_compute_ISASI/cube_load_aux_config/LoadImageToLocal.md) | Moves image data from Global Memory to Local Memory. Image preprocessing operations can be completed during the movement process, including image flipping, image resizing (cropping, edge trimming, scaling, stretching), color space conversion, and type conversion. The related parameters of image preprocessing are configured through SetAippFunctions. |
| [LoadUnzipIndex](cube_compute_ISASI/cube_load_aux_config/LoadUnzipIndex.md) | Loads the compressed index table on GM into internal registers. |
| [SetAippFunctions](cube_compute_ISASI/cube_load_aux_config/SetAippFunctions.md) | Sets parameters related to image preprocessing (AIPP, AI Core pre-process). It is used together with the LoadImageToLocal API. After setting, calling the LoadImageToLocal API can complete image preprocessing operations during the movement process, including data fill, channel swap, single-row read, data type conversion, channel fill, and color space conversion. When calling the SetAippFunctions API, the cube of the source image on Global Memory and the image format of the source image need to be passed in. |

### Mmad Computation

| API Name | Description |
| --- | --- |
| [Mmad](cube_compute_ISASI/mmad_compute/Mmad.md) | The Mmad API is the core cube multiply-accumulate computation API of Ascend C for Ascend AI processors. It is designed for high-performance operator development and encapsulates the cube multiply-accumulate computation capability of Ascend NPU hardware. It is widely used in the development of neural network layers (such as fully connected layers and convolution layers) and numerical computation operators. |
| [MmadMx](cube_compute_ISASI/mmad_compute/MmadMx.md) | MmadMx (full name: Microscaling Mmad) is cube multiplication with quantization coefficients, that is, both the left cube and the right cube have corresponding quantization coefficient matrices, namely the left quantization coefficient cube scaleA and the right quantization coefficient cube scaleB. In the MmadMx scenario, the left quantization coefficient cube is multiplied by the left cube, the right quantization coefficient cube is multiplied by the right cube, and cube multiplication is performed on the results of the two products. |
| [MmadBitMode](cube_compute_ISASI/mmad_compute/MmadBitMode.md) | MmadBitMode optimizes the construction of the MmadParams structure. This API is applicable to scenarios where the scalar pipeline becomes a performance optimization bottleneck, and it supports the basic Mmad/MmadMx computation functions. The difference between this API and the Mmad/MmadMx APIs lies in the way parameters are passed: this API passes the union structure MmadBitModeParams. |
| [MmadWithSparse](cube_compute_ISASI/mmad_compute/MmadWithSparse.md) | The MmadWithSparse API is responsible for completing special sparse cube multiply-accumulate operations. A sparse cube is a special type of cube that contains a relatively large number of zero elements. 4:2 structured sparsity requires that in a group of 4 consecutive weights or activation values (usually a row or a column in a tensor), at most 2 values are non-zero, and the remaining 2 are forced to be zero. |

### Mmad Register Configuration

| API Name | Description |
| --- | --- |
| [SetHF32Mode](cube_compute_ISASI/mmad_compute_aux_config/SetHF32Mode.md) | Used to set whether HF32 mode is enabled for Mmad computation. When this mode is enabled, FP32 data in the L0A Buffer/L0B Buffer is rounded to HF32 before participating in Mmad computation. |
| [SetHF32TransMode](cube_compute_ISASI/mmad_compute_aux_config/SetHF32TransMode.md) | Sets the specific rounding method for HF32 mode. SetHF32Mode must be used first to enable the HF32 rounding mode. |
| [SetMMColumnMajor/SetMMRowMajor](cube_compute_ISASI/mmad_compute_aux_config/SetMMColumnMajor-SetMMRowMajor.md) | Controls which direction of M/N is prioritized by Mmad/MmadWithSparse. |

### Cube Computation Storage

| API Name | Description |
| --- | --- |
| [DataCopy (L0C to GM Data Movement)](cube_compute_ISASI/cube_compute_store/DataCopy_L0CToGM.md) | The result of cube computation is stored in the L0C Buffer. The DataCopy API is used to move the result to Global Memory (GM), and supports operations such as inline format conversion during the movement process. |
| [Fixpipe (L0C to GM Data Movement)](cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md) | The result of cube computation is stored in the L0C Buffer. The Fixpipe API is used to move the result to Global Memory (GM), and supports operations such as inline format conversion during the movement process. |
| [DataCopy (L0C to L1 Data Movement)](cube_compute_ISASI/cube_compute_store/DataCopy_L0CToL1.md) | The result of cube computation is stored in the L0C Buffer. The DataCopy API is used to move the result to the L1 Buffer, and supports operations such as inline format conversion during the movement process. |
| [Fixpipe (L0C to L1 Data Movement)](cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToL1.md) | The result of cube computation is stored in the L0C Buffer. The Fixpipe API is used to move the result to the L1 Buffer, and supports operations such as inline format conversion during the movement process. |
| [Fixpipe (L0C Buffer to UB Data Movement)](cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToUB.md) | The result of cube computation is stored in the L0C Buffer. The Fixpipe API is used to move the result to the Unified Buffer (UB), and supports operations such as inline format conversion during the movement process. |

### L1 to GM Data Movement

| API Name | Description |
| --- | --- |
| [DataCopy (L1ToGM Continuous Data Movement)](cube_compute_ISASI/cube_compute_store/DataCopy_L1ToGM_continuous.md) | This API moves a cube from the L1 Buffer to Global Memory continuously, with the format and content preserved during data movement. |
| [DataCopy (L1ToGM High-Dimensional Splitting Data Movement)](cube_compute_ISASI/cube_compute_store/DataCopy_L1ToGM_highdim_split.md) | This API mainly moves a cube from the L1 Buffer to Global Memory, with the format and content preserved during data movement. |

### Store Register Configuration Description

| API Name | Description |
| --- | --- |
| [SetFixPipeConfig](cube_compute_ISASI/cube_store_aux_config/SetFixPipeConfig.md) | During inline quantization in the DataCopy data movement process (L0C->GM, L0C->L1), call this API to set the quantization parameters. |
| [SetFixpipePreQuantFlag](cube_compute_ISASI/cube_store_aux_config/SetFixpipePreQuantFlag.md) | During inline quantization in the DataCopy data movement process (L0C->GM, L0C->L1), call this API to set the scalar quantization parameters. |
| [SetFixpipeNz2ndFlag](cube_compute_ISASI/cube_store_aux_config/SetFixpipeNz2ndFlag.md) | During inline format conversion (from NZ format to ND format) in the DataCopy data movement process (L0C->GM), call this API to set the related configuration for format conversion. |
| [SetFixPipeClipRelu](cube_compute_ISASI/cube_store_aux_config/SetFixPipeClipRelu.md) | After inline quantization in the DataCopy data movement process (L0C->GM), call this API to set the maximum value for the ClipReLU operation. |
| [SetFixPipeAddr](cube_compute_ISASI/cube_store_aux_config/SetFixPipeAddr.md) | After inline quantization in the DataCopy data movement process (L0C->GM), call this API to set the address of the LocalTensor for the Elementwise operation. |

## Cube Computation (TensorAPI, experimental feature)

### Cube Data Load to L1

| API Name | Description |
| --- | --- |
| [Copy (Data Movement from Global Memory to L1 Buffer)](cube_compute_TensorAPI/cube_compute_load/Copy_GMToL1.md) | The Tensor API uniformly executes data movement across different paths through the `Copy` API. This API is used to move data from Global Memory to the L1 Buffer. The `Copy` API selects the specific movement implementation based on the storage location, data type, and layout of the source and destination tensors. The number of blocks to move, movement length, source/destination strides, and format conversion information are derived from the Tensor Layout, so users do not need to pass additional movement parameters in the `Copy` call. |

### Cube Data Load to L0

| API Name | Description |
| --- | --- |
| [Copy (L1 Buffer to L0A Buffer Data Movement)](cube_compute_TensorAPI/cube_compute_load/Copy_L1ToL0A.md) | The Tensor API uniformly performs data movement across different paths through the `Copy` API. This API is used to move the left cube data in the L1 Buffer to the L0A Buffer. |
| [Copy (L1 Buffer to L0B Buffer Data Movement)](cube_compute_TensorAPI/cube_compute_load/Copy_L1ToL0B.md) | The Tensor API uniformly performs data movement across different paths through the `Copy` API. This API is used to move the right cube data in the L1 Buffer to the L0B Buffer. |
| [Copy (L1 Buffer to L0ScaleA Buffer Data Movement)](cube_compute_TensorAPI/cube_compute_load/Copy_L1ToL0ScaleA.md) | The Tensor API uniformly performs data movement across different paths through the `Copy` API. This API is used to move the left cube scale data in the L1 Buffer to the L0ScaleA Buffer. The start address of the left cube scale data on the L0ScaleA Buffer is derived from 1/16 of the start address of the left cube on the L0A Buffer. |
| [Copy (L1 Buffer to L0ScaleB Buffer Data Movement)](cube_compute_TensorAPI/cube_compute_load/Copy_L1ToL0ScaleB.md) | The Tensor API uniformly performs data movement across different paths through the `Copy` API. This API is used to move the right cube scale data in the L1 Buffer to the L0ScaleB Buffer. The start address of the right cube scale data on the L0ScaleB Buffer is derived from 1/16 of the start address of the right cube on the L0B Buffer. |
| [Copy (L1 Buffer to BiasTable Buffer Data Movement)](cube_compute_TensorAPI/cube_compute_load/Copy_L1ToBiasTable.md) | The Tensor API uniformly performs data movement across different paths through the `Copy` API. This API is used to move the bias data in the L1 Buffer to the BiasTable Buffer, serving as the bias input in Mmad computation. |
| [Copy (L1 Buffer to Fixpipe Buffer Data Movement)](cube_compute_TensorAPI/cube_compute_load/Copy_L1ToFixpipe.md) | The Tensor API uniformly performs data movement across different paths through the `Copy` API. This API is used to move the quantization data in the L1 Buffer to the Fixpipe Buffer. The quantization data can be used for inline quantization when L0C output is written to GM/UB. |

### Mmad Computation

| API Name | Description |
| --- | --- |
| [Mmad](cube_compute_TensorAPI/mmad_compute/Mmad.md) | The `Mmad` API performs cube multiply-accumulate on the left cube A in the L0A Buffer and the right cube B in the L0B Buffer, and writes the result to the result cube C in the L0C Buffer. The default mode is ordinary cube computation. |

### Cube Computation Storage

| API Name | Description |
| --- | --- |
| [Copy (L0C Buffer to Global Memory Data Movement)](cube_compute_TensorAPI/cube_compute_store/Copy_L0CToGM.md) | The Tensor API uniformly performs data movement across different paths through the `Copy` API. This API is used to move the cube computation results in the L0C Buffer to Global Memory. The data in the L0C Buffer is usually the output of `Mmad`, with the data format `NZ`. When moving to Global Memory, the API automatically selects the inline format conversion from `NZ` to `ND`, `NZ` to `DN`, or `NZ` to `NZ` based on the destination tensor layout. |
| [Copy (L0C Buffer to Unified Buffer Data Movement)](cube_compute_TensorAPI/cube_compute_store/Copy_L0CToUB.md) | The Tensor API uniformly performs data movement across different paths through the `Copy` API. This API is used to move the cube computation results in the L0C Buffer to the Unified Buffer. The data in the L0C Buffer is usually the output of `Mmad`, with the data format `NZ`. When moving to the Unified Buffer, the API automatically selects the inline format conversion from `NZ` to `ND`, `NZ` to `DN`, or `NZ` to `NZ` based on the destination tensor format. |

## Memory Vector Computation

### Data Movement
| API Name | Description |
| --- | --- |
| [DataCopy (GM and UB - Continuous Data Movement)](memory_vector_compute/data_move/DataCopy_GMAndUB_continuous.md) | Supports continuous data movement between Global Memory and Unified Buffer, with the original format and content of the data preserved during transfer. |
| [DataCopy (GM and UB - High-Dimensional Split Data Movement)](memory_vector_compute/data_move/DataCopy_GMAndUB_highdim_split.md) | Supports high-dimensional split data movement between Global Memory and Unified Buffer, with the original format and content of the data preserved during transfer. |
| [DataCopy (GM and UB - Sliced Data Movement)](memory_vector_compute/data_move/DataCopy_GMAndUB_slice.md) | This is a software simulation API designed for ease of use. It supports sliced data movement by extracting a subset of a multi-dimensional Tensor for transfer. |
| [DataCopy (GM -> UB - Inline ND2NZ Conversion Movement)](memory_vector_compute/data_move/DataCopy_GMToUB_ND2NZ.md) | This is a software simulation API designed for ease of use. It supports ND-to-NZ format conversion during data movement from Global Memory to Unified Buffer. |
| [DataCopy (UB -> GM - Inline NZ2ND Conversion Movement)](memory_vector_compute/data_move/DataCopy_UBToGM_NZ2ND.md) | This is a software simulation API designed for ease of use. It supports NZ-to-ND format conversion during data movement from Unified Buffer to Global Memory. |
| [DataCopy (GM -> UB - Multi-Dimensional Data Movement NDDMA)](memory_vector_compute/data_move/DataCopy_GMToUB_NDDMA.md) | A multi-dimensional data movement API. Compared with the basic data movement API, it allows more flexible configuration of the dimension information to be loaded and the corresponding Stride. |
| [DataCopyPad (GM -> UB - Unaligned Data Movement)](memory_vector_compute/data_move/DataCopyPad_GMToUB.md) | This API provides the capability to move data from Global Memory to Unified Buffer in an unaligned manner, allowing developers to fill data as needed. |
| [DataCopyPad (UB -> GM - Unaligned Data Movement)](memory_vector_compute/data_move/DataCopyPad_UBToGM.md) | This API provides the capability to move data from Unified Buffer to Global Memory in an unaligned manner. |
| [SetPadValue(ISASI)](memory_vector_compute/data_move_aux_config/SetPadValue_ISASI.md) | When moving data from Global Memory to Unified Buffer in an unaligned manner, data can be filled as needed by the developer. SetPadValue is used to set the value to be filled by DataCopyPad. |
| [SetLoopModePara](memory_vector_compute/data_move_aux_config/SetLoopModePara.md) | During DataCopy and DataCopyPad, this API enables loop mode and sets the loop mode parameters. After data movement is complete, ResetLoopModePara is used to reset the loop mode parameters. |
| [ResetLoopModePara](memory_vector_compute/data_move_aux_config/ResetLoopModePara.md) | Resets the loop mode parameters. Used together with SetLoopModePara. In data movement scenarios where loop mode is enabled and its parameters are set, this function must be called to reset the loop mode parameters after data movement is complete. |
| [DataCopy (UB -> UB - Continuous Data Movement)](memory_vector_compute/data_move/DataCopy_UBToUB_continuous.md) | Supports continuous data movement between Unified Buffer and Unified Buffer, with the original format and content of the data preserved during transfer. |
| [DataCopy (UB -> UB - High-Dimensional Split Data Movement)](memory_vector_compute/data_move/DataCopy_UBToUB_highdim_split.md) | Supports high-dimensional split data movement between Unified Buffer and Unified Buffer, with the original format and content of the data preserved during transfer. |
| [Copy (UB -> UB - Continuous Data Movement)](memory_vector_compute/data_move/Copy_UBToUB_continuous.md) | Supports continuous data movement between Unified Buffer and Unified Buffer, with the format and content of the data preserved during transfer. |
| [Copy (UB -> UB - Masked High-Dimensional Data Movement)](memory_vector_compute/data_move/Copy_UBToUB_mask_highdim_split.md) | Supports data movement between Unified Buffer and Unified Buffer, with the format and content of the data preserved during transfer. Supports mask operations and DataBlock interval operations. |

### Basic Arithmetic
| API Name | Description |
| --- | --- |
| [Exp](memory_vector_compute/basic_arithmetic/Exp.md) | Exp is a unary vector computation API that computes the natural exponential of each element in the input tensor. |
| [Ln](memory_vector_compute/basic_arithmetic/Ln.md) | Ln is a unary vector computation API that computes the natural logarithm of each element in the input tensor. |
| [Abs](memory_vector_compute/basic_arithmetic/Abs.md) | Abs is a unary vector computation API that computes the absolute value of each element in the input tensor. |
| [Reciprocal](memory_vector_compute/basic_arithmetic/Reciprocal.md) | Reciprocal is a unary vector computation API that computes the reciprocal of each element in the input tensor. |
| [Sqrt](memory_vector_compute/basic_arithmetic/Sqrt.md) | Sqrt is a unary vector computation API that computes the square root of each element in the input tensor. |
| [Rsqrt](memory_vector_compute/basic_arithmetic/Rsqrt.md) | Rsqrt is a unary vector computation API that computes the square root of each element in the input tensor and then takes the reciprocal. |
| [Relu](memory_vector_compute/basic_arithmetic/Relu.md) | Relu is a unary vector computation API that performs the ReLU (Rectified Linear Unit) computation on each element in the input tensor. |
| [Neg(ISASI)](memory_vector_compute/basic_arithmetic/Neg_ISASI.md) | Performs the negation operation on each element. |
| [Add](memory_vector_compute/basic_arithmetic/Add.md) | Add is a binary vector computation API that computes the element-wise sum of two input tensors. |
| [Sub](memory_vector_compute/basic_arithmetic/Sub.md) | Sub is a binary vector computation API that computes the element-wise difference of two input tensors. |
| [Mul](memory_vector_compute/basic_arithmetic/Mul.md) | Mul is a binary vector computation API that computes the element-wise product of two input tensors. |
| [Div](memory_vector_compute/basic_arithmetic/Div.md) | Div is a binary vector computation API that computes the element-wise quotient of two input tensors. |
| [Max](memory_vector_compute/basic_arithmetic/Max.md) | Max is a binary vector computation API that computes the element-wise maximum of two input tensors. |
| [Min](memory_vector_compute/basic_arithmetic/Min.md) | Min is a binary vector computation API that computes the element-wise minimum of two input tensors. |
| [BilinearInterpolation(ISASI)](memory_vector_compute/basic_arithmetic/BilinearInterpolation_ISASI.md) | BilinearInterpolation is a binary vector computation API whose core performs multiply-accumulate operations through a double-layer loop of horizontal iteration and vertical iteration. It is suitable for vector operation scenarios involving irregular data access and weighted multiply-add (data Gather + Multiply + Reduce). |
| [Prelu(ISASI)](memory_vector_compute/basic_arithmetic/Prelu_ISASI.md) | When the source operand src0 is greater than 0, src0 is directly written to the destination operand dst; otherwise, the result of src0 * src1 is written to dst. |
| [Mull(ISASI)](memory_vector_compute/basic_arithmetic/Mull_ISASI.md) | Performs element-wise multiplication on the first count input data src0 and src1, writes the result to dst0Local, and writes the overflow part to dst1Local. |
| [Adds](memory_vector_compute/basic_arithmetic/Adds.md) | Adds is a binary scalar computation API that takes a vector tensor and a scalar as inputs. Adds computes the sum of each element in the vector tensor and the scalar. |
| [Adds (Flexible Scalar Position)](memory_vector_compute/basic_arithmetic/Adds_flexible_scalar.md) | Provides an API with flexible scalar position, supporting both scalar-first and scalar-last scenarios. The scalar input supports configuring a single-point element of LocalTensor. |
| [Muls](memory_vector_compute/basic_arithmetic/Muls.md) | Muls is a binary scalar computation API that takes a vector tensor and a scalar as inputs. Muls computes the product of each element in the vector tensor and the scalar. |
| [Muls (Flexible Scalar Position)](memory_vector_compute/basic_arithmetic/Muls_flexible_scalar.md) | Provides an API with flexible scalar position, supporting both scalar-first and scalar-last scenarios. The scalar input supports configuring a single-point element of LocalTensor. |
| [Maxs](memory_vector_compute/basic_arithmetic/Maxs.md) | Maxs is a binary scalar computation API that takes a vector tensor and a scalar as inputs. Maxs computes the maximum of each element in the vector tensor and the scalar. |
| [Maxs (Flexible Scalar Position)](memory_vector_compute/basic_arithmetic/Maxs_flexible_scalar.md) | Provides an API with flexible scalar position, supporting both scalar-first and scalar-last scenarios. The scalar input supports configuring a single-point element of LocalTensor. |
| [Mins](memory_vector_compute/basic_arithmetic/Mins.md) | Mins is a binary scalar computation API that takes a vector tensor and a scalar as inputs. Mins computes the minimum of each element in the vector tensor and the scalar. |
| [Mins (Flexible Scalar Position)](memory_vector_compute/basic_arithmetic/Mins_flexible_scalar.md) | Provides an API with flexible scalar position, supporting both scalar-first and scalar-last scenarios. The scalar input supports configuring a single-point element of LocalTensor. |
| [Subs](memory_vector_compute/basic_arithmetic/Subs_flexible_scalar.md) | Performs subtraction between each element in the vector and the scalar, supporting both scalar-first and scalar-last scenarios. The scalar input supports configuring a single-point element of LocalTensor. |
| [Divs](memory_vector_compute/basic_arithmetic/Divs_flexible_scalar.md) | Performs division between each element in the vector and the scalar, supporting both scalar-first and scalar-last scenarios. The scalar input supports configuring a single-point element of LocalTensor. |
| [LeakyRelu](memory_vector_compute/basic_arithmetic/LeakyRelu.md) | LeakyRelu is a binary scalar computation API that takes a vector tensor and a scalar as inputs. LeakyRelu performs the Leaky ReLU (Leaky Rectified Linear Unit) operation on each element in the vector tensor. |

### Logical Computation
| API Name | Description |
| --- | --- |
| [Not](memory_vector_compute/logical_compute/Not.md) | Performs bitwise NOT on elements. |
| [And](memory_vector_compute/logical_compute/And.md) | Performs bitwise AND on elements. |
| [Or](memory_vector_compute/logical_compute/Or.md) | Performs bitwise OR on each pair of elements. |
| [Ands](memory_vector_compute/logical_compute/Ands.md) | Performs an AND operation between each element in the vector and a scalar, supporting both scalar-first and scalar-last scenarios, where the scalar input supports configuring a single-point LocalTensor element. |
| [Ors](memory_vector_compute/logical_compute/Ors.md) | Performs an OR operation between each element in the vector and a scalar, supporting both scalar-first and scalar-last scenarios, where the scalar input supports configuring a single-point LocalTensor element. |
| [ShiftLeft](memory_vector_compute/logical_compute/ShiftLeft.md) | Performs a left shift operation on each element in the source operand, where the number of bits to shift is determined by the scalar scalarValue. |
| [ShiftLeft (shift amount as Tensor)](memory_vector_compute/logical_compute/ShiftLeft_shift_amount_tensor.md) | Performs a left shift operation on each element in the source operand. |
| [ShiftRight](memory_vector_compute/logical_compute/ShiftRight.md) | Performs a right shift operation on each element in the source operand, where the number of bits to shift is determined by the scalar scalarValue. |
| [ShiftRight (shift amount as Tensor)](memory_vector_compute/logical_compute/ShiftRight_shift_amount_tensor.md) | Performs a right shift on each element in the source operand. |

### Compound Computation
| API Name | Description |
| --- | --- |
| [Axpy](memory_vector_compute/composite_compute/Axpy.md) | Computes the product of a vector and a scalar, and adds the product result element-wise to the output vector. |
| [CastDequant](memory_vector_compute/composite_compute/CastDequant.md) | Performs quantization and precision conversion on the input, and applies saturation processing to the overflow of the output result. This API must be used together with the SetDeqScale API. |
| [AddRelu](memory_vector_compute/composite_compute/AddRelu.md) | Computes the element-wise sum, and then performs the Relu computation (compares the result with 0 and takes the larger value). |
| [AddReluCast](memory_vector_compute/composite_compute/AddReluCast.md) | Computes the element-wise sum, compares the result with 0 and takes the larger value, and performs precision conversion according to the data types of the source operand and destination operand Tensors. |
| [AddDeqRelu](memory_vector_compute/composite_compute/AddDeqRelu.md) | Sequentially computes the element-wise sum, performs deq quantization on the result, and then performs the relu computation (compares the result with 0 and takes the larger value). |
| [SubRelu](memory_vector_compute/composite_compute/SubRelu.md) | Computes the element-wise difference, and then performs the Relu computation (compares the result with 0 and takes the larger value). |
| [SubReluCast](memory_vector_compute/composite_compute/SubReluCast.md) | Computes the element-wise difference, then performs the Relu computation (compares the result with 0 and takes the larger value), and performs precision conversion according to the data types of the source operand and destination operand Tensors. |
| [MulAddDst](memory_vector_compute/composite_compute/MulAddDst.md) | Multiplies src0 and src1 element-wise and adds the result to dst, storing the final result into dst. |
| [MulCast](memory_vector_compute/composite_compute/MulCast.md) | Computes the element-wise product, and performs precision conversion according to the data types of the source operand and destination operand Tensors. |
| [FusedMulAdd](memory_vector_compute/composite_compute/FusedMulAdd.md) | Multiplies src0 and dst element-wise and adds src1, storing the final result into dst. |
| [MulAddRelu](memory_vector_compute/composite_compute/MulAddRelu.md) | Multiplies src0 and dst element-wise and adds src1, then performs the Relu computation (compares the result with 0 and takes the larger value), storing the final result into dst. |
| [AbsSub(ISASI)](memory_vector_compute/composite_compute/AbsSub_ISASI.md) | Subtracts src1 from src0Local and computes the absolute value, writing the computation result into dst. |
| [ExpSub(ISASI)](memory_vector_compute/composite_compute/ExpSub_ISASI.md) | Subtracts src1 from src0, uses the difference as the exponent to compute the power of the natural constant e, and writes the computation result into dst. |
| [MulsCast(ISASI)](memory_vector_compute/composite_compute/MulsCast_ISASI.md) | Multiplies the first count data of the vector source operand by a scalar, converts the result to the half type according to the CAST_ROUND mode, and writes the computation result into dst. This API supports both scenarios where the scalar is placed before and after. |

### Comparison and Selection
| API Name | Description |
| --- | --- |
| [Compare](memory_vector_compute/compare_and_select/Compare.md) | Compares two tensors element by element. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0. |
| [Compare (result stored in register)](memory_vector_compute/compare_and_select/Compare_store_to_register.md) | Compares two tensors element by element. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0. The computation result is stored in the CmpMask 128-bit register, which can be used for subsequent Select computation. The data stored in the register can also be obtained using the GetCmpMask API. |
| [Compares](memory_vector_compute/compare_and_select/Compares.md) | Compares the elements of one tensor with another Scalar element by element. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0. |
| [Compares (flexible scalar position)](memory_vector_compute/compare_and_select/Compares_flexible_scalar.md) | Provides an API with flexible scalar positions, supporting both scenarios where the scalar is placed before or after. The scalar input supports configuring a single-point element of a LocalTensor. |
| [GetCmpMask(ISASI)](memory_vector_compute/compare_and_select/GetCmpMask_ISASI.md) | This API is used to obtain the comparison result of the Compare (result stored in register) instruction. |
| [SetCmpMask(ISASI)](memory_vector_compute/compare_and_select/SetCmpMask_ISASI.md) | Sets the value of the comparison register. It is used together with the Select API that does not pass the mask parameter, and different data is passed according to different selMode values. |
| [Select](memory_vector_compute/compare_and_select/Select.md) | Given two source operands src0 and src1, elements are selected according to the bit values of selMask (the mask used for selection) to obtain the destination operand dst. The selection rule is: when the bit of selMask is 1, the element is selected from src0; when the bit is 0, it is selected from src1. |
| [Select (flexible scalar position)](memory_vector_compute/compare_and_select/Select_flexible_scalar.md) | Given two source operands src0 and src1, elements are selected according to the bit values of selMask (the mask used for selection) to obtain the destination operand dst. The selection rule is: when the bit of selMask is 1, the element is selected from src0; when the bit is 0, it is selected from src1. |
| [GatherMask](memory_vector_compute/compare_and_select/GatherMask.md) | Uses the binary representation of the value corresponding to a **built-in fixed pattern** or a **user-defined input Tensor** as the gather mask (the mask for data collection) to select elements from the source operand and write them into the destination operand. 1 indicates selection, and 0 indicates no selection. |

### Type Conversion
| API Name | Description |
| --- | --- |
| [SetDeqScale](memory_vector_compute/type_conversion_aux_config/SetDeqScale.md) | This API is used to set the value of the DEQSCALE register. The DEQSCALE register is 64 bits wide and is used for quantization computation on the Vector computation unit. The parameters stored in the register have different meanings in different scenarios. |
| [Cast](memory_vector_compute/type_conversion/Cast.md) | Performs precision conversion based on the data types of the source operand and destination operand tensors. |
| [Truncate(ISASI)](memory_vector_compute/type_conversion/Truncate_ISASI.md) | Truncates the floating-point elements of the source operand to integer bits while keeping the data type of the source operand unchanged. |

### Reduction Computation
| API Name | Description |
| --- | --- |
| [ReduceDataBlock](memory_vector_compute/reduction_compute/ReduceDataBlock.md) | The `ReduceDataBlock` API performs reduction on the input data in units of DataBlock. Based on the template parameter `reduceType`, it computes the sum/maximum/minimum of the data within each DataBlock. |
| [ReduceRepeat](memory_vector_compute/reduction_compute/ReduceRepeat.md) | The `ReduceRepeat` API performs reduction on all data within each repeat. Based on the template parameter reduceType, it computes the sum/maximum/minimum, and writes the results to the destination address in order. |
| [ReducePairElem](memory_vector_compute/reduction_compute/ReducePairElem.md) | The `ReducePairElem` API performs reduction on adjacent (odd-even) element pairs based on the template parameter `reduceType`, and writes the results to the destination address in order. **Currently only the sum operation is supported.** |
| [ReduceMax](memory_vector_compute/reduction_compute/ReduceMax.md) | The `ReduceMax` API is used to find the maximum value and its index from all input data. |
| [ReduceMin](memory_vector_compute/reduction_compute/ReduceMin.md) | The `ReduceMin` API is used to find the minimum value and its index from all input data. |
| [ReduceSum](memory_vector_compute/reduction_compute/ReduceSum.md) | The `ReduceSum` API computes the sum of all input data. |
| [GetReduceRepeatSumSpr(ISASI)](memory_vector_compute/reduction_compute_aux_config/GetReduceRepeatSumSpr_ISASI.md) | Obtains the computation result of the ReduceSum API. It is supported only by the API that performs continuous computation on the first n data of a tensor. The computation result is stored as a global variable and can be retrieved at any time. |
| [GetReduceRepeatMaxMinSpr(ISASI)](memory_vector_compute/reduction_compute_aux_config/GetReduceRepeatMaxMinSpr_ISASI.md) | This API is used to obtain the extreme values and their indices across all repeats when ReduceRepeat is called, or to obtain the extreme values returned by ReduceMax/ReduceMin. The computation result is stored as a global variable and can be retrieved at any time. |

### Data Layout Conversion
| API Name | Description |
| --- | --- |
| [Transpose](memory_vector_compute/data_layout_conversion/Transpose.md) | The Transpose API is used to transpose a 16*16 two-dimensional cube data block or to convert between the [N,C,H,W] and [N,H,W,C] data formats. |
| [TransDataTo5HD](memory_vector_compute/data_layout_conversion/TransDataTo5HD.md) | The TransDataTo5HD API performs data format conversion, generally used to convert the NCHW format to the NC1HWC0 format. In particular, it can also be used to transpose two-dimensional cube data blocks. When performing transposition, compared with the Transpose API, which supports only 16\*16 cube transposition, this API can process 512 bytes of data (16 DataBlocks) within a single repeat. Depending on the data type, it supports cube transposition of different shapes, and it also supports multiple repeat operations. |

### Data Fill
| API Name | Description |
| --- | --- |
| [Duplicate](memory_vector_compute/data_padding/Duplicate.md) | The Duplicate API copies a variable or an immediate value multiple times and fills it into a vector. |
| [Brcb](memory_vector_compute/data_padding/Brcb.md) | The Brcb API performs a fill operation on the input data by taking 8 numbers from the input tensor each time and filling each number into one DataBlock (32 bytes) of the result tensor. |
| [CreateVecIndex](memory_vector_compute/data_padding/CreateVecIndex.md) | The CreateVecIndex API creates a vector index with a specified starting value. |
| [VectorPadding(ISASI)](memory_vector_compute/data_padding/VectorPadding_ISASI.md) | Performs a fill operation on the source operand by DataBlock according to padMode (pad mode) and padSide (pad direction). |

### Sorting and Merging (ISASI)
| API Name | Description |
| --- | --- |
| [ProposalConcat](memory_vector_compute/sort_and_merge_ISASI/ProposalConcat.md) | Merges continuous elements into the corresponding positions within Region Proposals. Each iteration merges 16 continuous elements into the corresponding positions of 16 Region Proposals. |
| [ProposalExtract](memory_vector_compute/sort_and_merge_ISASI/ProposalExtract.md) | Performs the opposite function of ProposalConcat. It extracts a single element from the corresponding position within each Region Proposal and rearranges them. Each iteration processes 16 Region Proposals, extracting 16 elements and arranging them continuously. |
| [RpSort16](memory_vector_compute/sort_and_merge_ISASI/RpSort16.md) | Sorts Region Proposals based on their score field (larger scores come first), processing 16 Region Proposals at a time. |
| [MrgSort4](memory_vector_compute/sort_and_merge_ISASI/MrgSort4.md) | Arranges and merges up to 4 already-sorted Region Proposal queues into a single queue, with the result sorted by the score field in descending order. |
| [Sort32](memory_vector_compute/sort_and_merge_ISASI/Sort32.md) | The Sort32 API performs descending-order sorting of 32 numbers within a single iteration. |
| [MrgSort](memory_vector_compute/sort_and_merge_ISASI/MrgSort.md) | Merges and arranges up to 4 already-sorted queues into a single queue, with the result sorted by the score field in descending order. |
| [GetMrgSortResult](memory_vector_compute/sort_and_merge_ISASI/GetMrgSortResult.md) | The GetMrgSortResult API must be used together with the MrgSort API. When the ifExhaustedSuspension parameter in the MrgSort API is set to true, MrgSort stops executing if the data in an input queue is exhausted. In this case, the GetMrgSortResult API can be called to obtain the number of data items already processed in the 4 queues. |

### Scatter and Gather
| API Name | Description |
| --- | --- |
| [Gather](memory_vector_compute/scatter_gather/Gather.md) | The Gather API accepts an input tensor (src), an address offset tensor (srcOffset), and a base address (srcBaseAddr). It determines the indices of the input tensor based on the base address and address offsets, and gathers the corresponding elements from the input tensor into the result tensor (dst). |
| [Gatherb(ISASI)](memory_vector_compute/scatter_gather/Gatherb_ISASI.md) | The Gatherb API accepts an input tensor (src) and a DataBlock offset tensor (offset). Based on the index positions, it gathers the input tensor into the result tensor (dst) by DataBlock (32 bytes). |
| [Scatter(ISASI)](memory_vector_compute/scatter_gather/Scatter_ISASI.md) | Given a continuous input tensor and a destination address offset tensor, the Scatter instruction generates a new result tensor based on the offset addresses and then scatters the input tensor into the result tensor. |

### Mask Operation
| API Name | Description |
| --- | --- |
| [SetMaskCount](memory_vector_compute/mask_operations/SetMaskCount.md) | Sets the Mask mode to Counter mode. In this mode, developers do not need to perceive the number of iterations or handle unaligned tail blocks. They can directly pass the amount of data to be computed, and the actual number of iterations is automatically inferred by the Vector computation unit. This API is recommended to be used together with the isSetMask template parameter in APIs. When isSetMask is false, users can call this API to manually manage the Counter mode and use SetVectorMask to set the number of elements participating in computation in Counter mode. |
| [SetMaskNorm](memory_vector_compute/mask_operations/SetMaskNorm.md) | Sets the Mask mode to Normal mode. This mode is the system default mode and supports developers in configuring the number of iterations. This API is recommended to be used together with the isSetMask template parameter in APIs. When isSetMask is false, users can call this API to manually manage the Normal mode and use SetVectorMask to set the mask in Normal mode. |
| [SetVectorMask](memory_vector_compute/mask_operations/SetVectorMask.md) | This API is used to set the mask during vector computation. |
| [ResetMask](memory_vector_compute/mask_operations/ResetMask.md) | Restores the mask value to the default value (all 1s), indicating that all elements in each iteration of vector computation will participate in the operation. |

### Data Rearrangement (ISASI)
| API Name | Description |
| --- | --- |
| [Interleave](memory_vector_compute/data_rearrange_ISASI/Interleave.md) | Given source operands src0 and src1, interleaves the elements in src0 and src1 into destination operands dst0 and dst1. |
| [DeInterleave](memory_vector_compute/data_rearrange_ISASI/DeInterleave.md) | Given source operands src0 and src1, de-interleaves the elements in src0 and src1 into destination operands dst0 and dst1. |

## Reg Vector Computation

### Register Data Type
| API Name | Description |
| --- | --- |
| [RegTensor](reg_vector_compute/register_data_types/RegTensor.md) | The basic unit of Reg vector computation. The bit width of RegTensor is VL (Vector Length), and the specific value may vary depending on the AI processor model. |
| [MaskReg](reg_vector_compute/register_data_types/MaskReg.md) | MaskReg is used to indicate which elements participate in the computation, with a width of one-eighth of RegTensor (VL/8). |
| [UnalignRegForLoad & UnalignRegForStore](reg_vector_compute/register_data_types/UnalignRegForLoad-UnalignRegForStore.md) | UnalignRegForLoad and UnalignRegForStore are used as buffers to optimize the overhead of continuous unaligned address access between UB and RegTensor. Before reading an unaligned address, UnalignRegForLoad and UnalignRegForStore should be initialized through the LoadUnAlignPre API, and then the LoadUnAlign API is used. When writing an unaligned address, the StoreUnAlign API is used first. |
| [AddrReg](reg_vector_compute/register_data_types/AddrReg.md) | AddrReg is the Address Register, a register used to store address offsets. AddrReg should be initialized through the CreateAddrReg API, and then used in loops to store address offsets. AddrReg increments automatically in each loop level according to the configured stride. |

### Reg Data Load
| API Name | Description |
| --- | --- |
| [Continuous Aligned Load (LoadAlign)](reg_vector_compute/reg_data_load/LoadAlign_continuous.md) | LoadAlign moves data continuously from the Unified Buffer (UB) to RegTensor. It supports both single-load mode and dual-load mode. |
| [Non-continuous Aligned Load (LoadAlign)](reg_vector_compute/reg_data_load/LoadAlign_noncontinuous.md) | LoadAlign moves data non-continuously from the Unified Buffer (UB) to RegTensor, in units of DataBlock. |
| [Continuous Unaligned Load (LoadUnAlign)](reg_vector_compute/reg_data_load/LoadUnAlign_continuous.md) | LoadUnAlign moves data continuously from an unaligned Unified Buffer (UB) to RegTensor, using the unaligned register UnalignRegForLoad as a temporary buffer to hold data crossing alignment boundaries, thereby achieving efficient continuous unaligned data transfer. |
| [MaskReg Load (LoadAlign)](reg_vector_compute/reg_data_load/LoadAlign_MaskReg.md) | LoadAlign moves data from the Unified Buffer (UB) to MaskReg. |
| [MaskReg Load (MaskGenWithRegTensor)](reg_vector_compute/reg_data_load/MaskGenWithRegTensor_MaskReg.md) | MaskGenWithRegTensor moves data from RegTensor to MaskReg. |
| [Discrete Load (Gather)](reg_vector_compute/reg_data_load/Gather_discrete.md) | This instruction gathers the source operands element by element into the destination operand dstReg according to the index value index. |
| [Discrete Load (GatherB)](reg_vector_compute/reg_data_load/GatherB_discrete.md) | This instruction gathers the source operands into the destination operand dstReg in units of DataBlock (32B) according to the index value index. |
| [Load](reg_vector_compute/reg_data_load/Load.md) | Reg vector computation data movement API. It supports moving data from a source address srcAddr that is not 32-byte aligned in UB to RegTensor, with a movement size of VL (256B). This API encapsulates LoadUnAlignPre and LoadUnAlign. |
| [Move](reg_vector_compute/reg_data_load/Move.md) | Copies the valid elements in srcReg one by one to the corresponding positions in dstReg, while the invalid positions retain the original values of dstReg. |

### Reg Data Store
| API Name | Description |
| --- | --- |
| [Continuous Aligned Store (StoreAlign)](reg_vector_compute/reg_data_store/StoreAlign_continuous.md) | StoreAlign moves data continuously from RegTensor to Unified Buffer (UB), supporting both single-store mode and dual-store mode. |
| [Non-continuous Aligned Store (StoreAlign)](reg_vector_compute/reg_data_store/StoreAlign_noncontinuous.md) | StoreAlign moves data non-continuously from RegTensor to Unified Buffer (UB), in units of DataBlock. |
| [Continuous Unaligned Store (StoreUnAlign)](reg_vector_compute/reg_data_store/StoreUnAlign_continuous.md) | StoreUnAlign moves data continuously from RegTensor to an unaligned Unified Buffer (UB), using the unaligned register UnalignRegForStore as a temporary buffer to hold data crossing alignment boundaries, thereby achieving efficient continuous unaligned data transfer. |
| [MaskReg Store (StoreAlign)](reg_vector_compute/reg_data_store/StoreAlign_MaskReg.md) | StoreAlign moves data from MaskReg to Unified Buffer (UB). |
| [MaskReg Unaligned Store (StoreUnAlign)](reg_vector_compute/reg_data_store/StoreUnAlign_MaskReg.md) | StoreUnAlign moves data continuously from MaskReg to an unaligned Unified Buffer (UB), using the unaligned register UnalignRegForStore as a temporary buffer. |
| [Discrete Store (Scatter)](reg_vector_compute/reg_data_store/Scatter_discrete.md) | This instruction scatters the elements in the source operand srcReg into the destination operand UB according to the index value index. |
| [Store](reg_vector_compute/reg_data_store/Store.md) | Reg vector computation data movement API, supporting store from RegTensor to a UB address dstAddr that is not 32-byte aligned. This API encapsulates StoreUnAlign and StoreUnAlignPost. |

### MaskReg Computation
| API Name | Description |
| --- | --- |
| [Move](reg_vector_compute/MaskReg_compute/Move-10.md) | Copies the elements in src to the corresponding positions in dst. If an input mask is provided, only the valid elements selected by the mask are copied, and invalid elements are filled with 0. |
| [Interleave](reg_vector_compute/MaskReg_compute/Interleave-11.md) | Interleaves the elements in source operands src0 and src1 and stores them into destination operands dst0 and dst1. |
| [DeInterleave](reg_vector_compute/MaskReg_compute/DeInterleave-12.md) | De-interleaves the elements in source operands src0 and src1 and stores them into destination operands dst0 and dst1. |
| [Select](reg_vector_compute/MaskReg_compute/Select-13.md) | Given two source operands src0 and src1, selects elements based on the bit values of mask to obtain the destination operand dst. The selection rule is: when the bit of mask is 1, the element at the corresponding position is selected from src0; when the bit is 0, the element at the corresponding position is selected from src1. |
| [Pack](reg_vector_compute/MaskReg_compute/Pack.md) | Based on the selected low-bit mode or high-bit mode, extracts the even-position bits of the source operand src into the lower half or upper half of the destination operand dst. |
| [UnPack](reg_vector_compute/MaskReg_compute/UnPack.md) | Based on the selected low-bit mode or high-bit mode, expands the lower half or upper half of the source operand src into the destination operand dst. The expansion method is: each bit is expanded into 2 bits, with the high bit set to zero. |
| [MoveMask](reg_vector_compute/MaskReg_compute/MoveMask.md) | Reads the Mask value from the mask registers {MASK1, MASK0} set by SetVectorMask, converts it according to the data format corresponding to the template parameter T, and writes it into the return value MaskReg. |

### Basic Arithmetic
| API Name | Description |
| --- | --- |
| [Abs](reg_vector_compute/basic_arithmetic/Abs-15.md) | Computes the absolute value of each valid element in srcReg and writes the result to the corresponding position in dstReg. |
| [Relu](reg_vector_compute/basic_arithmetic/Relu-16.md) | Performs a ReLU (Rectified Linear Unit) operation on the input data srcReg according to mask, and writes the result to dstReg. |
| [Exp](reg_vector_compute/basic_arithmetic/Exp-17.md) | Performs an element-wise exponential operation on the source operand srcReg according to mask, and writes the result to the destination operand dstReg. |
| [Sqrt](reg_vector_compute/basic_arithmetic/Sqrt-18.md) | Performs an element-wise square root operation on the source operand srcReg according to mask, and writes the result to the destination operand dstReg. |
| [Ln](reg_vector_compute/basic_arithmetic/Ln-19.md) | Computes the natural logarithm of the source operand element by element according to mask, and writes the result to the destination operand. |
| [Log](reg_vector_compute/basic_arithmetic/Log.md) | Computes the natural logarithm of the source operand element by element according to mask, and writes the result to the destination operand. |
| [Log2](reg_vector_compute/basic_arithmetic/Log2.md) | Computes the base-2 logarithm of the input data srcReg according to mask, and writes the result to dstReg. |
| [Log10](reg_vector_compute/basic_arithmetic/Log10.md) | Computes the base-10 logarithm of the source operand element by element according to mask, and writes the result to the destination operand. |
| [Neg](reg_vector_compute/basic_arithmetic/Neg.md) | Computes the opposite number of the input data srcReg according to mask, and writes the result to dstReg. |
| [Add](reg_vector_compute/basic_arithmetic/Add-20.md) | Performs an element-wise addition operation on the source operands srcReg0 and srcReg1 according to mask, and writes the result to the destination operand dstReg. |
| [Sub](reg_vector_compute/basic_arithmetic/Sub-21.md) | Performs an element-wise subtraction operation on the source operands srcReg0 and srcReg1 according to mask, and writes the result to the destination operand dstReg. |
| [Mul](reg_vector_compute/basic_arithmetic/Mul-22.md) | Performs an element-wise multiplication operation on the input data srcReg0 and srcReg1 according to mask, and writes the result to dstReg. |
| [Div](reg_vector_compute/basic_arithmetic/Div-23.md) | Performs an element-wise division operation on the source operands srcReg0 and srcReg1 according to mask, and writes the result to the destination operand dstReg. |
| [Max](reg_vector_compute/basic_arithmetic/Max-24.md) | Performs an element-wise maximum operation on the source operands srcReg0 and srcReg1 according to mask, and writes the result to the destination operand dstReg. |
| [Min](reg_vector_compute/basic_arithmetic/Min-25.md) | Performs an element-wise minimum operation on the source operands srcReg0 and srcReg1 according to mask, and writes the result to the destination operand dstReg. |
| [Mull](reg_vector_compute/basic_arithmetic/Mull.md) | Performs an element-wise multiplication operation on the input data srcReg0 and srcReg1 according to mask, writes the low-order part of the multiplication result to dstReg0, and writes the overflow (high-order) part to dstReg1. |
| [AddC](reg_vector_compute/basic_arithmetic/AddC.md) | Performs an element-wise addition operation on the source operands srcReg0, srcReg1, and the input carry carrySrc according to mask, writes the result to the destination operand dstReg, and writes the carry result of each element to carry. |
| [SubC](reg_vector_compute/basic_arithmetic/SubC.md) | Performs an element-wise subtraction operation on the source operands srcReg0, srcReg1, and the input carry carrySrc according to mask, writes the result to the destination operand dstReg, and writes the carry result of each element to carry. |
| [Prelu](reg_vector_compute/basic_arithmetic/Prelu.md) | Implements the PReLU (Parametric ReLU) activation function: when the corresponding element in the source operand srcReg0 is greater than 0, the element is written directly to the destination operand dstReg; otherwise, the product of the corresponding elements in srcReg0 and srcReg1 is written to dstReg. |
| [Adds](reg_vector_compute/basic_arithmetic/Adds-26.md) | Performs an element-wise addition operation between the source operand srcReg and the scalar value scalarValue according to mask, and writes the result to the destination operand dstReg. |
| [Muls](reg_vector_compute/basic_arithmetic/Muls-27.md) | Computes the product of each element in the vector and a scalar. |
| [Maxs](reg_vector_compute/basic_arithmetic/Maxs-28.md) | Compares each element of the source operand srcReg with a scalar according to mask. If the element is greater than the scalar, the source operand value is taken; if it is smaller than the scalar, the scalar value is taken. The result is written to the destination operand dstReg. |
| [Mins](reg_vector_compute/basic_arithmetic/Mins-29.md) | Compares each element of the source operand srcReg with a scalar according to mask. If the element is greater than the scalar, the scalar value is taken; if it is smaller than the scalar, the source operand value is taken. The result is written to the destination operand dstReg. |
| [LeakyRelu](reg_vector_compute/basic_arithmetic/LeakyRelu-30.md) | Performs an element-wise Leaky ReLU (Leaky Rectified Linear Unit) operation: when an element in the source operand is greater than 0, the element is written directly to the destination operand; otherwise, the element is multiplied by the scalar value scalarValue and then written to the destination operand. |

### Logical Computation
| API Name | Description |
| --- | --- |
| [Not](reg_vector_compute/logical_compute/Not-32.md) | This section introduces two APIs, which perform bitwise NOT operation on the valid bits of RegTensor and MaskReg respectively to obtain and save the result. |
| [And](reg_vector_compute/logical_compute/And-33.md) | This section introduces two APIs, which perform logical AND operation on the valid bits of RegTensor and MaskReg respectively to obtain and save the result. |
| [Or](reg_vector_compute/logical_compute/Or-34.md) | This section introduces two APIs, which perform logical OR operation on the valid bits of RegTensor and MaskReg respectively to obtain and save the result. |
| [Xor](reg_vector_compute/logical_compute/Xor.md) | This section introduces two APIs, which perform bitwise XOR operation on the valid bits of RegTensor and MaskReg respectively to obtain and save the result. |
| [ShiftLeft](reg_vector_compute/logical_compute/ShiftLeft-35.md) | Based on mask, performs left shift operation on the input data srcReg0 according to the corresponding elements of srcReg1, and writes the result to dstReg. |
| [ShiftRight](reg_vector_compute/logical_compute/ShiftRight-36.md) | Based on mask, performs right shift operation on the input data srcReg0 according to the corresponding elements of srcReg1, and writes the result to dstReg. |
| [ShiftLefts](reg_vector_compute/logical_compute/ShiftLefts.md) | Performs logical left shift on each element in the source operand, where the number of shift bits is determined by the input parameter scalarValue. |
| [ShiftRights](reg_vector_compute/logical_compute/ShiftRights.md) | Performs right shift on each element in the source operand, where the number of shift bits is determined by the input parameter scalarValue. |

### Compound Computation
| API Name | Description |
| --- | --- |
| [Axpy](reg_vector_compute/composite_compute/Axpy-38.md) | Performs element-wise multiply-add operations on the input data dstReg, srcReg, and scalarValue according to the mask, and writes the results to dstReg. |
| [AbsSub](reg_vector_compute/composite_compute/AbsSub.md) | Subtracts srcReg1 from srcReg0 and computes the absolute value, then writes the results to dstReg according to the mask. |
| [ExpSub](reg_vector_compute/composite_compute/ExpSub.md) | Subtracts srcReg1 from srcReg0 and uses the difference as the exponent of e, then writes the results to dstReg according to the mask. |
| [MulDstAdd](reg_vector_compute/composite_compute/MulDstAdd.md) | Multiplies dstReg by srcReg0 and adds srcReg1, then writes the results to dstReg according to the mask. |
| [MulAddDst](reg_vector_compute/composite_compute/MulAddDst-39.md) | Multiplies srcReg0 by srcReg1 and adds the value of dstReg, then writes the results to dstReg according to the mask. |
| [MulsCast](reg_vector_compute/composite_compute/MulsCast.md) | Multiplies src by scalar and converts the result to the half type according to the CAST_ROUND mode, then writes the results to dst according to the mask. |

### Comparison and Selection
| API Name | Description |
| --- | --- |
| [Compare](reg_vector_compute/compare_and_select/Compare-41.md) | Compares two RegTensors element by element. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0. |
| [Compares](reg_vector_compute/compare_and_select/Compares-42.md) | Compares each element in a Tensor with a scalar element by element. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0. |
| [Select](reg_vector_compute/compare_and_select/Select-43.md) | Given two source operands srcReg0 and srcReg1, selects elements based on the bit values of mask to obtain the destination operand dstReg. The selection rule is as follows: when the bit of mask is 1, the element at the corresponding position is selected from srcReg0; when the bit is 0, the element at the corresponding position is selected from srcReg1. |
| [Squeeze](reg_vector_compute/compare_and_select/Squeeze.md) | Copies the valid elements selected by mask from the input srcReg to dstReg in sequence. The valid elements are arranged contiguously from low to high in dstReg. The elements in the remaining positions of dstReg are set to 0. |

### Type Conversion
| API Name | Description |
| --- | --- |
| [Cast](reg_vector_compute/type_conversion/Cast-45.md) | Cast is used for data type precision conversion, converting the source operand data type to the destination operand data type. It supports float-to-integer, float-to-float, integer-to-float, and integer-to-integer data type conversions. |
| [Truncate](reg_vector_compute/type_conversion/Truncate.md) | Truncates the floating-point elements in the source operand to integer values (retaining the original data type) and stores them in the destination operand. |

### Reduction Computation
| API Name | Description |
| --- | --- |
| [Reduce](reg_vector_compute/reduction_compute/Reduce.md) | The reduction instruction simplifies a data set into a single value or a smaller set based on ReduceType. |
| [ReduceDataBlock](reg_vector_compute/reduction_compute/ReduceDataBlock.md) | The reduction instruction simplifies a data set into a single value within each DataBlock (32B) based on ReduceType. |
| [PairReduceElem](reg_vector_compute/reduction_compute/PairReduceElem.md) | Adds two adjacent values in the input srcReg and stores the result in the low-order positions of dstReg. |

### Data Fill
| API Name | Description |
| --- | --- |
| [Duplicate](reg_vector_compute/data_padding/Duplicate-48.md) | Supports both Scalar and Tensor modes. |

### Discrete Operation
| API Name | Description |
| --- | --- |
| [Gather](reg_vector_compute/scatter_operations/Gather.md) | This instruction collects elements from the source operand srcReg into the destination operand dstReg element-wise according to the index value indexReg. |

### Data Rearrangement
| API Name | Description |
| --- | --- |
| [Interleave](reg_vector_compute/data_reorder/Interleave-51.md) | Given source operand registers srcReg0 and srcReg1, interleaves the elements in srcReg0 and srcReg1 and stores them into destination operand registers dstReg0 and dstReg1. |
| [DeInterleave](reg_vector_compute/data_reorder/DeInterleave-52.md) | Given source operand registers srcReg0 and srcReg1, de-interleaves the elements in srcReg0 and srcReg1 and stores them into destination operand registers dstReg0 and dstReg1. |

### Data Compression
| API Name | Description |
| --- | --- |
| [Unsqueeze](reg_vector_compute/data_compression/Unsqueeze.md) | Decompresses the data in dstReg according to mask. Decompression method: the 0th element in dstReg is set to 0, and the i-th element in dstReg equals the number of 1s from the 0th to the \(i-1\)-th element in mask. The highest bit of mask is ignored and not counted. |
| [Pack](reg_vector_compute/data_compression/Pack-53.md) | Selects the low 8 bits (for the b16 type), low 16 bits (for the b32 type), or low 32 bits (for the b64 type) of the elements in the source operand srcReg and writes them into the low half or high half of dstReg. |
| [UnPack](reg_vector_compute/data_compression/UnPack-54.md) | For unsigned integer types, writes the elements in the low half or high half of the source operand srcReg into dstReg by padding the high bits with 0 to expand the bit width. For signed integer types, writes the elements in the low half or high half of the source operand srcReg into dstReg by preserving the sign bit to expand the bit width. |

### Histogram Computation
| API Name | Description |
| --- | --- |
| [Histograms](reg_vector_compute/histogram_compute/Histograms.md) | Performs statistics on histogram data, adding the statistical results of the source operand srcReg data to the basic data of the destination operand dstReg, including frequency statistics and cumulative statistics of the data. |

### Index Operation
| API Name | Description |
| --- | --- |
| [Arange](reg_vector_compute/index_operations/Arange.md) | This function uses the value of the passed-in scalar as the start value, generates incrementing/decrementing indexes, and stores the indexes in dstReg. |

### Synchronization Control
| API Name | Description |
| --- | --- |
| [LocalMemBar](reg_vector_compute/sync_control/LocalMemBar.md) | A synchronization instruction between different pipelines within a Reg vector computation macro function. This synchronization instruction specifies the src source pipeline and the dst destination pipeline, and the destination pipeline waits until all instructions on the source pipeline are complete before executing. In read/write scenarios, when the register used by a read instruction is the same as the register used by a write instruction, register ordering can be triggered, and instructions execute in code order without inserting a synchronization instruction. When different registers are used, a synchronization instruction must be inserted to ensure ordered execution of read and write instructions. The same applies to write/write scenarios. |
| [AllocMutexID (ISASI)](sync_control/intra_core_sync/AllocMutexID_ISASI.md) | Obtains and occupies a MutexID from the framework. It is used together with ReleaseMutexID to manage the acquisition and release of MutexIDs. The obtained MutexID can be passed to the Mutex API for use, in which case Mutex can be used together with other APIs such as TQue. |
| [DataSyncBarrier(ISASI)](sync_control/intra_core_sync/DataSyncBarrier_ISASI.md) | Blocks the execution of subsequent instructions until all previous memory access instructions (the memory locations to wait for can be controlled by parameters) have completed, and is used to resolve Scalar memory dependencies. |
| [PipeBarrier(ISASI)](sync_control/intra_core_sync/PipeBarrier_ISASI.md) | Performs synchronization control within the same pipeline, and is used to constrain the execution order within the same pipeline. Its purpose is to ensure that all data read and write operations in preceding instructions are fully completed before subsequent instructions can execute. |
| [ReleaseMutexID (ISASI)](sync_control/intra_core_sync/ReleaseMutexID_ISASI.md) | Releases a MutexID from the framework, and is used together with AllocMutexID. |
| [SetFlag/WaitFlag(ISASI)](sync_control/intra_core_sync/SetFlag_WaitFlag_ISASI.md) | The SetFlag/WaitFlag APIs are used for synchronization between multiple pipelines within a core. |
| [CrossCoreSetFlag(ISASI)](sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md) | In inter-core synchronization scenarios, the CrossCoreSetFlag API and the CrossCoreWaitFlag API work in pairs. When used, the inter-core synchronization flag ID (flagId) must be passed in, and each ID corresponds to a counter used to control synchronization. The CrossCoreSetFlag API is used to notify the scheduling module that "the pipe pipeline task of this core has been completed", and the CrossCoreWaitFlag API is used to block the dispatch of subsequent instructions until all related cores have completed synchronization before the block is released. |
| [CrossCoreWaitFlag(ISASI)](sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md) | In inter-core synchronization scenarios, the CrossCoreSetFlag API and the CrossCoreWaitFlag API work in pairs. For details, see [CrossCoreSetFlag](sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md). |

### System Variable Access
| API Name | Description |
| --- | --- |
| [ClearSpr](reg_vector_compute/system_variable_access/ClearSpr.md) | Clears the specified special register. |

### Data Type
| API Name | Description |
| --- | --- |
| [PostLiteral](reg_vector_compute/aux_data_types/PostLiteral.md) | enum class PostLiteral  |
| [RegLayout](reg_vector_compute/aux_data_types/RegLayout.md) | enum class RegLayout  |
| [SatMode](reg_vector_compute/aux_data_types/SatMode.md) | Sets the saturation and non-saturation modes for Cast type conversion. |
| [MaskMergeMode](reg_vector_compute/aux_data_types/MaskMergeMode.md) | Determines whether elements not selected by the mask are set to 0 for the dst element. |
| [RoundMode](reg_vector_compute/aux_data_types/RoundMode.md) | Controls the rounding mode. |

## Scalar Computation
| API Name | Description |
| --- | --- |
| [GetBitCount](scalar_compute/GetBitCount.md) | Counts the occurrences of a specified bit value (0 or 1) in the binary representation of a uint64_t value. |
| [CountLeadingZero](scalar_compute/CountLeadingZero.md) | Counts the number of leading zeros in the binary representation of a uint64_t value (the number of '0's from the most significant bit to the first '1'; returns 64 when the input is 0). |
| [CountBitsCntSameAsSignBit](scalar_compute/CountBitsCntSameAsSignBit.md) | Counts the number of consecutive bits that are the same as the sign bit, starting from the most significant bit, in the binary representation of an int64_t value. |
| [GetSFFValue](scalar_compute/GetSFFValue.md) | Finds the position of the first specified bit value (0 or 1) starting from the least significant bit in the binary representation of a uint64_t value; returns -1 if not found. |
| [CeilDivision](scalar_compute/CeilDivision.md) | Computes the result of dividing two integers num1 and num2, rounded up. |
| [Cast (float to half/int32_t)](scalar_compute/Cast_float_to_half_int32.md) | Converts the data type of a scalar. |
| [Cast (float to bfloat16_t)](scalar_compute/Cast_float_to_bfloat16.md) | Converts float scalar data to bfloat16_t scalar data. |
| [Cast (multi-type to float)](scalar_compute/Cast_multi_type_to_float.md) | Converts the input data to the float type. |
| [Nop](scalar_compute/Nop.md) | The user inputs the number of delays to implement a delay of several cycles. |
| [WriteGmByPassDCache(ISASI)](scalar_compute/WriteGmByPassDCache_ISASI.md) | Writes data to a GM address without going through DCache. |
| [ReadGmByPassDCache(ISASI)](scalar_compute/ReadGmByPassDCache_ISASI.md) | Reads data from a GM address without going through DCache. |

## Resource Management
| API Name | Description |
| --- | --- |
| [TPipe](resource_management/TPipe/TPipe.md) | TPipe is a framework used to manage resources such as global memory. Through the APIs provided by the TPipe class, operations such as memory resource allocation and management can be completed. |
| [GetTPipePtr](resource_management/GetTPipePtr.md) | When a TPipe object is created, object initialization sets a globally unique TPipe pointer. This API is used to obtain that pointer, after which TPipe-related operations can be performed. |
| [TBufPool](resource_management/TBufPool/TBufPool.md) | TPipe can manage global memory resources, while TBufPool can manually manage or reuse Unified Buffer/L1 Buffer physical memory. It is mainly used in scenarios where Unified Buffer/L1 Buffer physical memory is insufficient during multi-stage computation. |
| [TQue](resource_management/TQue/TQue.md) | Provides APIs such as enqueue and dequeue, and completes synchronization between tasks through queues. |
| [TQueBind](resource_management/TQueBind/TQueBind.md) | TQueBind binds the source logical position and the destination logical position. Based on the source and destination positions, it determines the memory allocation location and inserts corresponding synchronization events, helping developers solve problems such as memory allocation and management, and synchronization. |
| [TBuf](resource_management/TBuf/TBuf.md) | During Ascend C programming, some temporary variables may be used. The memory occupied by these temporary variables can be managed using the TBuf data structure. |
| [InitSpmBuffer](resource_management/TPipe/InitSpmBuffer.md) | Initializes the SPM Buffer. |
| [WriteSpmBuffer](resource_management/TPipe/WriteSpmBuffer.md) | Copies data that needs to be spilled and temporarily stored into the SPM Buffer. |
| [ReadSpmBuffer](resource_management/TPipe/ReadSpmBuffer.md) | Reads data back from the SPM Buffer into local data. |
| [GetUserWorkspace](resource_management/workspace/GetUserWorkspace.md) | Obtains the workspace pointer used by the user. |
| [SetSysWorkSpace](resource_management/workspace/SetSysWorkSpace.md) | When programming fused operators, the framework communication mechanism requires the use of workspace, that is, the system workspace. Therefore, in this scenario, developers need to call this API to set the system workspace pointer. |
| [GetSysWorkSpacePtr](resource_management/workspace/GetSysWorkSpacePtr.md) | Obtains the system workspace pointer. |
| [PopStackBuffer](resource_management/PopStackBuffer.md) | Applies for temporary space at the specified position (logical position), with the space size being all remaining space at the specified position. |

## Synchronization Control
| API Name | Description |
| --- | --- |
| [TQueSync](sync_control/intra_core_sync/tquesync_template_parameter.md) | The TQueSync class provides synchronization control APIs. Developers can use these APIs to implement synchronization control on their own. |
| [IBSet](sync_control/inter_core_sync/IBSet.md) | Call IBSet to set the flag bit of a core. It is used in pairs with IBWait to represent the synchronization wait instruction between cores: the waited core calls IBSet to notify the waiting core by writing the value 1 to a block of addresses in global memory; the waiting core calls IBWait to continuously read the value of the corresponding address block in global memory until it becomes 1, thereby implementing inter-core synchronization. |
| [IBWait](sync_control/inter_core_sync/IBWait.md) | Call IBSet to set the flag bit of a core. It is used in pairs with IBWait to represent the synchronization wait instruction between cores: the waited core calls IBSet to notify the waiting core by writing the value 1 to a block of addresses in global memory; the waiting core calls IBWait to continuously read the value of the corresponding address block in global memory until it becomes 1, thereby implementing inter-core synchronization. |
| [SyncAll](sync_control/inter_core_sync/SyncAll.md) | SyncAll is an inter-core synchronization control API. |
| [InitDetermineComputeWorkspace](sync_control/inter_core_sync/InitDetermineComputeWorkspace.md) | InitDetermineComputeWorkspace is an initialization configuration API for deterministic computation based on sequential execution between cores. It initializes the value of GM shared memory. Only after the initialization is complete can WaitPreBlock and NotifyNextBlock be called. The three APIs above jointly implement deterministic computation based on sequential execution between cores. For the specific meaning of deterministic computation, see Deterministic Computation. |
| [NotifyNextBlock](sync_control/inter_core_sync/NotifyNextBlock.md) | WaitPreBlock and NotifyNextBlock are inter-core synchronization control APIs. NotifyNextBlock notifies other cores that the current core has finished execution by writing to global memory, so that other cores can continue execution. WaitPreBlock determines whether the current core can continue execution by reading global memory. |
| [WaitPreBlock](sync_control/inter_core_sync/WaitPreBlock.md) | WaitPreBlock and NotifyNextBlock are inter-core synchronization control APIs. NotifyNextBlock notifies other cores that the current core has finished execution by writing to global memory, so that other cores can continue execution. WaitPreBlock determines whether the current core can continue execution by reading global memory. |
| [SetNextTaskStart](sync_control/inter_task_sync/SetNextTaskStart.md) | Called in a sub-kernel of SuperKernel. The instructions after the call can run in parallel with subsequent sub-kernels, improving overall performance. SuperKernel calls sub-kernels in order. To ensure that data between sub-kernels does not interfere with each other, inter-operator synchronization is inserted between sub-kernels to preserve order. After sub-kernel N-1 calls this API, the subsequent instructions run in parallel with the following sub-kernel N. |
| [WaitPreTaskEnd](sync_control/inter_task_sync/WaitPreTaskEnd.md) | Called in a sub-kernel of SuperKernel. The instructions before the call can run in parallel with preceding sub-kernels, improving overall performance. SuperKernel calls sub-kernels in order. To ensure that data between sub-kernels does not interfere with each other, inter-operator synchronization is inserted between sub-kernels to preserve order. The instructions before sub-kernel N+1 calls this API run in parallel with the preceding sub-kernel N. |

## Cache Control
| API Name | Description |
| --- | --- |
| [DataCachePreload](cache_control/DataCachePreload.md) | Preloads data from the specific GM address where the source address resides into the DCache. Each call preloads only one Cache Line of data. |
| [DataCacheCleanAndInvalid](cache_control/DataCacheCleanAndInvalid.md) | Within the AI Core, both the Scalar unit and the DMA unit may access the GM. |
| [ICachePreLoad(ISASI)](cache_control/ICachePreLoad_ISASI.md) | Developers can manually call the ICachePreLoad API to preload instructions from the GM address where the instructions reside into the ICache. |
| [GetICachePreloadStatus(ISASI)](cache_control/GetICachePreloadStatus_ISASI.md) | GetICachePreloadStatus is a debug API, called after ICachePreLoad to obtain the PreLoad status of the ICache: when the return value is 0, the ICache PreLoad is complete; when the return value is 1, the ICache PreLoad is not complete. |

## Atomic Operation
| API Name | Description |
| --- | --- |
| [SetAtomicAdd](atomic_operations/SetAtomicAdd.md) | Enables atomic accumulation for subsequent data movement whose destination address is in GM. Atomic accumulation process: the content to be copied is summed with the existing content in GM, and the sum is then written to GM. The SetAtomicAdd API can set different accumulation data types through template parameters. |
| [SetAtomicType](atomic_operations/SetAtomicType.md) | Sets different data types for atomic operations through template parameters. |
| [DisableDmaAtomic](atomic_operations/DisableDmaAtomic.md) | Disables the inline atomic operation function of data movement. When data movement is subsequently executed, the original data in GM will be completely overwritten by the newly moved data. |
| [SetAtomicMax(ISASI)](atomic_operations/SetAtomicMax_ISASI.md) | Sets whether atomic comparison is performed on data subsequently moved to GM: the content to be copied is compared with the existing content in GM, and the maximum value is then written to GM. The SetAtomicMax API can set different data types through template parameters. |
| [SetAtomicMin(ISASI)](atomic_operations/SetAtomicMin_ISASI.md) | Sets whether atomic comparison is performed on data subsequently moved to GM: the content to be copied is compared with the existing content in GM, and the minimum value is then written to GM. The SetAtomicMin API can set different data types through template parameters. |
| [SetStoreAtomicConfig(ISASI)](atomic_operations/SetStoreAtomicConfig_ISASI.md) | Sets the atomic operation enable bit and the atomic operation type. |
| [GetStoreAtomicConfig(ISASI)](atomic_operations/GetStoreAtomicConfig_ISASI.md) | Obtains the values of the atomic operation enable bit and the atomic operation type. |
| [AtomicAdd](atomic_operations/AtomicAdd.md) | After this API is called, an atomic addition operation can be performed on the specified GM address. |
| [AtomicMin](atomic_operations/AtomicMin.md) | After this API is called, an atomic comparison-and-minimum operation can be performed on the specified GM address. |
| [AtomicMax](atomic_operations/AtomicMax.md) | After this API is called, an atomic maximum operation can be performed on the specified GM address. |
| [AtomicCas](atomic_operations/AtomicCas.md) | After this API is called, an atomic comparison can be performed on the specified GM address. If the value is equal to value1, the value of value2 is assigned to GM; if the value is not equal to value1, the value in GM remains unchanged. |
| [AtomicExch](atomic_operations/AtomicExch.md) | Performs an atomic exchange operation in GM memory. Specifically, it reads the data at the specified GM address and stores a new value back to the same address. The function returns the old value. |

## Debug API
| API Name | Description |
| --- | --- |
| [DumpTensor](debug_interface/onboard_print/DumpTensor.md) | This API prints the content of a Tensor and supports printing custom labels (only information of the uint32_t data type), such as the current line number. |
| [printf](debug_interface/onboard_print/printf.md) | This API provides formatted output in CPU domain/NPU domain debugging scenarios. |
| [DumpAccChkPoint](debug_interface/onboard_print/DumpAccChkPoint.md) | This API dumps the content of a specified Tensor. It also supports printing custom labels (only information of the uint32_t data type), such as the current line number. Unlike DumpTensor, this API supports printing a Tensor at a specified offset position. |
| [PrintTimeStamp](debug_interface/onboard_print/PrintTimeStamp.md) | Provides timestamp marking for marking key execution points in the operator Kernel code. |
| [ascendc_assert](debug_interface/exception_detection/ascendc_assert.md) | ascendc_assert provides an API for implementing assertion in the CPU/NPU domain. When the assertion condition is not met, the system outputs the assertion information and prints it on the screen in a formatted manner. |
| [assert](debug_interface/exception_detection/assert.md) | This API implements the assert function in the CPU/NPU domain. During operator execution, if the internal condition of assert is not true, the assert condition is output and the input information is printed on the screen in a formatted manner. |
| [Trap](debug_interface/exception_detection/Trap.md) | Called on the Kernel side. In NPU mode, it interrupts the execution of the AI Core. In CPU mode, it is equivalent to assert. It can be used for debugging abnormal scenarios on the Kernel side. |
| [CheckLocalMemoryIA(ISASI)](debug_interface/exception_detection/CheckLocalMemoryIA_ISASI.md) | Checks UB read/write behavior within a specified range. If read/write behavior within the specified range occurs, an EXCEPTION error is reported. If no read/write behavior within the specified range occurs, no error is reported. |
| [GmAlloc](debug_interface/cpu_twin_debug/GmAlloc.md) | Used to create shared memory when performing CPU-side execution verification of a kernel function: creates a shared file in the /tmp directory and returns the mapped pointer of the file. |
| [ICPU_RUN_KF](debug_interface/cpu_twin_debug/ICPU_RUN_KF.md) | The overall entry point for CPU debugging when performing CPU-side execution verification of a kernel function, completing the operator program invocation on the CPU side. |
| [ICPU_SET_TILING_KEY](debug_interface/cpu_twin_debug/ICPU_SET_TILING_KEY.md) | Used to specify the tilingKey for the current CPU debugging. During debugging execution, only the branch corresponding to this tilingKey in the operator kernel function is executed. |
| [GmFree](debug_interface/cpu_twin_debug/GmFree.md) | Used to release the shared memory allocated through GmAlloc when performing CPU-side execution verification of a kernel function. |
| [SetKernelMode](debug_interface/cpu_twin_debug/SetKernelMode.md) | For the separated mode, during CPU debugging, sets the kernel mode to single-AIV mode, single-AIC mode, or MIX mode to support CPU debugging of single-AIV vector operators, single-AIC cube operators, and MIX hybrid operators respectively. If this API is not called, the default is MIX mode. To ensure compatibility of operator code across multiple hardware platforms, it can also be called in coupled mode, in which case the API does not take effect and does not affect normal debugging. |
| [MetricsProfStart](debug_interface/performance_stats/MetricsProfStart.md) | Used to set the start of the performance data collection signal, used in conjunction with MetricsProfStop. When using the msOpProf tool for on-board operator tuning, MetricsProfStart and MetricsProfStop can be called before and after the Kernel-side code segment to specify the range of the code segment to be tuned. |
| [MetricsProfStop](debug_interface/performance_stats/MetricsProfStop.md) | Sets the stop of the performance data collection signal, used in conjunction with MetricsProfStart. When using the msOpProf tool for on-board operator tuning, MetricsProfStart and MetricsProfStop can be called before and after the Kernel-side code segment to specify the range of the code segment to be tuned. |
| [MarkStamp](debug_interface/performance_stats/MarkStamp.md) | By calling this API, the user marks a specific position during operator execution to facilitate later analysis of the code execution path and performance hotspots through the pipeline graph. |

## Utility API
| API Name | Description |
| --- | --- |
| [Async](tool_interface/execution_mode/Async.md) | Async encapsulates this isolation mode through a template function, providing a unified API for executing specific functions on different execution units (AIC or AIV), thereby avoiding the use of hardware conditional branches in the code. |
| [InitSocState](tool_interface/system_init/InitSocState.md) | This API initializes the global state of the AI Core, including the common state and the respective unique states of the AIC (Cube Core) and AIV (Vector Core). Because implementations differ across products, the actual initialization items executed also vary. |
| [Max](tool_interface/NumericLimits_utils/Max-77.md) | Returns the maximum finite value of the specified data type. |
| [Lowest](tool_interface/NumericLimits_utils/Lowest.md) | Returns the lowest finite value of the specified data type, that is, the finite value x for which there is no other finite value y satisfying y < x. |
| [Min](tool_interface/NumericLimits_utils/Min-78.md) | Returns the minimum finite value of the specified data type. |
| [Infinity](tool_interface/NumericLimits_utils/Infinity.md) | Returns the positive infinity value of the specified data type. |
| [NegativeInfinity](tool_interface/NumericLimits_utils/NegativeInfinity.md) | Returns the negative infinity value of the specified data type. |
| [QuietNaN](tool_interface/NumericLimits_utils/QuietNaN.md) | Returns the quiet NaN value of the specified data type (the highest bit of the floating-point mantissa is 1). |
| [SignalingNaN](tool_interface/NumericLimits_utils/SignalingNaN.md) | Returns the signaling NaN value of the specified data type (the highest bit of the floating-point mantissa is 0). |
| [DeNormMin](tool_interface/NumericLimits_utils/DeNormMin.md) | Returns the smallest positive denormal value of the specified data type. |
| [GetTaskRatio](tool_interface/system_resources_and_variables/GetTaskRatio.md) | In separation mode, obtains the ratio of the number of Cube Cores (AIC) or Vector Cores (AIV) started for a task to the number of logical AI Cores. |
| [GetUBSizeInBytes](tool_interface/system_resources_and_variables/GetUBSizeInBytes.md) | Obtains the size of the UB space in bytes. Developers calculate parameter values such as the number of loop iterations based on the UB size. |
| [GetRuntimeUBSize](tool_interface/system_resources_and_variables/GetRuntimeUBSize.md) | Obtains the size of the runtime UB space in bytes. Developers calculate parameter values such as the number of loop iterations based on the UB size. |
| [GetVecLen](tool_interface/system_resources_and_variables/GetVecLen.md) | Obtains the size of the RegTensor bit width VL (Vector Length). |
| [GetSsbufBaseAddr](tool_interface/system_resources_and_variables/GetSsbufBaseAddr.md) | This API is used to obtain the base address of the SSBuffer. |
| [GetSystemCycle(ISASI)](tool_interface/system_resources_and_variables/GetSystemCycle_ISASI.md) | Obtains the current cycle value of the system counter. By calculating the difference between the cycle values before and after a code segment, the number of system cycles consumed by the code segment can be obtained, and the execution time of the code segment can then be derived through conversion. |
| [GetBlockNum](tool_interface/system_resources_and_variables/GetBlockNum.md) | Obtains the number of logical AI Cores configured for the current task, used for multi-core logic control within the code. |
| [GetBlockIdx](tool_interface/system_resources_and_variables/GetBlockIdx.md) | Obtains the index of the current logical AI Core, used for multi-core logic control and multi-core offset calculation within the code. |
| [GetDataBlockSizeInBytes](tool_interface/system_resources_and_variables/GetDataBlockSizeInBytes.md) | Obtains the size of one DataBlock on the current chip version, in bytes. |
| [GetArchVersion](tool_interface/system_resources_and_variables/GetArchVersion.md) | Obtains the architecture version number of the current AI processor. |
| [GetProgramCounter(ISASI)](tool_interface/system_resources_and_variables/GetProgramCounter_ISASI.md) | Obtains the value of the program counter to track the current execution position of the program. |
| [GetSubBlockNum(ISASI)](tool_interface/system_resources_and_variables/GetSubBlockNum_ISASI.md) | In the separation mode architecture, obtains the number of Cube Cores (AIC) or Vector Cores (AIV) on one logical AI Core in the current configuration. |
| [GetSubBlockIdx(ISASI)](tool_interface/system_resources_and_variables/GetSubBlockIdx_ISASI.md) | In the separation mode architecture, obtains the logical index of the Cube Core (AIC) or Vector Core (AIV) on the logical AI Core. |

## Cube Group Management (ISASI)
| API Name | Description |
| --- | --- |
| [CreateCubeResGroup](cube_group_mgmt_ISASI/CubeResGroupHandle/CreateCubeResGroup.md) | Quickly creates a CubeResGroupHandle object, completing the allocation of message queue space and synchronization events internally. It is recommended to use this API instead of the CubeResGroupHandle constructor to create objects, so as to avoid message queue space conflicts and synchronization event errors between different objects. |
| [AssignQueue](cube_group_mgmt_ISASI/CubeResGroupHandle/AssignQueue.md) | Used by AIV to bind the sequence number of a message queue in CubeResGroupHandle. |
| [AllocMessage](cube_group_mgmt_ISASI/CubeResGroupHandle/AllocMessage.md) | AIV requests message space from the message queue to store the message structure, and returns the address of the currently requested message space. The depth of the message queue is fixed at 4, and the message space is requested from top to bottom and then cyclically. When the message space pointed to by the message queue pointer is in the FREE state, AllocMessage returns the address of the space; otherwise, it waits in a loop until the current space is in the FREE state. |
| [PostMessage](cube_group_mgmt_ISASI/CubeResGroupHandle/PostMessage.md) | After obtaining the message space address msg through the AllocMessage API, constructs the message structure CubeMsgType and sends the message. |
| [PostFakeMsg](cube_group_mgmt_ISASI/CubeResGroupHandle/PostFakeMsg.md) | After obtaining the message space address through the AllocMessage API, AIV sends a fake message and refreshes the message state msgState to FAKE. |
| [SetQuit](cube_group_mgmt_ISASI/CubeResGroupHandle/SetQuit.md) | After obtaining the message space address through the AllocMessage API, sends a quit message to inform the AIC corresponding to this message queue that it does not need to process messages in this queue. After the AIV corresponding to Queue5 sends the quit message, Block1 will no longer process any messages in Queue5. |
| [Wait](cube_group_mgmt_ISASI/CubeResGroupHandle/Wait.md) | After calling PostMessage or PostFakeMessage, queries whether the message has been processed by AIC. |
| [FreeMessage](cube_group_mgmt_ISASI/CubeResGroupHandle/FreeMessage.md) | In the custom callback function logic, after message processing is completed, calls this API to refresh the message state to FREE; or refreshes the message state to FREE when the message state is the specified state waitState. |
| [SetSkipMsg](cube_group_mgmt_ISASI/CubeResGroupHandle/SetSkipMsg.md) | AIC skips the processing of a specified number of fake messages, and is called only in the callback function. In the following figure, Block0 skips three fake messages by calling SetSkipMsg. |
| [Arrive](cube_group_mgmt_ISASI/GroupBarrier/Arrive.md) | Notifies other waiting AIVs that this AIV has completed its dependent tasks. |
| [GetWorkspaceLen](cube_group_mgmt_ISASI/GroupBarrier/GetWorkspaceLen.md) | Returns the size of the Global Memory message space occupied by the current GroupBarrier. |
| [UpdateKfcWorkspace](cube_group_mgmt_ISASI/KfcWorkspace/UpdateKfcWorkspace.md) | Updates the memory address used for the CubeResGroupHandle message communication area. When using the CubeResGroupHandle API, users need to use this API to manage the space address independently. |
| [GetKfcWorkspace](cube_group_mgmt_ISASI/KfcWorkspace/GetKfcWorkspace.md) | Obtains the memory address used for the CubeResGroupHandle message communication area. When using the CubeResGroupHandle API, users need to use this API to manage the space address independently. |

## Kernel-Tiling
| API Name | Description |
| --- | --- |
| [GET_TILING_DATA](Kernel-Tiling/GET_TILING_DATA.md) | Obtains the Tiling information passed into the operator kernel entry function and fills it into the registered TilingData structure. This function is compiled through macro expansion. In the corresponding operator host implementation, the TilingData structure must be defined, and the Tiling function that computes the TilingData must be implemented and registered. If the user registers multiple TilingData structures through TilingData structure registration, this API returns the default registered structure. |
| [GET_TILING_DATA_WITH_STRUCT](Kernel-Tiling/GET_TILING_DATA_WITH_STRUCT.md) | Specifies a structure name to obtain the specified tiling information and fill it into the corresponding Tiling structure. This function is compiled through macro expansion. The difference from GET\_TILING\_DATA is that GET\_TILING\_DATA can only obtain the default registered structure, while this API can obtain the corresponding structure by the specified structure name. It is commonly used when different structures are registered for different TilingKeys. |
| [GET_TILING_DATA_MEMBER](Kernel-Tiling/GET_TILING_DATA_MEMBER.md) | Obtains the member variables of the tiling structure. |
| [GET_TILING_DATA_PTR_WITH_STRUCT](Kernel-Tiling/GET_TILING_DATA_PTR_WITH_STRUCT.md) | When using this macro, developers can obtain the corresponding Tiling information by specifying a structure name and fill it into the corresponding Tiling structure. After the filling is complete, this macro returns a pointer to the Tiling structure and decorates the pointer with the \_\_tiling\_data\_ptr\_\_ modifier. This decoration ensures code uniformity and compatibility in both dynamic and static Shape scenarios. |
| [COPY_TILING_WITH_STRUCT](Kernel-Tiling/COPY_TILING_WITH_STRUCT.md) | Copies the Tiling structure and returns a pointer to the copied Tiling structure. This macro is applicable to nested structure scenarios and can copy the sub-structure member variables of a structure. This macro copies the specified structure to the stack, which is suitable for scenarios with frequent access to Tiling data and can accelerate data access. |
| [COPY_TILING_WITH_ARRAY](Kernel-Tiling/COPY_TILING_WITH_ARRAY.md) | Copies the array content of a specified size to the target array and returns a pointer to the copied array. It is applicable to the scenario of copying an array member variable of a structure. This macro copies the specified array to the stack, which is suitable for scenarios with frequent access to Tiling data and can accelerate data access. |
| [TILING_KEY_IS](Kernel-Tiling/TILING_KEY_IS.md) | Determines in the kernel function whether the tiling_key of the current execution equals a certain key set on the host side at runtime, thereby identifying a kernel branch where tiling_key==key. |
| [TILING_KEY_LIST](Kernel-Tiling/TILING_KEY_LIST.md) | The TILING_KEY_LIST function is used in the kernel function to determine whether the TilingKey of the current execution matches the specified TilingKey configured on the Host side, thereby identifying the branch logic that satisfies the condition TilingKey == key1 or TilingKey == key2. |
| [REGISTER_TILING_DEFAULT](Kernel-Tiling/REGISTER_TILING_DEFAULT.md) | Registers on the kernel side the default TilingData structure customized by the user using standard C++ syntax. |
| [REGISTER_TILING_FOR_TILINGKEY](Kernel-Tiling/REGISTER_TILING_FOR_TILINGKEY.md) | Registers on the kernel side the customized TilingData structure that matches the TilingKey. This API requires a logical expression, in which the string "TILING_KEY_VAR" represents the actual TilingKey and expresses the range satisfied by the TilingKey. |
| [REGISTER_NONE_TILING](Kernel-Tiling/REGISTER_NONE_TILING.md) | When using a TilingData structure customized with standard C++ syntax on the Kernel side, if the user is uncertain about which structures need to be registered, this API can be used to inform the framework side that the TilingData should be defined using unregistered standard C++ syntax, and it is used together with GET\_TILING\_DATA\_WITH\_STRUCT and GET\_TILING\_DATA\_MEMBER. |
| [Set Kernel Type](Kernel-Tiling/set_kernel_type.md) | Allows the user to customize the kernel type, controlling the operator execution to launch only kernels of that type, avoiding launching kernels that do not need to work and reducing kernel launch overhead. |

## Special Register Access
| API Name | Description |
| --- | --- |
| [SetCtrlSpr(ISASI)](special_register_access/SetCtrlSpr_ISASI.md) | Sets specific bits of the CTRL register (control register). |
| [GetCtrlSpr(ISASI)](special_register_access/GetCtrlSpr_ISASI.md) | Reads the value of specific bits in the CTRL register (control register). |
| [ResetCtrlSpr(ISASI)](special_register_access/ResetCtrlSpr_ISASI.md) | Resets specific bits of the CTRL register (control register). |
| [GetSaturationFlag(ISASI)](special_register_access/GetSaturationFlag_ISASI.md) | Obtains the enabled state of saturation mode during computation/precision conversion. |
| [SetSaturationFlag(ISASI)](special_register_access/SetSaturationFlag_ISASI.md) | Sets the saturation switch flag to control enabling and disabling of saturation mode during computation/precision conversion. |
