<!--
  Copyright (c) 2026 Huawei Technologies Co., Ltd.
  This program is free software, you can redistribute it and/or modify it under the terms and conditions of
  CANN Open Software License Agreement Version 2.0 (the "License").
  Please refer to the License for details. You may not use this file except in compliance with the License.
  THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
  See LICENSE in the root of the software repository for the full text of the License.
-->

<script setup>
const systems = [
  {
    key: 'simd',
    badge: 'SIMD',
    title: 'SIMD 编程体系',
    desc: '规整向量化 · 数据并行',
    cards: [
      {
        title: 'Basic API',
        tone: 'blue',
        lang: 'C++',
        subtitle: '无 Layout 约束的 Tensor 抽象，TPipe/TQue 框架统一托管内存调度与执行同步。',
        tags: ['通用算子快速开发', '内存同步托管'],
        doc: { href: '/api/SIMD-API/basic_api/basic_api' },
        list: { href: '/api/SIMD-API/basic_api/basic_api_list' },
      },
      {
        title: 'Tensor API',
        tone: 'purple',
        lang: 'C++',
        subtitle: '携带 Layout 语义的 Tensor 抽象，arch / atom / algorithm 三层解耦。',
        tags: ['精细调优', 'Layout布局表达'],
        doc: { href: '/api/SIMD-API/tensor_api/tensor_api' },
        list: { href: '/api/SIMD-API/tensor_api/tensor_api_list' },
      },
      {
        title: 'C API',
        tone: 'green',
        lang: 'C',
        subtitle: '原生指针编程，自主管控内存排布与执行同步，指令级透明、零封装开销。',
        tags: ['极致性能调优', '原生C范式', '底层可控'],
        doc: { href: '/api/SIMD-API/c_api/README' },
        list: { href: '/api/SIMD-API/c_api/c_api' },
      },
    ],
  },
  {
    key: 'simt',
    badge: 'SIMT',
    title: 'SIMT 编程体系',
    desc: '离散不规则 · 线程并行',
    cards: [
      {
        title: 'SIMT API',
        tone: 'orange',
        lang: 'C',
        subtitle: '以单线程为基本编程单元，原生支持离散、不规则并行计算。',
        tags: ['离散计算', 'SIMT/SIMD混合编程'],
        doc: { href: '/api/SIMT-API/overview' },
        list: { href: '/api/SIMT-API/SIMT_programming_intro/api_list' },
      },
    ],
  },
]
</script>

<template>
  <section class="home-api-guide" aria-label="Ascend C 编程体系引导">
    <header class="home-api-guide__header">
      <h2 class="home-api-guide__title">选择编程体系与接口</h2>
      <p class="home-api-guide__desc">
        先选择SIMD或SIMT编程体系；SIMD体系内提供Basic API、Tensor API、C API三种接口。
      </p>
    </header>

    <div class="home-api-guide__systems">
      <div
        v-for="system in systems"
        :key="system.key"
        class="api-system"
        :class="`api-system--${system.key}`"
      >
        <div class="api-system__head">
          <span class="api-system__badge">{{ system.badge }}</span>
          <h3 class="api-system__title">{{ system.title }}</h3>
          <span class="api-system__desc">{{ system.desc }}</span>
        </div>

        <div class="api-system__cards">
          <article
            v-for="card in system.cards"
            :key="card.title"
            :class="['api-card', 'api-card--' + card.tone]"
          >
            <div class="api-card__top">
              <h4 class="api-card__title">{{ card.title }}</h4>
              <span class="api-card__lang">{{ card.lang }}</span>
            </div>

            <p class="api-card__subtitle">{{ card.subtitle }}</p>

            <div class="api-card__tags">
              <span v-for="tag in card.tags" :key="tag">{{ tag }}</span>
            </div>

            <div class="api-card__footer">
              <a class="api-card__doc" :href="card.doc.href">
                接口文档
                <span aria-hidden="true">→</span>
              </a>
              <a class="api-card__doc" :href="card.list.href">
                API 列表
                <span aria-hidden="true">→</span>
              </a>
            </div>
          </article>
        </div>
      </div>
    </div>
  </section>
</template>

<style scoped>
.home-api-guide {
  width: min(1392px, calc(100% - 48px));
  margin: 32px auto 0;
}

.home-api-guide__header {
  margin-bottom: 20px;
  padding: 0 4px;
}

.home-api-guide__eyebrow {
  margin: 0 0 8px;
  font-size: 12px;
  font-weight: 750;
  letter-spacing: 0.12em;
  text-transform: uppercase;
  background: linear-gradient(90deg, #2873df 0%, #7b45db 38%, #008c94 68%, #e36b32 100%);
  -webkit-background-clip: text;
  background-clip: text;
  color: transparent;
}

.home-api-guide__title {
  margin: 0;
  color: var(--vp-c-text-1);
  font-size: 26px;
  line-height: 1.3;
  font-weight: 750;
}

.home-api-guide__desc {
  max-width: 820px;
  margin: 10px 0 0;
  color: var(--vp-c-text-2);
  font-size: 14px;
  line-height: 1.75;
  font-weight: 500;
}

.home-api-guide__systems {
  display: grid;
  grid-template-columns: minmax(0, 3fr) minmax(0, 1fr);
  gap: 18px;
  align-items: stretch;
}

.api-system {
  display: flex;
  flex-direction: column;
  gap: 14px;
  padding: 18px;
  border-radius: 14px;
}

.api-system--simd {
  background: linear-gradient(
    135deg,
    rgba(40, 115, 223, 0.07) 0%,
    rgba(123, 69, 219, 0.07) 50%,
    rgba(0, 140, 148, 0.07) 100%
  );
}

.api-system--simt {
  background: linear-gradient(
    135deg,
    rgba(227, 107, 50, 0.12) 0%,
    rgba(227, 107, 50, 0.03) 100%
  );
}

.api-system__head {
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  gap: 10px;
  padding: 0 2px;
}

.api-system__badge {
  padding: 3px 8px;
  border-radius: 4px;
  color: var(--asc-white);
  font-family: var(--vp-font-family-mono);
  font-size: 11px;
  font-weight: 750;
  letter-spacing: 0.04em;
}

.api-system--simd .api-system__badge {
  background: linear-gradient(135deg, #2873df 0%, #7b45db 52%, #008c94 100%);
}

.api-system--simt .api-system__badge {
  background: linear-gradient(135deg, #c1501c 0%, #e36b32 55%, #ff9a63 100%);
}

.api-system__title {
  margin: 0;
  color: var(--vp-c-text-1);
  font-size: 17px;
  line-height: 1.4;
  font-weight: 750;
}

.api-system--simd .api-system__title {
  background: linear-gradient(90deg, #2873df 0%, #7b45db 50%, #2fbfc7 100%);
  -webkit-background-clip: text;
  background-clip: text;
  color: transparent;
}

.api-system--simt .api-system__title {
  background: linear-gradient(90deg, #e36b32 0%, #ff9a63 100%);
  -webkit-background-clip: text;
  background-clip: text;
  color: transparent;
}

.api-system__desc {
  color: var(--vp-c-text-3);
  font-size: 12.5px;
  line-height: 1.4;
  font-weight: 600;
}

.api-system__cards {
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));
  gap: 14px;
  flex: 1;
}

.api-system--simt .api-system__cards {
  grid-template-columns: minmax(0, 1fr);
}

/* 每类 API 的系列色 */
.api-card--blue {
  --series: #2873df;
  --series-dark: #1e5cb5;
  --series-bright: #5b9bff;
  --series-light: #eaf2ff;
  --series-gradient: linear-gradient(135deg, #1e5cb5 0%, #2873df 48%, #5b9bff 100%);
}

.api-card--purple {
  --series: #7b45db;
  --series-dark: #5e2fb5;
  --series-bright: #a97cff;
  --series-light: #f2ecff;
  --series-gradient: linear-gradient(135deg, #5e2fb5 0%, #7b45db 48%, #a97cff 100%);
}

.api-card--green {
  --series: #008c94;
  --series-dark: #00727a;
  --series-bright: #2fbfc7;
  --series-light: #e6f7f8;
  --series-gradient: linear-gradient(135deg, #00727a 0%, #008c94 48%, #2fbfc7 100%);
}

.api-card--orange {
  --series: #e36b32;
  --series-dark: #c1501c;
  --series-bright: #ff9a63;
  --series-light: #fff0e8;
  --series-gradient: linear-gradient(135deg, #c1501c 0%, #e36b32 48%, #ff9a63 100%);
}

.api-card {
  display: flex;
  flex-direction: column;
  gap: 10px;
  min-width: 0;
  padding: 16px 16px 14px;
  border: 1px solid color-mix(in srgb, var(--series) 26%, var(--vp-c-divider));
  border-radius: 12px;
  background: linear-gradient(
    158deg,
    color-mix(in srgb, var(--series) 12%, var(--vp-c-bg-elv)) 0%,
    var(--vp-c-bg-elv) 66%
  );
  box-shadow: 0 6px 18px color-mix(in srgb, var(--series) 10%, transparent);
  transition: transform 0.2s ease, box-shadow 0.2s ease, border-color 0.2s ease;
}

.api-card:hover {
  border-color: color-mix(in srgb, var(--series) 52%, var(--vp-c-divider));
  box-shadow: 0 14px 34px color-mix(in srgb, var(--series) 22%, transparent);
  transform: translateY(-2px);
}

.api-card__top {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 8px;
}

.api-card__title {
  margin: 0;
  color: var(--series-dark);
  font-size: 16px;
  line-height: 1.4;
  font-weight: 750;
}

.api-card__lang {
  flex: none;
  padding: 2px 7px;
  border: 0;
  border-radius: 4px;
  background: var(--series-gradient);
  color: var(--asc-white);
  font-family: var(--vp-font-family-mono);
  font-size: 11px;
  line-height: 1.4;
  font-weight: 650;
}

.api-card__subtitle {
  margin: 0;
  color: var(--vp-c-text-2);
  font-size: 13px;
  line-height: 1.65;
  font-weight: 500;
}

.api-card__tags {
  display: flex;
  flex-wrap: wrap;
  gap: 6px;
}

.api-card__tags span {
  padding: 5px 7px;
  border-radius: 3px;
  background: color-mix(in srgb, var(--series-light) 82%, var(--vp-c-bg-elv));
  color: var(--series-dark);
  font-size: 11.5px;
  line-height: 1;
  font-weight: 650;
}

.api-card__footer {
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  gap: 8px 16px;
  margin-top: auto;
  padding-top: 4px;
}

.api-card__doc {
  display: inline-flex;
  align-items: center;
  gap: 4px;
  align-self: flex-start;
  color: var(--series-dark);
  font-size: 13px;
  font-weight: 750;
  text-decoration: none;
}

.api-card__doc:hover {
  color: var(--series);
  text-decoration: underline;
}

html.dark .api-system--simd {
  background: linear-gradient(
    135deg,
    rgba(40, 115, 223, 0.16) 0%,
    rgba(123, 69, 219, 0.16) 50%,
    rgba(0, 140, 148, 0.16) 100%
  );
}

html.dark .api-system--simt {
  background: linear-gradient(
    135deg,
    rgba(227, 107, 50, 0.2) 0%,
    rgba(227, 107, 50, 0.05) 100%
  );
}

html.dark .api-card {
  background: linear-gradient(
    158deg,
    color-mix(in srgb, var(--series) 22%, var(--vp-c-bg-elv)) 0%,
    var(--vp-c-bg-elv) 70%
  );
  border-color: color-mix(in srgb, var(--series) 42%, var(--vp-c-divider));
  box-shadow: 0 6px 18px rgba(0, 0, 0, 0.16);
}

html.dark .api-card__title,
html.dark .api-card__doc {
  color: var(--series-bright);
}

html.dark .api-card__doc:hover {
  color: var(--series);
}

html.dark .api-card__tags span {
  background: color-mix(in srgb, var(--series) 26%, transparent);
  color: var(--series-bright);
}

@media (max-width: 1024px) {
  .home-api-guide__systems {
    grid-template-columns: minmax(0, 1fr);
  }
}

@media (max-width: 760px) {
  .home-api-guide {
    width: min(100%, calc(100% - 32px));
    margin-top: 24px;
  }

  .home-api-guide__title {
    font-size: 22px;
  }

  .api-system__cards {
    grid-template-columns: minmax(0, 1fr);
  }
}
</style>
