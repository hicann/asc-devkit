# Vector Logical Architecture<a name="ZH-CN_TOPIC_0000002573902865"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:59:13.268Z pushedAt=2026-10-28T03:14:39.608Z nl=2 -->

## Vector Logical Architecture Description

The Vector computation unit is dedicated to executing vector computations. As shown in the following figure, the highlighted part shows the Vector computation unit and its associated storage units.

<!-- npu="A3,910b" id1 -->
- For the Atlas A3 products and Atlas A2 products, the Vector logical architecture is shown in Figure 1.

  **Figure 1** Vector computation unit architecture diagram

  ![](../../../../figures/a2a3_npu_arch.png)

  **Table 1** Storage units associated with the vector computation unit

  | Storage Unit | Description | 
  | --- | --- |
  | Unified Buffer (UB) | Internal physical storage unit of the AI Core, typically used to store the input/output data of vector computations. |

<!-- end id1 -->

<!-- npu="950" id2 -->
- For the Ascend 950PR&950DT products, the Vector logical architecture is shown in Figure 2.

  **Figure 2** Vector computation unit architecture diagram

  ![](../../../../figures/npu_3510_hw_arch_vector.png)

  **Table 2** Storage units associated with the vector computation unit

  | Storage Unit | Description | 
  | --- | --- |
  | UB | Internal physical storage unit in the AI Core, typically used to store input/output data for vector computation. |
  | SIMD Register File | Internal physical storage unit in the AI Core. In SIMD programs, data is moved from the UB to registers for computation, and intermediate results can be computed directly in registers without being written back to the UB. |

<!-- end id2 -->

## UB Introduction<a name="ZH-CN_TOPIC_0000002574022839"></a>

### General Constraints of UB

- For UB address alignment constraints, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#general-address-alignment-constraints).
- For UB address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#general-address-overlap-constraints).

### UB Memory Structure and Bank Conflicts

To improve data access efficiency and throughput, the UB adopts a structure of equal-sized memory modules (banks). When multiple read/write instructions access the UB simultaneously, hardware resource limitations prevent these instructions from executing at the same time, causing bank conflicts. In this case, the instructions must wait in a queue for resources and cannot complete within a single instruction cycle.

<!-- npu="A3,910b" id3 -->
- For Atlas A3 products and Atlas A2 products
  - UB memory structure

    **Figure 3** UB memory structure

    ![](../../../../figures/a2a3_ub_memory_structure.png)

    As shown in Figure 3, the UB has a total size of 192 KB and contains 16 bank groups (BG0 to BG15), each of which contains 3 banks. Each bank is 4 KB in size and consists of 128 rows, with each row being 32 B in length.
    - **Read/write conflict**: A read operation and a write operation simultaneously attempt to access the same bank.
    - **Write/write conflict**: Multiple write operations simultaneously attempt to access the same bank group.
    - **Read/read conflict**: Multiple read operations simultaneously attempt to access the same bank group.

  - bank conflict optimization

    See [Avoiding Bank Conflicts (NPU Architecture Version 2201)](../../../../../guide/operator_practice/simd_operator_optimization/memory_access/avoid_ub_bank_conflict/avoid_bank_conflict_npu_arch_2201.md).
<!-- end id3 -->

<!-- npu="950" id4 -->
- For Ascend 950PR&950DT products
  - UB memory structure

    **Figure 4** UB memory structure

    ![](../../../../figures/950_ub_memory_structure.png)

    As shown in Figure 4, the UB has a total size of 256 KB and contains eight bank groups (BG0 to BG7), each of which contains two banks. Each bank is 16 KB in size and consists of 512 rows, with each row being 32 B in length.
    - **Read-write conflict**: A read operation and a write operation simultaneously attempt to access the same bank.
    - **Write-write conflict**: Multiple write operations simultaneously attempt to access the same bank group.
    - **Read-read conflict**: Two read operations simultaneously attempt to access the same bank, or more than two read operations simultaneously attempt to access the same bank group.
  
  - bank conflict optimization

    See [avoiding bank conflicts (NPU architecture version 3510)](../../../../../guide/operator_practice/simd_operator_optimization/memory_access/avoid_ub_bank_conflict/avoid_bank_conflict_npu_arch_3510.md).
<!-- end id4 -->
