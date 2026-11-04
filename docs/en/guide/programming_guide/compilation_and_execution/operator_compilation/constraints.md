# Constraints<a name="ZH-CN_TOPIC_0000002507564342"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T15:06:10.331Z pushedAt=2026-11-03T07:05:13.458Z nl=2 -->

- In the same compilation unit, if multiple kernels exist, automatic inference of the kernel type is not supported. You need to manually set the kernel type.
    - In particular, for the following models, automatic inference is not supported when the kernel type is not set by the developer, regardless of whether multiple kernels exist in the same compilation unit. You are advised to manually set the kernel type.
        <!-- npu="950" id1 -->
        - Ascend 950PR&950DT products
        <!-- end id1 -->
        <!-- npu="310p" id2 -->
        - Atlas inference products
        <!-- end id2 -->

- The kernel is marked with \_\_cube\_\_ / \_\_vector\_\_ / \_\_mix\_\_(cube, vec).

- Automatic inference cannot be implemented for scalar-only operators.
    You need to manually mark the kernel type. You are advised to set it to the vector-only type and add the \_\_vector\_\_ attribute for marking:

    ```cpp
    __global__ __vector__ __aicore__ void func0(__gm__ uint8_t* Addr) {
        Addr[1] = Addr[0];
        AscendC::printf("Hello world");
    }
    ```

- Data types such as bfloat16\_t can be defined and declared on the host side only in the form of C++ templates. The specific data types are as follows:

    <!-- npu="950" id3 -->
    **Ascend 950PR & 950DT products**: bfloat16\_t, hifloat8\_t, fp8\_e5m2\_t, fp8\_e4m3fn\_t, fp8\_e8m0\_t, fp4x2\_e2m1\_t, fp4x2\_e1m2\_t, int4x2\_t.
    <!-- end id3 -->

    <!-- npu="910b" id4 -->
    **Atlas A2 products**: bfloat16\_t.
    <!-- end id4 -->

    <!-- npu="A3" id5 -->
    **Atlas A3 products**: bfloat16\_t.
    <!-- end id5 -->

- Defining and using device-side string literals outside the function scope is not supported. For example:

    ```cpp
    // Global space
    const __gm__ char* g_str = "g_string"; // Not supported

    __aicore__ void func() // Device-side function
    {
        const __gm__ char* str = "device_string"; // Supported
    }
    ```
