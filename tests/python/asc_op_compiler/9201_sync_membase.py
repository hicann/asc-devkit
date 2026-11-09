# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""Host argument-flow regression; extracts checkout function bodies, stubs hardware intrinsics.

Run directly with Python. MEMBASE_REVISION=2488a6f1d selects the audit baseline;
MEMBASE_CASE=1 / 2 isolates transpose / stride checks for red-baseline verification.
This checks address/argument generation and assertions, not device execution.
"""

from pathlib import Path
import subprocess
import tempfile
import os

repo = Path(__file__).resolve().parents[3]


def source(path):
    revision = os.environ.get("MEMBASE_REVISION")
    if revision:
        return subprocess.check_output(["git", "show", revision + ":" + path], cwd=repo, text=True)
    return (repo / path).read_text()


def function(text, marker, prefix=""):
    start = text.index(marker)
    begin = text.index("{", start)
    level = 1
    end = begin + 1
    while level:
        level += (text[end] == "{") - (text[end] == "}")
        end += 1
    return prefix + text[start:end] + "\n"


mm = source("impl/basic_api/dav_9201/kernel_operator_mm_impl.h")
dc = source("impl/basic_api/dav_9201/kernel_operator_data_copy_impl.h")
cpp = r"""
#include <cassert>
#include <cstdint>
#include <vector>
#include <tuple>
#define __aicore__
#define __cb__
#define __cbuf__
#define __gm__
#define __ubuf__
#define ASCEND_IS_AIC (true)
#define ASCENDC_ASSERT(c,...) do {if(!(c)) ++errors;} while(0)
#define KERNEL_LOG(...)
constexpr int KERNEL_ERROR=0, VALUE_512=512;
using half=short; using bfloat16_t=unsigned short;
template<class...> constexpr bool SupportType(){return true;}
struct LoadData2DParams {uint16_t startIndex; uint8_t repeatTimes; uint16_t srcStride; uint16_t sid; bool ifTranspose; uint16_t dstGap;};
struct DataCopyExtParams {uint16_t blockCount; uint32_t blockLen; int64_t srcStride,dstStride; uint8_t rsv;};
enum class PaddingMode {Normal};
constexpr bool g_gm_overflow_check=false;
uint8_t* GetSysWorkSpacePtr(){return nullptr;}
struct AscendCUtils {template<class... A> static void CheckGmMemOverflowNormal(A...){}};
int errors=0; std::vector<std::tuple<uintptr_t,uintptr_t,int>> calls;
template<class T, class... A> void load_cbuf_to_cb(T* d,T* s,int,int k,A...){calls.emplace_back((uintptr_t)d,(uintptr_t)s,k);}
template<class T,PaddingMode M,class... A> void CopyUbufToGmAlignV2(A...){}
"""
cpp += function(mm, "__aicore__ inline void LoadData2DL12L0BCal(", "template<class T>\n")
cpp += function(dc, "__aicore__ inline void CheckSrcGmDataCopyExtParamsRange(")
marker = "__aicore__ inline void DataCopyPadUB2GMImpl(\n    __gm__ T* dst, __ubuf__ T* src, const DataCopyExtParams&"
cpp += function(dc, marker, "template<class T,PaddingMode mode=PaddingMode::Normal>\n")
cpp += r"""
int main(){
    uint8_t dst[65536]{}, src[65536]{};
#if TEST_CASE == 0 || TEST_CASE == 1
    for(int start: {0,3,9}) for(int stride: {1,4}) for(int gap: {0,2,7}) {
        LoadData2DParams p{(uint16_t)start,3,(uint16_t)stride,0,true,(uint16_t)gap};
        calls.clear(); LoadData2DL12L0BCal(dst,src,p); assert(calls.size()==3);
        for(int i=0;i<3;i++) {
            assert(std::get<0>(calls[i])==(uintptr_t)(dst+i*(gap+1)*512));
            assert(std::get<1>(calls[i])==(uintptr_t)(src+(start+i*stride)*512));
            assert(std::get<2>(calls[i])==0);
        }
    }
#endif
#if TEST_CASE == 0 || TEST_CASE == 2
    for(int64_t n: {(1LL<<39),(1LL<<40)-1,(1LL<<40)}) {
        DataCopyExtParams p{1,32,n,0,0}; errors=0; CheckSrcGmDataCopyExtParamsRange(p);
        assert(errors==(n==(1LL<<40))); p.srcStride=0;p.dstStride=n;errors=0;
        DataCopyPadUB2GMImpl(dst,src,p); assert(errors==(n==(1LL<<40)));
    }
#endif
}
"""
cpp = "#define TEST_CASE " + os.environ.get("MEMBASE_CASE", "0") + "\n" + cpp
with tempfile.TemporaryDirectory() as td:
    p = Path(td) / "test.cpp"
    p.write_text(cpp)
    subprocess.run(["g++", "-std=c++17", str(p), "-o", td + "/test"], check=True)
    subprocess.run([td + "/test"], check=True)
print("selected transpose/stride cases PASS")
wrapper = source("impl/basic_api/kernel_operator_mm_load2d_impl.h")
wrapper = wrapper[wrapper.index("#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 9201)") :]
body = function(wrapper, "__aicore__ inline __inout_pipe__(MTE2) void LoadDataImpl(", "template<class T>\n")
fixture = r"""
#include <cstdint>
#include <cassert>
#define __aicore__
#define __inout_pipe__(x)
#define __ca__
#define __cb__
#define __cbuf__
#define __gm__
enum class Hardware {L0A,L0B,L1}; using TPosition=Hardware;
Hardware GetPhyType(Hardware p){return p;}
struct LoadData2DParams{};
template<class T> using PrimT=T;
template<class T> struct LocalTensor {int GetPosition() const{return (int)Hardware::L1;} void* GetPhyAddr()const{return nullptr;}};
template<class T> struct GlobalTensor {uint8_t cache;void* GetPhyAddr()const{return nullptr;}};
template<class T> uint8_t ExtractCacheMode(const GlobalTensor<T>& p){return p.cache;}
int seenCache=-1; bool seenPre=false;
template<class... A> void LoadData2DGM2L0ACal(A...){}
template<class... A> void LoadData2DGM2L0BCal(A...){}
template<class T> void LoadData2DGM2L1Cal(T*,T*,const LoadData2DParams&,uint8_t c,bool p){seenCache=c;seenPre=p;}
"""
fixture += (
    body
    + r"""
int main(){for(uint8_t c:{uint8_t(0),uint8_t(2)}) for(bool pre:{false,true}) {
 LocalTensor<float> d; GlobalTensor<float> s{c}; LoadData2DParams p;
 LoadDataImpl(d,s,p,pre); assert(seenCache==c && seenPre==pre);
}}
"""
)
fixture = "#include <initializer_list>\n" + fixture
with tempfile.TemporaryDirectory() as td:
    p = Path(td) / "test.cpp"
    p.write_text(fixture)
    subprocess.run(["g++", "-std=c++17", str(p), "-o", td + "/test"], check=True)
    subprocess.run([td + "/test"], check=True)
print("shared wrapper cache/preRead four combinations PASS")


# Exercise actual block-load/store dispatch with host intrinsic argument models.
reg_fixture = r"""
#include <cassert>
#include <cstdint>
#include <type_traits>
#include <initializer_list>
#define __simd_callee__
#define __ubuf__
enum class Post { NORMAL, UPDATE };
enum class PostLiteral { POST_MODE_NORMAL, POST_MODE_UPDATE };
enum class DataCopyMode { DATA_BLOCK_COPY };
struct DefaultType {};
struct RegTraitNumOne {};
struct complex32 { uint16_t real, imag; };
template<class T> struct RegTensor { using ActualT = T; };
struct MaskReg { bool value; };
constexpr unsigned ONE_BLK_SIZE = 32;
template<class, class> constexpr bool CheckRegTrait() { return true; }
template<class T, unsigned... N> constexpr bool SupportBytes() { return ((sizeof(T) == N) || ...); }
uintptr_t seenAddress;
uint32_t seenImmediate;
bool seenMask;
template<class R, class T> void vsldb(R&, T* p, uint32_t immediate, MaskReg& m) {
 seenAddress = reinterpret_cast<uintptr_t>(p); seenImmediate = immediate; seenMask = m.value;
}
template<class R, class T> void vsstb(R& r, T* p, uint32_t immediate, MaskReg& m) {
 vsldb(r, p, immediate, m);
}
template<class R, class T, class P> void vsldb(R& r, T*& p, uint32_t immediate, MaskReg& m, P) {
 vsldb(r, p, immediate, m);
 p += (immediate & 0xffffu) * 32 / sizeof(T);
}
template<class R, class T, class P> void vsstb(R& r, T*& p, uint32_t immediate, MaskReg& m, P post) {
 vsldb(r, p, immediate, m, post);
}
"""
reg_template = "template <typename T = DefaultType, DataCopyMode dataMode, typename U>\n"
post_template = "template <typename T = DefaultType, DataCopyMode dataMode, PostLiteral postMode, typename U>\n"
for name, low_marker, post_marker in [
    (
        "load",
        "DataCopyImpl(U& dstReg, __ubuf__ T* srcAddr, uint32_t dataBlockStride, MaskReg& mask)",
        "DataCopyImpl(\n    U& dstReg, __ubuf__ T*& srcAddr, uint32_t dataBlockStride, uint32_t repeatStride, MaskReg& mask)",
    ),
    (
        "store",
        "DataCopyImpl(__ubuf__ T* dstAddr, U& srcReg, uint32_t dataBlockStride, MaskReg& mask)",
        "DataCopyImpl(\n    __ubuf__ T*& dstAddr, U& srcReg, uint32_t dataBlockStride, uint32_t repeatStride, MaskReg& mask)",
    ),
]:
    body = source(f"impl/basic_api/reg_compute/dav_9201/kernel_reg_compute_datacopy_{name}_impl.h")
    for prefix, marker in [(reg_template, low_marker), (post_template, post_marker)]:
        reg_fixture += function(body, "__simd_callee__ inline void " + marker, prefix)
reg_fixture += r"""
template<class T, PostLiteral mode> void check() {
 alignas(32) T buffer[4096 / sizeof(T)]{};
 RegTensor<T> reg;
 for (uint32_t stride : {0u, 1u, 7u}) for (uint32_t repeat : {0u, 3u, 17u})
 for (bool active : {false, true}) {
  MaskReg mask{active};
  T* ptr = buffer;
  const uintptr_t base = reinterpret_cast<uintptr_t>(buffer);
  constexpr bool normal = mode == PostLiteral::POST_MODE_NORMAL;
  const uintptr_t effective = base + (normal ? repeat * 32 : 0);
  const uintptr_t after = base + (normal ? 0 : repeat * 32);
  const uint32_t immediate = (stride << 16u) | (normal ? 0 : repeat);
  DataCopyImpl<T,DataCopyMode::DATA_BLOCK_COPY,mode>(reg, ptr, stride, repeat, mask);
  assert(seenAddress == effective && seenImmediate == immediate && seenMask == active);
  assert(reinterpret_cast<uintptr_t>(ptr) == after);
  ptr = buffer;
  DataCopyImpl<T,DataCopyMode::DATA_BLOCK_COPY,mode>(ptr, reg, stride, repeat, mask);
  assert(seenAddress == effective && seenImmediate == immediate && seenMask == active);
  assert(reinterpret_cast<uintptr_t>(ptr) == after);
 }
}
template<class T> void checkModes() {
 check<T,PostLiteral::POST_MODE_NORMAL>(); check<T,PostLiteral::POST_MODE_UPDATE>();
}
int main() {
 checkModes<uint8_t>(); checkModes<int8_t>(); checkModes<uint16_t>(); checkModes<int16_t>();
 checkModes<uint32_t>(); checkModes<int32_t>(); checkModes<bool>(); checkModes<complex32>();
}
"""
with tempfile.TemporaryDirectory() as td:
    p = Path(td) / "reg_stride.cpp"
    p.write_text(reg_fixture)
    subprocess.run(["g++", "-std=c++17", str(p), "-o", td + "/test"], check=True)
    subprocess.run([td + "/test"], check=True)
print("reg block LoadAlign/StoreAlign: 576 address/immediate/pointer/mask cases PASS")
