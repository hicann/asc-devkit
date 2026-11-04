# Common Matrix Computation Flow<a name="ZH-CN_TOPIC_0000002568950891"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T13:35:56.034Z pushedAt=2026-10-26T07:41:44.207Z nl=2 -->

Ascend C provides four types of APIs for the matrix computation programming model, which respectively carry the movement capability and computation capability of each path in the Cube core, as shown in the following figure:

**Figure 1** Common matrix basic computation flowchart<a name="zh-cn_topic_0000002535567224_fig135639216485"></a>  
![](../../../../figures/matrix_computation_workflow.png "Common matrix basic computation flowchart")

1. Use the **DataCopy** API to move the GM data of the original matrices A and B to the L1 Buffer (if Bias/inline quantization exists, it is also moved to the L1 Buffer through **DataCopy**). For details, see [Moving Matrix Data into the L1 Buffer](../cube_compute_load/cube_compute_load.md).

2. Use the **LoadData** API to load matrices A and B to the L0A Buffer and L0B Buffer respectively for computation (if Bias/inline quantization exists, use **DataCopy** to move the bias data/quantization coefficient data in the L1 Buffer to the BT Buffer/Fixpipe Buffer). For details, see [Moving Matrix Data into the L0A Buffer/L0B Buffer](../cube_compute_load/cube_compute_load.md).

3. Use the **Mmad** API to perform matrix computation on the data in the L0A Buffer, L0B Buffer, and BT Buffer, and output the result to the L0C Buffer. For details, see [Mmad Computation](../mmad_compute/Mmad.md).

4. Use the **Fixpipe** API to process the data in the L0C Buffer and move it out to the GM. The **Fixpipe** API can use the Fixpipe Buffer data to perform operations such as inline quantization and Relu. For details, see [Fixpipe (Moving Data from L0C to GM)](../cube_compute_store/Fixpipe_L0CToGM.md).

<!-- npu="950" id1 -->
# Mx Matrix Computation Flow<a name="ZH-CN_TOPIC_0000002568950892"></a>

Mx matrix computation is supported only on the Ascend 950PR&950DT products models.

**MxMmad** (Microscaling Mmad) is a matrix multiplication with quantization coefficients, that is, both the left matrix and the right matrix have corresponding quantization coefficient matrices: the left quantization coefficient matrix **scaleA** and the right quantization coefficient matrix **scaleB**. In the MxMmad scenario, the left quantization coefficient matrix is multiplied by the left matrix, and the right quantization coefficient matrix is multiplied by the right matrix, and matrix multiplication is performed on the results of the two products. Ascend C provides corresponding Mx-class APIs for the Mx matrix computation programming model, as shown in the following figure:

**Figure 2** Mx matrix basic computation flowchart<a name="zh-cn_topic_0000002535567225_fig135639216486"></a>  
![](../../../../figures/mx_matrix_computation_workflow.png "Mx matrix basic computation flowchart")

1. Use the **DataCopy** API to move the GM data of the original matrices A and B, the [left coefficient matrix scaleA](../cube_compute_fractal_intro/aux_cube_fractal_format_details.md#section_mx_scalea_fractal_format), and the [right coefficient matrix scaleB](../cube_compute_fractal_intro/aux_cube_fractal_format_details.md#section_mx_scaleb_fractal_format) to the L1 Buffer (if Bias/inline quantization exists, it is also moved to the L1 Buffer through **DataCopy**). For details, see [Moving Matrix Data into the L1 Buffer](../cube_compute_load/cube_compute_load.md).

2. Use the [LoadData (MX matrix movement) API](../cube_compute_load/LoadData_2D_MX.md) to load matrix A and the scaleA matrix to the L0A Buffer and the built-in L0A_MX Buffer, and load matrix B and the scaleB matrix to the L0B Buffer and the L0B_MX Buffer respectively, in preparation for computation (if Bias/inline quantization exists, use **DataCopy** to move the bias data/quantization coefficient data in the L1 Buffer to the BT Buffer/Fixpipe Buffer). For details, see [Moving Matrix Data into the L0A Buffer/L0B Buffer](../cube_compute_load/cube_compute_load.md).

3. Use the **MmadMx** API to perform matrix computation on the data in the L0A Buffer, L0A_MX Buffer, L0B Buffer, L0B_MX Buffer, and BT Buffer, and output the result to the L0C Buffer. For details, see [MmadMx Computation](../mmad_compute/MmadMx.md).

4. Use the **Fixpipe** API to process the data in the L0C Buffer and move it out to the GM. The **Fixpipe** API can use the Fixpipe Buffer data to perform operations such as inline quantization and Relu. For details, see [Fixpipe (L0C to GM Data Movement)](../cube_compute_store/Fixpipe_L0CToGM.md).
<!-- end id1 -->

<!-- npu="910b,A3" id2 -->
# 4-Select-2 Sparse Matrix Computation Flow<a name="ZH-CN_TOPIC_0000002538231116"></a>

A sparse matrix is a special type of matrix that contains many zero elements. 4-select-2 structured sparse matrix computation requires that, in a group of four consecutive weights or activation values (usually a row or column in a tensor), at most two values are non-zero, and the remaining two are forced to zero. For Atlas A2 training products/Atlas A2 inference products and Atlas A3 training products/Atlas A3 inference products, Ascend C provides corresponding sparse APIs for the 4-select-2 sparse matrix computation programming model, as shown in the following figure:

**Figure 3** 4-select-2 sparse matrix basic computation flowchart<a name="zh-cn_topic_0000002566526987_fig12280145134813"></a>  
![](../../../../figures/sparse_matrix_computation_workflow.png "4-select-2 sparse matrix basic computation flowchart")

1. First, divide the original matrix B into groups of four consecutive elements according to a fixed granularity, and generate the corresponding dense matrix B and [index matrix](../cube_compute_fractal_intro/aux_cube_fractal_format_details.md#zh-cn_topic_0000002563445163_section1873692415233) offline as inputs according to the fixed-structure dense algorithm.

2. Use the DataCopy API to move the GM data of the original matrix A (sparse matrix), dense matrix B, and index matrix to the L1 Buffer (if inline quantization exists, it is also moved to the L1 Buffer through DataCopy). For details, see [Moving Matrix Data into the L1 Buffer](../cube_compute_load/cube_compute_load.md).

3. Use the LoadData API to load the original matrix A to the L0A Buffer, and use the [LoadDataWithSparse API](../cube_compute_load/LoadDataWithSparse.md) to load the dense matrix B and the index matrix to the L0B Buffer and the built-in IDX Buffer respectively for computation (if inline quantization exists, use DataCopy to move the quantization coefficient data in the L1 Buffer to the Fixpipe Buffer). For details, see [Moving Matrix Data into the L0A Buffer/L0B Buffer](../cube_compute_load/cube_compute_load.md).

4. Use the MmadWithSparse API to perform 4-select-2 sparse matrix computation on the data in the L0A Buffer, L0B Buffer, and built-in IDX Buffer, and output the result to the L0C Buffer. For details, see [MmadWithSparse Computation](../mmad_compute/MmadWithSparse.md).

5. Use the Fixpipe API to process the data in the L0C Buffer and move it out to the GM. The Fixpipe API can use the Fixpipe Buffer data to perform operations such as inline quantization and ReLU. For details, see [Fixpipe (L0C to GM Data Movement)](../cube_compute_store/Fixpipe_L0CToGM.md).
<!-- end id2 -->

# Conv2D Forward Computation Flow<a name="ZH-CN_TOPIC_0000002568950893"></a>

The essence of Conv2D forward computation is to convert the convolution operation into matrix multiplication: first, img2col (Image to Column) expands the FeatureMap into the left matrix A according to the convolution kernel sliding window, uses the Weight as the right matrix B, and then performs matrix multiply-add to obtain the output. The difference from the [ordinary matrix computation flow](#ZH-CN_TOPIC_0000002568950891) is that after matrix A is directly moved from GM, [LoadData (convolution data movement)](../cube_compute_load/LoadData_3D.md) completes the img2col spatial expansion online during the movement from L1 Buffer to L0A Buffer based on the convolution kernel size, stride, dilation, and padding, as shown in the following figure:

**Figure 4** Conv2D forward computation flowchart<a name="zh-cn_topic_0000002568950893_fig135639216487"></a>  
![](../../../../figures/conv2d_forward.png "Conv2D forward computation flowchart")

1. Move the GM data of FeatureMap and Weight to L1 Buffer through the DataCopy API. During the FeatureMap movement, the inline conversion to the [NC1HWC0 format](../cube_compute_fractal_intro/key_fractal_format_details.md#zh-cn_topic_0000002545089965_section217615301084) is completed; the Weight is moved to L1 Buffer to obtain the Nz fractal layout. For details, see [Moving In for Matrix Computation](../cube_compute_load/cube_compute_load.md).

2. Perform img2col on the FeatureMap in L1 Buffer through the [LoadData (convolution data movement)](../cube_compute_load/LoadData_3D.md) API, complete the spatial expansion and padding based on the convolution kernel size, stride, dilation, and padding, and generate the left matrix A on L0A Buffer; move the Weight in L1 Buffer to L0B Buffer through the LoadData API to obtain the right matrix B. The attribute description of FeatureMap (l1H, l1W, padList) can be set through the [LoadData3DParamsV1](../cube_compute_load/LoadData_3D.md#zh-cn_topic_0000002512171652_table679014222918)/[LoadData3DParamsV2](../cube_compute_load/LoadData_3D.md#zh-cn_topic_0000002512171652_table193501032193419)/[LoadData3DParamsV2Pro](../cube_compute_load/LoadData_3D.md#zh-cn_topic_0000002512171652_table118027314415) structure fields, or explicitly set through [SetFmatrix](../cube_load_aux_config/SetFmatrix.md) when the isSetFMatrix field of the defaultConfig parameter of the LoadData API is false (fmatrixMode must be consistent with the fMatrixCtrl of the structure); the repeat parameter of img2col must be configured through [SetLoadDataRepeat](../cube_load_aux_config/SetLoadDataRepeat.md), and the padding value can be set through the structure field or [SetLoadDataPaddingValue](../cube_load_aux_config/SetLoadDataPaddingValue.md). For details, see [Moving In for Matrix Computation](../cube_compute_load/cube_compute_load.md).

3. Perform matrix computation on the data in L0A Buffer and L0B Buffer through the Mmad API, and output the result to L0C Buffer. For details, see [Mmad Computation](../mmad_compute/Mmad.md).

4. Process the data in L0C Buffer through the Fixpipe API and move it out to GM in the specified format. The Fixpipe API can use the Fixpipe Buffer data to perform operations such as inline quantization and Relu. The convolution result stored in L0C Buffer is in NC1HWC0 format, and Fixpipe converts it to NHWC format for output through the [NZ2ND](../cube_store_key_features/NZ2ND.md) capability.<!-- npu="950" id3 -->In particular, for the Ascend 950PR&950DT products, Fixpipe also supports converting the convolution result in L0C Buffer to NCHW format for output through the [NZ2DN](../cube_store_key_features/NZ2DN.md) capability.<!-- end id3 -->For details, see [Convolution Format Conversion](../cube_store_key_features/convolution_format_conversion.md). For more content related to moving out, see [Moving Out for Matrix Computation](../cube_compute_store/cube_compute_store.md).
