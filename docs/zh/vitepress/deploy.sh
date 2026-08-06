#!/bin/bash
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
LOG_FILE="$SCRIPT_DIR/deploy.log"

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

log()  { echo -e "${GREEN}[INFO]${NC} $(date '+%H:%M:%S') $*" | tee -a "$LOG_FILE"; }
warn() { echo -e "${YELLOW}[WARN]${NC} $(date '+%H:%M:%S') $*" | tee -a "$LOG_FILE"; }
err()  { echo -e "${RED}[ERROR]${NC} $(date '+%H:%M:%S') $*" | tee -a "$LOG_FILE"; }

# ============================================================
# 参数解析
# ============================================================
MODE="build"
NGINX_PORT=80
INSTALL_NODE=false
SERVE_PORT=38090
while [[ $# -gt 0 ]]; do
    case "$1" in
        --nginx)    MODE="nginx"; shift ;;
        --port)     NGINX_PORT="$2"; shift 2 ;;
        --preview)  MODE="preview"; shift ;;
        --serve)    MODE="serve"; SERVE_PORT="$2"; shift 2 ;;
        --help|-h)
            echo "用法: $0 [选项]"
            echo ""
            echo "选项:"
            echo "  (无参数)        构建静态站点到docs/.vitepress/dist"
            echo "  --nginx         构建并配置Nginx，注册为systemd开机自启服务"
            echo "  --port <端口>   Nginx监听端口 (默认 80)"
            echo "  --preview       构建并以预览模式启动 (开发用)"
            echo "  --serve <端口>   构建并在指定端口提供静态服务 (默认 38090)"
            echo "  --help, -h      显示帮助"
            exit 0
            ;;
        *)          err "未知参数: $1"; exit 1 ;;
    esac
done

> "$LOG_FILE"

# ============================================================
# 0. 系统检测
# ============================================================
detect_os() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        OS=$ID
        OS_VERSION=$VERSION_ID
    elif [ -f /etc/redhat-release ]; then
        OS="centos"
        OS_VERSION=$(rpm -q --qf "%{VERSION}" $(rpm -q --whatprovides redhat-release) 2>/dev/null || echo "7")
    else
        OS="unknown"
        OS_VERSION="unknown"
    fi
    log "检测到系统: $OS $OS_VERSION"
}

# ============================================================
# 1. 安装Node.js (>=18)
# ============================================================
install_nodejs() {
    if command -v node &>/dev/null; then
        NODE_VER=$(node -v | sed 's/v//' | cut -d. -f1)
        if [ "$NODE_VER" -ge 18 ]; then
            log "Node.js $(node -v)已满足要求 (>=18)"
            return 0
        fi
        warn "当前Node.js $(node -v)版本过低，需要 >=18，正在升级..."
    fi

    log "安装Node.js 18 LTS..."

    case "$OS" in
        ubuntu|debian)
            log "使用NodeSource安装Node.js 18..."
            if ! command -v curl &>/dev/null; then
                apt-get update -y && apt-get install -y curl gnupg
            fi
            curl -fsSL https://deb.nodesource.com/setup_18.x | bash -
            apt-get install -y nodejs
            ;;

        centos|rhel|fedora|tencentos|anolis|openEuler)
            log "使用NodeSource安装Node.js 18..."
            if ! command -v curl &>/dev/null; then
                yum install -y curl
            fi
            curl -fsSL https://rpm.nodesource.com/setup_18.x | bash -
            yum install -y nodejs
            ;;

        *)
            err "不支持的系统: $OS，请手动安装Node.js >=18"
            exit 1
            ;;
    esac

    log "Node.js安装完成: $(node -v)"
    log "npm版本: $(npm -v)"
}

# ============================================================
# 2. 安装项目依赖
# ============================================================
install_deps() {
    cd "$SCRIPT_DIR"

    if [ ! -f "package.json" ]; then
        err "未找到package.json，请在vitepress目录下执行本脚本"
        exit 1
    fi

    if [ ! -d "node_modules" ]; then
        log "安装项目依赖..."
        if command -v pnpm &>/dev/null; then
            pnpm install 2>&1 | tee -a "$LOG_FILE"
        else
            log "pnpm未安装，使用npm..."
            npm install --legacy-peer-deps 2>&1 | tee -a "$LOG_FILE"
        fi
        log "依赖安装完成"
    else
        log "依赖已存在，跳过安装"
    fi
}

# ============================================================
# 3. 安装Python依赖
# ============================================================
install_python_deps() {
    log "检查Python依赖 (cmarkgfm, pygments)..."

    local missing=0
    if ! python3 -c "import cmarkgfm" &>/dev/null; then missing=1; fi
    if ! python3 -c "import pygments" &>/dev/null; then missing=1; fi

    if [ "$missing" -eq 0 ]; then
        log "Python依赖已满足"
        return 0
    fi

    log "安装Python依赖..."
    cd "$SCRIPT_DIR"

    local pip_cmd=""
    if command -v pip3 &>/dev/null; then
        pip_cmd="pip3"
    elif command -v pip &>/dev/null; then
        pip_cmd="pip"
    else
        err "未找到pip，请手动安装: pip install -r requirements.txt"
        exit 1
    fi

    $pip_cmd install -r requirements.txt --break-system-packages -i https://mirrors.huaweicloud.com/repository/pypi/simple 2>&1 | tee -a "$LOG_FILE"
    log "Python依赖安装完成"
}

# ============================================================
# 4. 构建静态站点
# ============================================================
build_docs() {
    cd "$SCRIPT_DIR"

    log "开始构建VitePress站点..."
    npm run docs:prebuild 2>&1 | tee -a "$LOG_FILE"
    npx cross-env NODE_OPTIONS=--max-old-space-size=14336 vitepress build docs 2>&1 | tee -a "$LOG_FILE"

    DIST_DIR="$SCRIPT_DIR/docs/.vitepress/dist"
    if [ ! -d "$DIST_DIR" ]; then
        err "构建失败: 未生成 $DIST_DIR"
        exit 1
    fi
    log "构建成功! 产物目录: $DIST_DIR"
}

# ============================================================
# 5. Nginx部署
# ============================================================
deploy_nginx() {
    DIST_DIR="$SCRIPT_DIR/docs/.vitepress/dist"
    NGINX_RELEASES_ROOT="/usr/share/nginx/html/ascendc-docs-releases"
    NGINX_CURRENT="/usr/share/nginx/html/ascendc-docs-current"
    NGINX_PREVIOUS="/usr/share/nginx/html/ascendc-docs-previous"
    NGINX_CONF="/etc/nginx/conf.d/ascendc-docs.conf"

    log "=== Nginx部署模式 ==="

    # 安装Nginx
    if ! command -v nginx &>/dev/null; then
        log "安装Nginx..."
        case "$OS" in
            ubuntu|debian)     apt-get update -y && apt-get install -y nginx ;;
            centos|rhel|fedora|tencentos|anolis|openEuler)
                yum install -y epel-release 2>/dev/null || true
                yum install -y nginx
                ;;
            *)                 err "请手动安装Nginx"; exit 1 ;;
        esac
    fi

    # 先完成整个release，再原子切换服务目录，避免部署期间出现半成品站点。
    RELEASE_ID="$(date '+%Y%m%d%H%M%S')-$$"
    RELEASE_DIR="$NGINX_RELEASES_ROOT/$RELEASE_ID"
    NEXT_LINK="${NGINX_CURRENT}.next"
    PREVIOUS_NEXT_LINK="${NGINX_PREVIOUS}.next"

    log "拷贝构建产物到 $RELEASE_DIR"
    mkdir -p "$NGINX_RELEASES_ROOT"
    mkdir "$RELEASE_DIR"
    cp -a "$DIST_DIR"/. "$RELEASE_DIR"/

    if [ -e "$NEXT_LINK" ] || [ -L "$NEXT_LINK" ] || \
       [ -e "$PREVIOUS_NEXT_LINK" ] || [ -L "$PREVIOUS_NEXT_LINK" ]; then
        err "临时发布链接已存在，请检查 $NEXT_LINK 和 $PREVIOUS_NEXT_LINK"
        exit 1
    fi

    # 保留发布前的完整版本，为旧页面请求的哈希资源提供回退。
    if [ -L "$NGINX_CURRENT" ]; then
        CURRENT_RELEASE="$(readlink -f "$NGINX_CURRENT")"
        ln -s "$CURRENT_RELEASE" "$PREVIOUS_NEXT_LINK"
        mv -Tf "$PREVIOUS_NEXT_LINK" "$NGINX_PREVIOUS"
    fi

    ln -s "$RELEASE_DIR" "$NEXT_LINK"
    mv -Tf "$NEXT_LINK" "$NGINX_CURRENT"
    log "文件发布完成, 共 $(find "$RELEASE_DIR" -type f | wc -l) 个文件"

    # 生成Nginx配置
    log "生成Nginx配置: $NGINX_CONF"
    cat > "$NGINX_CONF" << NGINX_EOF
# Ascend C Documentation - VitePress Static Site
server {
    listen       ${NGINX_PORT};
    server_name  _;
    root         ${NGINX_CURRENT};
    index        index.html;

    charset utf-8;

    # 日志
    access_log  /var/log/nginx/ascendc-docs-access.log;
    error_log   /var/log/nginx/ascendc-docs-error.log;

    # HTML和扩展名省略路由必须重新验证，避免客户端拿到旧路由表。
    location / {
        try_files \$uri \$uri.html \$uri/ \$uri/index.html =404;
        add_header Cache-Control "no-cache, must-revalidate" always;
    }

    # VitePress在页面模块加载失败时会重新请求该文件，禁止缓存旧版本。
    location = /hashmap.json {
        try_files \$uri =404;
        add_header Cache-Control "no-cache, no-store, must-revalidate" always;
    }

    # VitePress assets文件名包含内容哈希，可以长期缓存。
    location ^~ /assets/ {
        try_files \$uri @previous_assets;
        add_header Cache-Control "public, max-age=31536000, immutable" always;
    }

    location @previous_assets {
        root ${NGINX_PREVIOUS};
        try_files \$uri =404;
        add_header Cache-Control "public, max-age=31536000, immutable" always;
    }

    # Pagefind索引路径不含构建哈希，必须跟随当前release更新。
    location ^~ /pagefind/ {
        try_files \$uri =404;
        add_header Cache-Control "no-cache, must-revalidate" always;
    }

    # 非哈希媒体资源使用有限时长缓存。
    location ~* \.(png|jpg|jpeg|gif|ico|svg|woff|woff2|ttf|eot)$ {
        try_files \$uri =404;
        add_header Cache-Control "public, max-age=2592000" always;
    }

    error_page 404 /404.html;
    location = /404.html {
        internal;
        add_header Cache-Control "no-cache, no-store, must-revalidate" always;
    }

    # 禁止访问隐藏文件
    location ~ /\. {
        deny all;
    }

    # Gzip压缩
    gzip on;
    gzip_types text/plain text/css application/json application/javascript text/xml application/xml text/javascript image/svg+xml;
    gzip_min_length 1000;
}
NGINX_EOF

    # 测试配置
    nginx -t 2>&1 | tee -a "$LOG_FILE"
    if ! nginx -t &>/dev/null; then
        err "Nginx配置测试失败，请检查配置"
        exit 1
    fi

    # 启动/重载Nginx
    systemctl enable nginx 2>/dev/null || true
    systemctl restart nginx 2>/dev/null || service nginx restart 2>/dev/null || nginx -s reload

    # 当前和上一release已由符号链接保留，其余版本可以安全清理。
    CURRENT_RELEASE="$(readlink -f "$NGINX_CURRENT")"
    PREVIOUS_RELEASE="$(readlink -f "$NGINX_PREVIOUS" 2>/dev/null || true)"
    for candidate in "$NGINX_RELEASES_ROOT"/*; do
        [ -d "$candidate" ] || continue
        if [ "$candidate" != "$CURRENT_RELEASE" ] && [ "$candidate" != "$PREVIOUS_RELEASE" ]; then
            rm -rf -- "$candidate"
        fi
    done

    # 检查服务状态
    sleep 1
    if curl -s -o /dev/null -w "%{http_code}" "http://127.0.0.1:${NGINX_PORT}/" | grep -q "200\|301\|302"; then
        log "部署成功! 访问地址: http://$(hostname -I 2>/dev/null | awk '{print $1}' || echo '127.0.0.1'):${NGINX_PORT}"
    else
        warn "Nginx可能未正常启动，请手动检查"
    fi
}

# ============================================================
# 6. 预览模式
# ============================================================
run_preview() {
    cd "$SCRIPT_DIR"
    log "启动预览模式 (Ctrl+C退出)..."
    if command -v pnpm &>/dev/null; then
        pnpm docs:preview --port "$SERVE_PORT" --host 0.0.0.0
    else
        npm run docs:preview -- --port "$SERVE_PORT" --host 0.0.0.0
    fi
}

# ============================================================
# 7. 静态服务模式 (使用http-server或python)
# ============================================================
run_serve() {
    cd "$SCRIPT_DIR"
    DIST_DIR="docs/.vitepress/dist"

    if [ ! -d "$DIST_DIR" ]; then
        err "请先构建: $0"
        exit 1
    fi

    log "在 ${SERVE_PORT} 端口提供静态服务..."

    if command -v npx &>/dev/null; then
        log "使用http-server提供服务..."
        npx http-server "$DIST_DIR" -p "$SERVE_PORT" -c-1 --gzip -a 0.0.0.0
    elif command -v python3 &>/dev/null; then
        log "使用python3提供服务..."
        python3 -m http.server "$SERVE_PORT" --directory "$DIST_DIR" --bind 0.0.0.0
    else
        err "未找到http-server或python3"
        exit 1
    fi
}

# ============================================================
# 主流程
# ============================================================
detect_os

# 需要root权限的场景
case "$MODE" in
    nginx)
        if [ "$(id -u)" -ne 0 ]; then
            err "Nginx部署需要root权限，请使用sudo执行"
            exit 1
        fi
        install_nodejs
        install_deps
        install_python_deps
        build_docs
        deploy_nginx
        ;;
    build)
        install_nodejs
        install_deps
        install_python_deps
        build_docs
        log "构建完成! 使用以下命令启动服务:"
        log "  sudo $0 --nginx        # Nginx生产部署"
        log "  $0 --serve          # 临时静态服务 (端口 $SERVE_PORT)"
        log "  $0 --serve 8080     # 指定端口"
        log "  $0 --preview        # VitePress预览模式"
        ;;
    preview)
        install_nodejs
        install_deps
        install_python_deps
        build_docs
        run_preview
        ;;
    serve)
        install_nodejs
        install_deps
        install_python_deps
        build_docs
        run_serve
        ;;
esac
