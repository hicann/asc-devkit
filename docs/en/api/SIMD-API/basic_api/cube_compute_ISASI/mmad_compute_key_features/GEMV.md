# GEMV<a name="ZH-CN_TOPIC_0000002538231187"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T13:29:56.923Z pushedAt=2026-10-26T07:41:44.197Z nl=2 -->

**Description:**

The core function of GEMV is as follows: when the dimension M of matrix A is 1, the API automatically enables the GEMV function, and the operation degenerates into a multiplication between a 1×K row vector and a K×N matrix.

<!-- npu="950" id1 -->
For Ascend 950PR&950DT products, you can disable this function by setting the **disableGemv** parameter of **MmadParams** to **true**.
<!-- end id1 -->

**Constraints:**

1. The 1×K matrix A must be 512B address-aligned, and the K data elements are stored contiguously. Taking the half type as an example, when K=256, the software side can regard it as a 16\*16 block. After configuring m=1, the hardware parses it as a 1\*256 vector, and the 256 half data elements can be moved from the L1 Buffer to the L0A Buffer through the **LoadData** API.

2. Matrix C is a (1\*N) vector. When a 1×N vector is divided into multiple 1×16 sub-vectors, each sub-vector occupies 1024B in the L0C Buffer, but the actual valid data occupies only the lowest 64B. For example, when N=50, it is divided into 4 1\*16 sub-vectors, occupying 4\*1024B = 4096B, of which the valid data is only 4\*64B = 256B.

**Figure 1** GEMV mode, matrix multiplication illustration

![GEMV mode - matrix multiplication illustration](../../../../figures/mmad_gemv.png "GEMV mode - matrix multiplication illustration")

**Advantages:**

When M=1, the GEMV mode is automatically enabled, and the matrix multiplication processes the M direction as a non-aligned scenario. Compared with the non-aligned processing method, the GEMV mode transfers less data and delivers better performance. The following uses a matrix multiplication example with M=1, K=256, N=32, and the data type of both the left and right matrices being half for illustration.

- GEMV mode

    When transferring matrix A from the L1 Buffer (**TPosition:A1**) to the L0A Buffer (**TPosition:A2**), the 1\*256 vector is processed as a 16\*16 matrix, and the **LoadData** API is called once to complete the transfer of a matrix of 16\*16 fractal size. The transfer of matrix B and the matrix multiplication computation are the same as in the basic scenario, as shown in the following figure.

    **Figure 2** Matrix multiplication computation illustration for GEMV mode with M=1

    ![Matrix multiplication computation illustration for GEMV mode with M=1](../../../../figures/mmad_gemv_compare.png "Matrix multiplication computation illustration for GEMV mode with M=1")

- Non-GEMV mode

    When transferring matrix A from the L1 Buffer (**TPosition:A1**) to the L0A Buffer (**TPosition:A2**), the 1\*256 vector is processed as non-aligned matrix data, and the M direction is aligned to 32 bytes before transferring. The **LoadData** API is called to transfer a matrix of 16\*16 fractal size each time, for a total of K/16 = 16 transfers, which increases the amount of data transferred and results in worse performance than the GEMV mode, as shown in the following figure.

    **Figure 3** Matrix multiplication computation illustration for non-GEMV mode with M=1

    ![Matrix multiplication computation illustration for non-GEMV mode with M=1](../../../../figures/mmad_nongemv_compare.png "Matrix multiplication computation illustration for non-GEMV mode with M=1")

**Example:**

**Table 1** Example configuration description

| Matrix | Dimension Size | Data Type |
| --- | --- | --- |
| A | 1 * 4096 | half |
| B | 4096 * 256 | half |
| C | 1 * 256 | float |

Matrix A is a vector of 1\* 4096, which can be regarded from the software side as 64\*64 data (NZ fractal for Ascend 950PR&950DT products), and is moved to the L0A Buffer through LoadData;

Matrix C is a 1 * 256 vector, which can be divided into 16 sub-vectors of size 1 * 16, occupying 16 * 1024B = 16384B (f162f32), where the valid data is only 16 * 64B = 1024B.

**Figure 4** GEMV mode matrix multiplication illustration

![GEMV mode matrix multiplication illustration demo](../../../../figures/mmad_gemv_demo.png "GEMV mode matrix multiplication illustration demo")

For a complete example of the GEMV function, see [GEMV sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/mmad_gemv).

```cpp
AscendC::MmadParams mmadParams;
// Set m = 1 to automatically enable the GEMV capability.
mmadParams.m = 1;
mmadParams.n = n;
mmadParams.k = k;
AscendC::Mmad(c, a, b, mmadParams);
```

<!-- npu="950" id2 -->
For Ascend 950PR&950DT products, you can disable this function by setting the **disableGemv** parameter of **MmadParams** to **true**, as shown in the following example:

```cpp
AscendC::MmadParams mmadParams;
// Set m = 1 to automatically enable the GEMV capability.
mmadParams.m = 1;
mmadParams.n = n;
mmadParams.k = k;
// Disable GEMV.
mmadParams.disableGemv = true;
AscendC::Mmad(c, a, b, mmadParams);
```
<!-- end id2 -->
