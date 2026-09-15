# GEMV<a name="ZH-CN_TOPIC_0000002538231187"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T17:02:18.809Z -->

**Description:**

The core function of GEMV is as follows: when the dimension M of cube A is 1, the API automatically enables the GEMV function, and the operation degenerates into a multiplication between a 1×K row vector and a K×N cube.

<!-- npu="950" id1 -->
For Ascend 950PR/Ascend 950DT, this function can be disabled by setting the **disableGemv** parameter of **MmadParams** to **true**.
<!-- end id1 -->

**Constraints:**

1. The 1×K cube A must satisfy 512B address alignment, with K data elements stored contiguously. Taking the half type as an example, when K=256, the software side can regard it as a 16\*16 block. After m=1 is configured, the hardware parses it as a 1\*256 vector, and 256 half data elements can be moved from the L1 Buffer to the L0A Buffer through the **LoadData** API.

2. The C cube is a (1\*N) vector. When the 1×N vector is divided into multiple 1×16 sub-vectors, each sub-vector occupies 1024B (u8/s8/f162f32) in the L0C Buffer. However, the actual valid data occupies only the lowest 32B or 64B. For example, when N=50, the vector is divided into four 1\*16 sub-vectors, occupying 4\*512B = 2048B (f162f16), of which the valid data is only 4\*32B = 128B.

**Figure 1** GEMV mode, cube multiplication illustration

![GEMV mode - cube multiplication illustration](../../../../figures/mmad_gemv.png "GEMV mode - cube multiplication illustration")

**Advantages:**

When M=1, the GEMV mode is automatically enabled, and the cube multiplication processes the M dimension as a non-aligned scenario. Compared with the non-aligned processing method, the GEMV mode moves less data and delivers better performance. The following uses a cube multiplication example with M=1, K=256, N=32, and the left and right cube data type of half for illustration.

- GEMV mode

    When cube A is moved from the L1 Buffer (TPosition:A1) to the L0A Buffer (TPosition:A2), the 1\*256 vector is processed as a 16\*16 cube, and the **LoadData** API is called once to complete the cube move of a 16\*16 fractal size. The move of cube B and the cube multiplication computation are the same as those in the basic scenario, as shown in the following figure.

    **Figure 2** Cube multiplication computation illustration of the GEMV mode with M=1

    ![Cube multiplication computation illustration of the GEMV mode with M=1](../../../../figures/mmad_gemv_compare.png "Cube multiplication computation illustration of the GEMV mode with M=1")

- Non-GEMV mode

    When cube A is moved from the L1 Buffer (TPosition:A1) to the L0A Buffer (TPosition:A2), the 1\*256 vector is processed as non-aligned cube data, and the M dimension is aligned to 32 bytes before the move. The **LoadData** API is called to move a cube of a 16\*16 fractal size each time, for a total of K/16=16 times, which increases the amount of moved data and results in poorer performance than the GEMV mode, as shown in the following figure.

    **Figure 3** Cube multiplication computation illustration of the non-GEMV mode with M=1

    ![Cube multiplication computation illustration of the non-GEMV mode with M=1](../../../../figures/mmad_nongemv_compare.png "Cube multiplication computation illustration of the non-GEMV mode with M=1")

**Example:**

**Table 1** Example configuration description

| Cube | Dimension Size | Data Type |
| --- | --- | --- |
| A | 1 * 4096 | half |
| B | 4096 * 256 | half |
| C | 1 * 256 | float |

Cube A is a 1\* 4096 vector, which can be regarded as 64\*64 data (NZ fractal for Ascend 950PR/Ascend 950DT products) from the software side, and is moved to the L0A Buffer through **LoadData**.

Cube C is a 1\* 256 vector, which can be divided into 16 sub-vectors of 1 \* 16, occupying 16 \* 1024B = 16384B (f162f32), of which the valid data is only 16 \* 64B = 1024B.

**Figure 4** GEMV mode cube multiplication illustration

![GEMV mode cube multiplication illustration demo](../../../../figures/mmad_gemv_demo.png "GEMV mode cube multiplication illustration demo")

For a complete example of the GEMV feature, see [GEMV sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_cube_compute/mmad_gemv).

```cpp
AscendC::MmadParams mmadParams;
// Set m = 1 to automatically enable the GEMV capability.
mmadParams.m = 1;
mmadParams.n = n;
mmadParams.k = k;
AscendC::Mmad(c, a, b, mmadParams);
```

<!-- npu="950" id2 -->
For Ascend 950PR/Ascend 950DT products, this feature can be disabled by setting the **disableGemv** parameter of **MmadParams** to **true**, as shown in the following example:

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
