# Optimizing Stack Space by Reducing Tensor ShapeInfo Dimensions<a name="ZH-CN_TOPIC_0000001918624089"></a>

<!-- md-trans-meta sourceCommit=69a6cac29cbde3698f408c141a0f6188e0b2b8a0 translatedAt=2026-10-23T12:41:43.680Z pushedAt=2026-11-03T07:05:13.336Z nl=2 -->

[Priority] Medium

[Description] GlobalTensor and LocalTensor store shape information through a member variable of the ShapeInfo type. **SetShapeInfo**/**GetShapeInfo** can set or obtain **ShapeInfo**, which is used to store and pass shape information inside operator implementations. By default, the maximum supported dimension is 8. If the **ShapeInfo** feature is not used, this information is not needed, and you can set it to 0 through the **K\_MAX\_SHAPE\_DIM** macro. Tests show that reducing the **K\_MAX\_SHAPE\_DIM** value can reduce stack space, decrease scalar instructions and cache miss probability, and improve operator performance.

```
...
#ifndef K_MAX_SHAPE_DIM
#define K_MAX_SHAPE_DIM 8
#endif
...
struct ShapeInfo {
public:
    ...
    uint32_t shape[K_MAX_SHAPE_DIM];
    uint32_t originalShape[K_MAX_SHAPE_DIM];
};

template <typename T> class GlobalTensor {
....
private:
    ShapeInfo shapeInfo_;
}
template <typename T> class LocalTensor {
....
private:
    ShapeInfo shapeInfo_;
}
...
```

[Negative example]

The operator does not need to use **ShapeInfo**, but the **ShapeInfo** size is not limited (the default value **8** is used), wasting K\_MAX\_SHAPE\_DIM \* sizeof\(uint32\_t\) \* 2 \* 4 bytes of stack space. 2 indicates that there are two arrays, shape and originalShape, and 4 indicates that four tensors, GlobalTensor and LocalTensor, are used in this example.

```
...
#include "kernel_operator.h" ...
extern "C" __global__ __aicore__ void add_custom(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z, __gm__ uint8_t* workspace, __gm__ uint8_t* tiling)
{
    ...
    GlobalTensor<T> dataIn;
    GlobalTensor<T> dataOut;
    LocalTensor<T> vecIn;
    LocalTensor<T> vecOut;
    ...
}
...
```

[Positive Example]

The operator does not need to use **ShapeInfo**. Setting **\#define K\_MAX\_SHAPE\_DIM 0** effectively reduces the stack space of K\_MAX\_SHAPE\_DIM \* sizeof\(uint32\_t\) \* 2 \* 4.

```
#define K_MAX_SHAPE_DIM 0
...
#include "kernel_operator.h" //Note that the K_MAX_SHAPE_DIM macro must be defined before including Ascend C related header files
...
extern "C" __global__ __aicore__ void add_custom(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z, __gm__ uint8_t* workspace, __gm__ uint8_t* tiling)
{
    ...
    GlobalTensor<T> dataIn;
    GlobalTensor<T> dataOut;
    LocalTensor<T> vecIn;
    LocalTensor<T> vecOut;
    ...
}
...
```
