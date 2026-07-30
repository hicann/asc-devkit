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
import { mkdtempSync, mkdirSync, rmSync, writeFileSync } from 'node:fs'
import { tmpdir } from 'node:os'
import { join, resolve } from 'node:path'
import test, { after } from 'node:test'

import {
  installRepositoryLinkRewrite,
  rewriteRepositoryHref,
  rewriteRepositoryLinks,
} from '../rewrite-repository-links.mjs'

const repoRoot = mkdtempSync(join(tmpdir(), 'asc-devkit-links-'))
const zhSource = join(repoRoot, 'docs', 'zh', 'guide', 'page.md')
const enSource = join(repoRoot, 'docs', 'en', 'page.md')

function createFile(path, content = '') {
  mkdirSync(resolve(path, '..'), { recursive: true })
  writeFileSync(path, content, 'utf-8')
}

createFile(zhSource)
createFile(enSource)
createFile(join(repoRoot, 'docs', 'zh', 'guide', 'other.md'))
createFile(join(repoRoot, 'docs', 'en', 'other.md'))
createFile(join(repoRoot, 'examples', 'sample', 'README.md'))
mkdirSync(join(repoRoot, 'examples', 'sample dir'), { recursive: true })
createFile(join(repoRoot, 'include', 'api.h'))
mkdirSync(join(repoRoot, 'impl', 'detail'), { recursive: true })
createFile(join(repoRoot, 'tests', 'test.cpp'))
createFile(join(repoRoot, 'tools', 'script.py'))
createFile(join(repoRoot, '.devcontainer', 'README.md'))
createFile(join(repoRoot, 'build.sh'))
createFile(join(repoRoot, 'CONTRIBUTING_en.md'))
createFile(join(repoRoot, 'examples', 'image.png'))

after(() => rmSync(repoRoot, { recursive: true, force: true }))

test('rewrites repository directories as GitCode tree links', () => {
  const html = [
    '<a href="../../../examples/sample">sample</a>',
    '<a href="../../../impl/detail">impl</a>',
  ].join('')

  assert.equal(
    rewriteRepositoryLinks(html, zhSource, repoRoot),
    [
      '<a href="https://gitcode.com/cann/asc-devkit/tree/master/examples/sample">sample</a>',
      '<a href="https://gitcode.com/cann/asc-devkit/tree/master/impl/detail">impl</a>',
    ].join('')
  )
})

test('rewrites repository files as GitCode blob links and preserves suffixes', () => {
  const html = [
    '<a href="../../../include/api.h?plain=1#usage">header</a>',
    '<a href="../../../tests/test.cpp">test</a>',
    '<a href="../../../tools/script.py">tool</a>',
  ].join('')

  assert.equal(
    rewriteRepositoryLinks(html, zhSource, repoRoot),
    [
      '<a href="https://gitcode.com/cann/asc-devkit/blob/master/include/api.h?plain=1#usage">header</a>',
      '<a href="https://gitcode.com/cann/asc-devkit/blob/master/tests/test.cpp">test</a>',
      '<a href="https://gitcode.com/cann/asc-devkit/blob/master/tools/script.py">tool</a>',
    ].join('')
  )
})

test('rewrites repository Markdown files before VitePress changes them to HTML links', () => {
  assert.equal(
    rewriteRepositoryHref('../../../CONTRIBUTING_en.md', zhSource, repoRoot),
    'https://gitcode.com/cann/asc-devkit/blob/master/CONTRIBUTING_en.md'
  )
})

test('supports root files, hidden directories, and encoded path segments', () => {
  assert.equal(
    rewriteRepositoryLinks("<a href='../../build.sh'>build</a>", join(repoRoot, 'docs', 'zh', 'page.md'), repoRoot),
    "<a href='https://gitcode.com/cann/asc-devkit/blob/master/build.sh'>build</a>"
  )
  assert.equal(
    rewriteRepositoryLinks('<a href="../../.devcontainer/README.md">container</a>', enSource, repoRoot),
    '<a href="https://gitcode.com/cann/asc-devkit/blob/master/.devcontainer/README.md">container</a>'
  )
  assert.equal(
    rewriteRepositoryLinks("<a href='../../../examples/sample dir'>sample</a>", zhSource, repoRoot),
    "<a href='https://gitcode.com/cann/asc-devkit/tree/master/examples/sample%20dir'>sample</a>"
  )
  assert.equal(
    rewriteRepositoryLinks("<a href='../../../examples/sample%20dir'>encoded sample</a>", zhSource, repoRoot),
    "<a href='https://gitcode.com/cann/asc-devkit/tree/master/examples/sample%20dir'>encoded sample</a>"
  )
})

test('does not rewrite links inside code or pre blocks', () => {
  const html = [
    '<a href="../../../include/api.h">link</a>',
    '<pre><code>// see <a href="../../../include/api.h">api.h</a> here</code></pre>',
    '<code>href="../../../include/api.h"</code>',
  ].join('')

  const result = rewriteRepositoryLinks(html, zhSource, repoRoot)

  assert.ok(result.includes('https://gitcode.com/cann/asc-devkit/blob/master/include/api.h'))
  assert.equal(
    (result.match(/href="\.\.\/\.\.\/\.\.\/include\/api\.h"/g) || []).length,
    2,
  )
})

test('does not rewrite documentation, image, external, missing, or src links', () => {
  const html = [
    '<a href="./other.md">Chinese documentation</a>',
    '<a href="../other.md">English documentation</a>',
    '<a href="../../../examples/image.png">linked image</a>',
    '<a href="https://gitcode.com/cann/asc-devkit/tree/master/examples/sample">external</a>',
    '<a href="../../../missing/file.h">missing</a>',
    '<a href="../../../../../outside.txt">outside</a>',
    '<img src="../../../include/api.h">',
  ].join('')

  assert.equal(rewriteRepositoryLinks(html, zhSource, repoRoot), html)
})

test('rewrites Markdown link tokens before the VitePress link renderer appends HTML', () => {
  const md = {
    renderer: {
      rules: {
        link_open(tokens, idx) {
          const href = tokens[idx].attrs[tokens[idx].attrIndex('href')][1]
          return `<a href="${href.startsWith('https://') ? href : `${href}.html`}">`
        },
      },
    },
  }
  const token = {
    attrs: [['href', '../../../include/api.h']],
    attrIndex(name) {
      return this.attrs.findIndex(([attribute]) => attribute === name)
    },
  }

  installRepositoryLinkRewrite(md, () => zhSource, repoRoot)

  assert.equal(
    md.renderer.rules.link_open([token], 0, {}, {}, {}),
    '<a href="https://gitcode.com/cann/asc-devkit/blob/master/include/api.h">'
  )
})
