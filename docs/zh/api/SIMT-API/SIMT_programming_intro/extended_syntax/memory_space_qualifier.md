# 地址空间限定符

在SIMT编程中使用地址空间限定符`__ubuf__`来表示动、静态内存，静态内存的大小在编译期是确定的，动态内存的大小在核函数（Kernel）启动时确定。

-   静态内存通过数组分配：

    ```cpp
    __ubuf__ half staticBuf[1024];
    ```

-   动态内存通过以下方式申请使用：

    ```cpp
    extern __ubuf__ half dynamicBuf[];
    ```

    动态内存的实际内存大小需要在核函数（Kernel）启动时配置。
