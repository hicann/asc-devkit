# Constraints<a name="ZH-CN_TOPIC_0000002507564342"></a>

<!-- md-trans-meta sourceCommit=198cbfdf29f72f78c8de735d88a5244dc258f2e7 translatedAt=2026-08-26T10:51:26.676Z pushedAt=2026-08-31T02:36:57.721Z -->

-   In the same compilation unit, if multiple kernel functions exist, automatic inference of the kernel type is not supported. You need to manually set the kernel type.
    -   In particular, for the following models, automatic inference is not supported when the kernel type is not set by the developer, regardless of whether multiple kernel functions exist in the same compilation unit. You are advised to manually set the kernel type.
        -   Ascend 950PR/Ascend 950DT
        -   Atlas inference products
</br>

-   You are advised to mark the kernel function with \_\_cube\_\_ / \_\_vector\_\_ / \_\_mix\_\_(cube, vec). The KERNEL\_TASK\_TYPE\_DEFAULT API will be deprecated soon.
</br>

-   Automatic inference cannot be implemented for scalar-only operators.
    You need to manually mark the kernel function type. You are advised to set it to the vector-only type and add the \_\_vector\_\_ attribute for marking:

    ```cpp
    __global__ __vector__ __aicore__ void func0(__gm__ uint8_t* Addr) {
        Addr[1] = Addr[0];
        AscendC::printf("Hello world");
    }
    ```
</br>

-   Data types such as bfloat16\_t can be defined and declared on the host side only in the form of C++ templates. The specific data types are as follows:

    **Ascend 950PR/Ascend 950DT**: bfloat16\_t, hifloat8\_t, fp8\_e5m2\_t, fp8\_e4m3fn\_t, fp8\_e8m0\_t, fp4x2\_e2m1\_t, fp4x2\_e1m2\_t, int4x2\_t.

    **Atlas A2 training products/Atlas A2 inference products**: bfloat16\_t.

    **Atlas A3 training products/Atlas A3 inference products**: bfloat16\_t.

-   Defining and using device-side string literals outside the function scope is not supported. For example:

    ```cpp
    // global space
    const __gm__ char* g_str = "g_string"; // not supported

    __aicore__ void func() // device-side function
    {
        const __gm__ char* str = "device_string"; // supported
    }
    ```
