# -*- coding: utf-8 -*-
"""
本周清单 —— 本地网页勾选器
============================================================
作用：在浏览器里把「本周清单.md」的 `- [ ]` 显示成可点击的复选框，
      点一下打勾、再点一下取消，**直接改写 .md 文件本身**。

所以：进度会进 git（push.ps1 会带上），AI 也读得到。没有第二份副本。

用法：双击同目录下的「打开清单.bat」。
      关掉那个黑窗口就等于关掉服务。

只用 Python 标准库，不需要 pip install 任何东西。
============================================================
"""

import json
import os
import re
import socket
import sys
import threading
import webbrowser
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

# ---------------------------------------------------------------- 配置

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
MD_PATH = os.path.join(BASE_DIR, "本周清单.md")
HOST = "127.0.0.1"
DEFAULT_PORT = 8765
PORT_TRIES = 20

HEADING_RE = re.compile(r"^(#{2,3})\s+(.*?)\s*$")
TITLE_RE = re.compile(r"^#\s+(.*?)\s*$")
ITEM_RE = re.compile(r"^(\s*)- \[([ xX])\]\s*(.*)$")
FENCE_RE = re.compile(r"^\s*```")
# 用于写回：拆出 前缀 / 勾选符 / 中间 / 正文
TOGGLE_RE = re.compile(r"^(\s*- \[)([ xX])(\]\s*)(.*)$", re.S)


# ---------------------------------------------------------------- 解析

def parse_checklist():
    """读 本周清单.md，返回 {title, total, done, blocks:[...]}"""
    with open(MD_PATH, "r", encoding="utf-8") as f:
        lines = f.read().splitlines()

    title = "本周清单"
    blocks = []
    in_fence = False

    for i, line in enumerate(lines):
        if FENCE_RE.match(line):
            in_fence = not in_fence          # 代码块整体跳过，里面的 - [ ] 不算任务
            continue
        if in_fence:
            continue

        m = TITLE_RE.match(line)
        if m:
            title = m.group(1)
            continue

        m = HEADING_RE.match(line)
        if m:
            blocks.append({"type": "h" + str(len(m.group(1))), "text": m.group(2)})
            continue

        m = ITEM_RE.match(line)
        if m:
            blocks.append({
                "type": "item",
                "line": i,
                "indent": len(m.group(1)),
                "checked": m.group(2).lower() == "x",
                "text": m.group(3).strip(),
            })

    # 丢掉「下面没有任何勾选项」的标题（比如「本周硬节点」这类只有表格的段落）
    kept = []
    for idx, b in enumerate(blocks):
        if b["type"].startswith("h"):
            lvl = int(b["type"][1])
            has_item = False
            for nxt in blocks[idx + 1:]:
                if nxt["type"].startswith("h") and int(nxt["type"][1]) <= lvl:
                    break
                if nxt["type"] == "item":
                    has_item = True
                    break
            if not has_item:
                continue
        kept.append(b)

    items = [b for b in kept if b["type"] == "item"]
    return {
        "title": title,
        "total": len(items),
        "done": sum(1 for b in items if b["checked"]),
        "blocks": kept,
    }


def toggle_item(line_no, expected_text, checked):
    """把第 line_no 行的 [ ] / [x] 改成想要的状态。返回 (成功?, 错误信息)"""
    with open(MD_PATH, "r", encoding="utf-8", newline="") as f:
        raw = f.read()
    lines = raw.splitlines(keepends=True)

    if line_no < 0 or line_no >= len(lines):
        return False, "行号越界，请刷新页面"

    line = lines[line_no]
    body = line.rstrip("\r\n")               # 保留行尾原本是 \r\n 还是 \n
    tail = line[len(body):]

    m = TOGGLE_RE.match(body)
    if not m:
        return False, "这一行不是勾选项，请刷新页面"

    # 用「正文」比对，而不是整行 —— 否则打勾之后行文本变了，第二次点就会误判
    if m.group(4).strip() != expected_text.strip():
        return False, "文件已经变了，请刷新页面"

    new_body = m.group(1) + ("x" if checked else " ") + m.group(3) + m.group(4)
    lines[line_no] = new_body + tail

    tmp = MD_PATH + ".tmp"
    with open(tmp, "w", encoding="utf-8", newline="") as f:
        f.write("".join(lines))
    os.replace(tmp, MD_PATH)                 # 原子替换，中途出错不会写坏原文件
    return True, None


# ---------------------------------------------------------------- 网页

PAGE_HTML = r"""<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>本周清单</title>
<style>
  :root {
    --bg:#f6f7f9; --fg:#1f2328; --card:#fff; --line:#e3e6ea;
    --dim:#6b7280; --accent:#2563eb; --ok:#16a34a;
  }
  @media (prefers-color-scheme: dark) {
    :root { --bg:#14171a; --fg:#e6e8eb; --card:#1c2024; --line:#2c3137;
            --dim:#9aa2ad; --accent:#60a5fa; --ok:#4ade80; }
  }
  * { box-sizing:border-box; }
  body { margin:0; padding:0 16px 64px; background:var(--bg); color:var(--fg);
         font:15px/1.65 "Microsoft YaHei","Segoe UI",system-ui,sans-serif; }
  header { position:sticky; top:0; z-index:9; background:var(--bg);
           padding:16px 0 12px; border-bottom:1px solid var(--line); }
  h1 { font-size:17px; margin:0 0 10px; font-weight:600; }
  .meta { display:flex; align-items:center; gap:12px; font-size:13px; color:var(--dim); }
  .bar { flex:1; height:8px; border-radius:99px; background:var(--line); overflow:hidden; }
  .bar > i { display:block; height:100%; width:0; background:var(--ok); transition:width .18s; }
  button { font:inherit; font-size:13px; padding:4px 12px; border-radius:8px;
           border:1px solid var(--line); background:var(--card); color:var(--fg); cursor:pointer; }
  button:hover { border-color:var(--accent); color:var(--accent); }
  h2 { font-size:15px; margin:30px 0 8px; padding-bottom:6px;
       border-bottom:1px solid var(--line); font-weight:600; }
  h3 { font-size:14px; margin:20px 0 6px; color:var(--accent); font-weight:600; }
  label.row { display:flex; align-items:flex-start; gap:10px; background:var(--card);
              border:1px solid var(--line); border-radius:10px;
              padding:10px 12px; margin:6px 0; cursor:pointer; user-select:none; }
  label.row:hover { border-color:var(--accent); }
  label.row input { width:19px; height:19px; margin:2px 0 0; flex:0 0 auto;
                    accent-color:var(--ok); cursor:pointer; }
  label.row span { flex:1; }
  label.row.done span { color:var(--dim); text-decoration:line-through; }
  .err { position:fixed; left:50%; bottom:20px; transform:translateX(-50%);
         background:#dc2626; color:#fff; padding:9px 16px; border-radius:9px;
         font-size:13px; display:none; }
</style>
</head>
<body>
<header>
  <h1 id="title">本周清单</h1>
  <div class="meta">
    <div class="bar"><i id="fill"></i></div>
    <span id="count">加载中…</span>
    <button id="refresh">刷新</button>
  </div>
</header>
<main id="list"></main>
<div class="err" id="err"></div>

<script>
const listEl  = document.getElementById('list');
const countEl = document.getElementById('count');
const fillEl  = document.getElementById('fill');
let total = 0, done = 0;

function showErr(msg) {
  const e = document.getElementById('err');
  e.textContent = msg;
  e.style.display = 'block';
  clearTimeout(e._t);
  e._t = setTimeout(() => { e.style.display = 'none'; }, 3500);
}

function updateCount() {
  countEl.textContent = '已完成 ' + done + ' / ' + total;
  fillEl.style.width = total ? (done * 100 / total) + '%' : '0';
}

async function load() {
  try {
    const r = await fetch('/api/tasks', { cache: 'no-store' });
    if (!r.ok) throw new Error('HTTP ' + r.status);
    const d = await r.json();
    document.getElementById('title').textContent = d.title;
    total = d.total; done = d.done;
    render(d.blocks);
    updateCount();
  } catch (e) {
    showErr('读不到清单：' + e.message);
  }
}

function render(blocks) {
  listEl.innerHTML = '';
  for (const b of blocks) {
    if (b.type === 'h2' || b.type === 'h3') {
      const h = document.createElement(b.type);
      h.textContent = b.text;
      listEl.appendChild(h);
      continue;
    }
    const label = document.createElement('label');
    label.className = 'row' + (b.checked ? ' done' : '');

    const cb = document.createElement('input');
    cb.type = 'checkbox';
    cb.checked = b.checked;

    const span = document.createElement('span');
    span.textContent = b.text;

    cb.addEventListener('change', async () => {
      const want = cb.checked;
      try {
        const r = await fetch('/api/toggle', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ line: b.line, text: b.text, checked: want })
        });
        const d = await r.json();
        if (!r.ok) throw new Error(d.error || ('HTTP ' + r.status));
        b.checked = want;
        label.classList.toggle('done', want);
        done = d.done;
        updateCount();
      } catch (e) {
        cb.checked = !want;               // 失败就退回原状态
        showErr('没写进去：' + e.message);
      }
    });

    label.appendChild(cb);
    label.appendChild(span);
    listEl.appendChild(label);
  }
}

document.getElementById('refresh').addEventListener('click', load);
load();
</script>
</body>
</html>
"""


# ---------------------------------------------------------------- HTTP

class Handler(BaseHTTPRequestHandler):
    server_version = "ChecklistLocal/1.0"

    def _send(self, code, body, ctype="application/json; charset=utf-8"):
        data = body if isinstance(body, bytes) else body.encode("utf-8")
        self.send_response(code)
        self.send_header("Content-Type", ctype)
        self.send_header("Content-Length", str(len(data)))
        self.send_header("Cache-Control", "no-store")
        self.end_headers()
        self.wfile.write(data)

    def _json(self, code, obj):
        self._send(code, json.dumps(obj, ensure_ascii=False))

    def do_GET(self):
        path = self.path.split("?")[0]
        if path in ("/", "/index.html"):
            self._send(200, PAGE_HTML, "text/html; charset=utf-8")
        elif path == "/api/tasks":
            try:
                self._json(200, parse_checklist())
            except Exception as e:
                self._json(500, {"error": str(e)})
        else:
            self._json(404, {"error": "not found"})

    def do_POST(self):
        if self.path.split("?")[0] != "/api/toggle":
            self._json(404, {"error": "not found"})
            return
        try:
            n = int(self.headers.get("Content-Length") or 0)
            payload = json.loads(self.rfile.read(n).decode("utf-8"))
            line_no = int(payload["line"])
            text = str(payload.get("text", ""))
            checked = bool(payload.get("checked"))
        except Exception as e:
            self._json(400, {"error": "请求格式不对: " + str(e)})
            return

        try:
            ok, msg = toggle_item(line_no, text, checked)
        except Exception as e:
            self._json(500, {"error": str(e)})
            return

        if not ok:
            self._json(409, {"error": msg})
            return
        self._json(200, parse_checklist())

    def log_message(self, *args):
        pass                                  # 别刷屏


def pick_port():
    for p in range(DEFAULT_PORT, DEFAULT_PORT + PORT_TRIES):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            try:
                s.bind((HOST, p))
                return p
            except OSError:
                continue
    return None


def main():
    if not os.path.exists(MD_PATH):
        print("找不到清单文件：", MD_PATH)
        print("请确认 checklist_server.py 和 本周清单.md 在同一个目录里。")
        return 1

    port = pick_port()
    if port is None:
        print("端口 %d~%d 都被占用了，先关掉占用的程序再试。"
              % (DEFAULT_PORT, DEFAULT_PORT + PORT_TRIES))
        return 1

    url = "http://%s:%d/" % (HOST, port)
    print("=" * 58)
    print("  本周清单 已启动")
    print("  网页地址：%s" % url)
    print("  清单文件：%s" % MD_PATH)
    print()
    print("  勾选会直接写回上面的 .md 文件，进度会进 git。")
    print("  关掉这个窗口 ＝ 关掉服务。")
    print("=" * 58)

    # CHECKLIST_NO_BROWSER=1 可以只起服务、不开浏览器（自测用）
    if os.environ.get("CHECKLIST_NO_BROWSER") != "1":
        threading.Timer(0.6, lambda: webbrowser.open(url)).start()

    srv = ThreadingHTTPServer((HOST, port), Handler)
    try:
        srv.serve_forever()
    except KeyboardInterrupt:
        print("\n已停止。")
    finally:
        srv.server_close()
    return 0


if __name__ == "__main__":
    sys.exit(main())
