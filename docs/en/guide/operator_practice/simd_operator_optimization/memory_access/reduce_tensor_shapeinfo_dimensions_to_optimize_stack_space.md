# Optimizing Stack Space by Reducing Tensor ShapeInfo Dimensions<a name="ZH-CN_TOPIC_0000001918624089"></a>

<!-- md-trans-meta sourceCommit=422f0a57372c5cd64a25c58bfa0c6c515b11577b translatedAt=2026-08-26T14:21:55.726Z -->

[Priority] Medium

[Description] In GlobalTensor and LocalTensor, shape information is stored in a member variable of the ShapeInfo type. SetShapeInfo/GetShapeInfo can set or obtain ShapeInfo, which is used inside the operator implementation to store and pass shape information. By default, the maximum supported dimension is 8. When the ShapeInfo feature is not used, this information is not needed, and you can set it to 0 through the K\_MAX\_SHAPE\_DIM macro. Tests show that reducing the K\_MAX\_SHAPE\_DIM value can reduce stack space, decrease the number of scalar instructions and the cache miss rate, and improve operator performance.

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

[Negative Example]

The operator does not need to use ShapeInfo, but the ShapeInfo size is not limited (the default value 8 is used), wasting K\_MAX\_SHAPE\_DIM \* sizeof\(uint32\_t\) \* 2 \* 4 bytes of stack space. Here, 2 indicates that there are two arrays, shape and originalShape, and 4 indicates that four tensors (GlobalTensor and LocalTensor) are used in this example.

```
...
#include "kernel_operator.h" ...
extern "C" __global__ __aicore__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
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

The operator does not need to use ShapeInfo. Setting \#define K\_MAX\_SHAPE\_DIM 0 effectively reduces the stack space by K\_MAX\_SHAPE\_DIM \* sizeof\(uint32\_t\) \* 2 \* 4 bytes.

```
#define K_MAX_SHAPE_DIM 0
...
#include "kernel_operator.h" // Note that the K_MAX_SHAPE_DIM macro must be defined before including the Ascend C related header files.
...
extern "C" __global__ __aicore__ void add_custom(GM_ADDR x, GM_ADDR x, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
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

