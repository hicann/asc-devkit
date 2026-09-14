# NPU Architecture Version 2002<a name="ZH-CN_TOPIC_0000002507560474"></a>

<!-- md-trans-meta sourceCommit=198cbfdf29f72f78c8de735d88a5244dc258f2e7 translatedAt=2026-08-26T11:43:30.170Z pushedAt=2026-09-02T08:17:10.062Z -->

This section describes the hardware constraints and recommended solutions. The corresponding product model is Atlas inference products.

-   Constraints on using global variables

    NPU architecture version 2002 does not support Generic Addressing (for address spaces such as UB, stack, and GM). Therefore, the address spaces must match at the language level. Conversion between different address space information is not allowed and does not conform to the syntax. Global variables reside in GM, while parameters reside on the stack. An error is reported when a global variable is used by a parameter passed within a function. Currently, the compiler adapts constexpr only when the optimization level is O0: it first moves the global variable from DDR to the stack. Therefore, global variables can be defined and used with constexpr only at O0, and are not supported in other scenarios.

    -   Supported scenarios

        At O0, referencing small-scale constexpr global variables (integer and floating-point types) is supported, and retrieving array elements is supported.

        ```
        constexpr int a=1;
        int *pa=&a; 
        // array
        constexpr uint8_t padList[4] = {0, 1, 3, 5};
        __aicore__ uint64_t Compute(uint32_t padNumber){
          uint64_t regFMatrix =3;
            for (uint32_t i = 0; i < padNumber; i++) {
                regFMatrix |= uint64_t(padList[i]);    } 
         return regFMatrix;
        }
        ```

    -   Unsupported scenarios

        An error is reported for global parameters that are not defined with constexpr.

        ```
        const float aa = 3.141592653589; // const is not supported; change it to constexpr.
        template<typename T>
        __global__ __aicore__ void hello_world3()
        {   
            AscendC::printf("global var is %f\n", aa);
        };
        ```
