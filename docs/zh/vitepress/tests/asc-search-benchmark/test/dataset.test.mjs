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
import { readFile } from 'node:fs/promises'
import { test } from 'node:test'

const dataset = JSON.parse(await readFile(new URL('../cases/ascendc.json', import.meta.url), 'utf8'))

test('dataset has unique and complete cases', () => {
  assert.equal(dataset.cases.length, 170)
  assert.equal(new Set(dataset.cases.map(item => item.id)).size, dataset.cases.length)
  for (const item of dataset.cases) {
    assert.ok(item.id)
    assert.ok(item.query)
    assert.ok(item.category)
    assert.ok(Array.isArray(item.expect.targets))
    for (const key of [
      'descriptionIncludes',
      'highlightIncludes',
      'titleHighlightIncludes',
      'descriptionHighlightIncludes',
      'sectionTitleIncludes',
      'sectionHighlightIncludes',
    ]) {
      if (item.expect[key] === undefined) continue
      assert.ok(Array.isArray(item.expect[key]))
      assert.ok(item.expect[key].length > 0)
      assert.ok(item.expect[key].every(value => typeof value === 'string' && value))
    }
    if (item.expect.sectionHash !== undefined) {
      assert.match(item.expect.sectionHash, /^#./)
    }
    if (item.expect.maxFirstRelevantRank !== undefined) {
      assert.ok(Number.isInteger(item.expect.maxFirstRelevantRank))
      assert.ok(item.expect.maxFirstRelevantRank >= 1 && item.expect.maxFirstRelevantRank <= 10)
    }
    if (item.expect.minFirstRelevantRank !== undefined) {
      assert.ok(Number.isInteger(item.expect.minFirstRelevantRank))
      assert.ok(item.expect.minFirstRelevantRank >= 1 && item.expect.minFirstRelevantRank <= 10)
      if (item.expect.maxFirstRelevantRank !== undefined) {
        assert.ok(item.expect.minFirstRelevantRank <= item.expect.maxFirstRelevantRank)
      }
    }
    if (item.productFilter !== undefined) {
      assert.ok(['all', '950', 'A3', '910b', '910', '310p', '310b', 'x90', '9030'].includes(item.productFilter))
    }
    if (item.expect.excludedTargets !== undefined) assert.ok(Array.isArray(item.expect.excludedTargets))
    for (const target of [...item.expect.targets, ...(item.expect.excludedTargets || [])]) {
      assert.equal(['exact', 'prefix', 'contains'].filter(key => key in target).length, 1)
      assert.ok([1, 2, 3].includes(target.grade))
    }
  }
})

test('dataset covers positive and negative search intents', () => {
  assert.ok(dataset.cases.some(item => item.expect.targets.length === 0))
  assert.ok(dataset.cases.some(item => item.expect.targets.length > 0))
  assert.ok(dataset.cases.some(item => item.category === 'api-case-sensitive'))
  assert.ok(dataset.cases.some(item => item.category === 'zh-content'))
})

test('dataset covers compiler option identifier regressions', () => {
  const target = '/guide/编程指南/编译与运行/算子编译/AI-Core算子编译基本用法'
  for (const query of ['NO_OVER', 'NO_OVERLAP_IN_MULTI_REPEAT', 'no over', 'no-over', 'NO_O', 'NO_OV', 'NO_OVE']) {
    const item = dataset.cases.find(candidate => candidate.query === query)
    assert.ok(item, `missing case for ${query}`)
    assert.equal(item.expect.minResults, 1)
    assert.equal(item.expect.maxResults, 1)
    assert.deepEqual(item.expect.targets, [{ exact: target, grade: 3 }])
    assert.deepEqual(item.expect.descriptionIncludes, ['NO_OVERLAP_IN_MULTI_REPEAT', '内存同步指令'])
  }
  assert.equal(dataset.cases.find(item => item.query === 'NO_OVER').expect.sectionHash, '#内置编译宏开关')
  assert.deepEqual(dataset.cases.find(item => item.query === 'NO_OVER').expect.highlightIncludes, ['NO_OVER'])
  assert.deepEqual(dataset.cases.find(item => item.query === 'NO_OVER').expect.sectionTitleIncludes, ['内置编译宏开关'])
})

test('dataset covers omitted predefined macro decorators', () => {
  for (const query of ['__NPU_ARCH__', '_NPU_ARCH', 'NPU_ARCH']) {
    const item = dataset.cases.find(candidate => candidate.query === query)
    assert.ok(item, `missing case for ${query}`)
    assert.equal(item.expect.maxFirstRelevantRank, 1)
    assert.equal(item.expect.sectionHash, '#npu-arch')
    assert.deepEqual(item.expect.targets[0], {
      exact: '/guide/编程指南/语言扩展层/SIMD-BuiltIn关键字',
      grade: 3,
    })
  }
})

test('dataset covers existing section navigation', () => {
  const item = dataset.cases.find(candidate => candidate.id === 'zh-section-memory-lifecycle')
  assert.ok(item)
  assert.equal(item.expect.sectionHash, '#内存生命周期管理：AllocTensor/FreeTensor解决读后写冲突')
  assert.deepEqual(item.expect.sectionTitleIncludes, ['内存生命周期管理：AllocTensor/FreeTensor解决读后写冲突'])
  assert.deepEqual(item.expect.sectionHighlightIncludes, ['内存生命周期管理'])
})

test('dataset covers humanized identifiers and incomplete prefixes', () => {
  for (const category of ['identifier-separator', 'identifier-prefix', 'datatype-separator', 'parameter-separator']) {
    assert.ok(dataset.cases.some(item => item.category === category), `missing category ${category}`)
  }
  for (const query of ['uint32 t', 'fp8 e4m3fn t', 'dst repeat stride', 'GET_TIL', 'vector_uint']) {
    assert.ok(dataset.cases.some(item => item.query === query), `missing case for ${query}`)
  }
})

test('dataset covers kernel launch syntax variants', () => {
  const targets = [
    { exact: '/api/SIMT-API/SIMD_SIMT_hybrid_programming_intro/extended_syntax/kernel_function_config', grade: 3 },
    { exact: '/guide/编程指南/语言扩展层/SIMD与SIMT混合编程BuiltIn关键字', grade: 3 },
    { exact: '/api/SIMT-API/SIMT_programming_intro/extended_syntax/kernel_function_config', grade: 3 },
  ]
  for (const query of ['<<<', '<<<>>>', '<<<...>>>']) {
    const item = dataset.cases.find(candidate => candidate.query === query)
    assert.ok(item, `missing case for ${query}`)
    assert.deepEqual(item.expect.targets, targets)
    assert.deepEqual(item.expect.highlightIncludes, ['<<<'])
    assert.equal(item.expect.sectionHash, '#<<<>>>调用')
  }

  const mobileHighlight = dataset.cases.find(item => item.id === 'syntax-kernel-launch-prefix-second-visible-mobile')
  assert.ok(mobileHighlight)
  assert.equal(mobileHighlight.viewportWidth, 375)
  assert.deepEqual(mobileHighlight.expect.highlightIncludes, ['<<<'])
  assert.deepEqual(mobileHighlight.expect.sectionTitleIncludes, ['<<<>>>调用'])
  assert.deepEqual(mobileHighlight.expect.sectionHighlightIncludes, ['<<<'])
  assert.deepEqual(mobileHighlight.expect.targets, [targets[1]])

  const prefix = dataset.cases.find(item => item.id === 'syntax-kernel-launch-prefix')
  assert.deepEqual(prefix.expect.sectionTitleIncludes, ['<<<>>>调用'])
  assert.deepEqual(prefix.expect.sectionHighlightIncludes, ['<<<'])
})

test('dataset covers exact English words outranking spelling corrections', () => {
  const item = dataset.cases.find(candidate => candidate.query === 'block')
  assert.ok(item)
  assert.equal(item.category, 'technical-concept')
  assert.equal(item.expect.targets.length, 6)
  assert.ok(item.expect.targets.every(target => target.exact?.includes('/cooperative_groups/thread_block')))
})

test('dataset targets only current multicore and lowercase API routes', () => {
  const targets = dataset.cases.flatMap(item => item.expect.targets)
    .flatMap(target => [target.exact, target.prefix, target.contains])
    .filter(Boolean)
  assert.ok(targets.some(target => target.includes('/多核Tiling切分/')))
  for (const staleRoute of [
    '/多核-Tiling切分/',
    '/isnan1',
    '/isinf1',
    '/isfinite1',
    '/abs1',
    '/sqrt1',
    '/max1',
    '/max-267',
    '/min1',
    '/min-268',
  ]) {
    assert.equal(targets.some(target => target.includes(staleRoute)), false, `stale target ${staleRoute}`)
  }
})

test('dataset covers product-filtered API and guide search', () => {
  const excluded = dataset.cases.find(item => item.id === 'product-filter-310p-excludes-datastorebarrier')
  assert.equal(excluded.productFilter, '310p')
  assert.deepEqual(excluded.expect.excludedTargets, [{
    exact: '/api/AI-CPU-API/DataStoreBarrier',
    grade: 3,
  }])

  const supported = dataset.cases.find(item => item.id === 'product-filter-950-keeps-datastorebarrier')
  assert.equal(supported.productFilter, '950')
  assert.deepEqual(supported.expect.targets[0], {
    exact: '/api/AI-CPU-API/DataStoreBarrier',
    grade: 3,
  })

  const guide = dataset.cases.find(item => item.id === 'product-filter-keeps-guide-results')
  assert.equal(guide.productFilter, '310p')
  assert.ok(guide.expect.targets[0].exact.startsWith('/guide/'))

  const definition = dataset.cases.find(item => item.id === 'product-filter-950-prefers-copy-align-definition')
  assert.equal(definition.productFilter, '950')
  assert.equal(definition.expect.maxFirstRelevantRank, 1)
  assert.deepEqual(definition.expect.titleHighlightIncludes, ['asc_copy_gm2ub_align'])
  assert.deepEqual(definition.expect.descriptionHighlightIncludes, ['asc_copy_gm2ub_align'])

  const reference = dataset.cases.find(item => item.id === 'product-filter-950-copy-align-reference-context')
  assert.deepEqual(reference.expect.descriptionIncludes, ['该接口仅对asc_copy_gm2ub_align接口有效'])
  assert.equal(reference.expect.sectionHash, '#功能说明')

  for (const id of [
    'product-filter-950-demotes-copy-align-catalog',
    'product-filter-950-demotes-vector-catalog',
  ]) {
    const item = dataset.cases.find(candidate => candidate.id === id)
    assert.equal(item.expect.minFirstRelevantRank, 3)
    assert.equal(item.expect.maxFirstRelevantRank, 10)
  }

  const catalog = dataset.cases.find(item => item.id === 'catalog-title-remains-authoritative')
  assert.equal(catalog.expect.maxFirstRelevantRank, 1)
  assert.deepEqual(catalog.expect.titleHighlightIncludes, ['矢量数据搬运'])
})
