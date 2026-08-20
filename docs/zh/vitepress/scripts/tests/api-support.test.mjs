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
import test from 'node:test'
import {
  extractUnsupportedProducts,
  filterNavigationCandidates,
  filterSidebarItems,
  getFilteredSidebarConfig,
  getProductFilterValue,
  isApiRouteUnsupported,
  normalizeApiRoute,
  renumberSidebarItems,
} from '../api-support.mjs'

test('maps all supported product labels to existing filter values', () => {
  const labels = {
    'Ascend 950PR/Ascend 950DT': '950',
    'Atlas A3 训练系列产品/Atlas A3 推理系列产品': 'A3',
    'Atlas A2 训练系列产品/Atlas A2 推理系列产品': '910b',
    'Atlas 200I/500 A2 推理产品': '310b',
    'Atlas 推理系列产品AI Core': '310p',
    'Atlas 推理系列产品 AI Core': '310p',
    'Atlas 推理系列产品Vector Core': '310p',
    'Atlas 推理系列产品 Vector Core': '310p',
    'Atlas 训练系列产品': '910',
    'Kirin X90': 'x90',
    'Kirin 9030': '9030',
  }

  for (const [label, expected] of Object.entries(labels)) {
    assert.equal(getProductFilterValue(label), expected)
  }
})

test('treats a product family as supported when any member is supported', () => {
  const markdown = `# Abs

## 产品支持情况<a name="support"></a>

- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持

## 功能说明

- Atlas 推理系列产品AI Core：不支持
`

  assert.deepEqual(extractUnsupportedProducts(markdown), [])
})

test('marks a product family unsupported only when every member is unsupported', () => {
  const markdown = `## 产品支持情况
- Ascend 950PR/Ascend 950DT：支持
- Atlas 推理系列产品 AI Core：不支持
- Atlas 推理系列产品 Vector Core：不支持
- Kirin X90：不支持
`

  assert.deepEqual(extractUnsupportedProducts(markdown), ['310p', 'x90'])
})

test('defaults to visible when support information is missing or unrecognized', () => {
  assert.deepEqual(extractUnsupportedProducts('# Page\n\n## 功能说明\n正文'), [])
  assert.deepEqual(
    extractUnsupportedProducts('## 产品支持情况\n- 未知产品：不支持\n'),
    []
  )
})

test('normalizes Markdown, HTML, encoded, and query-string routes', () => {
  assert.equal(normalizeApiRoute('api/目录/Abs.md'), '/api/目录/Abs')
  assert.equal(normalizeApiRoute('/api/%E7%9B%AE%E5%BD%95/Abs.html?q=1#top'), '/api/目录/Abs')
})

test('checks explicit unsupported routes and keeps unknown routes visible', () => {
  const index = { '/api/目录/Abs': ['310p'] }
  assert.equal(isApiRouteUnsupported(index, '/api/目录/Abs.html', '310p'), true)
  assert.equal(isApiRouteUnsupported(index, '/api/目录/Abs', '950'), false)
  assert.equal(isApiRouteUnsupported(index, '/api/目录/Unknown', '310p'), false)
  assert.equal(isApiRouteUnsupported(index, '/api/目录/Abs', 'all'), false)
})

test('filters unsupported leaves, empty groups, and unsupported parent links', () => {
  const source = [
    {
      text: 'Group',
      link: '/api/group',
      items: [
        { text: 'Hidden', link: '/api/hidden' },
        { text: 'Visible', link: '/api/visible' },
      ],
    },
    { text: 'Empty group', items: [{ text: 'Only hidden', link: '/api/only-hidden' }] },
    { text: 'Unknown', link: '/api/unknown' },
  ]
  const index = {
    '/api/group': ['310p'],
    '/api/hidden': ['310p'],
    '/api/only-hidden': ['310p'],
  }

  const filtered = filterSidebarItems(source, '310p', index)

  assert.deepEqual(filtered, [
    { text: 'Group', items: [{ text: 'Visible', link: '/api/visible' }] },
    { text: 'Unknown', link: '/api/unknown' },
  ])
  assert.equal(source[0].link, '/api/group')
  assert.equal(source[0].items.length, 2)
})

test('returns a cloned complete sidebar for the all filter', () => {
  const source = [{ text: 'Page', link: '/api/page' }]
  const filtered = filterSidebarItems(source, 'all', { '/api/page': ['310p'] })

  assert.deepEqual(filtered, source)
  assert.notEqual(filtered, source)
  assert.notEqual(filtered[0], source[0])
})

test('builds an API sidebar config for the selected product without mutating the source', () => {
  const apiSidebarSource = [
    { text: 'Hidden', link: '/api/hidden' },
    { text: 'Visible', link: '/api/visible' },
  ]
  const theme = {
    sidebar: {
      '/guide/': [{ text: 'Guide', link: '/guide/start' }],
      '/api/': apiSidebarSource,
    },
    apiUnsupportedIndex: { '/api/hidden': ['310p'] },
  }

  const filtered = getFilteredSidebarConfig(theme, '310p')

  assert.deepEqual(filtered['/api/'], [{
    text: '1. Visible',
    link: '/api/visible',
    unnumberedText: 'Visible',
  }])
  assert.equal(filtered['/guide/'], theme.sidebar['/guide/'])
  assert.equal(theme.sidebar['/api/'].length, 2)
})

test('renumbers every visible sidebar level after product filtering', () => {
  const source = [{
    text: '2.2.9. Reg vector',
    unnumberedText: 'Reg vector',
    items: [
      { text: '2.2.9.1. Overview', unnumberedText: 'Overview', link: '/api/reg/overview' },
      { text: '2.2.9.2. Hidden one', unnumberedText: 'Hidden one', link: '/api/reg/hidden-one' },
      { text: '2.2.9.3. Types', unnumberedText: 'Types', link: '/api/reg/types' },
      { text: '2.2.9.4. Hidden two', unnumberedText: 'Hidden two', link: '/api/reg/hidden-two' },
      { text: '2.2.9.5. Features', unnumberedText: 'Features', link: '/api/reg/features' },
    ],
  }]
  const index = {
    '/api/reg/hidden-one': ['A3'],
    '/api/reg/hidden-two': ['A3'],
  }

  const filtered = filterSidebarItems(source, 'A3', index)
  const renumbered = renumberSidebarItems(filtered)

  assert.deepEqual(renumbered[0].items.map(item => item.text), [
    '1.1. Overview',
    '1.2. Types',
    '1.3. Features',
  ])
})

test('uses the untouched original sidebar config for the all filter', () => {
  const apiSidebarSource = [{
    text: '2.2.9. Reg vector',
    unnumberedText: 'Reg vector',
    link: '/api/reg/reg',
  }]
  const theme = {
    sidebar: { '/api/': apiSidebarSource },
    apiSidebarSource,
    apiUnsupportedIndex: { '/api/reg/reg': ['A3'] },
  }

  const filtered = getFilteredSidebarConfig(theme, 'all')

  assert.equal(filtered, theme.sidebar)
  assert.equal(filtered['/api/'], apiSidebarSource)
  assert.equal(filtered['/api/'][0].text, '2.2.9. Reg vector')
})

test('keeps the current page as a prev-next anchor while skipping unsupported neighbors', () => {
  const candidates = [
    { text: 'Previous', link: '/api/previous' },
    { text: 'Hidden previous', link: '/api/hidden-previous' },
    { text: 'Current', link: '/api/current' },
    { text: 'Hidden next', link: '/api/hidden-next' },
    { text: 'Next', link: '/api/next' },
  ]
  const index = {
    '/api/hidden-previous': ['310p'],
    '/api/current': ['310p'],
    '/api/hidden-next': ['310p'],
  }

  const filtered = filterNavigationCandidates(
    candidates,
    'api/current.md',
    '310p',
    index
  )

  assert.deepEqual(filtered.map(item => item.text), ['Previous', 'Current', 'Next'])
})

test('hides a group when product filtering leaves only its overview page', () => {
  const source = [{
    text: 'Logic',
    link: '/api/logic/logic',
    items: [
      { text: 'Overview', link: '/api/logic/overview' },
      { text: 'And', link: '/api/logic/and' },
      { text: 'Or', link: '/api/logic/or' },
    ],
  }]
  const index = {
    '/api/logic/and': ['A3'],
    '/api/logic/or': ['A3'],
  }

  assert.deepEqual(filterSidebarItems(source, 'A3', index), [])
})

test('hides a group when all interfaces disappear even if its own page remains supported', () => {
  const source = [{
    text: 'Reduce',
    link: '/api/reduce/reduce',
    items: [
      { text: 'ReduceOne', link: '/api/reduce/reduce-one' },
      { text: 'ReduceTwo', link: '/api/reduce/reduce-two' },
    ],
  }]
  const index = {
    '/api/reduce/reduce-one': ['A3'],
    '/api/reduce/reduce-two': ['A3'],
  }

  assert.deepEqual(filterSidebarItems(source, 'A3', index), [])
})

test('keeps overview and same-name-only groups that were structural before filtering', () => {
  const source = [
    {
      text: 'Overview only',
      link: '/api/overview-only/overview-only',
      items: [{ text: 'Overview', link: '/api/overview-only/overview' }],
    },
    {
      text: 'Self page only',
      items: [{ text: 'Self page', link: '/api/self-page-only/self-page-only' }],
    },
  ]

  assert.deepEqual(filterSidebarItems(source, 'A3', {}), source)
})

test('keeps a group and its overview when at least one interface supports the product', () => {
  const source = [{
    text: 'Logic',
    link: '/api/logic/logic',
    items: [
      { text: 'Overview', link: '/api/logic/introduction' },
      { text: 'Hidden', link: '/api/logic/hidden' },
      { text: 'Visible', link: '/api/logic/visible' },
    ],
  }]
  const index = { '/api/logic/hidden': ['A3'] }

  assert.deepEqual(filterSidebarItems(source, 'A3', index), [{
    text: 'Logic',
    link: '/api/logic/logic',
    items: [
      { text: 'Overview', link: '/api/logic/introduction' },
      { text: 'Visible', link: '/api/logic/visible' },
    ],
  }])
})

test('hides parent groups after their interface-bearing child groups disappear', () => {
  const source = [{
    text: 'Parent',
    items: [{
      text: 'Logic',
      link: '/api/parent/logic/logic',
      items: [
        { text: 'Overview', link: '/api/parent/logic/overview' },
        { text: 'And', link: '/api/parent/logic/and' },
      ],
    }],
  }]
  const index = { '/api/parent/logic/and': ['A3'] }

  assert.deepEqual(filterSidebarItems(source, 'A3', index), [])
})

test('keeps an intrinsic documentation subgroup when sibling interfaces disappear', () => {
  const source = [{
    text: 'Parent',
    link: '/api/parent/parent',
    items: [
      {
        text: 'Documentation',
        link: '/api/parent/documentation/documentation',
        items: [{ text: 'Overview', link: '/api/parent/documentation/overview' }],
      },
      { text: 'Hidden interface', link: '/api/parent/hidden-interface' },
    ],
  }]
  const index = { '/api/parent/hidden-interface': ['A3'] }

  assert.deepEqual(filterSidebarItems(source, 'A3', index), [{
    text: 'Parent',
    link: '/api/parent/parent',
    items: [{
      text: 'Documentation',
      link: '/api/parent/documentation/documentation',
      items: [{ text: 'Overview', link: '/api/parent/documentation/overview' }],
    }],
  }])
})

test('keeps the complete original group structure for the all filter', () => {
  const source = [{
    text: 'Logic',
    link: '/api/logic/logic',
    items: [
      { text: 'Overview', link: '/api/logic/overview' },
      { text: 'And', link: '/api/logic/and' },
    ],
  }]

  assert.deepEqual(
    filterSidebarItems(source, 'all', { '/api/logic/and': ['A3'] }),
    source
  )
})

test('prev-next candidates follow the filtered tree and skip hidden structural pages', () => {
  const candidates = [
    { text: 'Previous', link: '/api/previous' },
    { text: 'Logic', link: '/api/logic/logic' },
    { text: 'Overview', link: '/api/logic/overview' },
    { text: 'Current', link: '/api/logic/current' },
    { text: 'Next', link: '/api/next' },
  ]
  const filteredApiSidebar = [
    { text: 'Previous', link: '/api/previous' },
    { text: 'Next', link: '/api/next' },
  ]

  const filtered = filterNavigationCandidates(
    candidates,
    'api/logic/current.md',
    'A3',
    { '/api/logic/current': ['A3'] },
    filteredApiSidebar
  )

  assert.deepEqual(filtered.map(item => item.text), ['Previous', 'Current', 'Next'])
})

test('prev-next candidates use titles from the renumbered visible tree', () => {
  const candidates = [
    { text: '2. Previous', link: '/api/previous' },
    { text: '5. Next', link: '/api/next' },
  ]
  const filteredApiSidebar = [
    { text: '1. Previous', link: '/api/previous' },
    { text: '2. Next', link: '/api/next' },
  ]

  const filtered = filterNavigationCandidates(
    candidates,
    '/api/previous',
    'A3',
    {},
    filteredApiSidebar
  )

  assert.deepEqual(filtered.map(item => item.text), ['1. Previous', '2. Next'])
})
