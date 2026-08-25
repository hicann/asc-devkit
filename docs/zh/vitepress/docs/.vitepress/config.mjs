/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

import { defineConfig } from 'vitepress'
import { existsSync, readFileSync, copyFileSync, cpSync, readdirSync } from 'node:fs'
import { execFileSync } from 'node:child_process'
import { relative, resolve } from 'node:path'
import { load as cheerioLoad } from 'cheerio'
import { pagefindPlugin } from 'vitepress-plugin-pagefind'
import {
  installRepositoryLinkRewrite,
  rewriteRepositoryLinks,
} from '../../scripts/rewrite-repository-links.mjs'
import { renderCannFilterTags } from '../../scripts/filter-tags.mjs'
import {
  extractUnsupportedProducts,
  normalizeApiRoute,
} from '../../scripts/api-support.mjs'
import { loadGitTimestamps } from '../../scripts/git-timestamps.mjs'
import { shellQuote } from '../../scripts/shell-utils.mjs'

const docsRoot = resolve(import.meta.dirname, '..')
const repoRoot = resolve(import.meta.dirname, '..', '..', '..', '..', '..')
const sourceDocsRoot = resolve(repoRoot, 'docs', 'zh')

function buildApiUnsupportedIndex(apiRoot) {
  const index = {}

  function visit(directory) {
    for (const entry of readdirSync(directory, { withFileTypes: true })) {
      const fullPath = resolve(directory, entry.name)
      if (entry.isDirectory()) {
        visit(fullPath)
        continue
      }
      if (!entry.isFile() || !entry.name.endsWith('.md')) continue

      const unsupported = extractUnsupportedProducts(readFileSync(fullPath, 'utf-8'))
      if (unsupported.length === 0) continue
      const sourceRelativePath = relative(sourceDocsRoot, fullPath).replace(/\\/g, '/')
      const route = normalizeApiRoute('/' + sourceRelativePath)
      index[route] = unsupported
    }
  }

  visit(apiRoot)
  return index
}

const apiUnsupportedIndex = buildApiUnsupportedIndex(resolve(sourceDocsRoot, 'api'))

function getOriginalSourceFile(relativePath) {
  if (!relativePath) return null
  const normalizedPath = relativePath.replace(/\\/g, '/')
  if (normalizedPath.startsWith('en/')) {
    return resolve(repoRoot, 'docs', normalizedPath)
  }
  return resolve(repoRoot, 'docs', 'zh', normalizedPath)
}

const distDir = resolve(docsRoot, '.vitepress', 'dist')
const pagefindTitleScript = resolve(import.meta.dirname, 'build-pagefind-title-index.mjs')
const pagefindIndexingCommand = `npx pagefind --site ${shellQuote(distDir)} --exclude-selectors ${shellQuote('div.aside, a.header-anchor')} --force-language zh --include-characters ${shellQuote('_:')} && node ${shellQuote(pagefindTitleScript)} ${shellQuote(distDir)}`

function removeSelfRefItems(items) {
  return items.map(item => {
    const entry = { ...item }
    if (entry.items && entry.items.length > 0) {
      const children = removeSelfRefItems(entry.items)
      if (children.length > 0 && children[0].text === entry.text && children[0].link) {
        entry.items = children.slice(1)
      } else {
        entry.items = children
      }
    }
    return entry
  })
}

function addNumbering(items, prefix = '') {
  return items.map((item, i) => {
    const num = prefix ? `${prefix}${i + 1}.` : `${i + 1}.`
    const unnumberedText = item.unnumberedText || item.text
    return {
      ...item,
      unnumberedText,
      text: `${num} ${unnumberedText}`,
      ...(item.items ? { items: addNumbering(item.items, num) } : {}),
    }
  })
}

function parseMdSidebar(filePath, urlPrefix) {
  const content = readFileSync(filePath, 'utf-8')
  const lines = content.split('\n')
  const rawEntries = []

  for (const line of lines) {
    const m = line.match(/^(\s*)-\s+\[(.+?)\]\((.+)\)/)
    if (!m) continue
    const indent = m[1].length
    const text = m[2].replace(/\\([\\`*_{}[\]()#+\-.!|~&<>])/g, '$1')
    let link = m[3].replace(/^\.\//, '')
    link = link.replace(/\.md$/, '')
    rawEntries.push({ indent, text, link })
  }

  if (rawEntries.length === 0) return []

  const indentValues = [...new Set(rawEntries.filter(e => e.indent > 0).map(e => e.indent))].sort((a, b) => a - b)
  const indentUnit = indentValues[0] || 4

  const root = { items: [] }
  const stack = [{ depth: -indentUnit, node: root }]

  for (const entry of rawEntries) {
    const depth = entry.indent
    const current = { text: entry.text, link: urlPrefix + entry.link }

    while (stack.length > 1 && stack[stack.length - 1].depth >= depth) {
      stack.pop()
    }

    const parent = stack[stack.length - 1].node
    if (!parent.items) parent.items = []
    parent.items.push(current)

    stack.push({ depth, node: current })
  }

  function addCollapsed(items) {
    return items.map(item => {
      if (item.items && item.items.length > 0) {
        return { ...item, collapsed: true, items: addCollapsed(item.items) }
      }
      return item
    })
  }

  return addCollapsed(root.items)
}

function filterEmptyPages(items, docsRoot) {
  return items.reduce((acc, item) => {
    const entry = { ...item }
    if (item.items) {
      entry.items = filterEmptyPages(item.items, docsRoot)
    }
    if (item.items && entry.items.length > 0) {
      acc.push(entry)
      return acc
    }
    if (item.link) {
      const relPath = item.link.startsWith('/') ? item.link.slice(1) : item.link
      const headerPath = resolve(docsRoot, relPath + '.md.header')
      if (!existsSync(headerPath)) {
        acc.push(entry)
        return acc
      }
      try {
        const headers = JSON.parse(readFileSync(headerPath, 'utf-8'))
        if (headers.length === 0) return acc
      } catch {
        return acc
      }
    }
    acc.push(entry)
    return acc
  }, [])
}

function runGit(args) {
  try {
    return execFileSync('git', args, {
      cwd: repoRoot,
      encoding: 'utf-8',
      maxBuffer: 16 * 1024 * 1024,
      stdio: ['ignore', 'pipe', 'ignore'],
    }).trim()
  } catch {
    return ''
  }
}

function resolveBuildDate() {
  const configured = process.env.DOCS_BUILD_TIME
  if (configured) {
    const date = new Date(configured)
    if (!Number.isNaN(date.getTime())) return date
  }

  const sourceDateEpoch = Number(process.env.SOURCE_DATE_EPOCH)
  if (Number.isFinite(sourceDateEpoch) && sourceDateEpoch > 0) {
    return new Date(sourceDateEpoch * 1000)
  }
  return new Date()
}

const buildDate = resolveBuildDate()
const buildCommit = process.env.DOCS_COMMIT || process.env.CI_COMMIT_SHA ||
  process.env.GIT_COMMIT || runGit(['rev-parse', 'HEAD'])
const buildRef = process.env.DOCS_REF || process.env.CI_COMMIT_REF_NAME ||
  process.env.GITHUB_REF_NAME || runGit(['branch', '--show-current'])
const buildVersion = process.env.DOCS_VERSION || process.env.CI_COMMIT_TAG ||
  buildRef || buildCommit.slice(0, 12) || '未知'
const documentationBuild = {
  version: buildVersion,
  ref: buildRef,
  commit: buildCommit,
  shortCommit: buildCommit.slice(0, 12),
  dirty: Boolean(runGit(['status', '--porcelain', '--untracked-files=no'])),
  builtAt: buildDate.toISOString(),
  builtAtText: new Intl.DateTimeFormat('zh-CN', {
    timeZone: 'Asia/Shanghai',
    year: 'numeric',
    month: '2-digit',
    day: '2-digit',
    hour: '2-digit',
    minute: '2-digit',
    second: '2-digit',
    hour12: false,
  }).format(buildDate),
}
const gitTimestamps = loadGitTimestamps(runGit)

const sidebarConfigs = [
  { prefix: '/guide/', sourceFileName: 'index.md' },
  { prefix: '/api/', sourceFileName: 'README.md' },
]

const filteredSidebars = {}

for (const { prefix, sourceFileName } of sidebarConfigs) {
  const sourceDir = prefix === '/api/' ? 'api' : 'guide'
  const sourcePath = resolve(repoRoot, 'docs', 'zh', sourceDir, sourceFileName)
  let items = parseMdSidebar(sourcePath, prefix)
  items = removeSelfRefItems(items)
  items = addNumbering(items)
  if (prefix === '/api/') {
    items.push({ text: 'README', link: '/api/README' })
  }
  filteredSidebars[prefix] = filterEmptyPages(items, docsRoot)
}

function normalizeDocRoute(route) {
  let normalized = decodeURIComponent(String(route || '').split('#')[0])
  normalized = normalized.replace(/\.html?$/, '').replace(/\.md$/, '')
  normalized = normalized.replace(/\/$/, '')
  return normalized.startsWith('/') ? normalized : `/${normalized}`
}

function stripSidebarNumber(text) {
  return String(text || '').replace(/^\d+(?:\.\d+)*\.\s*/, '')
}

function buildBreadcrumbMap() {
  const roots = {
    '/api/': { text: 'Ascend C API', link: '/api/api_list' },
    '/guide/': { text: 'Ascend C算子开发指南', link: '/guide/getting_started/ascend_c_overview_and_learning_path' },
  }
  const breadcrumbs = new Map()

  function visit(items, trail, rootLink) {
    for (const item of items) {
      const link = item.link ? normalizeDocRoute(item.link) : ''
      const isRootItem = link && link === rootLink
      const nextTrail = isRootItem
        ? trail
        : [...trail, { text: stripSidebarNumber(item.text), ...(item.link ? { link: item.link } : {}) }]
      if (link) breadcrumbs.set(link, nextTrail)
      if (item.items?.length) visit(item.items, nextTrail, rootLink)
    }
  }

  for (const [prefix, items] of Object.entries(filteredSidebars)) {
    const root = roots[prefix]
    const trail = root ? [root] : []
    const rootLink = root ? normalizeDocRoute(root.link) : ''
    if (rootLink) breadcrumbs.set(rootLink, trail)
    visit(items, trail, rootLink)
  }
  return breadcrumbs
}

function resolveSourceTimestamp(relativePath) {
  const path = String(relativePath || '').replace(/\\/g, '/')
  const candidates = path.startsWith('en/')
    ? [`docs/${path}`]
    : [`docs/zh/${path}`, `docs/${path}`]
  for (const candidate of candidates) {
    const timestamp = gitTimestamps.get(candidate)
    if (timestamp) return timestamp
  }
}

const breadcrumbMap = buildBreadcrumbMap()

function extractBodyContent(html) {
  const bodyMatch = html.match(/<body[^>]*>([\s\S]*)<\/body>/i)
  if (bodyMatch) return bodyMatch[1]
  return html
}

function escapeCodeAngleBrackets(html) {
  return html
    .replace(/`([^`]+)`/g, (_, content) => '`' + content.replace(/</g, '&lt;').replace(/>/g, '&gt;') + '`')
    .replace(/``([\s\S]*?)``/g, (_, content) => '``' + content.replace(/</g, '&lt;').replace(/>/g, '&gt;') + '``')
}

function normalizeHtml(html) {
  html = escapeCodeAngleBrackets(html)
  html = html.replace(/<!--cann-filter:([\w,]+)-->/g, '<cann-filter npu-type="$1">')
  html = html.replace(/<!--\/cann-filter-->/g, '</cann-filter>')
  html = html.replace(/<!--\s*npu\s*=\s*"([^"]*)"\s+(\w+)\s*-->/g, '<cann-filter npu-type="$1">')
  html = html.replace(/<!--\s*end\s+(\w+)\s*-->/g, '</cann-filter>')
  const $ = cheerioLoad(html, {
    xml: {
      xmlMode: false,
      decodeEntities: false,
      withStartIndices: false,
      withEndIndices: false,
    },
  }, false)
  const result = $('body').length ? $('body').html() : $.html()
  if (result !== null && result !== undefined) {
    return balanceTags(result)
  }
  return ''
}

function loadRawHtml(htmlPath) {
  const content = readFileSync(htmlPath, 'utf-8')
  return '\n<!-- RAW_HTML -->\n' + normalizeHtml(extractBodyContent(content)) + '\n'
}

function balanceTags(html) {
  const VOID_TAGS = new Set([
    'area', 'base', 'br', 'col', 'embed', 'hr', 'img', 'input',
    'link', 'meta', 'param', 'source', 'track', 'wbr'
  ])
  const tagRegex = /<\/?([a-zA-Z][a-zA-Z0-9-]*)[^>]*>/g
  const stack = []
  const parts = []
  let lastIdx = 0
  let match
  while ((match = tagRegex.exec(html)) !== null) {
    const full = match[0]
    const tag = match[1].toLowerCase()
    const isClose = full.startsWith('</')
    const isSelfClose = full.endsWith('/>')
    const pos = match.index
    if (isClose) {
      if (stack.length > 0 && stack[stack.length - 1] === tag) {
        stack.pop()
      } else if (stack.includes(tag)) {
        let closeStr = ''
        while (stack.length > 0 && stack[stack.length - 1] !== tag) {
          closeStr += '</' + stack.pop() + '>'
        }
        stack.pop()
        parts.push(html.slice(lastIdx, pos))
        parts.push(closeStr)
        lastIdx = pos
      }
    } else if (!isSelfClose && !VOID_TAGS.has(tag)) {
      stack.push(tag)
    }
  }
  parts.push(html.slice(lastIdx))
  if (stack.length > 0) {
    let closeStr = ''
    for (let i = stack.length - 1; i >= 0; i--) {
      closeStr += '</' + stack[i] + '>'
    }
    parts.push(closeStr)
  }
  return parts.join('')
}

function htmlAsMdPlugin() {
  const root = resolve(import.meta.dirname, '..')

  return {
    name: 'html-as-md',
    enforce: 'pre',
    resolveId(id) {
      if (!id.endsWith('.md') || id.includes('?')) return null

      const absId = id.startsWith(root)
        ? id
        : id.startsWith('/')
          ? resolve(root, '.' + id)
          : resolve(root, id)

      const htmlPath = absId.replace(/\.md$/, '.html')
      if (!existsSync(htmlPath)) return null

      if (existsSync(absId)) return null
      return htmlPath + '?rawhtml=1'
    },
    load(id) {
      if (id.includes('?rawhtml=1')) {
        const htmlPath = id.split('?')[0]
        this.addWatchFile(htmlPath)
        return loadRawHtml(htmlPath)
      }

      if (!id.endsWith('.md') || id.includes('?')) return null
      const htmlPath = id.replace(/\.md$/, '.html')
      if (!existsSync(htmlPath)) return null

      this.addWatchFile(htmlPath)
      return loadRawHtml(htmlPath)
    },
  }
}

function placeholderPlugin() {
  const root = resolve(import.meta.dirname, '..')
  const placeholder = resolve(root, 'api', 'figures', 'placeholder.png')
  return {
    name: 'placeholder-images',
    resolveId(id, importer) {
      if (!importer || !id.match(/\.(png|jpg|jpeg|gif|svg|webp)$/i)) return
      let importerDir = resolve(importer, '..')
      if (importerDir.includes('?')) {
        importerDir = importerDir.split('?')[0]
      }
      const resolved = resolve(importerDir, id)
      if (!existsSync(resolved)) {
        return placeholder
      }
      return resolved
    },
  }
}

function replaceNpuCommentTags(html) {
  const tokenRegex = /<!--\s*(?:npu\s*=\s*"([^"]*)"\s+(\w+)|end\s+(\w+))\s*-->/g
  const parts = []
  const stack = []
  let lastIdx = 0
  let match
  while ((match = tokenRegex.exec(html)) !== null) {
    const pos = match.index
    const npuType = match[1]
    const openId = match[2]
    const closeId = match[3]
    parts.push(html.slice(lastIdx, pos))
    lastIdx = pos + match[0].length
    if (closeId) {
      if (stack.length > 0 && stack[stack.length - 1].id === closeId) {
        stack.pop()
        parts.push('</cann-filter>')
      }
    } else if (npuType && openId) {
      stack.push({ id: openId })
      parts.push(`<cann-filter npu-type="${npuType}">`)
    }
  }
  parts.push(html.slice(lastIdx))
  while (stack.length > 0) {
    stack.pop()
    parts.push('</cann-filter>')
  }
  return parts.join('')
}

function fixCannFilterTags(src) {
  if (!/<(cann-filter|term|ph|__gm__|__ubuf__)\b/i.test(src)) return src

  return src
    .replace(/^[ \t]*<\/?cann-filter\b[^>]*>[ \t]*\n/gm, '')
    .replace(/<\/?cann-filter\b[^>]*>/gi, '')
    .replace(/<(term|ph|__gm__|__ubuf__)\b[^>]*>([\s\S]*?)<\/\1>/gis, '$2')
    .replace(/<(term|ph|__gm__|__ubuf__)\b[^>]*>/gi, '')
    .replace(/<\/(term|ph|__gm__|__ubuf__)>/gi, '')
}

function stripInnerTags(text) {
  return text
    .replace(/<a\s+name\s*=\s*"[^"]*"\s*><\/a>/gi, '')
    .replace(/<\/?span\b[^>]*>/gi, '')
    .replace(/<\/?p\b[^>]*>/gi, '')
    .replace(/<a\s+href\s*=\s*"([^"]*)"\s*>/gi, '<a href="$1">')
    .trim()
}

function convertHtmlTablesToMarkdown(src) {
  let result = src
  result = result.replace(
    /<table\b[\s\S]*?<\/table>/gi,
    (table) => {
      const theadMatch = table.match(/<thead\b[\s\S]*?<\/thead>/i)
      const tbodyMatches = []
      const tbodyRegex = /<tbody\b[\s\S]*?<\/tbody>/gi
      let m
      while ((m = tbodyRegex.exec(table)) !== null) {
        tbodyMatches.push(m[0])
      }

      const rows = []
      if (theadMatch) {
        const thsMatch = theadMatch[0].match(/<th\b[\s\S]*?<\/th>/gi)
        if (thsMatch) {
          rows.push(thsMatch.map(th => stripInnerTags(th.replace(/<th\b[^>]*>/i, '').replace(/<\/th>/i, ''))))
        }
      }

      for (const tbody of tbodyMatches) {
        const trsMatch = tbody.match(/<tr\b[\s\S]*?<\/tr>/gi)
        if (trsMatch) {
          for (const tr of trsMatch) {
            const tdsMatch = tr.match(/<td\b[\s\S]*?<\/td>/gi)
            if (tdsMatch) {
              const cells = tdsMatch.map(td => {
                let text = td.replace(/<td\b[^>]*>/i, '').replace(/<\/td>/i, '')
                text = stripInnerTags(text)
                const linkMatch = text.match(/<a href="([^"]*)">([^<]*)<\/a>/i)
                if (linkMatch) {
                  text = text.replace(linkMatch[0], '[' + linkMatch[2] + '](' + linkMatch[1] + ')')
                }
                return text
              })
              rows.push(cells)
            }
          }
        }
      }

      if (rows.length === 0) return table
      const colCount = Math.max(...rows.map(r => r.length))
      const aligned = rows.map(r => {
        while (r.length < colCount) r.push('')
        return r
      })

      const mdRows = aligned.map((r, i) => '| ' + r.join(' | ') + ' |')
      const sep = '| ' + Array(colCount).fill('---').join(' | ') + ' |'
      return '\n' + mdRows[0] + '\n' + sep + '\n' + mdRows.slice(1).join('\n') + '\n'
    }
  )

  result = result.replace(/<a\s+name\s*=\s*"[^"]*"\s*><\/a>/gi, '')
  result = result.replace(/<br\s*\/?>/gi, '\n')
  return result
}

function fixIndentedBlockquotes(src) {
  return src.replace(/^[ \t]{4,}(?=>)/gm, '')
}

function fixSpanCrossingTableCells(src) {
  let prev
  do {
    prev = src
    src = src.replace(
      /(<span[^>]*>)([\s\S]*?)\|([\s\S]*?)<\/span>/g,
      (match, open, beforePipe, afterPipe) =>
        open + beforePipe + '</span>|' + afterPipe
    )
  } while (src !== prev)
  return src
}

function escapeVueInterpolations(html) {
  return html.replace(
    /(<code\b[\s\S]*?<\/code>|<pre\b[\s\S]*?<\/pre>)|(\{\{[\s\S]*?\}\})/g,
    (match, codeBlock, templateExpr) => {
      if (codeBlock) return codeBlock
      return '<span v-pre>' + templateExpr + '</span>'
    }
  )
}

function addHeadingAnchors(html) {
  const headingRegex = /<h([23456])((?:\s[^>]*)?)>([\s\S]*?)<\/h\1>/gi
  const seen = new Map()
  return html.replace(headingRegex, (match, level, attrs, content) => {
    const text = content.replace(/<[^>]+>/g, '').trim()
    if (!text) return match
    const baseSlug = text.replace(/\s+/g, '-')
    const count = seen.get(baseSlug) || 0
    const slug = count === 0 ? baseSlug : `${baseSlug}-${count}`
    seen.set(baseSlug, count + 1)
    const idAttr = attrs.match(/\sid\s*=\s*["']/) ? '' : ` id="${slug}"`
    const anchor = `<a class="header-anchor" href="#${slug}">&#x200B;</a>`
    return `<h${level}${attrs}${idAttr}>${content}${anchor}</h${level}>`
  })
}

function loadHeaderCache(filePath) {
  const absPath = resolve(import.meta.dirname, '..', filePath)
  const cachePath = absPath + '.header'
  if (!existsSync(cachePath)) return null
  try {
    return JSON.parse(readFileSync(cachePath, 'utf-8'))
  } catch {
    return null
  }
}

export default defineConfig({
  mpa: false,
  srcExclude: ['public/api-source/**'],
  title: 'Ascend C',
  description: 'Ascend C 算子开发文档',
  lang: 'zh-CN',
  ignoreDeadLinks: true,
  lastUpdated: false,

  markdown: {
    math: true,
    config(md) {
      installRepositoryLinkRewrite(
        md,
        env => getOriginalSourceFile(env?.relativePath),
        repoRoot
      )
      const originalRender = md.render.bind(md)
      const originalRenderAsync = md.renderAsync ? md.renderAsync.bind(md) : null
      const processSource = (src) => {
        src = fixCannFilterTags(src)
        if (src.startsWith('    >') || src.includes('\n    >')) {
          src = fixIndentedBlockquotes(src)
        }
        if (src.includes('<span') && src.includes('|')) {
          src = fixSpanCrossingTableCells(src)
        }
        if (src.includes('<table')) {
          src = convertHtmlTablesToMarkdown(src)
        }
        return src
      }
      const processHtml = (html, env) => {
        if (html.includes('{{')) {
          html = escapeVueInterpolations(html)
        }
        return rewriteRepositoryLinks(html, getOriginalSourceFile(env?.relativePath), repoRoot)
      }
      const renderFn = (src, env) => {
        if (src.includes('<!-- RAW_HTML -->')) {
          const htmlContent = src.replace(/^.*<!-- RAW_HTML -->\s*/s, '').replace(/\s*$/, '')
           let html = addHeadingAnchors(htmlContent)

function balanceDivTags(html) {
  const parts = []
  let lastIdx = 0
  const divRegex = /<div\b[^>]*>|<\/div>/gi
  const stack = []
  let m2
  while ((m2 = divRegex.exec(html)) !== null) {
    if (m2[0].startsWith('</')) {
      if (stack.length === 0) {
        parts.push(html.slice(lastIdx, m2.index))
        lastIdx = m2.index + m2[0].length
      } else { stack.pop() }
    } else { stack.push(true) }
  }
  parts.push(html.slice(lastIdx))
  if (stack.length > 0) parts.push('</div>'.repeat(stack.length))
  return parts.join('')
}

          html = replaceNpuCommentTags(html)
          html = renderCannFilterTags(html)
          html = html.replace(
            /<tr\b[^>]*>((?:(?!<\/tr>)[\s\S])*?)<\/tr>/gi,
            (rowMatch) => {
              const filterOpen = rowMatch.match(/<td\b[^>]*>\s*<(?:div|span)\s+data-filter\s*=\s*"([^"]+)">/i)
              if (!filterOpen) return rowMatch
              const filterClose = rowMatch.match(/<\/(?:div|span)>\s*<\/td>/)
              if (!filterClose) return rowMatch
              const filterVal = filterOpen[1]
              let newRow = rowMatch
                .replace(/<(?:div|span)\s+data-filter\s*=\s*"[^"]*">\s*/ig, '')
                .replace(/\s*<\/(?:div|span)>/ig, '')
              newRow = newRow.replace(
                /<tr\b([^>]*)>/i,
                (m, attrs) => {
                  const existing = m.match(/\sdata-filter\s*=\s*"([^"]*)"/i)
                  if (existing) {
                    return m.replace(existing[0], ` data-filter="${existing[1]},${filterVal}"`)
                  }
                  return `<tr${attrs} data-filter="${filterVal}">`
                }
              )
              return newRow
            }
          )
          html = html.replace(/<a\s+name\s*=\s*"([^"]+)"\s*><\/a>/gi, '<span id="$1"></span>')
          html = escapeVueInterpolations(html)

          const renderMathTex = (tex, display) => {
            const rule = display ? 'math_block' : 'math_inline'
            if (md.renderer.rules[rule]) {
              try {
                return md.renderer.rules[rule]([{ content: tex }], 0)
              } catch {}
            }
            return display ? `$$${tex}$$` : `$${tex}$`
          }

          const codeBlocks = []
          html = html.replace(/<(code|pre)\b[^>]*>[\s\S]*?<\/\1>/gi, m => {
            codeBlocks.push(m); return `\x00CB${codeBlocks.length - 1}\x00`
          })

          html = html.replace(/\$\$([\s\S]*?)\$\$/g, (match) => {
            let inside = match.slice(2, -2)
            inside = inside.replace(/<em>/g, '_').replace(/<\/em>/g, '_')
            inside = inside.replace(/&(amp|gt|lt);/g, (m, e) => ({ amp: '&', gt: '>', lt: '<' })[e])
            inside = inside.replace(/\\text\{([^}]*)\}/g, (m, c) => '\\text{' + c.replace(/(?<!\\)_/g, '\\_') + '}')
            inside = inside.replace(
              /(<\/[a-z][a-z0-9]*\s*>)\s*(<[a-z][a-z0-9]*\b[^>]*>)/gi,
              '$$$$\n$1\n$2\n$$$$'
            )
            return '$$' + inside.trim() + '$$'
          })
          html = html.replace(/\$(?!\{[\w]+\})([^$\n]+?)\$/g, (match) => {
            let inside = match.slice(1, -1)
            inside = inside.replace(/<em>/g, '_').replace(/<\/em>/g, '_')
            inside = inside.replace(/<[^>]+>/g, '')
            inside = inside.replace(/&(amp|gt|lt);/g, (m, e) => ({ amp: '&', gt: '>', lt: '<' })[e])
            inside = inside.replace(/\\text\{([^}]*)\}/g, (m, c) => '\\text{' + c.replace(/(?<!\\)_/g, '\\_') + '}')
            return '$' + inside + '$'
          })

          html = html.replace(/\$\$([\s\S]*?)\$\$/g, (_, tex) => renderMathTex(tex, true))
          html = html.replace(/\$(?!\{[\w]+\})([^$\n]+?)\$/g, (_, tex) => renderMathTex(tex, false))

          html = html.replace(/<mjx-assistive-mml[\s\S]*?<\/mjx-assistive-mml>/gi, '')

          html = html.replace(/\x00CB(\d+)\x00/g, (_, i) => codeBlocks[+i])

          html = html.replace(
            /href="https:\/\/gitcode\.com\/cann\/asc-devkit\/blob\/master\/docs\/(?:zh\/)?(api|guide)\/([^"]+)\.md(#[^"]*)?"/gi,
            (_, type, path, hash) => `href="/${type}/${path}.html${hash || ''}"`
          )

          html = balanceDivTags(html)
          html = rewriteRepositoryLinks(html, getOriginalSourceFile(env?.relativePath), repoRoot)

          return `<div v-pre>\n${html}\n</div>`
        }
        src = processSource(src)
        let html = originalRender(src, env)
        html = processHtml(html, env)
        return `<div v-pre>${html}</div>`
      }
      md.render = function (src, env) {
        return renderFn(src, env)
      }
      if (originalRenderAsync) {
        md.renderAsync = async function (src, env) {
          return renderFn(src, env)
        }
      }
    },
  },

  transformPageData(pageData) {
    const cache = loadHeaderCache(pageData.filePath)
    if (cache && cache.length > 0) {
      pageData.outlineHeaders = cache
    }
    const route = normalizeDocRoute(pageData.relativePath)
    const breadcrumbs = breadcrumbMap.get(route)
    if (breadcrumbs?.length) {
      pageData.frontmatter.breadcrumbs = breadcrumbs
    }
    const sourceTimestamp = resolveSourceTimestamp(pageData.relativePath)
    if (sourceTimestamp) {
      pageData.lastUpdated = sourceTimestamp
    }
  },

  buildConcurrency: 1,
  metaChunk: true,

  transformHtml(code) {
    code = code.replace(/href="\/\/pagefind/g, 'href="/pagefind')
    code = code.replace('<article class="markdown-body">', '<article class="markdown-body" data-pagefind-body>')
    return code
  },

  vite: {
    sourcemap: false,
    build: {
      chunkSizeWarningLimit: 2000,
      rollupOptions: {
        external: ['/pagefind/pagefind.js'],
        output: {
          manualChunks(id) {
            if (id.includes('node_modules')) return 'vendor'
          },
        },
      },
    },
    resolve: {
      preserveSymlinks: true,
    },
    plugins: [htmlAsMdPlugin(), placeholderPlugin(), pagefindPlugin({
      btnPlaceholder: '搜索',
      placeholder: '简易搜索，暂不支持多语言和特殊符号',
      emptyText: '未找到结果',
      heading: '共: {{searchResult}} 条结果',
      forceLanguage: 'zh',
      indexingCommand: pagefindIndexingCommand,
    }), {
      name: 'copy-figures-to-dist',
      closeBundle() {
        const dst = resolve(docsRoot, '.vitepress', 'dist', 'figures')
        for (const sub of ['zh/figures', 'zh/guide/figures', 'zh/api/figures']) {
          const src = resolve(docsRoot, sub)
          if (existsSync(src)) cpSync(src, dst, { recursive: true })
        }
      },
    }, {
      name: 'vitepress-override-search-vue',
      enforce: 'post',
      config: () => ({
        resolve: {
          alias: {
            './VPNavBarSearch.vue': resolve(import.meta.dirname, 'theme', 'Search.vue'),
          },
        },
      }),
    }, {
      name: 'patch-vitepress-api-navigation',
      enforce: 'pre',
      resolveId(source) {
        if (source === 'virtual:api-navigation-filter') {
          return resolve(import.meta.dirname, 'theme', 'api-navigation-filter.mjs')
        }
      },
      transform(code, id) {
        const normalizedId = id.replace(/\\/g, '/')
        const importLine = "import { getProductFilteredNavigationCandidates, getProductFilteredSidebarConfig } from 'virtual:api-navigation-filter';\n"

        if (normalizedId.includes('vitepress/dist/client/theme-default/composables/sidebar.js')) {
          const result = code.replace(
            'const sidebarConfig = theme.value.sidebar;',
            'const sidebarConfig = getProductFilteredSidebarConfig(theme.value);'
          )
          return result === code ? null : importLine + result
        }

        if (normalizedId.includes('vitepress/dist/client/theme-default/composables/prev-next.js')) {
          const original = "const candidates = uniqBy(links, (link) => link.link.replace(/[?#].*$/, ''));"
          const replacement = "const candidates = getProductFilteredNavigationCandidates(uniqBy(links, (link) => link.link.replace(/[?#].*$/, '')), page.value.relativePath, theme.value);"
          const result = code.replace(original, replacement)
          return result === code ? null : importLine + result
        }

        return null
      },
    }, {
      name: 'patch-vitepress-sidebar',
      enforce: 'pre',
      transform(code, id) {
        if (!id.includes('vitepress')) return null
        let result = code
        if (id.includes('VPSidebar.vue')) {
          result = result.replace(
            '<VPSidebarGroup :items="sidebarGroups" :key="key" />',
            '<ClientOnly><VPSidebarGroup :items="sidebarGroups" :key="key" /></ClientOnly>'
          )
        } else if (id.includes('VPSidebarItem')) {
          result = result.replace('depth < 5', 'true')
          result = result.replace('props.depth + 2 === 7', 'props.depth + 2 >= 7')
          result = result.replace(
            '<div v-if="item.items && item.items.length" class="items">',
            '<div v-if="item.items && item.items.length && !collapsed" class="items">'
          )
        }
        if (result === code) return null
        return result
      },
    }],
    vue: {
      template: {
        compilerOptions: {
          onError: (error) => {
            if ([2, 21, 46].includes(error.code)) return
            if (error.message && error.message.includes('missing end tag')) return
          },
        },
      },
    },
  },

  head: [],

  themeConfig: {
    documentationBuild,

    nav: [
      { text: '首页', link: '/' },
      { text: 'AscendC算子开发指南', link: '/guide/getting_started/ascend_c_overview_and_learning_path' },
      { text: 'Ascend C API', link: '/api/README' },
    ],

    search: {
      provider: 'algolia',
    },

    sidebar: filteredSidebars,
    apiUnsupportedIndex,

    outline: false,

    docFooter: {
      prev: '上一页',
      next: '下一页',
    },

    lastUpdated: {
      text: '源文档更新于',
      formatOptions: {
        dateStyle: 'medium',
        timeStyle: 'short',
        forceLocale: true,
      },
    },

    footer: {
      message: '免责声明：本站内容由 <a href="https://gitcode.com/cann/asc-devkit/" target="_blank">asc-devkit</a> 仓 master 分支自动编译生成，属于持续开发版本，可能存在缺陷，仅供预览与参考。如需稳定及商用资料，请查阅官方 <a href="https://www.hiascend.com/zh/document" target="_blank">昇腾社区</a>。',
      copyright: 'Copyright Huawei Technologies Co., Ltd.',
    },
  },
})
