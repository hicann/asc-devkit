# tuple

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T12:32:12.320Z pushedAt=2026-09-11T04:54:56.516Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

In C++, **tuple** is a powerful container that allows storing multiple elements of different types. Its specific usage scenarios are as follows:

-   When a function needs to return multiple values of different types, using **tuple** is a good choice. It avoids creating a structure or class to encapsulate multiple return values, especially when there is no need to create a separate type for these return values.
-   When data of different types needs to be stored, **tuple** can be used to store heterogeneous elements. For example, storing query results that contain fields of different types.
-   In function calls, when multiple parameters of different types need to be passed but you do not want to create a structure for them, you can use **tuple** to group these parameters.
-   In template metaprogramming, **tuple** can serve as a type list to store multiple type information.
-   When a program needs to store and pass multiple state information at different stages, you can use **tuple** to store these states uniformly for easier management.
-   In some generic algorithms, you can use **tuple** to store computation results or intermediate results.
-   In template metaprogramming, you can use **tuple** to store and expand parameter packs.

The following describes the constructors of **tuple**:

-   Default constructor: Creates an empty tuple.
-   Initializer-list constructor: Provides element values directly when creating an object.
-   Copy constructor: Copies an existing tuple.

In addition, Ascend C provides the helper function [make\_tuple](make_tuple.md) for creating tuples. It can automatically deduce the element types, making the code more concise. You can also use make\_tuple to construct an element list.

## Prototype

```
template <typename Tp, typename ...Tps>
class tuple<Tp, Tps...> : public tuple<Tps...>
{
public:
    __aicore__ inline tuple();

    __aicore__ inline tuple(const Tp& val, const Tps& ...params);

    __aicore__ inline tuple(Tp&& val, Tps&& ...params);

    template <typename Head, typename ...Args>
    __aicore__ inline tuple<Tp, Tps...>& operator=(const tuple<Head, Args...>& t);
}
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| Tps... | A parameter pack representing the input types, with the number of parameters ranging from [0, 64].<br><br>Ascend 950PR/Ascend 950DT supports the following data types: bool, int4b_t, int8_t, uint8_t, fp8_e8m0_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, LocalTensor, GlobalTensor.<br><br>Atlas A3 training products/Atlas A3 inference products support the following data types: bool, int4b_t, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, LocalTensor, GlobalTensor.<br><br>Atlas A2 training products/Atlas A2 inference products support the following data types: bool, int4b_t, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, LocalTensor, GlobalTensor. |

## Constraints

-   The instantiation depth of **tuple** is 64, which means it supports aggregation of data types with up to 64 elements.
-   When the constructor uses an initializer list, the data in the list must be all lvalues or all rvalues and cannot be mixed. The number and types of elements in the initializer list must strictly match the number and types of elements defined in the tuple.
-   Variable-length data types such as arrays are not supported.
-   Implicit conversion constructors are not supported.
-   Because the current implementation of tuple is based on inheritance, a notation such as tuple<int, int\> a =make\_tuple\(1, 2, 3\) triggers implicit conversion. In addition, due to the C++ object slicing feature, the excess data is discarded. Therefore, avoid using the notation above.

## Return Value

None

## Example

```
AscendC::LocalTensor<T> src0Local = inQueueX.AllocTensor<T>();
AscendC::LocalTensor<T> src1Local = inQueueX2.AllocTensor<T>();

// Aggregate Tensor class structures with make_tuple.
auto testMakeTensor = AscendC::Std::make_tuple(src0Local, src1Local, src0_global, src1_global);

AscendC::PRINTF("tuple size is --> %d\n", AscendC::Std::tuple_size<decltype(testMakeTensor)>::value);

// Aggregate via initializer list construction.
AscendC::Std::tuple<AscendC::LocalTensor<T>, AscendC::LocalTensor<T>, AscendC::GlobalTensor<T>, AscendC::GlobalTensor<T>> testTensor{src0Local, src1Local, src0_global, src1_global};

// Copy construction.
AscendC::Std::tuple<AscendC::LocalTensor<T>, AscendC::LocalTensor<T>, AscendC::GlobalTensor<T>, AscendC::GlobalTensor<T>> test2Tensor = testTensor;

AscendC::Std::tuple<AscendC::LocalTensor<T>, AscendC::LocalTensor<T>, AscendC::GlobalTensor<T>, AscendC::GlobalTensor<T>> test3Tensor;

// Operator overloading.
test3Tensor = test2Tensor;

AscendC::PRINTF("tuple size is --> %d\n", AscendC::Std::tuple_size<decltype(test3Tensor)>::value);

// Obtain the corresponding element with the get method.
AscendC::LocalTensor<T> src0LocalTuple = AscendC::Std::get<0>(test3Tensor);
AscendC::LocalTensor<T> src1LocalTuple = AscendC::Std::get<1>(test3Tensor);

AscendC::GlobalTensor<T> src0_globalTuple = AscendC::Std::get<2>(test3Tensor);
AscendC::GlobalTensor<T> src1_globalTuple = AscendC::Std::get<3>(test3Tensor);

// Aggregate multiple data types via initializer list.
AscendC::Std::tuple<AscendC::int4b_t, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, uint64_t, int64_t, half, float, \
bfloat16_t, fp8_e8m0_t, bool> test1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10.0, 11.0, 12.0, 13.0, true};

AscendC::PRINTF("tuple size is --> %d\n", AscendC::Std::tuple_size<decltype(test1)>::value);

// Obtain elements of multiple data types with the get method.
AscendC::int4b_t        number_int4b_t =        AscendC::Std::get<0>(test1);
int8_t                  number_int8_t =         AscendC::Std::get<1>(test1);
uint8_t                 number_uint8_t =        AscendC::Std::get<2>(test1);
int16_t                 number_int16_t =        AscendC::Std::get<3>(test1);
uint16_t                number_uint16_t =       AscendC::Std::get<4>(test1);
int32_t                 number_int32_t =        AscendC::Std::get<5>(test1);
uint32_t                number_uint32_t =       AscendC::Std::get<6>(test1);
uint64_t                number_uint64_t =       AscendC::Std::get<7>(test1);
int64_t                 number_int64_t =        AscendC::Std::get<8>(test1);
half                    number_half =           AscendC::Std::get<9>(test1);
float                   number_float =          AscendC::Std::get<10>(test1);
bfloat16_t              number_bfloat16_t =     AscendC::Std::get<11>(test1);
fp8_e8m0_t              number_fp8_e8m0_t =     AscendC::Std::get<12>(test1);
bool                    number_bool =           AscendC::Std::get<13>(test1);

// Obtain element references with the get method for subsequent operations.
AscendC::Std::get<1>(test1)+= 1 ;
AscendC::Std::get<2>(test1)+= 1 ;
AscendC::Std::get<3>(test1)+= 1 ;
AscendC::Std::get<4>(test1)+= 1 ;
AscendC::Std::get<5>(test1)+= 1 ;
AscendC::Std::get<6>(test1)+= 1 ;
AscendC::Std::get<7>(test1)+= 1 ;
AscendC::Std::get<8>(test1)+= 1 ;
AscendC::Std::get<10>(test1) += (float)1.0 ;

// Fix element data types in the make_tuple initializer list.
auto test2 = AscendC::Std::make_tuple(AscendC::int4b_t (1) ,int8_t (2) ,uint8_t (3) ,int16_t (4) ,uint16_t (5) ,int32_t (6) , \
    uint32_t (7) ,uint64_t (8) ,int64_t (9) ,half (10) ,float (11) ,bfloat16_t (12) ,fp8_e8m0_t (13) , bool (true));

AscendC::PRINTF("tuple size is --> %d\n", AscendC::Std::tuple_size<decltype(test2)>::value);

// Obtain elements of multiple data types using the get method.
number_int4b_t =        AscendC::Std::get<0>(test2);
number_int8_t =         AscendC::Std::get<1>(test2);
number_uint8_t =        AscendC::Std::get<2>(test2);
number_int16_t =        AscendC::Std::get<3>(test2);
number_uint16_t =       AscendC::Std::get<4>(test2);
number_int32_t =        AscendC::Std::get<5>(test2);
number_uint32_t =       AscendC::Std::get<6>(test2);
number_uint64_t =       AscendC::Std::get<7>(test2);
number_int64_t =        AscendC::Std::get<8>(test2);
number_half =           AscendC::Std::get<9>(test2);
number_float =          AscendC::Std::get<10>(test2);
number_bfloat16_t =     AscendC::Std::get<11>(test2);
number_fp8_e8m0_t =     AscendC::Std::get<12>(test2);
number_bool =           AscendC::Std::get<13>(test2);

// Obtain element references using the get method for subsequent operations.
AscendC::Std::get<1>(test2)+= 1 ;
AscendC::Std::get<2>(test2)+= 1 ;
AscendC::Std::get<3>(test2)+= 1 ;
AscendC::Std::get<4>(test2)+= 1 ;
AscendC::Std::get<5>(test2)+= 1 ;
AscendC::Std::get<6>(test2)+= 1 ;
AscendC::Std::get<7>(test2)+= 1 ;
AscendC::Std::get<8>(test2)+= 1 ;
AscendC::Std::get<10>(test2) += (float)1.0 ;

// Aggregate variable initialization lists.
AscendC::Std::tuple<AscendC::int4b_t, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, uint64_t, int64_t, half, float, \
bfloat16_t, fp8_e8m0_t, bool> test3 = {
    number_int4b_t, number_int8_t, number_uint8_t, number_int16_t, number_uint16_t, number_int32_t, number_uint32_t, \
    number_uint64_t, number_int64_t, number_half, number_float, number_bfloat16_t, number_fp8_e8m0_t, number_bool,
};

AscendC::PRINTF("tuple size is --> %d\n", AscendC::Std::tuple_size<decltype(test3)>::value);

uint32_t const_uint32_t = 0;
float const_float = 0.0;
bool const_bool = false;

// const constant type.
const AscendC::Std::tuple<uint32_t, float, bool> test4{11, 2.2, true};

// Obtain const constant types using the get method.
const_uint32_t = AscendC::Std::get<0>(test4);
const_float = AscendC::Std::get<1>(test4);
const_bool = AscendC::Std::get<2>(test4);

AscendC::Std::tuple_element<0,decltype(test4)>::type first = 77;
AscendC::Std::tuple_element<1,decltype(test4)>::type second = 7.7;
AscendC::Std::tuple_element<2,decltype(test4)>::type third = false;

AscendC::PRINTF("The value of the test element is: %d, %f, %d\n", first, second, third);

AscendC::Std::tie(const_uint32_t, const_float, const_bool) = test4;

AscendC::PRINTF("The value of the test element is: %d, %f, %d\n", const_uint32_t, const_float, const_bool);

// Aggregate const elements.
AscendC::Std::tuple<const uint32_t, const float, const bool> test5{33, 4.4, true};

// Obtain const elements using the get method.
const_uint32_t = AscendC::Std::get<0>(test5);
const_float = AscendC::Std::get<1>(test5);
const_bool = AscendC::Std::get<2>(test5);

const AscendC::Std::tuple<const uint32_t, const float, const bool> test6{33, 4.4, true};

const_uint32_t = AscendC::Std::get<0>(test6);
const_float = AscendC::Std::get<1>(test6);
const_bool = AscendC::Std::get<2>(test6);

// Default constructor initialization.
AscendC::Std::tuple<\
uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, \
uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, \
uint32_t, uint32_t, uint32_t, uint8_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,\
uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, \
uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, \
uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, \
uint32_t, uint32_t, uint32_t, uint32_t> test7;

// Aggregate default element initialization.
auto test8 = AscendC::Std::make_tuple(\
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, \
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20, \
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30, \
    31, 32, 33, 34, 35, 36, 37, 38, 39, 40, \
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, \
    51, 52, 53, 54, 55, 56, 57, 58, 59, 60, \
    61, 62, 63, 64);

// Operator overloading assignment.
test7 = test8;

AscendC::PRINTF("tuple size is --> %d\n", AscendC::Std::tuple_size<decltype(test7)>::value);
AscendC::PRINTF("tuple size is --> %d\n", AscendC::Std::tuple_size<decltype(test8)>::value);

// make_tuple aggregate initialization
AscendC::Std::tuple<uint32_t, float, bool> test9 = AscendC::Std::make_tuple(const_uint32_t, const_float, const_bool);

const_uint32_t = AscendC::Std::get<0>(test9);
const_float = AscendC::Std::get<1>(test9);
const_bool = AscendC::Std::get<2>(test9);

AscendC::PRINTF("tuple size is --> %d\n", AscendC::Std::tuple_size<decltype(test9)>::value);

using Element0Type = AscendC::Std::tuple_element<0, decltype(test9)>::type;
Element0Type element0 = 88;

using Element1Type = AscendC::Std::tuple_element<1, decltype(test9)>::type;
Element1Type element1 = 8.8;

using Element2Type = AscendC::Std::tuple_element<2, decltype(test9)>::type;
Element2Type element2 = true;

AscendC::PRINTF("The value of the test element is: %d, %f, %d\n", element0, element1, element2);

AscendC::Std::tie(const_uint32_t, const_float, const_bool) = test9;

AscendC::PRINTF("The value of the test element is: %d, %f, %d\n", const_uint32_t, const_float, const_bool);
```

```
// Execution result:
(testMakeTensor) tuple size is --> 4
(test3Tensor) tuple size is --> 4
(test1) tuple size is --> 14
(test2) tuple size is --> 14
(test3) tuple size is --> 14
(test4 tuple_element) The value of the test element is: 77, 7.700000, 0
(test4 tie) The value of the test element is: 11, 2.200000, 1
(test7) tuple size is --> 64
(test8) tuple size is --> 64
(test9) tuple size is --> 3
(test9 tuple_element) The value of the test element is: 88, 8.800000, 1
(test9 tie) The value of the test element is: 33, 4.400000, 1
```

