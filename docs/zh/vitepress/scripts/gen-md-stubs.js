/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

const { readdirSync, writeFileSync, existsSync, mkdirSync, readFileSync } = require('node:fs')
const { join, resolve } = require('node:path')
const { load } = require('cheerio')

const docsDir = resolve(__dirname, '..', 'docs')
const manifestFile = resolve(__dirname, '..', '.stubs-manifest.json')
const figuresDir = join(docsDir, 'api', 'figures')

const PLACEHOLDER_PNG = Buffer.from(
  'iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mP8/5+hHgAHggJ/PchI7wAAAABJRU5ErkJggg==',
  'base64'
)

function extractBodyContent(html) {
  const bodyMatch = html.match(/<body[^>]*>([\s\S]*)<\/body>/i)
  if (bodyMatch) return bodyMatch[1]
  return html
}

function slugify(text) {
  return text.trim().replace(/\s+/g, '-')
}

function extractHeaders(html) {
  const $ = load(html, null, false)
  const headers = []
  const seen = new Map()
  $('h2,h3,h4,h5,h6').each((_, heading) => {
    const level = Number(heading.tagName.slice(1))
    const text = $(heading).text().trim()
    if (!text) return
    const baseSlug = slugify(text)
    const count = seen.get(baseSlug) || 0
    const slug = count === 0 ? baseSlug : `${baseSlug}-${count}`
    seen.set(baseSlug, count + 1)
    headers.push({ level, title: text, slug, link: `#${slug}` })
  })
  return headers
}

function genHeaderCache(htmlPath, mdPath) {
  try {
    const raw = readFileSync(htmlPath, 'utf-8')
    const html = extractBodyContent(raw)
    const headers = extractHeaders(html)
    if (headers.length > 0) {
      writeFileSync(mdPath + '.header', JSON.stringify(headers, null, 2), 'utf-8')
    }
    return headers.length
  } catch {
    return 0
  }
}

function findAllHtml(dir) {
  const results = []
  const entries = readdirSync(dir, { withFileTypes: true })
  for (const entry of entries) {
    const fullPath = join(dir, entry.name)
    if (entry.isDirectory()) {
      results.push(...findAllHtml(fullPath))
    } else if (entry.name.endsWith('.html')) {
      results.push(fullPath)
    }
  }
  return results
}

function genStubs(subdir, allStubs) {
  const dir = join(docsDir, subdir)
  if (!existsSync(dir)) {
    console.log(`Directory ${subdir}/ not found, skipping`)
    return
  }
  const htmlFiles = findAllHtml(dir)
  const stubs = []
  let totalHeaders = 0
  for (const htmlPath of htmlFiles) {
    const mdPath = htmlPath.replace(/\.html$/, '.md')
    const count = genHeaderCache(htmlPath, mdPath)
    totalHeaders += count
    if (!existsSync(mdPath)) {
      writeFileSync(mdPath, '', 'utf-8')
      stubs.push(mdPath)
    }
  }
  allStubs.push(...stubs)
  console.log(`  ${subdir}/: generated ${stubs.length} .md stubs, ${totalHeaders} headers (${htmlFiles.length} .html files)`)
}

function main() {
  const allStubs = []
  genStubs('api', allStubs)
  genStubs('guide', allStubs)

  writeFileSync(manifestFile, JSON.stringify(allStubs, null, 2), 'utf-8')
  console.log(`Total: ${allStubs.length} .md stubs generated`)

  if (!existsSync(figuresDir)) {
    mkdirSync(figuresDir, { recursive: true })
  }
  if (!existsSync(join(figuresDir, 'placeholder.png'))) {
    writeFileSync(join(figuresDir, 'placeholder.png'), PLACEHOLDER_PNG)
    console.log('Created figures/placeholder.png')
  }
}

if (require.main === module) main()

module.exports = { extractHeaders, slugify }
