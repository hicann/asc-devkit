/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

import assert from 'node:assert/strict'
import { resolve } from 'node:path'
import { test } from 'node:test'

import { load as loadHtml } from 'cheerio'

import {
  findKernelLaunch,
  kernelLaunchMatches,
  normalizeKernelLaunchQuery,
} from '../../docs/.vitepress/search-syntax.mjs'
import {
  bestPagefindSection,
  collectCandidates,
  createIdentifierIndexLoader,
  createQueryPlan,
  deserializePagefindMeta,
  findIdentifierIndexMatches,
  highlightIdentifierExcerpt,
  highlightSearchText,
  initializePagefindSearch,
  MAX_CJK_SUBPHRASES,
  MAX_QUERY_LENGTH,
  mergeRouteResult,
  rankIdentifierIndex,
  searchPagefind,
} from '../../docs/.vitepress/theme/pagefind-search.mjs'
import {
  isCatalogOnlyArticle,
  sourcePathsForRoute,
} from '../../docs/.vitepress/build-pagefind-title-index.mjs'

test('normalizes kernel launch syntax queries and prefixes', () => {
  for (const query of [
    '<<<',
    '<<<gridDim',
    '<<<gridDim, blockDim>>',
    '<<<>>>',
    ' <<<...>>> ',
    '<<<gridDim, blockDim>>>',
  ]) {
    assert.equal(normalizeKernelLaunchQuery(query), '<<<>>>')
  }
})

test('does not treat partial or mixed text as a kernel launch syntax query', () => {
  for (const query of ['', '<<', '>>>', 'prefix <<<>>> suffix', 'foo<<<1>>>']) {
    assert.equal(normalizeKernelLaunchQuery(query), '')
  }
})

test('finds complete launch syntax while preferring the canonical spelling', () => {
  assert.deepEqual(kernelLaunchMatches('a <<<1, 2>>> b <<<>>>'), [
    { index: 2, value: '<<<1, 2>>>' },
    { index: 15, value: '<<<>>>' },
  ])
  assert.deepEqual(findKernelLaunch('a <<<1>>> b <<<>>>'), {
    index: 12,
    value: '<<<>>>',
  })
})

test('centers identifier excerpts around the highlighted match', () => {
  const highlighted = highlightIdentifierExcerpt(
    `${'前'.repeat(40)}核函数通过<<<...>>>启动${'后'.repeat(120)}`,
    '<<<>>>',
    true,
  )
  const markStart = highlighted.indexOf('<mark>')
  const markEnd = highlighted.indexOf('</mark>')

  assert.ok(highlighted.startsWith('...'))
  assert.equal(Array.from(highlighted.slice(3, markStart)).length, 12)
  assert.equal(highlighted.slice(markStart, markEnd + 7), '<mark>&lt;&lt;&lt;...&gt;&gt;&gt;</mark>')
  assert.ok(highlighted.endsWith('...'))
})

test('escapes identifier excerpts before adding highlight markup', () => {
  assert.equal(
    highlightIdentifierExcerpt('<tag>NO_OVER suffix', 'no_over'),
    '&lt;tag&gt;<mark>NO_OVER</mark> suffix',
  )
  assert.equal(
    highlightIdentifierExcerpt('<tag>without a match', 'no_over'),
    '&lt;tag&gt;without a match',
  )
})

test('selects the Pagefind section containing the strongest match', () => {
  const section = bestPagefindSection({
    url: '/guide/page.html',
    weighted_locations: [{ location: 18, weight: 10 }],
    sub_results: [
      {
        url: '/guide/page.html#first',
        anchor: { text: 'First section' },
        locations: [4],
      },
      {
        url: '/guide/page.html#second',
        anchor: { text: 'Second：section' },
        locations: [18],
      },
    ],
  })

  assert.deepEqual(section, {
    route: '/guide/page.html#second',
    sectionTitle: 'Second：section',
  })
})

test('weights title alias candidates above content candidates', async () => {
  const pagefind = {
    async search() {
      return { results: [{ id: 'page', score: 1 }] }
    },
  }
  const variants = [{ query: 'page', weight: 1 }]
  const content = await collectCandidates(pagefind, variants)
  const title = await collectCandidates(pagefind, variants, new Map(), 'title-alias')

  assert.equal(content.get('content:page').retrievalScore, 1 / 61)
  assert.equal(title.get('title-alias:page').retrievalScore, 1.3 / 61)
})

test('filters Pagefind candidates before applying the Top 20 result limit', async () => {
  const pagefindResults = Array.from({ length: 21 }, (_, index) => {
    const route = index === 20 ? '/api/supported.html' : `/api/unsupported-${index}.html`
    return {
      id: route,
      score: 21 - index,
      async data() {
        return {
          url: route,
          meta: { title: `过滤测试 ${index}` },
          content: '过滤测试',
          excerpt: '过滤测试',
        }
      },
    }
  })
  const pagefind = {
    async search() {
      return { results: pagefindResults }
    },
  }

  const results = await searchPagefind(
    pagefind,
    '过滤测试',
    route => route === '/api/supported.html',
  )

  assert.deepEqual(results.map(result => result.route), ['/api/supported.html'])
})

test('prefers concrete identifier owners and references over navigation catalogs', () => {
  const identifier = 'asc_copy_gm2ub_align'
  const index = {
    p: [
      ['/api/copy/asc_copy_gm2ub_align_arch_3510.html', identifier, '', 0],
      ['/api/copy/asc_copy_gm2ub_align_arch_2201.html', identifier, '', 0],
      ['/api/copy/asc_copy_gm2ub_align.html', identifier, '', 1],
      ['/api/vector_data_move.html', '矢量数据搬运', '', 1],
      ['/api/asc_set_copy_pad_val.html', 'asc_set_copy_pad_val', '', 0],
    ],
    i: [[identifier, [0, 1, 2, 3, 4], [0, 0, 1, 1, 2], [0, 0, -1, -1, 1]]],
    s: [
      `约束说明 ${identifier} 指令`,
      identifier,
      `该接口仅对${identifier}接口有效。`,
    ],
    h: [
      ['约束说明', '约束说明'],
      ['功能说明', '功能说明'],
    ],
  }
  const results = rankIdentifierIndex(
    index,
    createQueryPlan(identifier),
    route => !route.includes('arch_2201'),
  )

  assert.deepEqual(results.results.map(result => result.route), [
    '/api/copy/asc_copy_gm2ub_align_arch_3510.html#%E7%BA%A6%E6%9D%9F%E8%AF%B4%E6%98%8E',
    '/api/asc_set_copy_pad_val.html#%E5%8A%9F%E8%83%BD%E8%AF%B4%E6%98%8E',
    '/api/copy/asc_copy_gm2ub_align.html',
    '/api/vector_data_move.html',
  ])
  assert.match(results.results[0].description, /<mark>asc_copy_gm2ub_align<\/mark>/)
  assert.equal(
    results.results[1].description,
    '该接口仅对<mark>asc_copy_gm2ub_align</mark>接口有效。',
  )
})

test('keeps a catalog authoritative when no concrete identifier owner is visible', () => {
  const identifier = 'catalog_identifier'
  const index = {
    p: [
      ['/api/catalog_identifier.html', identifier, identifier, 1],
      ['/api/catalog_identifier_arch.html', identifier, identifier, 0],
    ],
    i: [[identifier, [0, 1], [0, 0]]],
    s: [identifier],
    h: [],
  }
  const results = rankIdentifierIndex(
    index,
    createQueryPlan(identifier),
    route => !route.endsWith('_arch.html'),
  )

  assert.equal(results.results[0].route, '/api/catalog_identifier.html')
  assert.equal(results.results[0].score, 3720000)
})

test('classifies link-only navigation pages without treating content pages as catalogs', () => {
  const catalog = loadHtml(`
    <article class="markdown-body">
      <h1>API</h1>
      <ul><li><p><a href="child.html">child</a></p></li></ul>
    </article>
  `)
  const content = loadHtml(`
    <article class="markdown-body">
      <h1>API</h1>
      <p>API behavior and constraints.</p>
      <ul><li><a href="related.html">related</a></li></ul>
    </article>
  `)

  assert.equal(isCatalogOnlyArticle(catalog, catalog('.markdown-body')), true)
  assert.equal(isCatalogOnlyArticle(content, content('.markdown-body')), false)
})

test('restores the primary index when a failed title merge poisons Pagefind state', async () => {
  let mergeCalls = 0
  let destroyCalls = 0
  let initCalls = 0
  let poisoned = false
  const pagefind = {
    async mergeIndex() {
      mergeCalls++
      poisoned = true
      throw new Error('404')
    },
    async destroy() {
      destroyCalls++
      poisoned = false
    },
    async init() {
      initCalls++
    },
    async search() {
      if (poisoned) throw new Error('404')
      return { results: ['primary'] }
    },
  }
  const originalWarn = console.warn
  console.warn = () => {}
  try {
    assert.equal(await initializePagefindSearch(pagefind), pagefind)
    assert.deepEqual(await pagefind.search('query'), { results: ['primary'] })
    assert.equal(await initializePagefindSearch(pagefind), pagefind)
  }
  finally {
    console.warn = originalWarn
  }
  assert.equal(mergeCalls, 1)
  assert.equal(destroyCalls, 1)
  assert.equal(initCalls, 1)
})

test('retries title initialization when primary index recovery fails', async () => {
  let mergeCalls = 0
  let destroyCalls = 0
  let initCalls = 0
  const pagefind = {
    async mergeIndex() {
      mergeCalls++
      throw new Error('404')
    },
    async destroy() {
      destroyCalls++
      if (destroyCalls === 1) throw new Error('destroy failed')
    },
    async init() {
      initCalls++
    },
  }
  const originalWarn = console.warn
  console.warn = () => {}
  try {
    await assert.rejects(initializePagefindSearch(pagefind), /destroy failed/)
    assert.equal(await initializePagefindSearch(pagefind), pagefind)
  }
  finally {
    console.warn = originalWarn
  }
  assert.equal(mergeCalls, 2)
  assert.equal(destroyCalls, 2)
  assert.equal(initCalls, 1)
})

test('retries the identifier index after a temporary request failure', async () => {
  const expected = { p: [['/api/page', 'Page']], i: [['identifier', [0]]], s: [], h: [] }
  let calls = 0
  const load = createIdentifierIndexLoader(async () => {
    calls++
    if (calls === 1) throw new Error('temporary failure')
    return expected
  })

  assert.deepEqual(await load(), { p: [], i: [], s: [], h: [] })
  assert.equal(await load(), expected)
  assert.equal(await load(), expected)
  assert.equal(calls, 2)
})

test('restores Pagefind dates and serialized frontmatter metadata', () => {
  const frontmatter = { category: 'api', date: 1786410419730, publish: false }
  const base64 = Buffer.from(encodeURIComponent(JSON.stringify(frontmatter))).toString('base64')

  assert.deepEqual(deserializePagefindMeta({ base64, title: 'Page title' }), {
    category: 'api',
    date: 1786410419730,
    publish: false,
    title: 'Page title',
  })
  assert.deepEqual(deserializePagefindMeta({
    date: '1786410419730',
    title: 'Page title',
  }), {
    date: 1786410419730,
    title: 'Page title',
  })
})

test('keeps content metadata and result when a title alias ranks higher', () => {
  const contentData = { meta: { date: '1786410419730', title: 'Page title' } }
  const titleData = { meta: { ascSearchSource: 'title-route', title: 'Page title' } }
  const merged = mergeRouteResult({
    route: '/guide/page.html',
    description: 'Content excerpt',
    meta: { category: 'guide', date: 1786410419730, description: 'Content excerpt', title: 'Page title' },
    result: contentData,
    score: 10,
  }, {
    route: '/guide/page.html',
    description: '',
    meta: { description: '', title: 'Page title' },
    result: titleData,
    score: 20,
  })

  assert.equal(merged.description, 'Content excerpt')
  assert.equal(merged.meta.category, 'guide')
  assert.equal(merged.meta.date, 1786410419730)
  assert.equal(merged.result, contentData)
})

test('escapes and highlights section titles', () => {
  assert.equal(
    highlightSearchText('配置<<<...>>>调用', '<<<'),
    '配置<mark>&lt;&lt;&lt;...&gt;&gt;&gt;</mark>调用',
  )
  assert.equal(
    highlightSearchText('<unsafe> section', 'section'),
    '&lt;unsafe&gt; <mark>section</mark>',
  )
})

test('matches omitted identifier decorators and prefers canonical macro spelling', () => {
  const entries = [
    ['__npu_arch__'],
    ['acl_rtc_npu_arch'],
    ['npu_arch'],
    ['npu_arch_2201'],
  ]
  for (const query of ['__NPU_ARCH__', '_NPU_ARCH', 'NPU_ARCH']) {
    const plan = createQueryPlan(query)
    assert.equal(plan.preferDecoratedIdentifier, true)
    assert.deepEqual(
      findIdentifierIndexMatches(entries, plan.identifierQuery, plan.preferDecoratedIdentifier)
        .map(match => match.entry[0]),
      ['__npu_arch__', 'npu_arch', 'npu_arch_2201'],
    )
  }

  const lowercasePlan = createQueryPlan('npu_arch')
  assert.equal(lowercasePlan.preferDecoratedIdentifier, false)
  assert.equal(
    findIdentifierIndexMatches(entries, lowercasePlan.identifierQuery)[0].entry[0],
    'npu_arch',
  )
})

test('bounds long CJK queries and generated subphrases', () => {
  const plan = createQueryPlan('中'.repeat(2048))

  assert.equal(Array.from(plan.query).length, MAX_QUERY_LENGTH)
  assert.ok(plan.cjkSubPhrases[0].length <= MAX_CJK_SUBPHRASES)
  assert.ok(plan.cjkSubPhrases[0].includes('中中'))
})

test('maps directory routes to intermediate index pages', () => {
  const root = resolve('/tmp/intermediate-html')

  assert.deepEqual(sourcePathsForRoute(root, '/guide/'), [
    resolve(root, 'guide.html'),
    resolve(root, 'guide/index.html'),
  ])
  assert.deepEqual(sourcePathsForRoute(root, '/'), [resolve(root, 'index.html')])
  assert.deepEqual(sourcePathsForRoute(root, '/../outside'), [])
})
