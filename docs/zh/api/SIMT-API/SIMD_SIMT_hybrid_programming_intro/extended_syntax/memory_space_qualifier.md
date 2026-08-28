# 地址空间限定符

## \_\_gm\_\_

在SIMD与SIMT混合编程中使用地址空间限定符`__gm__`来表示分配于设备侧全局内存的对象，全局内存对象可以声明为指向标量或用户自定义结构体的指针。

```cpp
__gm__ int *var; // var points to an array of int elements

typedef struct {
    float a[3];
    int b[2];
} foo_t;

__gm__ foo_t *info; // info points to an array of foo_t elements
```

## \_\_ubuf\_\_

在SIMD与SIMT混合编程中使用地址空间限定符`__ubuf__`来表示动、静态内存，静态内存的大小在编译期是确定的，动态内存的大小在核函数（Kernel）执行时确定。

-   静态内存通过数组分配：

    ```cpp
    __ubuf__ half staticBuf[1024];
    ```

-   动态内存通过以下方式申请使用：

    ```cpp
    extern __ubuf__ half dynamicBuf[];
    ```

    动态内存的实际内存大小需要在核函数（Kernel）启动时配置。
