# Introduction to FP8

<!-- md-trans-meta sourceCommit=7a7156352f594b5ca79b533f72c5a99f19b87983 translatedAt=2026-09-01T10:51:17.688Z pushedAt=2026-09-03T07:28:07.253Z -->

SIMT programming supports three FP8 data types: float8\_e4m3\_t, float8\_e5m2\_t, and hifloat8\_t. Their value ranges are as follows:

| Type | Sign Bit Width | Exponent Bit Width | Mantissa Bit Width | Value Range |
| --- | --- | --- | --- | --- |
| float8_e4m3_t | 1 | 4 | 3 | [$2^{6} - 2^{9}$, $2^{9} - 2^{6}$] |
| float8_e5m2_t | 1 | 5 | 2 | [$2^{13} - 2^{16}$, $2^{16} - 2^{13}$] |
| hifloat8_t | 1 | Determined by the point domain encoding | Determined by the point domain encoding | The point domain encoding determines the data precision and value range |

A floating-point number consists of three parts: the sign bit (S), the exponent (E), and the mantissa (M). For different floating-point types, the number of bits occupied by these three parts may differ.

-   float8\_e4m3\_t

    The following figure shows an example of the fp8\_e4m3fn\_t type, in which the sign bit occupies 1 bit, the exponent occupies 4 bits, and the mantissa occupies 3 bits. The represented result is \(-1\)^1 × 2^-3 × 2^-6.

    **Figure 1**  Diagram of float8\_e4m3\_t  
    ![](../../../figures/float8_e4m3_t_diagram.png "Diagram of float8_e4m3_t")

-   float8\_e5m2\_t

    The following figure shows an example of the fp8\_e5m2\_t type, in which the sign bit occupies 1 bit, the exponent occupies 5 bits, and the mantissa occupies 2 bits. The represented result is \(-1\)^0 × \(2 - 0.25\) × 2^\(30 -15\)=1.75 × 2^15.

    **Figure 2**  Diagram of float8\_e5m2\_t  
    ![](../../../figures/float8_e5m2_t_diagram.png "Diagram of float8_e5m2_t")

-   **hifloat8_t**

    Compared with other types, the **hifloat8_t** type adds an exponent bit width control field **D**, which is used to indicate the encoding scheme of the exponent bits and mantissa bits.

    The **hifloat8_t** type has different encoding schemes depending on the point domain, which are listed below.

    **Figure 3**  Bit distribution of S-E-M under different point field D values  
    ![](../../../figures/S_E_M_bit_distribution_under_different_point_field_D_values_65.png "Bit distribution of S-E-M under different point field D values- 65")

    In the following example, the sign bit occupies 1 bit, the exponent occupies 2 bits, and the mantissa occupies 3 bits. The **D** field is 2 bits, b01, with S<sub>v</sub>=1, E<sub>v</sub>=3, and M<sub>v</sub> = 2<sup>-1</sup> + 2<sup>-2</sup>, representing the result 14. The subscript v indicates the specific value of each part.

    ![](../../../figures/pipeline_task_runtime_diagram_66.png)
