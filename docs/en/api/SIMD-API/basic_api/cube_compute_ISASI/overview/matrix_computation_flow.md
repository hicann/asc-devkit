# Ordinary Cube Computation Flow<a name="ZH-CN_TOPIC_0000002568950891"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T15:07:39.700Z -->

Ascend C provides four types of APIs for the cube computation programming model, which respectively carry the movement and computation capabilities of each path in the Cube core, as shown in the following figure:

**Figure 1** Ordinary cube basic computation flowchart<a name="zh-cn_topic_0000002535567224_fig135639216485"></a>  
![](../../../../figures/cube_computation_workflow.png "Ordinary cube basic computation flowchart")

1. Use the **DataCopy** API to move the GM data of the original A and B matrices into the L1 Buffer (if Bias/inline quantization exists, similarly move them into the L1 Buffer through **DataCopy**). For detailed content, refer to [Moving Cube Data into the L1 Buffer](../cube_compute_load/matrix_computation_load.md).

2. Use the **LoadData** API to load the A and B matrices to the L0A Buffer and L0B Buffer respectively to prepare for computation (if Bias/inline quantization exists, then move the bias data/quantization coefficient data in the L1 Buffer to the BT Buffer/Fixpipe Buffer through **DataCopy**). For detailed content, refer to [Moving Cube Data into the L0A Buffer/L0B Buffer](../cube_compute_load/matrix_computation_load.md).

3. Use the **Mmad** API to perform cube computation on the above data in the L0A Buffer, L0B Buffer, and BT Buffer, and output the result to the L0C Buffer. For detailed content, refer to [Mmad Computation](../mmad_compute/Mmad.md).

4. Use the **Fixpipe** API to process the data in the L0C Buffer and move it out to GM. The **Fixpipe** API can utilize the Fixpipe Buffer data to perform operations such as inline quantization and Relu. For detailed content, refer to [Fixpipe (Data Movement from L0C to GM)](../cube_compute_store/Fixpipe_L0CToGM.md).

<!-- npu="950" id1 -->
# Mx Cube Computation Flow<a name="ZH-CN_TOPIC_0000002568950892"></a>

Mx cube computation is supported only on Ascend 950PR/Ascend 950DT.

MxMmad (Microscaling Mmad) is cube multiplication with quantization coefficients, that is, both the left cube and the right cube have corresponding quantization coefficient matrices, namely the left quantization coefficient cube scaleA and the right quantization coefficient cube scaleB. In the MxMmad scenario, the left quantization coefficient cube is multiplied by the left cube, and the right quantization coefficient cube is multiplied by the right cube, and then cube multiplication is performed on the results of the two products. Ascend C provides corresponding Mx-class APIs for the Mx cube computation programming model, as shown in the following figure:

**Figure 2** Mx cube basic computation flowchart<a name="zh-cn_topic_0000002535567225_fig135639216486"></a>  
![](../../../../figures/mx_cube_computation_workflow.png "Mx cube basic computation flowchart")

1. Use the DataCopy API to move the GM data of the original matrices A and B, the [left coefficient cube scaleA](../cube_compute_fractal_intro/auxiliary_matrix_fractal_format_details.md#section_mx_scalea_fractal_format), and the [right coefficient cube scaleB](../cube_compute_fractal_intro/auxiliary_matrix_fractal_format_details.md#section_mx_scaleb_fractal_format) into the L1 Buffer (if Bias/inline quantization exists, similarly move them into the L1 Buffer through DataCopy). For detailed content, refer to [Moving Cube Data into the L1 Buffer](../cube_compute_load/matrix_computation_load.md).

2. Use the [Load2DMx API](../cube_compute_load/Load2DMX.md) to load cube A and the scaleA cube to the L0A Buffer and the built-in L0A_MX Buffer, and load cube B and the scaleB cube to the L0B Buffer and the L0B_MX Buffer respectively to prepare for computation (if Bias/inline quantization exists, then through DataCopy move the bias data/quantization coefficient data in the L1 Buffer to the BT Buffer/Fixpipe Buffer). For detailed content, refer to [Moving Cube Data into the L0A Buffer/L0B Buffer](../cube_compute_load/matrix_computation_load.md).

3. Use the MmadMx API to perform cube computation on the data in the L0A Buffer, L0A_MX Buffer, L0B Buffer, L0B_MX Buffer, and BT Buffer, and output the result to the L0C Buffer. For detailed content, refer to [MmadMx Computation](../mmad_compute/MmadMx.md).

4. Use the Fixpipe API to process the data in the L0C Buffer and move it out to GM. The Fixpipe API can utilize the Fixpipe Buffer data to perform operations such as inline quantization and Relu. For detailed content, refer to [Fixpipe (L0C to GM Data Movement)](../cube_compute_store/Fixpipe_L0CToGM.md).
<!-- end id1 -->

<!-- npu="910b,A3" id2 -->
# 4-choose-2 Sparse Cube Computation Flow<a name="ZH-CN_TOPIC_0000002538231116"></a>

A sparse cube is a special type of cube that contains many zero elements. 4-choose-2 structured sparse cube computation requires that, in a group of four consecutive weights or activation values (usually a row or column in a tensor), at most two values are non-zero, and the remaining two are forced to zero. For Atlas A2 training products/Atlas A2 inference products and Atlas A3 training products/Atlas A3 inference products, Ascend C provides corresponding sparse-class APIs for the 4-choose-2 sparse cube computation programming model, as shown in the following figure:

**Figure 3** 4-choose-2 sparse cube basic computation flowchart<a name="zh-cn_topic_0000002566526987_fig12280145134813"></a>  
![](../../../../figures/sparse_cube_computation_workflow.png "4-choose-2 sparse cube basic computation flowchart")

1. First, divide the original cube B into groups of four consecutive elements according to a fixed granularity, and generate the corresponding dense cube B and [index cube](../cube_compute_fractal_intro/auxiliary_matrix_fractal_format_details.md#zh-cn_topic_0000002563445163_section1873692415233) offline as inputs according to the fixed-structure dense algorithm.

2. Use the **DataCopy** API to move the GM data of the original cube A (sparse cube), dense cube B, and index cube into the L1 Buffer (if inline quantization exists, similarly move it into the L1 Buffer through **DataCopy**). For detailed content, refer to [Moving Cube Data into the L1 Buffer](../cube_compute_load/matrix_computation_load.md).

3. Use the **LoadData** API to load the original cube A to the L0A Buffer, and use the [**LoadDataWithSparse** API](../cube_compute_load/LoadDataWithSparse.md) to load the dense cube B and the index cube to the L0B Buffer and the built-in IDX Buffer respectively to prepare for computation (if inline quantization exists, then through **DataCopy** move the quantization coefficient data in the L1 Buffer to the Fixpipe Buffer). For detailed content, refer to [Moving Cube Data into the L0A Buffer/L0B Buffer](../cube_compute_load/matrix_computation_load.md).

4. Use the **MmadWithSparse** API to perform 4-choose-2 sparse cube computation on the above data in the L0A Buffer, L0B Buffer, and built-in IDX Buffer, and output the result to the L0C Buffer. For detailed content, refer to [MmadWithSparse Computation](../mmad_compute/MmadWithSparse.md).

5. Use the **Fixpipe** API to process the data in the L0C Buffer and move it out to GM. The **Fixpipe** API can utilize the Fixpipe Buffer data to perform operations such as inline quantization and ReLU. For detailed content, refer to [Fixpipe (L0C to GM Data Movement)](../cube_compute_store/Fixpipe_L0CToGM.md).
<!-- end id2 -->
