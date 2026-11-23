import bpy, math, random
from mathutils import Vector, noise

M = bpy.data.materials

def sstep(a, b, x):
    t = (x - a) / (b - a)
    t = 0.0 if t < 0 else (1.0 if t > 1 else t)
    return t * t * (3 - 2 * t)

def lerp(a, b, t):
    return a + (b - a) * t

def segd(px, py, a, b):
    ax, ay = a; vx, vy = b[0] - ax, b[1] - ay
    L = vx * vx + vy * vy or 1e-9
    t = ((px - ax) * vx + (py - ay) * vy) / L
    t = 0.0 if t < 0 else (1.0 if t > 1 else t)
    return math.hypot(px - ax - t * vx, py - ay - t * vy), t

def polyd(px, py, pts):
    return min(segd(px, py, pts[i], pts[i + 1])[0] for i in range(len(pts) - 1))

# ramp: ((x0,y0), z0, (x1,y1), z1, width)   plateau: (cx, cy, rx, ry, height)
CFG = {
 # plateau height = absolute top height above ground (overlaps take the max)
 # exit: (id, side, pos, width, target_area, target_exit_id)
 "A0": dict(name="Area0_BaseCamp", size=(50, 40), amp=0.3, seed=1.0, forest=False, wall=14,
    plateaus=[(-12, 12, 7, 8, 3)], ramps=[((0, 2), 0, (-7, 8), 3, 5)], ponds=[], rivers=[],
    paths=[[(-5, -3), (25, 0)], [(0, 2), (-7, 8)], [(-7, 8), (-12, 13), (-12, 20)]],
    exits=[("E", "E", 0, 8, "A1", "W"), ("N", "N", -12, 7, "A1", "W2")],
    trees=12, mindist=6, rocks=15, grass=500, camp=True),
 "A1": dict(name="Area1_Grassland", size=(80, 70), amp=0.5, seed=2.0, forest=False, wall=16,
    plateaus=[(10, 22, 32, 12, 6), (-33, 26, 9, 7, 3), (-25, -22, 6, 5, 1.5)],
    ramps=[((-28, 0), 0, (-14, 16), 6, 6), ((-27, 26), 3, (-17, 26), 6, 5)],
    ponds=[(12, -8, 9)], rivers=[],
    paths=[[(-40, -5), (-25, -3), (-28, 0)], [(-28, 0), (-14, 16)], [(-14, 16), (10, 20), (40, 22)],
           [(-40, 26), (-27, 26)], [(-27, 26), (-17, 26)], [(-17, 26), (-5, 21)],
           [(-28, 0), (-5, -24), (40, -22)]],
    exits=[("W", "W", -5, 8, "A0", "E"), ("W2", "W", 26, 7, "A0", "N"),
           ("E", "E", 22, 8, "A2", "W"), ("E2", "E", -22, 8, "A2", "W2")],
    trees=25, mindist=7, rocks=30, grass=1800),
 "A2": dict(name="Area2_Hills", size=(80, 70), amp=0.5, seed=3.0, forest=False, wall=22,
    plateaus=[(-28, 15, 16, 20, 6), (12, 0, 18, 14, 5), (16, 4, 8, 7, 10), (32, 18, 10, 14, 5)],
    ramps=[((-24, -1), 6, (-24, -18), 0, 6), ((-8, -26), 0, (8, -10), 5, 6), ((26, -8), 5, (18, 0), 10, 5)],
    ponds=[], rivers=[],
    paths=[[(-40, 22), (-24, 15), (-24, -1)], [(-24, -1), (-24, -18)], [(-24, -18), (10, -25), (40, -22)],
           [(-8, -26), (8, -10)], [(26, -8), (18, 0)],
           [(-40, -22), (-24, -20)], [(8, -10), (22, 8), (40, 22)]],
    exits=[("W", "W", 22, 8, "A1", "E"), ("W2", "W", -22, 8, "A1", "E2"),
           ("E", "E", -22, 8, "A3", "W"), ("E2", "E", 22, 8, "A3", "W2")],
    trees=30, mindist=6, rocks=40, grass=1800),
 "A3": dict(name="Area3_Forest", size=(80, 70), amp=0.7, seed=4.0, forest=True, wall=18,
    plateaus=[(28, 10, 16, 27, 4), (-30, 20, 12, 9, 4)],
    ramps=[((16, -28), 0, (24, -10), 4, 6), ((-24, 14), 4, (-16, 2), 0, 5)], ponds=[],
    rivers=[([(6, 36), (2, 18), (9, 0), (2, -16), (5, -36)], 6)],
    paths=[[(-40, -22), (-6, -24)], [(10, -24), (16, -28)], [(16, -28), (24, -10)], [(24, -10), (25, 35)],
           [(-40, 20), (-26, 18), (-24, 14)], [(-24, 14), (-16, 2)], [(-16, 2), (-10, 15), (-12, 35)]],
    exits=[("W", "W", -22, 8, "A2", "E"), ("W2", "W", 20, 7, "A2", "E2"),
           ("N", "N", 25, 8, "A4", "S"), ("N2", "N", -12, 7, "A4", "S2")],
    trees=400, mindist=2.8, rocks=35, grass=700, bridge=((3, -24), 14)),
 "A4": dict(name="Area4_DeepForest", size=(70, 70), amp=0.7, seed=5.0, forest=True, wall=26,
    plateaus=[(0, 24, 32, 11, 12), (-22, 6, 7, 5, 6)],
    ramps=[((-8, -6), 0, (-20, 4), 6, 5), ((-17, 8), 6, (-6, 18), 12, 5), ((-27, -14), 0, (-24, 4), 6, 5)],
    ponds=[(15, -8, 9)], rivers=[],
    paths=[[(10, -35), (5, -15), (-8, -6)], [(-8, -6), (-20, 4)], [(-17, 8), (-6, 18)],
           [(-22, -35), (-27, -14)], [(-27, -14), (-24, 4)]],
    exits=[("S", "S", 10, 8, "A3", "N"), ("S2", "S", -22, 7, "A3", "N2")],
    trees=450, mindist=2.6, rocks=40, grass=700, giant=(-5, -24), nest=(8, 26)),
}

# --- enlarge every area (layout above is written at 1.0 scale)
S = 1.5; SW = 1.25
def _xy(p): return (p[0] * S, p[1] * S)
for _c in CFG.values():
    _c["size"] = (_c["size"][0] * S, _c["size"][1] * S)
    _c["plateaus"] = [(cx * S, cy * S, rx * S, ry * S, h) for (cx, cy, rx, ry, h) in _c["plateaus"]]
    _c["ramps"] = [(_xy(a), z0, _xy(b), z1, w * SW) for (a, z0, b, z1, w) in _c["ramps"]]
    _c["ponds"] = [(cx * S, cy * S, r * S) for (cx, cy, r) in _c["ponds"]]
    _c["rivers"] = [([_xy(p) for p in pts], w * SW) for (pts, w) in _c["rivers"]]
    _c["paths"] = [[_xy(p) for p in pts] for pts in _c["paths"]]
    _c["exits"] = [(i, s, pos * S, w * SW, t, ti) for (i, s, pos, w, t, ti) in _c["exits"]]
    for k in ("giant", "nest"):
        if k in _c: _c[k] = _xy(_c[k])
    if "bridge" in _c: _c["bridge"] = (_xy(_c["bridge"][0]), _c["bridge"][1] * S)
    _c["trees"] = int(_c["trees"] * S * S); _c["rocks"] = int(_c["rocks"] * S * S); _c["grass"] = int(_c["grass"] * S * S)

def make_height(c):
    seed = c["seed"]; hx, hy = c["size"][0] / 2, c["size"][1] / 2
    def h(x, y):
        n = noise.fractal(Vector((x * 0.035 + seed * 10, y * 0.035, 0.5)), 0.5, 2.0, 4)
        n2 = noise.noise(Vector((x * 0.12, y * 0.12, seed * 3.7)))
        base = 0.5 + c["amp"] * n
        top = 0.0
        for (cx, cy, rx, ry, ph) in c["plateaus"]:
            dx, dy = x - cx, y - cy
            a = math.atan2(dy, dx)
            jit = 1 + 0.1 * noise.noise(Vector((math.cos(a) * 1.5 + cx, math.sin(a) * 1.5 + cy, seed)))
            dn = ((abs(dx) / rx) ** 4 + (abs(dy) / ry) ** 4) ** 0.25
            top = max(top, ph * sstep(-0.9, 0.9, (jit - dn) * min(rx, ry)))
        z = base + top
        for (cx, cy, r) in c["ponds"]:
            z = lerp(-2.0, z, sstep(r - 4, r, math.hypot(x - cx, y - cy)))
        for (pts, w) in c["rivers"]:
            z = lerp(-2.0, z, sstep(w / 2 - 1, w / 2 + 3, polyd(x, y, pts)))
        for (a, z0, b, z1, w) in c["ramps"]:
            d, t = segd(x, y, a, b)
            k = 1 - sstep(w / 2, w / 2 + 1.5, d)
            if k > 0:
                z = lerp(z, base + lerp(z0, z1, t), k)
        n4 = noise.noise(Vector((x * 0.3, y * 0.3, seed * 1.9)))
        d = min(hx - abs(x), hy - abs(y)) + 3.0 * n2
        wall = c["wall"] * (1 - sstep(0, 7, d)) * (1 + 0.3 * n2 + 0.12 * n4)
        for (_, side, pos, w, _, _) in c["exits"]:
            if side in "EW":
                lat = abs(y - pos); near = hx - x if side == "E" else hx + x
            else:
                lat = abs(x - pos); near = hy - y if side == "N" else hy + y
            wall *= 1 - (1 - sstep(w / 2, w / 2 + 3, lat)) * (1 - sstep(11, 15, near))
        return z + wall
    return h

def remove_scene(name):
    sc = bpy.data.scenes.get(name)
    if not sc:
        return
    for o in list(sc.collection.all_objects):
        bpy.data.objects.remove(o)
    for col in list(sc.collection.children_recursive):
        bpy.data.collections.remove(col)
    bpy.data.scenes.remove(sc)

def build(code):
    c = CFG[code]; name = c["name"]
    remove_scene(name)
    sx, sy = c["size"]; hx, hy = sx / 2, sy / 2; st = 0.5
    nx, ny = int(sx / st), int(sy / st)
    hf = make_height(c)
    H = [[hf(-hx + i * st, -hy + j * st) for i in range(nx + 1)] for j in range(ny + 1)]

    def samp(x, y):
        fi = (x + hx) / st; fj = (y + hy) / st
        i = max(0, min(nx - 1, int(fi))); j = max(0, min(ny - 1, int(fj)))
        u = fi - i; v = fj - j
        return (H[j][i] * (1 - u) + H[j][i + 1] * u) * (1 - v) + (H[j + 1][i] * (1 - u) + H[j + 1][i + 1] * u) * v

    def slope(x, y):
        return max(abs(samp(x + 0.5, y) - samp(x - 0.5, y)), abs(samp(x, y + 0.5) - samp(x, y - 0.5)))

    def near_water(x, y):
        return any(math.hypot(x - cx, y - cy) < r + 2 for (cx, cy, r) in c["ponds"]) or \
               any(polyd(x, y, pts) < w / 2 + 4 for (pts, w) in c["rivers"])

    # --- scene / collections
    sc = bpy.data.scenes.new(name)
    sc["area_id"] = code
    sc.world = bpy.data.worlds.get("World")
    col = bpy.data.collections.new(name); sc.collection.children.link(col)
    def sub(n):
        cc = bpy.data.collections.new(code + "_" + n); col.children.link(cc); return cc
    def obj(n, data, cc):
        o = bpy.data.objects.new(n, data); cc.objects.link(o); return o

    # --- terrain
    verts = [(-hx + i * st, -hy + j * st, H[j][i]) for j in range(ny + 1) for i in range(nx + 1)]
    W = nx + 1
    faces = [(j * W + i, j * W + i + 1, (j + 1) * W + i + 1, (j + 1) * W + i) for j in range(ny) for i in range(nx)]
    me = bpy.data.meshes.new(code + "_Terrain"); me.from_pydata(verts, [], faces); me.update()
    for m in ("Grass_Light", "Grass_Dark", "Forest_Floor", "Dirt", "Rock", "Sand"):
        me.materials.append(M[m])
    idx = []
    for p in me.polygons:
        x, y, z = p.center; nz = p.normal.z
        n3 = noise.noise(Vector((x * 0.08, y * 0.08, c["seed"] * 5)))
        if z < 0.05 and near_water(x, y): k = 5
        elif nz < 0.75: k = 4
        elif any(polyd(x, y, pts) < 2.6 + 0.6 * n3 for pts in c["paths"]): k = 3
        elif c["forest"]: k = 2 if n3 > -0.35 else 1
        else: k = 0 if n3 > -0.1 else 1
        idx.append(k)
    me.polygons.foreach_set("material_index", idx)
    me.polygons.foreach_set("use_smooth", [True] * len(me.polygons))
    obj(code + "_Terrain", me, col)

    if c["ponds"] or c["rivers"]:
        bx = [cx - r - 1 for (cx, cy, r) in c["ponds"]] + [p[0] - w for (ps, w) in c["rivers"] for p in ps]
        bX = [cx + r + 1 for (cx, cy, r) in c["ponds"]] + [p[0] + w for (ps, w) in c["rivers"] for p in ps]
        by = [cy - r - 1 for (cx, cy, r) in c["ponds"]] + [p[1] - w for (ps, w) in c["rivers"] for p in ps]
        bY = [cy + r + 1 for (cx, cy, r) in c["ponds"]] + [p[1] + w for (ps, w) in c["rivers"] for p in ps]
        x0, x1 = max(-hx + 8, min(bx)), min(hx - 8, max(bX)); y0, y1 = max(-hy + 8, min(by)), min(hy - 8, max(bY))
        wm = bpy.data.meshes.new(code + "_Water")
        wm.from_pydata([(x0, y0, -0.9), (x1, y0, -0.9), (x1, y1, -0.9), (x0, y1, -0.9)], [], [(0, 1, 2, 3)])
        wm.materials.append(M["Water"]); obj(code + "_Water", wm, col)

    # --- exits / spawns
    mc = sub("Markers"); exit_pts = []
    for (eid, side, pos, w, tgt, tid) in c["exits"]:
        ex, ey, ix, iy = {"E": (hx - 1, pos, -1, 0), "W": (-hx + 1, pos, 1, 0),
                          "N": (pos, hy - 1, 0, -1), "S": (pos, -hy + 1, 0, 1)}[side]
        exit_pts.append((ex, ey))
        e = obj(code + "_Exit_" + eid, None, mc)
        e.empty_display_type = 'CUBE'
        e.location = (ex, ey, samp(ex + ix * 2, ey + iy * 2) + 2.5)
        e.scale = (1.5, w / 2, 2.5) if side in "EW" else (w / 2, 1.5, 2.5)
        e["target_area"] = CFG[tgt]["name"]
        e["target_spawn"] = tgt + "_Spawn_" + tid
        s = obj(code + "_Spawn_" + eid, None, mc)
        s.empty_display_type = 'SINGLE_ARROW'; s.empty_display_size = 2
        s.location = (ex + ix * 9, ey + iy * 9, samp(ex + ix * 9, ey + iy * 9) + 0.1)
        s.rotation_euler = (math.radians(-90), 0, math.atan2(iy, ix) + math.pi / 2)

    rng = random.Random(int(c["seed"] * 100))
    def blocked(x, y, margin):
        if min(hx - abs(x), hy - abs(y)) < 3: return True
        if any(polyd(x, y, pts) < margin for pts in c["paths"]): return True
        if any(segd(x, y, a, b)[0] < w / 2 + margin for (a, _, b, _, w) in c["ramps"]): return True
        if any(math.hypot(x - ex, y - ey) < 14 for (ex, ey) in exit_pts): return True
        return False

    # --- special props
    pc = sub("Props"); pts = []
    if c.get("camp"):
        z = samp(-10 * S, -8 * S)
        o = obj(code + "_Tent", bpy.data.meshes["Camp_Tent"], pc); o.location = (-10 * S, -8 * S, z); o.rotation_euler = (0, 0, 0.78)
        for (x, y, s) in [(-3, -9, (1.2, 0.8, 0.7)), (-2, -6, (0.8, 0.8, 0.8)), (-14, -2, (1.5, 1.0, 0.9))]:
            x, y = x * S, y * S
            o = obj(code + "_Box", bpy.data.meshes["Camp_Box"], pc); o.location = (x, y, samp(x, y) + s[2] / 2); o.scale = s
        for i in range(7):
            a = i / 7 * 6.28; x, y = -6 * S + math.cos(a) * 0.8, -2 * S + math.sin(a) * 0.8
            o = obj(code + "_FireStone", bpy.data.meshes["Rock_1"], pc); o.location = (x, y, samp(x, y)); o.scale = (0.25,) * 3
        pts += [(-10 * S, -8 * S), (-6 * S, -2 * S), (-14 * S, -2 * S)]
    if c.get("bridge"):
        (bx, by), L = c["bridge"]
        o = obj(code + "_Bridge", bpy.data.meshes["Camp_Box"], pc)
        o.location = (bx, by, max(samp(bx - L / 2, by), samp(bx + L / 2, by)) + 0.15); o.scale = (L, 2.4, 0.35)
    if c.get("giant"):
        x, y = c["giant"]
        o = obj(code + "_GiantTree", bpy.data.meshes["Tree_Broad"], pc); o.location = (x, y, samp(x, y) - 0.5); o.scale = (3.5, 3.5, 3.2)
        pts += [(x + dx, y + dy) for dx in (-4, 0, 4) for dy in (-4, 0, 4)]
    if c.get("nest"):
        x, y = c["nest"]
        nm = bpy.data.meshes.get("Nest")
        if not nm:
            import bmesh
            bm = bmesh.new()
            bmesh.ops.create_cone(bm, cap_ends=True, segments=12, radius1=2.5, radius2=3.2, depth=0.8)
            nm = bpy.data.meshes.new("Nest"); bm.to_mesh(nm); bm.free(); nm.materials.append(M["Trunk"])
        o = obj(code + "_Nest", nm, pc); o.location = (x, y, samp(x, y) + 0.2)
        pts += [(x, y)]

    # --- trees
    tc = sub("Trees"); pine = bpy.data.meshes["Tree_Pine"]; broad = bpy.data.meshes["Tree_Broad"]
    tries = 0; n_t = 0; md = c["mindist"]
    while n_t < c["trees"] and tries < c["trees"] * 40:
        tries += 1
        x = rng.uniform(-hx + 3, hx - 3); y = rng.uniform(-hy + 3, hy - 3)
        z = samp(x, y)
        if near_water(x, y) or slope(x, y) > 0.45 or blocked(x, y, 3.0): continue
        if not c["forest"]:
            d = min(hx - abs(x), hy - abs(y))
            if rng.random() > 0.25 + 0.75 * (1 - sstep(6, 20, d)): continue
        if any((px - x) ** 2 + (py - y) ** 2 < md * md for px, py in pts): continue
        pts.append((x, y)); n_t += 1
        s = rng.uniform(0.8, 1.4)
        o = obj(code + "_Tree", pine if (c["forest"] and rng.random() < 0.6) else broad, tc)
        o.location = (x, y, z - 0.3); o.scale = (s, s, s * rng.uniform(0.9, 1.2)); o.rotation_euler = (0, 0, rng.uniform(0, 6.28))

    # --- rocks
    rc = sub("Rocks"); rocks = [bpy.data.meshes["Rock_%d" % i] for i in range(3)]; n_r = 0
    for _ in range(c["rocks"] * 20):
        if n_r >= c["rocks"]: break
        x = rng.uniform(-hx + 3, hx - 3); y = rng.uniform(-hy + 3, hy - 3)
        z = samp(x, y)
        if z < -0.5 or blocked(x, y, 2.0): continue
        if any((px - x) ** 2 + (py - y) ** 2 < 4 for px, py in pts): continue
        s = rng.uniform(1.5, 3.0) if slope(x, y) > 0.45 else rng.uniform(0.4, 1.6)
        o = obj(code + "_Rock", rng.choice(rocks), rc)
        o.location = (x, y, z - 0.35 * s); o.scale = (s, s * rng.uniform(0.8, 1.2), s); o.rotation_euler = (0, 0, rng.uniform(0, 6.28))
        n_r += 1

    # --- grass tufts / flowers
    gv = []; gf = []; gmi = []; n_g = 0
    for _ in range(c["grass"] * 4):
        if n_g >= c["grass"]: break
        x = rng.uniform(-hx + 2, hx - 2); y = rng.uniform(-hy + 2, hy - 2)
        z = samp(x, y)
        if near_water(x, y) or slope(x, y) > 0.3 or any(polyd(x, y, p) < 1.8 for p in c["paths"]): continue
        for k in range(3):
            a = rng.uniform(0, 6.28); hgt = rng.uniform(0.4, 0.8)
            dx, dy = math.cos(a) * 0.12, math.sin(a) * 0.12
            b = len(gv)
            gv += [(x - dx, y - dy, z - 0.05), (x + dx, y + dy, z - 0.05), (x - dy * 1.2, y + dx * 1.2, z + hgt)]
            gf.append((b, b + 1, b + 2)); gmi.append(0)
        if not c["forest"] and rng.random() < 0.12:
            r = 0.12; fz = z + 0.45; b = len(gv)
            gv += [(x + r, y, fz), (x - r, y, fz), (x, y + r, fz), (x, y - r, fz), (x, y, fz + r), (x, y, fz - r)]
            mi = rng.choice([1, 2])
            for (p, q) in [(0, 2), (2, 1), (1, 3), (3, 0)]:
                gf += [(b + p, b + q, b + 4), (b + q, b + p, b + 5)]; gmi += [mi, mi]
        n_g += 1
    gm = bpy.data.meshes.new(code + "_Grass"); gm.from_pydata(gv, [], gf); gm.update()
    for m in ("Grass_Blade", "Flower_W", "Flower_Y"): gm.materials.append(M[m])
    gm.polygons.foreach_set("material_index", gmi)
    obj(code + "_Grass", gm, col)

    # --- light / camera
    sun = obj(code + "_Sun", bpy.data.lights["Sun"], col); sun.rotation_euler = (math.radians(50), 0, math.radians(35))
    cd = bpy.data.cameras.new(code + "_Cam"); cd.lens = 24; cd.clip_end = 1000
    cam = obj(code + "_Cam", cd, col)
    cam.location = (0, -sy * 0.75, max(sx, sy) * 1.1)
    cam.rotation_euler = (Vector((0, 0, 0)) - cam.location).to_track_quat('-Z', 'Y').to_euler()
    sc.camera = cam

    zs = [v[2] for v in verts]
    inner = [H[j][i] for j in range(ny + 1) for i in range(nx + 1)
             if min(hx - abs(-hx + i * st), hy - abs(-hy + j * st)) > 10]
    return {"area": name, "trees": n_t, "rocks": n_r, "grass": n_g,
            "inner_z": [round(min(inner), 1), round(max(inner), 1)], "z": [round(min(zs), 1), round(max(zs), 1)]}

result = [build(k) for k in AREAS]
