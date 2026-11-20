# Introduction to fp8 Data Types

<!-- md-trans-meta sourceCommit=4952d23d863568f8976789364b7af331909bb993 translatedAt=2026-10-26T08:41:57.785Z pushedAt=2026-10-26T11:03:37.435Z nl=2 -->

SIMT programming supports three fp8 data types: float8\_e4m3\_t, float8\_e5m2\_t, and hifloat8\_t. Their data ranges are as follows:

| Type | Sign Bit Width | Exponent Bit Width | Mantissa Bit Width | Value Range |
| --- | --- | --- | --- | --- |
| float8_e4m3_t | 1 | 4 | 3 | [$2^{6} - 2^{9}$, $2^{9} - 2^{6}$] |
| float8_e5m2_t | 1 | 5 | 2 | [$2^{13} - 2^{16}$, $2^{16} - 2^{13}$] |
| hifloat8_t | 1 | Determined by point domain encoding | Determined by point domain encoding | Point domain encoding determines data precision and value range |

A floating-point number consists of three parts: the sign bit (S), the exponent (E), and the mantissa (M). Different floating-point types may allocate different numbers of bits to these three parts.

-   float8\_e4m3\_t

    The following figure shows an example of the fp8\_e4m3fn\_t type, where the sign bit occupies 1 bit, the exponent occupies 4 bits, and the mantissa occupies 3 bits. The represented result is \(-1\)^1 × 2^-3 × 2^-6.

    **Figure 1**  float8\_e4m3\_t example diagram  
    ![](../../../figures/float8_e4m3_t_example.png "float8_e4m3_t example diagram")

-   float8\_e5m2\_t

    The following figure shows an example of the fp8\_e5m2\_t type, where the sign bit occupies 1 bit, the exponent occupies 5 bits, and the mantissa occupies 2 bits. The represented result is \(-1\)^0 × \(2 - 0.25\) × 2^\(30 -15\)=1.75 × 2^15.

    **Figure 2**  float8\_e5m2\_t example diagram  
    ![](../../../figures/float8_e5m2_t_example.png "float8_e5m2_t example diagram")

-   hifloat8\_t

    Compared with other types, the  hifloat8\_t type adds an exponent bit width control field D, which indicates the encoding scheme of the exponent bits and mantissa bits.

    The hifloat8\_t type uses different encoding schemes depending on the point domain, as listed below.

    **Figure 3**  Bit distribution of S, E, and M under different point domain D values  
    ![](../../../figures/sem_bit_distribution.png "Bit distribution of S, E, and M under different point domain D values - 65")

    In the following example, the sign bit occupies 1 bit, the exponent occupies 2 bits, and the mantissa occupies 3 bits. The D field is 2 bits with the value b01, S<sub>v</sub>=1, E<sub>v</sub>=3, and M<sub>v</sub> = 2<sup>-1</sup> + 2<sup>-2</sup>, representing the result 14. The subscript v indicates the specific value of each part.

    ![](../../../figures/hifloat8_binary.png)
