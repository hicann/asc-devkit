# NPU架构版本2002<a name="ZH-CN_TOPIC_0000002507560474"></a>

本节介绍硬件约束以及解决方案建议。对应的产品型号为：Atlas 推理系列产品。

-   全局变量使用约束

    NPU架构版本2002不支持Generic Addressing通用寻址（针对UB、stack、GM等地址空间），因此语言层面地址空间必须匹配。不同的地址空间信息不允许转换，不符合语法。全局变量位于GM上，传参位于stack上，函数内传参使用全局变量时会报错。目前编译器仅在优化级别为O0的场景下，对constexpr做了适配处理：将全局变量先从DDR上放到stack上。所以全局变量仅支持在O0下使用constexpr进行定义和使用，在其他场景均不支持。

    -   支持场景

        在O0下支持constexpr全局变量小规模类型（整型、浮点型）数据的引用，支持数组取元素。

        ```
        constexpr int a=1；
        int *pa=&a; 
        // 数组
        constexpr uint8_t padList[4] = {0, 1, 3, 5};
        __aicore__ uint64_t Compute(uint32_t padNumber){
          uint64_t regFMatrix =3;
            for (uint32_t i = 0; i < padNumber; i++) {
                regFMatrix |= uint64_t(padList[i]);    } 
         return regFMatrix;
        }
        ```

    -   不支持场景

        对于使用constexpr之外的全局参数将报错。

        ```
        const float aa = 3.141592653589; // 不支持const应修改为constexpr
        template<typename T>
        __global__ __aicore__ void hello_world3()
        {   
            AscendC::printf("global var is %f\n", aa);
        };
        ```
