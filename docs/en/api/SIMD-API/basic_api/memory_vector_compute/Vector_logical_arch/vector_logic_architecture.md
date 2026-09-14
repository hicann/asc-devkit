# Vector Logical Architecture<a name="ZH-CN_TOPIC_0000002573902865"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-09-09T15:36:00.443Z -->

## Vector Logical Architecture Description

The Vector computation unit is dedicated to vector computation. As shown in the following figure, the highlighted part shows the Vector computation unit and its associated storage units.

<!-- npu="A3,910b" id1 -->
- For Atlas A3 Training Series Products/Atlas A3 Inference Series Products and Atlas A2 Training Series Products/Atlas A2 Inference Series Products, the Vector logical architecture is shown in Figure 1.

  **Figure 1** Vector computation unit architecture

  ![](../../../../figures/a2a3_npu_arch.png)

  **Table 1** Storage units associated with the Vector computation unit

  | Storage Unit | Description | 
  | --- | --- |
  | Unified Buffer | Internal physical storage unit of the AI Core, typically used to store the input/output data of vector computation. |
<!-- end id1 -->

<!-- npu="950" id2 -->
- For Ascend 950PR/Ascend 950DT, the Vector logical architecture is shown in Figure 2.

  **Figure 2** Vector computation unit architecture

  ![](../../../../figures/950_npu_arch.png)

  **Table 2** Storage units associated with the vector computation unit

  | Storage Unit | Description | 
  | --- | --- |
  | Unified Buffer | An internal physical storage unit of the AI Core, typically used to store the input/output data of vector computation. |
  | SIMD Register File | An internal physical storage unit of the AI Core. In a SIMD program, data is moved from the Unified Buffer to the registers for computation, and the intermediate results can be computed directly in the registers without being transferred back to the Unified Buffer. |
<!-- end id2 -->

## Unified Buffer Introduction<a name="ZH-CN_TOPIC_0000002574022839"></a>

### General Constraints on Unified Buffer

- For Unified Buffer address alignment constraints, see [Universal Address Alignment Constraints](../../../general_description_and_constraints.md#general-address-alignment-constraints).
- For Unified Buffer address overlap constraints, see [Universal Address Overlap Constraints](../../../general_description_and_constraints.md#general-address-overlap-constraints).

### Unified Buffer Memory Structure and Bank Conflicts

To improve the efficiency and throughput of data access, the Unified Buffer adopts a structure of equal-sized memory modules (banks). When multiple read/write instructions access the Unified Buffer concurrently, these instructions cannot be executed simultaneously due to hardware resource limitations, resulting in bank conflicts. In this case, the instructions must wait in a queue for resources and cannot be completed within a single instruction cycle.

<!-- npu="A3,910b" id3 -->
- For Atlas A3 Training Series Products/Atlas A3 Inference Series Products, Atlas A2 Training Series Products/Atlas A2 Inference Series Products
  - Unified Buffer Memory Structure

    **Figure 3** Unified Buffer memory structure diagram

    ![](../../../../figures/a2a3_UB_memory_structure.png)

    As shown in Figure 3, the UB has a total size of 192 KB and contains 16 bank groups (BG0 to BG15), each of which contains 3 banks. Each bank is 4 KB in size and consists of 128 rows, with each row being 32 B in length.
    - **Read/write conflict**: A read operation and a write operation attempt to access the same bank concurrently.
    - **Write/write conflict**: Multiple write operations attempt to access the same bank group concurrently.
    - **Read/read conflict**: Multiple read operations attempt to access the same bank group concurrently.

  - Bank conflict optimization

    For details, see [Avoiding Bank Conflicts (NPU Architecture Version 2201)](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/guide/operator_practice/simd_operator_optimization/memory_access/avoid_ub_bank_conflict/avoid_bank_conflict_npu_arch_2201.md).
<!-- end id3 -->

<!-- npu="950" id4 -->
- For Ascend 950PR/Ascend 950DT
  - Memory structure of the Unified Buffer

    **Figure 4** Memory structure of the Unified Buffer

    ![](../../../../figures/950_ub_memory_structure.png)

    As shown in Figure 4, the UB has a total size of 256 KB and contains eight bank groups (BG0 to BG7), each of which contains two banks. Each bank is 16 KB in size and consists of 512 rows, with each row being 32 B in length.
    - **Read-write conflict**: A read operation and a write operation attempt to access the same bank concurrently.
    - **Write-write conflict**: Multiple write operations attempt to access the same bank group concurrently.
    - **Read-read conflict**: Two read operations attempt to access the same bank concurrently, or more than two read operations attempt to access the same bank group concurrently.

  - Bank conflict optimization

    For details, see [Avoiding Bank Conflicts (NPU Architecture Version 3510)](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/guide/operator_practice/simd_operator_optimization/memory_access/avoid_ub_bank_conflict/avoid_bank_conflict_npu_arch_3510.md).
<!-- end id4 -->
