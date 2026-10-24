# The retail bone rig for the skinned MEF models

Verification status: `STRUCTURALLY_VERIFIED` for the transcribed navigation
helpers and the skeleton reader. The model-to-skeleton selection is a host
helper; the reasoning and the measurements are recorded below.

## The question the Level Editor answered with hardcoded data

The skinned MEF models carry no skeleton. The MESH header, the 40-byte vertex
influence records and the draw records are all the file holds: there is no
chunk with bone names, no parent-child linkage and no rest-pose joint offsets.
The Level Editor and the file converter rebuild the skeleton from four
hardcoded rig archetypes, keyed by the model name and by the highest bone
index the model's vertex stream uses.

`igi.exe` does not hardcode a rig. The character skeletons live in the retail
animation files under `COMMON/ANIMS`, and the MTP package names them in its
`BANM` chunk. This reconstruction therefore reads the rig from the retail data
rather than hardcoding the archetypes.

## The retail natives

| Address    | Role |
| ---------- | ---- |
| 0x00517890 | find a chunk or form by tag, descending into nested forms |
| 0x00517940 | find a chunk by tag among a form's children |
| 0x005179C0 | the chunk's payload pointer |
| 0x005178E0 | find a chunk by tag, starting below a matching form |
| 0x004D67A0 | the animation file loader, which reads the skeleton |

The four navigation helpers are transcribed in
`engine/src/retail/model/iff_chunk_tree.cpp` and the skeleton read in
`engine/src/retail/model/iff_skeleton.cpp`.

The retail's chunk node layout the helpers read:

| Offset | Meaning |
| ------ | ------- |
| +0x00  | the next sibling node |
| +0x08  | the first child of a form |
| +0x10  | the chunk's tag, or the FORM marker for a form |
| +0x14  | the form type of a form |
| +0x1C  | the chunk's payload |

`0x004D67A0` walks to the `BOBJ` form and then to the `BOBH` form and reads
three chunks: `BOSH` holds the object id and the bone count, `PLST` the parent
index of every bone and `TLST` the rest-pose joint offset of every bone. The
container is big-endian, like the retail's IFF reader, and the chunk payloads
are little-endian, like the rest of the retail data.

The MTP package's `BANM` chunk holds the animation file names; the retail
loads each as `"<prefix>anims/<name>.iff"` (`DynObj_LoadPackageAnimations`,
0x004CF910). The retail `COMMON/ANIMS` folder holds `000`, `001`, `002`,
`003`, `005` and `006`, which is exactly the six names the `location0` MTP
package lists.

## Choosing the skeleton for a model

A skinned MEF stores every influencing bone of a vertex in that bone's own
local space, so the records of one vertex must place it on the same point in
model space. The selection measures that agreement: the median distance
between a vertex's base record and its extra influence records once both are
placed in model space.

Measured against the retail Level 4 models with the retail `COMMON/ANIMS`
skeletons, the matching skeleton agrees to 0.03-0.10 units while every other
candidate is off by 55 to 1723 units. The choice is therefore decided by the
model data alone, with no name list and no hardcoded rig.

| Model      | Selected object id | Bones | Agreement |
| ---------- | ----------------- | --------- | --------- |
| 000_01_1   | 0                  | 33        | 0.090     |
| 003_01_1   | 3                  | 33        | 0.104     |
| 013_01_1   | 1                  | 33        | 0.070     |
| 015_01_1   | 6                  | 33        | 0.034     |
| 021_01_1   | 1                  | 33        | 0.071     |
| 022_01_1   | 1                  | 33        | 0.081     |
| 009_02_1   | 2                  | 33        | 0.098     |
| 020_01_1   | 2                  | 33        | 0.072     |

The agreement is in retail world units, so 0.1 units is 0.024 mm.

The six retail skeletons are four distinct rigs, which is exactly the Level
Editor's four archetypes:

| Object ids | Bones | Distinct rig |
| ---------- | ----- | ------------ |
| 0          | 33    | the cinematic actors |
| 1, 2, 3    | 33    | the standard soldier (1, 2 and 3 differ by 0.0008 units, which is float noise) |
| 6          | 33    | the heavy soldier |
| 5          | 56    | the advanced finger rig |

The Level Editor's four archetypes are the rest offsets of these same retail
skeletons: the advanced finger rig archetype matches the 56-bone skeleton of
`005.IFF` to 0.0001 units, and the three 33-bone archetypes match `000`, `001`
and `006` up to the root bone's own offset. The converter hardcodes them only
because it cannot ship the retail animation files.

## The palette

The retail skinner at 0x0049B700 applies `(R*v + T) * weight` per influence,
with the palette entry holding the rotation at byte +0x24 and the translation
at byte +0x18. The rest pose uses the identity rotation and the bone's world
position, which is the bone's own rest offset plus every ancestor's.

The per-vertex influence weights sum to exactly 1.0 over the retail records,
which is the property the skinner evaluates.

## Runtime evidence

`open-igi.exe window level4` and `window level1` both load the retail
skeletons and select one per skinned model:

```
Character skeletons loaded: 6 of 6 animation files
Skinned MEF model=015_01_1 rig_object_id=6 bones=33 influence_agreement=0.033604 units
Skinned MEF model=021_01_1 rig_object_id=1 bones=33 influence_agreement=0.071037 units
Skinned MEF model=009_02_1 rig_object_id=2 bones=33 influence_agreement=0.097534 units
Skinned MEF model=000_01_1 rig_object_id=0 bones=33 influence_agreement=0.090240 units
```

Level 1 loads five skinned models and level 4 loads eight, and every one of
them gets the skeleton its own data selects.

## What is still missing

- The animation clips. The retail `BOAN` forms hold the per-frame root
  translation and per-bone rotation keys; the port reads the skeleton but does
  not yet play the clips, so the characters are drawn in the rest pose.
- The `005` skeleton's 56 bones are only used by the model whose vertex
  stream indexes bone 48, which is the advanced finger rig model.
