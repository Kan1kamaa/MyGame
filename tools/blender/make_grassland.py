# -*- coding: utf-8 -*-
#
# ゲーム用の草原マップ(原神の最初のエリアのような、広い草原＋周囲を丘で囲んだ地形)を
# 手続き的に生成して FBX で書き出す Blender スクリプト。
#
# 実行例(GUI 不要):
#   blender --background --python tools/blender/make_grassland.py
#
# 生成物:
#   Data/Models/Field/Field_grassland.fbx    … マップ本体(この後 DxLibModelViewer で mv1 に変換)
#   Data/Models/Field/Field_grassland.blend  … 手直し用
#   tools/blender/preview_grassland.png      … 俯瞰プレビュー
#   tools/blender/preview_gameview.png       … ゲームの視点に近いプレビュー
#
# スケールの考え方:
#   1 Blender メートル = ゲーム 1 単位。キャラの背丈がだいたい 16 単位。
#   プレイヤーの移動範囲は ±300(CharacterManager.cpp)。その内側は完全に平ら。
#   丘・山は見た目だけで当たり判定は無い(地面 = Y0 平面 の前提を崩さない)。
#   ゲーム側はライティング OFF なので、マテリアルの色 = 最終的な見た目。

import os
import math
import random

import bpy
import bmesh
from mathutils import Vector, noise

# ───────────────────────── 調整用パラメータ ─────────────────────────
SEED          = 20260908
PLAY_HALF     = 300.0     # プレイヤーが動ける範囲の半分(この内側は平ら)
HILL_START    = 470.0     # この距離から丘がせり上がる
GROUND_HALF   = 1300.0    # 地面メッシュの広がり(半分)
GROUND_SUBDIV = 180
HILL_HEIGHT   = 210.0     # 外周の山の最大の高さ

CHAR_H        = 16.0      # キャラのおおよその背丈(サイズ感の基準)

N_TREES       = 46        # 木(spawn 地点の周りは避ける)
N_BOULDERS    = 30        # 大きめの岩
N_ROCKS       = 90        # 小石(プレイエリア内にも置く)
N_BUSHES      = 46        # 茂み
N_GRASS       = 650       # 草の房(全体にびっしり)
N_FLOWERS     = 240       # 花
N_STANDING    = 6         # 立石(ランドマーク)

HERO_TREE_POS = (0.0, -190.0)   # 大きな一本木の位置(spawn から見て正面奥)

# ボス専用ステージ(ボスゴーレム戦の広場)。既存の木/岩/草の生成物とは重ならないよう
# avoid ゾーンに登録した上で、同じ M_Rock/M_RockDark を使った岩の円陣として作る。
BOSS_ARENA_POS      = (135.0, -125.0)   # (x, z) spawn からやや離れた開けた一角
BOSS_ARENA_FLOOR_R  = 78.0              # 焼け跡・岩肌の地面になる半径
BOSS_ARENA_RING_R   = 112.0             # 岩の尖塔が並ぶ円の半径
BOSS_ARENA_AVOID_R  = 130.0             # 他の草木/岩をここでは生成しないための半径
N_ARENA_SPIRES      = 8                 # 尖塔の本数(1本分は入口として空ける)
ARENA_ENTRANCE_DEG  = None              # None なら spawn の方を自動で向く

OUT_DIR_REL  = "Data/Models/Field"
FBX_NAME     = "Field_grassland.fbx"
BLEND_NAME   = "Field_grassland.blend"

PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
OUT_DIR      = os.path.join(PROJECT_ROOT, OUT_DIR_REL)

random.seed(SEED)
noise.seed_set(SEED)
MATS = {}


# ───────────────────────── 汎用 ─────────────────────────
def smoothstep(a, b, x):
    if a == b:
        return 0.0 if x < a else 1.0
    t = max(0.0, min(1.0, (x - a) / (b - a)))
    return t * t * (3.0 - 2.0 * t)


def reset_scene():
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.context.scene.unit_settings.system = 'METRIC'
    bpy.context.scene.unit_settings.scale_length = 1.0


def make_material(name, rgb):
    mat = bpy.data.materials.new(name)
    mat.use_nodes = True
    bsdf = mat.node_tree.nodes.get("Principled BSDF")
    r, g, b = rgb
    bsdf.inputs["Base Color"].default_value = (r, g, b, 1.0)
    bsdf.inputs["Roughness"].default_value = 0.95
    if "Specular IOR Level" in bsdf.inputs:
        bsdf.inputs["Specular IOR Level"].default_value = 0.05
    mat.diffuse_color = (r, g, b, 1.0)
    return mat


def new_object(name, mesh):
    obj = bpy.data.objects.new(name, mesh)
    bpy.context.collection.objects.link(obj)
    return obj


def set_shade(obj, flat=True):
    for p in obj.data.polygons:
        p.use_smooth = not flat


def join(objs, name):
    objs = [o for o in objs if o]
    if not objs:
        return None
    for o in list(bpy.context.selected_objects):
        o.select_set(False)
    for o in objs:
        o.select_set(True)
    bpy.context.view_layer.objects.active = objs[0]
    if len(objs) > 1:
        bpy.ops.object.join()
    result = bpy.context.view_layer.objects.active
    result.name = name
    return result


def scatter_points(count, r_min, r_max, avoid=(), min_dist=0.0):
    """ドーナツ状の範囲にランダムな点をばらまく。avoid は (x, z, r) のリスト。"""
    pts = []
    tries = 0
    while len(pts) < count and tries < count * 80:
        tries += 1
        ang = random.uniform(0, 6.28318)
        rad = math.sqrt(random.uniform(r_min * r_min, r_max * r_max))
        x, z = math.cos(ang) * rad, math.sin(ang) * rad
        if any(math.hypot(x - ax, z - az) < ar for (ax, az, ar) in avoid):
            continue
        if min_dist > 0 and any(math.hypot(x - px, z - pz) < min_dist for px, pz in pts):
            continue
        pts.append((x, z))
    return pts


# ───────────────────────── 地形の高さ ─────────────────────────
def ground_height(x, z):
    r = math.hypot(x, z)
    r_eff = r + noise.noise(Vector((x * 0.0026, z * 0.0026, 41.0))) * 120.0
    ripple = noise.noise(Vector((x * 0.012, z * 0.012, 21.0))) * 0.35   # プレイエリア内の微起伏

    if r <= PLAY_HALF:
        return ripple

    h = ripple
    roll = (noise.noise(Vector((x * 0.0020, z * 0.0020, 0.0))) +
            0.5 * noise.noise(Vector((x * 0.0052, z * 0.0052, 11.0))))
    h += roll * 10.0 * smoothstep(PLAY_HALF, HILL_START, r_eff)

    if r_eff > HILL_START:
        t = smoothstep(HILL_START, GROUND_HALF * 0.95, r_eff)
        base = (t ** 1.7) * HILL_HEIGHT
        rugged = (noise.noise(Vector((x * 0.0034, z * 0.0034, 3.0))) * 0.6 +
                  noise.noise(Vector((x * 0.0095, z * 0.0095, 7.0))) * 0.4)
        h += base + rugged * 40.0 * t
        ang = math.atan2(z, x)                       # 北(+Z)に谷間の入口
        gap = math.exp(-((ang - math.radians(90.0)) ** 2) / (2 * 0.24 ** 2))
        h -= gap * base * 0.85
    return h


# ───────────────────────── パーツ生成 ─────────────────────────
def build_ground():
    mesh = bpy.data.meshes.new("Ground")
    bm = bmesh.new()
    n = GROUND_SUBDIV
    step = (GROUND_HALF * 2.0) / n
    g = [[None] * (n + 1) for _ in range(n + 1)]
    for i in range(n + 1):
        for j in range(n + 1):
            x = -GROUND_HALF + i * step
            z = -GROUND_HALF + j * step
            g[i][j] = bm.verts.new((x, ground_height(x, z), z))
    bm.verts.ensure_lookup_table()
    for i in range(n):
        for j in range(n):
            bm.faces.new((g[i][j], g[i][j + 1], g[i + 1][j + 1], g[i + 1][j]))  # +Y を向く向き
    bm.normal_update()
    bm.to_mesh(mesh)
    bm.free()

    obj = new_object("Ground", mesh)
    mesh.materials.append(MATS["grass"])       # 0
    mesh.materials.append(MATS["grass2"])      # 1 少し違う緑(まだら)
    mesh.materials.append(MATS["slope"])       # 2 丘の斜面(枯れ草)
    mesh.materials.append(MATS["rock_dark"])   # 3 高い所の岩肌
    mesh.materials.append(MATS["scorched"])    # 4 ボスアリーナの焼け跡
    ax, az = BOSS_ARENA_POS
    for p in mesh.polygons:
        c = p.center
        arena_d = math.hypot(c.x - ax, c.z - az)
        arena_d += noise.noise(Vector((c.x * 0.02, c.z * 0.02, 60.0))) * 8.0   # 縁をギザギザに
        if arena_d < BOSS_ARENA_FLOOR_R:
            p.material_index = 4       # 焼け焦げた地面(M_Scorched)で統一
            p.use_smooth = True
            continue
        rock_line = 115.0 + noise.noise(Vector((c.x * 0.006, c.z * 0.006, 9.0))) * 30.0
        slope_line = 45.0 + noise.noise(Vector((c.x * 0.006, c.z * 0.006, 2.0))) * 26.0
        if c.y > rock_line:
            p.material_index = 3
        elif c.y > slope_line:
            p.material_index = 2
        elif noise.noise(Vector((c.x * 0.06, c.z * 0.06, 5.0))) > 0.10:
            p.material_index = 1
        else:
            p.material_index = 0
        p.use_smooth = True
    return obj


def _cone(verts, r1, r2, depth, loc, mat, flat=True, rotx=0.0):
    bpy.ops.mesh.primitive_cone_add(vertices=verts, radius1=r1, radius2=r2, depth=depth, location=loc)
    o = bpy.context.active_object
    if rotx:
        o.rotation_euler = (rotx, 0, 0)
        bpy.ops.object.transform_apply(rotation=True)
    o.data.materials.append(mat)
    set_shade(o, flat)
    return o


def _ico(subdiv, radius, loc, mat, squash=1.0, jitter=0.0, flat=True):
    bpy.ops.mesh.primitive_ico_sphere_add(subdivisions=subdiv, radius=radius, location=loc)
    o = bpy.context.active_object
    if jitter:
        for v in o.data.vertices:
            v.co += Vector((random.uniform(-jitter, jitter), random.uniform(-jitter, jitter),
                            random.uniform(-jitter, jitter))) * radius
    o.scale = (1.0, squash, 1.0)
    o.rotation_euler = (0, random.uniform(0, 6.28), 0)
    bpy.ops.object.transform_apply(scale=True, rotation=True)
    o.data.materials.append(mat)
    set_shade(o, flat)
    return o


def build_hero_tree():
    parts = []
    px, pz = HERO_TREE_POS
    py = ground_height(px, pz)
    H = 110.0
    parts.append(_cone(12, 8.0, 3.5, H, (px, py + H * 0.5, pz), MATS["bark"], flat=False,
                       rotx=math.radians(90)))
    for k in range(5):
        a = math.radians(k * 72 + random.uniform(-14, 14))
        br = _cone(8, 3.0, 1.0, 40.0,
                   (px + math.cos(a) * 10, py + H * 0.72 + random.uniform(-4, 6),
                    pz + math.sin(a) * 10),
                   MATS["bark"], flat=False)
        br.rotation_euler = (math.radians(random.uniform(45, 68)), 0, a + math.radians(90))
        bpy.ops.object.transform_apply(rotation=True)
        parts.append(br)
    for (dx, dy, dz, rad) in [(0, 116, 0, 52), (-26, 104, 16, 38), (28, 104, -14, 38),
                              (6, 132, 22, 32), (-12, 126, -20, 32)]:
        parts.append(_ico(2, rad, (px + dx, py + dy, pz + dz),
                          random.choice([MATS["leaf"], MATS["leaf2"]]), squash=0.8))
    return join(parts, "HeroTree")


def build_tree(x, y, z):
    parts = []
    th = random.uniform(26.0, 52.0)
    tr = random.uniform(1.6, 3.0)
    parts.append(_cone(7, tr, tr * 0.5, th, (x, y + th * 0.5, z), MATS["bark"], flat=False,
                       rotx=math.radians(90)))
    layers = random.randint(2, 3)
    for i in range(layers):
        rad = random.uniform(10.0, 16.0) * (1.0 - i * 0.2)
        parts.append(_ico(1, rad,
                          (x + random.uniform(-3, 3), y + th + i * rad * 0.8 + random.uniform(-2, 2),
                           z + random.uniform(-3, 3)),
                          random.choice([MATS["leaf"], MATS["leaf2"], MATS["leaf3"]]),
                          squash=random.uniform(0.7, 0.95)))
    return join(parts, "Tree")


def build_boulder(x, y, z):
    s = random.uniform(7.0, 18.0)
    return _ico(1, s, (x, y + s * 0.28, z),
                random.choice([MATS["rock"], MATS["rock_dark"]]),
                squash=random.uniform(0.5, 0.8), jitter=0.30)


def build_rock(x, y, z):
    s = random.uniform(1.8, 5.5)
    return _ico(1, s, (x, y + s * 0.25, z),
                random.choice([MATS["rock"], MATS["rock_dark"]]),
                squash=random.uniform(0.5, 0.8), jitter=0.32)


def build_bush(x, y, z):
    rad = random.uniform(3.5, 8.0)
    return _ico(1, rad, (x, y + rad * 0.4, z),
                random.choice([MATS["leaf"], MATS["leaf3"], MATS["grass2"]]),
                squash=random.uniform(0.45, 0.7), jitter=0.18)


def build_grass(x, y, z):
    """細い葉を数枚まとめた草の房"""
    parts = []
    for _ in range(random.randint(2, 4)):
        h = random.uniform(2.2, 4.8)
        ox, oz = random.uniform(-1.2, 1.2), random.uniform(-1.2, 1.2)
        blade = _cone(3, random.uniform(0.28, 0.5), 0.0, h, (x + ox, y + h * 0.5, z + oz),
                      random.choice([MATS["grass"], MATS["grass2"], MATS["leaf"], MATS["leaf3"]]))
        blade.rotation_euler = (math.radians(random.uniform(-16, 16)), random.uniform(0, 6.28),
                                math.radians(random.uniform(-16, 16)))
        bpy.ops.object.transform_apply(rotation=True)
        parts.append(blade)
    return join(parts, "GrassTuft")


def build_flower(x, y, z):
    h = random.uniform(2.0, 4.5)
    stem = _cone(3, 0.35, 0.0, h, (x, y + h * 0.5, z), MATS["leaf3"])
    head = _ico(1, random.uniform(0.9, 1.7), (x, y + h, z),
                random.choice([MATS["fl_white"], MATS["fl_pink"], MATS["fl_blue"], MATS["fl_yellow"]]),
                squash=0.5)
    return join([stem, head], "Flower")


def build_standing_stones():
    parts = []
    cx, cz = -170.0, 150.0
    for k in range(N_STANDING):
        a = math.radians(k * (360.0 / N_STANDING))
        x, z = cx + math.cos(a) * 48.0, cz + math.sin(a) * 48.0
        y = ground_height(x, z)
        hh = random.uniform(26.0, 40.0)
        bpy.ops.mesh.primitive_cube_add(size=1.0, location=(x, y + hh * 0.5, z))
        o = bpy.context.active_object
        o.scale = (random.uniform(7, 10), hh, random.uniform(5, 7))
        o.rotation_euler = (0, a + random.uniform(-0.2, 0.2), math.radians(random.uniform(-7, 7)))
        bpy.ops.object.transform_apply(scale=True, rotation=True)
        for v in o.data.vertices:
            v.co += Vector((random.uniform(-0.4, 0.4), random.uniform(-0.4, 0.4),
                            random.uniform(-0.4, 0.4)))
        o.data.materials.append(MATS["rock"])
        set_shade(o, flat=True)
        parts.append(o)
    return join(parts, "StandingStones")


def build_ember_crack(x0, z0, x1, z1, width):
    """地面に張り付く、ひび割れ状の発光帯(照明無し描画なので色=発光に見える)"""
    mesh = bpy.data.meshes.new("EmberCrack")
    bm = bmesh.new()
    steps = 10
    prev = None
    for i in range(steps + 1):
        t = i / steps
        x = x0 + (x1 - x0) * t + noise.noise(Vector((x0 * 0.1, x1 * 0.1, t * 30.0))) * width * 1.4
        z = z0 + (z1 - z0) * t + noise.noise(Vector((z0 * 0.1, z1 * 0.1, t * 30.0 + 5.0))) * width * 1.4
        y = ground_height(x, z) + 0.22
        w = width * (0.35 + 0.65 * (1.0 - abs(t - 0.5) * 2.0))
        perp = Vector((-(z1 - z0), 0, (x1 - x0))).normalized() * w
        left = bm.verts.new((x - perp.x, y, z - perp.z))
        right = bm.verts.new((x + perp.x, y, z + perp.z))
        if prev:
            bm.faces.new((prev[0], left, right, prev[1]))
        prev = (left, right)
    bm.to_mesh(mesh)
    bm.free()
    obj = new_object("EmberCrack", mesh)
    mesh.materials.append(MATS["ember"])
    for p in mesh.polygons:
        p.use_smooth = True
    return obj


def build_arena_spire(x, z):
    """円陣を囲む、ゴツゴツと積み重なった岩の尖塔"""
    y = ground_height(x, z)
    total_h = random.uniform(85.0, 150.0)
    n_stack = random.randint(3, 4)
    parts = []
    cursor = y
    for i in range(n_stack):
        # 半径は高さ配分の半分程度(球なので)。tan- 上に行くほど細く。
        s = (total_h / n_stack) * 0.5 * random.uniform(0.8, 1.1) * (1.0 - i * 0.15)
        ox, oz = random.uniform(-4, 4), random.uniform(-4, 4)
        parts.append(_ico(2, s, (x + ox, cursor + s * 0.55, z + oz),
                          random.choice([MATS["rock"], MATS["rock_dark"]]),
                          squash=random.uniform(0.85, 1.15), jitter=0.30))
        cursor += s * (1.55 - i * 0.10)
    return join(parts, "ArenaSpire")


def build_boss_arena():
    """ボスゴーレム専用の戦闘フィールド: 岩の尖塔で囲われた、焼け焦げた円形の広場。
    中央には割れた巨石(ゴーレムが目覚めた跡)を置き、地面には発光する亀裂を走らせる。"""
    parts = []
    cx, cz = BOSS_ARENA_POS
    entrance = ARENA_ENTRANCE_DEG
    if entrance is None:
        entrance = math.degrees(math.atan2(0.0 - cz, 0.0 - cx))   # spawn(0,0)側を自動で向く

    # 周囲の尖塔(入口の1本だけ抜く)
    for k in range(N_ARENA_SPIRES):
        deg = entrance + k * (360.0 / N_ARENA_SPIRES)
        if k == 0:
            continue  # 入口
        a = math.radians(deg)
        x = cx + math.cos(a) * BOSS_ARENA_RING_R
        z = cz + math.sin(a) * BOSS_ARENA_RING_R
        parts.append(build_arena_spire(x, z))

    # 中央: 割れて転がった巨石(ボスが割って出てきたようなクレーター跡)
    cy = ground_height(cx, cz)
    core_parts = []
    for i in range(5):
        a = math.radians(i * 72 + random.uniform(-12, 12))
        r = random.uniform(6.0, 22.0)
        s = random.uniform(9.0, 16.0)
        core_parts.append(_ico(2, s,
                               (cx + math.cos(a) * r, cy + s * 0.32, cz + math.sin(a) * r),
                               random.choice([MATS["rock"], MATS["rock_dark"]]),
                               squash=random.uniform(0.45, 0.65), jitter=0.32))
    parts.append(join(core_parts, "ArenaCore"))

    # 中間: 瓦礫(小岩)を円陣の内側にまばらに
    rubble_pts = [(cx + x, cz + z) for (x, z) in
                  scatter_points(14, BOSS_ARENA_FLOOR_R * 0.55, BOSS_ARENA_RING_R * 0.92, min_dist=18)]
    rubble = [build_rock(x, ground_height(x, z), z) for (x, z) in rubble_pts]
    parts.append(join(rubble, "ArenaRubble"))

    # 地面の発光する亀裂(中央から放射状)
    for i in range(6):
        a = math.radians(entrance + 25 + i * 51)
        ex, ez = cx + math.cos(a) * BOSS_ARENA_RING_R * 0.98, cz + math.sin(a) * BOSS_ARENA_RING_R * 0.98
        parts.append(build_ember_crack(cx, cz, ex, ez, width=random.uniform(1.6, 2.6)))

    return join(parts, "BossArena")


def build_path():
    mesh = bpy.data.meshes.new("Path")
    bm = bmesh.new()
    half_w = 5.0
    prev = None
    z = -GROUND_HALF * 0.5
    while z <= GROUND_HALF * 0.5:
        cx = 90.0 * math.sin(z * 0.0032) + 18.0 * math.sin(z * 0.011)
        y = ground_height(cx, z) + 0.18
        left = bm.verts.new((cx - half_w, y, z))
        right = bm.verts.new((cx + half_w, y, z))
        if prev:
            bm.faces.new((prev[0], left, right, prev[1]))   # +Y を向く向き
        prev = (left, right)
        z += 20.0
    bm.to_mesh(mesh)
    bm.free()
    obj = new_object("Path", mesh)
    mesh.materials.append(MATS["dirt"])
    for p in mesh.polygons:
        p.use_smooth = True
    return obj


def build_pond():
    x, z = 150.0, 120.0
    bpy.ops.mesh.primitive_cylinder_add(vertices=26, radius=48.0, depth=1.0, location=(x, 0.1, z))
    o = bpy.context.active_object
    o.rotation_euler = (math.radians(90), 0, 0)
    o.scale = (1.25, 0.9, 1.0)
    bpy.ops.object.transform_apply(rotation=True, scale=True)
    for v in o.data.vertices:
        v.co += Vector((random.uniform(-3, 3), 0, random.uniform(-3, 3)))
    o.data.materials.append(MATS["water"])
    set_shade(o, flat=False)
    return o


# ───────────────────────── 組み立て ─────────────────────────
def build():
    reset_scene()
    MATS.clear()
    MATS.update({
        "grass":     make_material("M_Grass",     (0.40, 0.62, 0.29)),
        "grass2":    make_material("M_Grass2",    (0.43, 0.64, 0.31)),
        "slope":     make_material("M_Slope",     (0.47, 0.58, 0.33)),
        "dirt":      make_material("M_Dirt",      (0.55, 0.43, 0.29)),
        "rock":      make_material("M_Rock",      (0.53, 0.53, 0.55)),
        "rock_dark": make_material("M_RockDark",  (0.40, 0.40, 0.42)),
        "bark":      make_material("M_Bark",      (0.38, 0.27, 0.19)),
        "leaf":      make_material("M_Leaf",      (0.28, 0.52, 0.24)),
        "leaf2":     make_material("M_Leaf2",     (0.36, 0.60, 0.29)),
        "leaf3":     make_material("M_Leaf3",     (0.22, 0.44, 0.20)),
        "water":     make_material("M_Water",     (0.30, 0.55, 0.68)),
        "fl_white":  make_material("M_FlWhite",   (0.92, 0.92, 0.86)),
        "fl_pink":   make_material("M_FlPink",    (0.90, 0.55, 0.68)),
        "fl_blue":   make_material("M_FlBlue",    (0.50, 0.60, 0.90)),
        "fl_yellow": make_material("M_FlYellow",  (0.95, 0.82, 0.35)),
        "scorched":  make_material("M_Scorched",  (0.14, 0.12, 0.11)),
        "ember":     make_material("M_Ember",     (0.95, 0.42, 0.10)),
    })

    HERO = (HERO_TREE_POS[0], HERO_TREE_POS[1], 55.0)
    SPAWN = (0.0, 0.0, 90.0)        # spawn 周りは大きい木を置かない
    STONES = (-170.0, 150.0, 90.0)
    ARENA = (BOSS_ARENA_POS[0], BOSS_ARENA_POS[1], BOSS_ARENA_AVOID_R)

    pieces = [build_ground(), build_pond(), build_path(),
              build_hero_tree(), build_standing_stones(), build_boss_arena()]

    trees = [build_tree(x, ground_height(x, z), z)
             for (x, z) in scatter_points(N_TREES, 120, 1000, avoid=[HERO, SPAWN, STONES, ARENA], min_dist=42)]
    pieces.append(join(trees, "Trees"))

    boulders = [build_boulder(x, ground_height(x, z), z)
                for (x, z) in scatter_points(N_BOULDERS, 90, 900, avoid=[HERO, (0, 0, 55), ARENA], min_dist=40)]
    pieces.append(join(boulders, "Boulders"))

    rocks = [build_rock(x, ground_height(x, z), z)
             for (x, z) in scatter_points(N_ROCKS, 20, 850, avoid=[ARENA], min_dist=16)]
    pieces.append(join(rocks, "Rocks"))

    bushes = [build_bush(x, ground_height(x, z), z)
              for (x, z) in scatter_points(N_BUSHES, 40, 850, avoid=[ARENA], min_dist=22)]
    pieces.append(join(bushes, "Bushes"))

    grass = [build_grass(x, ground_height(x, z), z)
             for (x, z) in scatter_points(N_GRASS, 12, 620, avoid=[ARENA], min_dist=7)]
    pieces.append(join(grass, "Grass"))

    flowers = [build_flower(x, ground_height(x, z), z)
               for (x, z) in scatter_points(N_FLOWERS, 15, 560, avoid=[ARENA], min_dist=10)]
    pieces.append(join(flowers, "Flowers"))

    field = join(pieces, "Field")
    for o in list(bpy.context.selected_objects):
        o.select_set(False)
    field.select_set(True)
    bpy.context.view_layer.objects.active = field
    # 「高さ = Y」で組んだので +90°X で Blender の Z-up に立て直して適用
    field.rotation_euler = (math.radians(90.0), 0.0, 0.0)
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
    return field


def _setup_render(scene):
    scene.render.engine = 'BLENDER_WORKBENCH'
    try:
        scene.display.shading.color_type = 'MATERIAL'
        scene.display.shading.light = 'STUDIO'
        scene.display.shading.background_type = 'VIEWPORT'
        scene.display.shading.background_color = (0.55, 0.72, 0.92)
    except Exception:
        pass
    scene.render.resolution_x = 1280
    scene.render.resolution_y = 720


def render_preview(overview_path, gameview_path):
    scene = bpy.context.scene
    _setup_render(scene)

    sun_data = bpy.data.lights.new("Sun", 'SUN')
    sun_data.energy = 3.0
    sun = bpy.data.objects.new("Sun", sun_data)
    sun.rotation_euler = (math.radians(48), 0.0, math.radians(35))
    bpy.context.collection.objects.link(sun)

    cam_data = bpy.data.cameras.new("Cam")
    cam_data.clip_start = 1.0
    cam_data.clip_end = 20000.0
    cam = bpy.data.objects.new("Cam", cam_data)
    bpy.context.collection.objects.link(cam)
    scene.camera = cam

    # 俯瞰
    cam.location = (950, -1250, 850)
    cam.rotation_euler = (Vector((0, 40, 20)) - Vector(cam.location)).to_track_quat('-Z', 'Y').to_euler()
    cam_data.lens = 22
    scene.render.filepath = overview_path
    bpy.ops.render.render(write_still=True)

    # ゲーム視点(MainCamera.cpp と同じ: 注視点(0,10,0)、距離35、ピッチ0.7rad)
    # ゲームは Y-up、この .blend は Z-up なので Y↔Z を入れ替えて配置する
    cam.location = (0, -26.8, 32.5)
    cam.rotation_euler = (Vector((0, 0, 10)) - Vector(cam.location)).to_track_quat('-Z', 'Y').to_euler()
    cam_data.lens = 24
    scene.render.filepath = gameview_path
    bpy.ops.render.render(write_still=True)


def export(field):
    os.makedirs(OUT_DIR, exist_ok=True)
    fbx_path = os.path.join(OUT_DIR, FBX_NAME)
    blend_path = os.path.join(OUT_DIR, BLEND_NAME)

    for o in list(bpy.context.selected_objects):
        o.select_set(False)
    field.select_set(True)
    bpy.context.view_layer.objects.active = field

    bpy.ops.export_scene.fbx(
        filepath=fbx_path, use_selection=True,
        apply_unit_scale=True, global_scale=1.0, bake_space_transform=False,
        axis_forward='-Z', axis_up='Y',
        mesh_smooth_type='FACE', use_mesh_modifiers=True, use_triangles=True,
        add_leaf_bones=False, path_mode='COPY', embed_textures=False,
    )
    bpy.ops.wm.save_as_mainfile(filepath=blend_path)

    tris = sum(len(p.vertices) - 2 for p in field.data.polygons)
    print("=" * 52)
    print("FBX  :", fbx_path)
    print("verts:", len(field.data.vertices), "tris:", tris, "materials:", len(field.data.materials))
    print("=" * 52)


if __name__ == "__main__":
    f = build()
    render_preview(os.path.join(PROJECT_ROOT, "tools/blender/preview_grassland.png"),
                   os.path.join(PROJECT_ROOT, "tools/blender/preview_gameview.png"))
    export(f)
