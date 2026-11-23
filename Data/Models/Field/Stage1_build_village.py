# Builds the first village (hub) as its own scene "Village_FirstVillage" in the current .blend.
# Needs the materials / meshes that Stage1_Areas.blend already has (Grass_*, Dirt, Rock, Trunk, Tree_*, Rock_*).
import bpy, bmesh, math, random
from mathutils import Vector, Matrix, noise

SCENE = "Village_FirstVillage"; CODE = "V0"
PX, PY = 30, 26                       # half size of the walkable village area
SX, SY = 140, 120; HX, HY = SX / 2, SY / 2; ST = 0.5   # terrain incl. open scenery around it
rng = random.Random(42)

def sstep(a, b, x):
    t = (x - a) / (b - a); t = 0.0 if t < 0 else (1.0 if t > 1 else t); return t * t * (3 - 2 * t)
def lerp(a, b, t): return a + (b - a) * t
def segd(px, py, a, b):
    ax, ay = a; vx, vy = b[0] - ax, b[1] - ay; L = vx * vx + vy * vy or 1e-9
    t = max(0.0, min(1.0, ((px - ax) * vx + (py - ay) * vy) / L))
    return math.hypot(px - ax - t * vx, py - ay - t * vy), t
def polyd(px, py, pts): return min(segd(px, py, pts[i], pts[i + 1])[0] for i in range(len(pts) - 1))

# ---------------- layout
PLAZA = (0, -3, 8)                                   # dirt plaza with the well
TERRACE = (3, 14, 18, 6, 3.0)                        # north terrace: player house + elder tree
RAMP = ((0, 4), 0, (0, 11), 3.0, 5)
STREAM = ([(-24, 60), (-20, 28), (-19, 12), (-22, 0), (-18, -12), (-20, -28), (-26, -60)], 5)
PATHS = [[(0, -3), (0, -26), (3, -40), (-2, -60)], [(0, -3), (0, 4)], [(0, 4), (0, 11)], [(0, 11), (-7, 13)], [(0, 11), (9, 12)],
         [(0, -3), (11, -1)], [(0, -3), (-9, -2)], [(0, -3), (12, -14)], [(0, -3), (-10, -11)]]
GATE_X = 0; GATE_Y = -20

def height(x, y):
    n = noise.fractal(Vector((x * 0.04 + 70, y * 0.04, 0.5)), 0.5, 2.0, 4)
    n2 = noise.noise(Vector((x * 0.12, y * 0.12, 9.1)))
    base = 0.5 + 0.35 * n
    cx, cy, rx, ry, ph = TERRACE
    dn = ((abs(x - cx) / rx) ** 4 + (abs(y - cy) / ry) ** 4) ** 0.25
    z = base + ph * sstep(-0.5, 0.5, (1 - dn) * min(rx, ry))          # sharp stone terrace wall
    pts, w = STREAM
    z = lerp(-1.6, z, sstep(w / 2 - 1, w / 2 + 2, polyd(x, y, pts)))
    (a, z0, b, z1, rw) = RAMP
    d, t = segd(x, y, a, b); k = 1 - sstep(rw / 2, rw / 2 + 1, d)
    z = lerp(z, base + lerp(z0, z1, t), k)
    # plaza and paths are flattened a bit
    pk = 1 - sstep(PLAZA[2], PLAZA[2] + 3, math.hypot(x - PLAZA[0], y - PLAZA[1]))
    z = lerp(z, 0.5, pk * 0.8)
    # open surroundings: low rolling hills just outside the village, gentle mountains far away
    od = max(abs(x) - PX, abs(y) - PY) + 3.0 * n2          # >0 outside the village area
    hills = 3.0 * sstep(-2, 14, od) * (1 + 0.5 * n) + 10.0 * sstep(18, 38, od) * (1 + 0.4 * n)
    if y < 0:                                                # the road south stays low
        hills *= sstep(4, 10, abs(x - GATE_X))
    return z + hills

# ---------------- scene setup
M = bpy.data.materials
def mat(name, c, rough=0.9, emit=0.0):
    m = M.get(name)
    if m: return m
    m = M.new(name); m.use_nodes = True
    b = [n for n in m.node_tree.nodes if n.type == 'BSDF_PRINCIPLED'][0]
    b.inputs["Base Color"].default_value = (*c, 1); b.inputs["Roughness"].default_value = rough
    if emit:
        b.inputs["Emission Color"].default_value = (*c, 1); b.inputs["Emission Strength"].default_value = emit
    return m
mat("Wood_Wall", (0.50, 0.36, 0.22)); mat("Wood_Dark", (0.22, 0.14, 0.08)); mat("Plaster", (0.82, 0.76, 0.64))
mat("Roof_Red", (0.45, 0.14, 0.08)); mat("Roof_Thatch", (0.55, 0.45, 0.22)); mat("Stone", (0.45, 0.43, 0.40))
mat("Fire", (1.0, 0.45, 0.1), emit=6.0); mat("Cloth_Blue", (0.15, 0.30, 0.60)); mat("Paper", (0.85, 0.80, 0.65))
mat("Crop", (0.30, 0.55, 0.12)); mat("Soil", (0.28, 0.18, 0.10))

old = bpy.data.scenes.get(SCENE)
if old:
    for o in list(old.collection.all_objects): bpy.data.objects.remove(o)
    for c in list(old.collection.children_recursive): bpy.data.collections.remove(c)
    bpy.data.scenes.remove(old)
sc = bpy.data.scenes.new(SCENE); sc["area_id"] = CODE; sc.world = bpy.data.worlds.get("World")
root = bpy.data.collections.new(SCENE); sc.collection.children.link(root)
def sub(n):
    c = bpy.data.collections.new(CODE + "_" + n); root.children.link(c); return c
def link(o, col): col.objects.link(o); return o

# ---------------- terrain
nx, ny = int(SX / ST), int(SY / ST)
H = [[height(-HX + i * ST, -HY + j * ST) for i in range(nx + 1)] for j in range(ny + 1)]
def samp(x, y):
    fi = (x + HX) / ST; fj = (y + HY) / ST
    i = max(0, min(nx - 1, int(fi))); j = max(0, min(ny - 1, int(fj))); u = fi - i; v = fj - j
    return (H[j][i] * (1 - u) + H[j][i + 1] * u) * (1 - v) + (H[j + 1][i] * (1 - u) + H[j + 1][i + 1] * u) * v
def slope(x, y): return max(abs(samp(x + .5, y) - samp(x - .5, y)), abs(samp(x, y + .5) - samp(x, y - .5)))
W = nx + 1
verts = [(-HX + i * ST, -HY + j * ST, H[j][i]) for j in range(ny + 1) for i in range(nx + 1)]
faces = [(j * W + i, j * W + i + 1, (j + 1) * W + i + 1, (j + 1) * W + i) for j in range(ny) for i in range(nx)]
me = bpy.data.meshes.new(CODE + "_Terrain"); me.from_pydata(verts, [], faces); me.update()
for m in ("Grass_Light", "Grass_Dark", "Dirt", "Rock", "Sand", "Stone"): me.materials.append(M[m])
idx = []
for p in me.polygons:
    x, y, z = p.center; nz = p.normal.z
    n3 = noise.noise(Vector((x * 0.08, y * 0.08, 3.3)))
    if z < 0.1 and polyd(x, y, STREAM[0]) < 6: k = 4
    elif nz < 0.6 and z < 5 and abs(x) < PX and abs(y) < PY: k = 5            # terrace retaining wall
    elif nz < 0.75: k = 3
    elif math.hypot(x - PLAZA[0], y - PLAZA[1]) < PLAZA[2] + n3 or any(polyd(x, y, q) < 1.6 + 0.4 * n3 for q in PATHS): k = 2
    else: k = 0 if n3 > -0.1 else 1
    idx.append(k)
me.polygons.foreach_set("material_index", idx); me.polygons.foreach_set("use_smooth", [True] * len(idx))
link(bpy.data.objects.new(CODE + "_Terrain", me), root)
wm = bpy.data.meshes.new(CODE + "_Water")
wm.from_pydata([(-31, -HY, -0.7), (-14, -HY, -0.7), (-14, HY, -0.7), (-31, HY, -0.7)], [], [(0, 1, 2, 3)])
wm.materials.append(M["Water"]); link(bpy.data.objects.new(CODE + "_Water", wm), root)

# ---------------- mesh helpers
def mesh(name, verts, faces, mats, fmat=None):
    m = bpy.data.meshes.new(name); m.from_pydata(verts, [], faces); m.update()
    for mm in mats: m.materials.append(M[mm])
    if fmat: m.polygons.foreach_set("material_index", fmat)
    return m
def box_vf(sx, sy, sz, ox=0, oy=0, oz=0):
    v = [(ox + dx * sx / 2, oy + dy * sy / 2, oz + dz * sz) for dz in (0, 1) for dy in (-1, 1) for dx in (-1, 1)]
    f = [(0, 2, 3, 1), (4, 5, 7, 6), (0, 1, 5, 4), (2, 6, 7, 3), (0, 4, 6, 2), (1, 3, 7, 5)]
    return v, f
def box(name, sx, sy, sz, matn, col, loc, rot=0.0):
    v, f = box_vf(sx, sy, sz)
    o = bpy.data.objects.new(name, mesh(name, v, f, [matn])); o.location = loc; o.rotation_euler = (0, 0, rot)
    return link(o, col)
def cyl(name, r, h, matn, col, loc, seg=10, r2=None):
    bm = bmesh.new()
    bmesh.ops.create_cone(bm, cap_ends=True, segments=seg, radius1=r, radius2=r if r2 is None else r2, depth=h,
                          matrix=Matrix.Translation((0, 0, h / 2)))
    m = bpy.data.meshes.new(name); bm.to_mesh(m); bm.free(); m.materials.append(M[matn])
    o = bpy.data.objects.new(name, m); o.location = loc; return link(o, col)
def gable(name, w, d, h, matn, col, loc, rot):
    v = [(-w / 2, -d / 2, 0), (w / 2, -d / 2, 0), (w / 2, d / 2, 0), (-w / 2, d / 2, 0), (-w / 2, 0, h), (w / 2, 0, h)]
    f = [(0, 1, 5, 4), (2, 3, 4, 5), (0, 4, 3), (1, 2, 5), (0, 3, 2, 1)]
    o = bpy.data.objects.new(name, mesh(name, v, f, [matn])); o.location = loc; o.rotation_euler = (0, 0, rot)
    return link(o, col)
def ground_min(x, y, w, d):
    return min(samp(x + dx * w / 2, y + dy * d / 2) for dx in (-1, 0, 1) for dy in (-1, 0, 1))

def house(name, x, y, w, d, hgt, rot, col, wall="Plaster", roof="Roof_Red"):
    """front (door side) faces -Y before rotation"""
    z = ground_min(x, y, w, d)
    e = bpy.data.objects.new(name, None); e.location = (x, y, z); e.rotation_euler = (0, 0, rot); link(e, col)
    parts = [box(name + "_Base", w + 0.4, d + 0.4, 0.8, "Stone", col, (0, 0, -0.5)),
             box(name + "_Body", w, d, hgt, wall, col, (0, 0, 0.3)),
             gable(name + "_Roof", w + 1.2, d + 1.4, hgt * 0.55, roof, col, (0, 0, hgt + 0.3), 0),
             box(name + "_Door", 1.4, 0.15, 2.2, "Wood_Dark", col, (0, -d / 2 - 0.05, 0.3))]
    for (px, py) in ((-w / 2, -d / 2), (w / 2, -d / 2), (-w / 2, d / 2), (w / 2, d / 2)):
        parts.append(box(name + "_Beam", 0.35, 0.35, hgt, "Wood_Dark", col, (px, py, 0.3)))
    for px in (-w / 4 - 0.3, w / 4 + 0.3):
        parts.append(box(name + "_Window", 1.0, 0.12, 0.9, "Wood_Dark", col, (px, -d / 2 - 0.05, hgt * 0.5)))
    for p in parts: p.parent = e
    return e, z

# ---------------- buildings
bc = sub("Buildings"); pc = sub("Props"); mc = sub("Markers")
occupied = []   # (x, y, radius) kept free of trees / rocks
def mark(name, x, y, z, kind, **props):
    e = bpy.data.objects.new(CODE + "_" + name, None); e.empty_display_type = kind; e.empty_display_size = 1.5
    e.location = (x, y, z + 0.1); link(e, mc)
    for k, v in props.items(): e[k] = v
    return e

# player house + elder's tree on the terrace
_, z = house(CODE + "_PlayerHouse", -7, 16, 8, 5.5, 3.4, 0, bc, wall="Plaster", roof="Roof_Thatch")
mark("Spawn_Home", -7, 12, samp(-7, 12), 'SINGLE_ARROW')
mark("NPC_Home", -7, 12, samp(-7, 12), 'SPHERE', npc="home")
occupied.append((-7, 16, 6))
t = bpy.data.objects.new(CODE + "_ElderTree", bpy.data.meshes["Tree_Broad"]); t.location = (9, 16, samp(9, 16) - 0.5)
t.scale = (2.0, 2.0, 2.0); link(t, pc); occupied.append((9, 16, 5))
box(CODE + "_ElderBench", 3.0, 0.8, 0.5, "Wood_Wall", pc, (9, 11.5, samp(9, 11.5)))
mark("NPC_Elder", 9, 11, samp(9, 11), 'SPHERE', npc="elder")

# blacksmith (east of plaza)
e, z = house(CODE + "_Blacksmith", 15, 1, 8, 6, 4.0, math.radians(90), bc, wall="Wood_Wall", roof="Roof_Red")
cyl(CODE + "_Chimney", 0.6, 3.2, "Stone", bc, (16.5, 3, z + 3.8), seg=8)
box(CODE + "_Forge", 2.0, 2.0, 1.1, "Stone", pc, (10, 4.5, samp(10, 4.5)))
box(CODE + "_ForgeFire", 1.1, 1.1, 0.3, "Fire", pc, (10, 4.5, samp(10, 4.5) + 1.1))
cyl(CODE + "_Anvil", 0.45, 0.8, "Stone", pc, (10, 0.5, samp(10, 0.5)), seg=6)
mark("NPC_Blacksmith", 10, -1.5, samp(10, -1.5), 'SPHERE', npc="blacksmith")
occupied.append((15, 1, 7))

# item shop stall (west of plaza)
sx_, sy_ = -10, -2; z = samp(sx_, sy_)
box(CODE + "_ShopCounter", 5, 1.2, 1.1, "Wood_Wall", pc, (sx_, sy_, z))
for (dx, dy) in ((-2.6, -0.8), (2.6, -0.8), (-2.6, 2.2), (2.6, 2.2)):
    box(CODE + "_ShopPost", 0.25, 0.25, 3.0, "Wood_Dark", pc, (sx_ + dx, sy_ + dy, z))
aw = box(CODE + "_ShopAwning", 6.2, 4.0, 0.15, "Cloth_Blue", pc, (sx_, sy_ + 0.7, z + 3.0)); aw.rotation_euler = (math.radians(-12), 0, 0)
for i in range(5):
    box(CODE + "_Crate", 0.8, 0.8, 0.7, "Wood_Wall", pc, (sx_ - 2 + i, sy_ + 1.6, z))
mark("NPC_ItemShop", sx_, sy_ + 1.0, z, 'SPHERE', npc="item_shop")
occupied.append((sx_, sy_, 6))

# quest board
qx, qy = 4, -9; z = samp(qx, qy)
for dx in (-1.4, 1.4): box(CODE + "_BoardPost", 0.25, 0.25, 2.6, "Wood_Dark", pc, (qx + dx, qy, z))
box(CODE + "_Board", 3.2, 0.15, 1.6, "Wood_Wall", pc, (qx, qy, z + 1.0))
for i, (px, pz) in enumerate([(-0.9, 1.9), (0.1, 1.7), (0.9, 2.0), (-0.4, 1.25), (0.7, 1.2)]):
    box(CODE + "_Quest%d" % i, 0.6, 0.05, 0.45, "Paper", pc, (qx + px, qy - 0.1, z + pz - 0.2))
mark("NPC_QuestBoard", qx, qy - 1.5, z, 'SPHERE', npc="quest_board")
occupied.append((qx, qy, 3))

# well in the middle of the plaza
z = samp(0, -3)
cyl(CODE + "_Well", 1.4, 0.9, "Stone", pc, (0, -3, z), seg=12)
for dx in (-1.2, 1.2): box(CODE + "_WellPost", 0.2, 0.2, 2.6, "Wood_Dark", pc, (dx, -3, z))
gable(CODE + "_WellRoof", 3.2, 2.4, 0.9, "Roof_Thatch", pc, (0, -3, z + 2.6), math.radians(90))
occupied.append((0, -3, 3))

# villager houses
for i, (x, y, w, rot) in enumerate([(-11, -13, 7, math.radians(-20)), (-12, 7, 7, math.radians(80)),
                                    (18, 15, 6, 0.0)]):
    house(CODE + "_House%d" % i, x, y, w, 5.0, 3.1, rot, bc, wall=rng.choice(["Plaster", "Wood_Wall"]),
          roof=rng.choice(["Roof_Red", "Roof_Thatch"]))
    occupied.append((x, y, 6))

# farm (east-south) with fence
fx0, fx1, fy0, fy1 = 12, 24, -20, -9
for r in range(3):
    y = fy0 + 2 + r * 3.5
    box(CODE + "_FarmRow", fx1 - fx0 - 3, 1.6, 0.25, "Soil", pc, ((fx0 + fx1) / 2, y, samp((fx0 + fx1) / 2, y) - 0.05))
    for k in range(int((fx1 - fx0 - 4) / 1.4)):
        x = fx0 + 2.5 + k * 1.4
        cyl(CODE + "_Crop", 0.35, 0.6, "Crop", pc, (x, y, samp(x, y) + 0.15), seg=5, r2=0.05)
fence = [(fx0, fy0), (fx1, fy0), (fx1, fy1), (fx0, fy1)]
for a, b in zip(fence, fence[1:] + fence[:1]):
    L = math.hypot(b[0] - a[0], b[1] - a[1]); n = int(L / 2)
    for k in range(n + 1):
        x = lerp(a[0], b[0], k / n); y = lerp(a[1], b[1], k / n)
        if abs(x - fx0) < 0.1 and abs(y - (fy0 + fy1) / 2) < 2: continue      # farm gate
        box(CODE + "_FencePost", 0.2, 0.2, 1.1, "Wood_Wall", pc, (x, y, samp(x, y) - 0.1))
occupied.append(((fx0 + fx1) / 2, (fy0 + fy1) / 2, 8))

# village gate to the hunting grounds (south)
gz = samp(GATE_X, GATE_Y)
for dx in (-3.5, 3.5): box(CODE + "_GatePillar", 0.7, 0.7, 5.5, "Wood_Dark", pc, (GATE_X + dx, GATE_Y, gz - 0.3))
box(CODE + "_GateBeam", 9, 0.8, 0.6, "Wood_Dark", pc, (GATE_X, GATE_Y, gz + 4.9))
box(CODE + "_GateSign", 2.6, 0.2, 0.9, "Wood_Wall", pc, (GATE_X, GATE_Y - 0.5, gz + 3.8))
ex = mark("Exit_HuntingGrounds", GATE_X, -PY - 1, samp(GATE_X, -PY - 1) + 2, 'CUBE',
          target_area="Area0_BaseCamp", target_spawn="A0_Spawn_E", exit_type="quest_departure")
ex.scale = (3.5, 1.5, 2.5)
mark("Spawn_FromHunt", GATE_X, GATE_Y + 3.5, samp(GATE_X, GATE_Y + 3.5), 'SINGLE_ARROW')
occupied.append((GATE_X, GATE_Y - 2, 6))

# torches around the plaza
for i in range(6):
    a = i / 6 * 6.283 + 0.5; x, y = math.cos(a) * 9, PLAZA[1] + math.sin(a) * 9
    if any(polyd(x, y, q) < 2 for q in PATHS): continue
    z = samp(x, y)
    box(CODE + "_TorchPole", 0.18, 0.18, 2.2, "Wood_Dark", pc, (x, y, z))
    box(CODE + "_TorchFire", 0.4, 0.4, 0.45, "Fire", pc, (x, y, z + 2.2))

# ---------------- nature around the village
def free(x, y, extra=0):
    if any(math.hypot(x - ox, y - oy) < r + extra for (ox, oy, r) in occupied): return False
    if math.hypot(x - PLAZA[0], y - PLAZA[1]) < PLAZA[2] + 2: return False
    if any(polyd(x, y, q) < 2.5 for q in PATHS): return False
    if polyd(x, y, STREAM[0]) < 5: return False
    return True
# invisible play boundary for the game (no visible wall any more)
bnd = mark("PlayBounds", 0, 0, 0, 'CUBE', bounds=True); bnd.scale = (PX, PY, 10)

tc = sub("Trees"); pts = []
for _ in range(20000):
    if len(pts) >= 260: break
    x = rng.uniform(-HX + 2, HX - 2); y = rng.uniform(-HY + 2, HY - 2)
    od = max(abs(x) - PX, abs(y) - PY)
    if od > 0 and y < 0 and abs(x - GATE_X) < 7: continue          # keep the road south open
    if rng.random() > 0.12 + 0.88 * sstep(-6, 8, od): continue       # few inside, forest outside
    if slope(x, y) > 0.45 or not free(x, y, 2): continue
    if any((px - x) ** 2 + (py - y) ** 2 < (20 if od < 0 else 14) for px, py in pts): continue
    pts.append((x, y)); s = rng.uniform(0.9, 1.4)
    o = bpy.data.objects.new(CODE + "_Tree", bpy.data.meshes["Tree_Pine" if rng.random() < 0.4 else "Tree_Broad"])
    o.location = (x, y, samp(x, y) - 0.3); o.scale = (s, s, s); o.rotation_euler = (0, 0, rng.uniform(0, 6.28)); link(o, tc)
rc = sub("Rocks")
for _ in range(400):
    x = rng.uniform(-HX + 3, HX - 3); y = rng.uniform(-HY + 3, HY - 3)
    if not free(x, y, 1) or len(rc.objects) >= 40: continue
    s = rng.uniform(1.5, 3) if slope(x, y) > 0.45 else rng.uniform(0.4, 1.2)
    o = bpy.data.objects.new(CODE + "_Rock", bpy.data.meshes["Rock_%d" % rng.randint(0, 2)])
    o.location = (x, y, samp(x, y) - 0.35 * s); o.scale = (s,) * 3; o.rotation_euler = (0, 0, rng.uniform(0, 6.28)); link(o, rc)
gv, gf, gmi = [], [], []
for _ in range(20000):
    if len(gf) >= 3 * 3000: break
    x = rng.uniform(-HX + 2, HX - 2); y = rng.uniform(-HY + 2, HY - 2)
    if slope(x, y) > 0.3 or not free(x, y): continue
    z = samp(x, y)
    for k in range(3):
        a = rng.uniform(0, 6.28); hgt = rng.uniform(0.35, 0.7); dx, dy = math.cos(a) * 0.12, math.sin(a) * 0.12
        b = len(gv); gv += [(x - dx, y - dy, z - 0.05), (x + dx, y + dy, z - 0.05), (x - dy * 1.2, y + dx * 1.2, z + hgt)]
        gf.append((b, b + 1, b + 2)); gmi.append(0)
gm = mesh(CODE + "_Grass", gv, gf, ["Grass_Blade"]); link(bpy.data.objects.new(CODE + "_Grass", gm), root)

# ---------------- light / camera
sun = bpy.data.objects.new(CODE + "_Sun", bpy.data.lights["Sun"]); sun.rotation_euler = (math.radians(50), 0, math.radians(35)); link(sun, root)
cd = bpy.data.cameras.new(CODE + "_Cam"); cd.lens = 24; cd.clip_end = 1000
cam = bpy.data.objects.new(CODE + "_Cam", cd); cam.location = (5, -48, 30); link(cam, root)
cam.rotation_euler = (Vector((0, 0, 0)) - cam.location).to_track_quat('-Z', 'Y').to_euler()
sc.camera = cam
sc.render.resolution_x = 960; sc.render.resolution_y = 540
try: sc.eevee.taa_render_samples = 16
except Exception: pass
result = {"scene": SCENE, "objects": len(root.all_objects), "trees": len(pts), "rocks": len(rc.objects)}
print("VILLAGE", result)
