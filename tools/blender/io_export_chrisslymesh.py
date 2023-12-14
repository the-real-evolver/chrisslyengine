#------------------------------------------------------------------------------
#  io_export_chrisslymesh.py
#  (C) 2022 Christian Bleicher
#------------------------------------------------------------------------------
bl_info = {
    "name": "ChrisslyEngine-Mesh format(.mesh)",
    "author": "Christian Bleicher (evolver)",
    "version": (0, 0, 1),
    "blender": (3, 1, 0),
    "location": "File > Export > ChrisslyMesh (.mesh)",
    "description": "Export to ChrisslyEngine-Mesh format (.mesh)",
    "warning": "",
    "doc_url": "",
    "category": "Import-Export",
}

import operator
import sys
import os
import re
import bpy
import bmesh
import time
import math
import mathutils
from mathutils import Vector, Matrix
from array import array
from bpy_extras.io_utils import axis_conversion
from bpy_extras.io_utils import ExportHelper

#------------------------------------------------------------------------------
M_SUBMESH = b'\x01'
M_MESH_BOUNDS = b'\x02'
M_ANIMATION = b'\x03'
M_ANIMATION_TRACK = b'\x04'
M_ANIMATION_MORPH_KEYFRAME = b'\x05'
M_MESH_SKELETON_FILE = b'\x06'

PF_R8G8B8A8 = b'\x05'

BYTES_PER_VERTEX = 36

BONE_WEIGHTS_PER_VERTEX = 4

#------------------------------------------------------------------------------
def ce_get_bb_min_max_edges(objects):
    min_edge = mathutils.Vector((sys.float_info.max, sys.float_info.max, sys.float_info.max))
    max_edge = mathutils.Vector((-sys.float_info.max, -sys.float_info.max, -sys.float_info.max))
    for ob in objects:
        if ob.type == 'MESH' and len(ob.data.polygons) != 0:
            for corner in ob.bound_box: # relative to object origin
                p = mathutils.Vector(corner)
                if p.x < min_edge.x:
                    min_edge.x = p.x
                if p.x > max_edge.x:
                    max_edge.x = p.x
                if p.y < min_edge.y:
                    min_edge.y = p.y
                if p.y > max_edge.y:
                    max_edge.y = p.y
                if p.z < min_edge.z:
                    min_edge.z = p.z
                if p.z > max_edge.z:
                    max_edge.z = p.z
    return min_edge, max_edge

#------------------------------------------------------------------------------
def ce_get_bounding_sphere(objects):
    min_edge, max_edge = ce_get_bb_min_max_edges(objects)
    radius = (max_edge - min_edge).length * 0.5
    center = mathutils.Vector(((min_edge.x + max_edge.x) * 0.5, (min_edge.y + max_edge.y) * 0.5, (min_edge.z + max_edge.z) * 0.5))
    return radius, center

#------------------------------------------------------------------------------
def ce_get_longest_extend(objects):
    min_edge, max_edge = ce_get_bb_min_max_edges(objects)
    longest_extend = 0.0
    if max_edge.x - min_edge.x > longest_extend:
        longest_extend = max_edge.x - min_edge.x
    if max_edge.y - min_edge.y > longest_extend:
        longest_extend = max_edge.y - min_edge.y
    if max_edge.z - min_edge.z > longest_extend:
        longest_extend = max_edge.z - min_edge.z
    if longest_extend <= 0.0:
        longest_extend = 1.0
    return longest_extend

#------------------------------------------------------------------------------
def ce_export_path(path):
    # 1. tokenize path
    tokens = []
    while 1:
        token = os.path.split(path)
        if token[0] == path:  # os.path.split returns drive in tokens[0] for absolute paths
            tokens.insert(0, token[0])
            break
        elif token[1] == path: # os.path.split returns directory in tokens[1] for relative paths
            tokens.insert(0, token[1])
            break
        else:
            path = token[0]
            tokens.insert(0, token[1])

    # 2. extract path relative to export folder
    index = tokens.index("export") if "export" in tokens else -1
    if index == -1 or index == (len(tokens) - 2):
        path = ""
    else:
        p = tokens[index + 1:len(tokens) - 1]
        path = os.path.join(*p)
        path += ("/")
        path = os.path.normcase(path)
        path = re.escape(path)
    return path

#------------------------------------------------------------------------------
def ce_write_texture(image, file_path):
    file = open(file_path + "\\" + os.path.splitext(image.name)[0] + ".tex", 'wb')
    # 1. write format tag
    file.write(PF_R8G8B8A8)
    # 2. write width and height
    byte_array = array('H', [image.size[0], image.size[1]])
    byte_array.tofile(file)
    # 3. write number of mipmaps
    file.write(b'\x00')
    # 4. write swizzled flag
    file.write(b'\x00')
    # 5. write pixel data
    byte_array = bytearray(len(image.pixels))
    for i, p in enumerate(image.pixels):
        c = int(p * 255.0)
        byte_array[i] = c if c < 255 else 255
    file.write(byte_array)
    file.close()

#------------------------------------------------------------------------------
def ce_write_material(file_path, materials):
    file = open(file_path, 'wt')
    for mat in materials:
        file.write("material \"%s\"\n" % (mat.name))
        file.write("{\n")
        file.write("    pass\n")
        file.write("    {\n")
        file.write("        lighting on\n")
        if mat.blend_method == 'BLEND':
            file.write("        scene_blend src_alpha one_minus_src_alpha\n")
            file.write("        depth_write off\n")
        elif mat.blend_method == 'CLIP':
            file.write("        alpha_test less 0.001\n")
        file.write("        diffuse %f %f %f %f\n" % (mat.diffuse_color[0], mat.diffuse_color[1], mat.diffuse_color[2], mat.diffuse_color[3]))
        file.write("        specular %f %f %f 1.0 %f\n" % (mat.specular_color[0], mat.specular_color[1], mat.specular_color[2], mat.specular_intensity * 64.0))
        file.write("        cull_hardware %s\n" % ('clockwise' if mat.use_backface_culling else 'none'))
        file.write("        roughness %f\n" % (mat.roughness))
        file.write("        metallic %f\n" % (mat.metallic))
        if 'gpu_program' in mat:
            file.write("        gpu_program \"%s\"\n" % (mat.get('gpu_program', "default")))
        if hasattr(mat.node_tree, 'nodes'):
            for tex in mat.node_tree.nodes:
                if tex.type == 'TEX_IMAGE' and hasattr(tex.image, 'name'):
                    file.write("        texture_unit\n")
                    file.write("        {\n")
                    file.write("            texture \"%s\"\n" % (ce_export_path(file_path) + os.path.splitext(tex.image.name)[0] + ".tex"))
                    if tex.projection == 'SPHERE':
                        file.write("            env_map spherical\n")
                    if tex.interpolation == 'Closest':
                        file.write("            filtering point point point\n")
                    for input in tex.inputs:
                        for link in input.links:
                            if link.from_node.type == 'MAPPING':
                                scale = link.from_node.inputs['Scale'].default_value
                                file.write("            scale %f %f\n" % (scale[0], scale[1]))
                    file.write("        }\n")
                    ce_write_texture(tex.image, os.path.dirname(file_path))
        if 'cubic_texture' in mat:
            file.write("        texture_unit\n")
            file.write("        {\n")
            file.write("            cubic_texture \"%s\"\n" % (mat.get('cubic_texture', "default")))
            file.write("        }\n")
        file.write("    }\n")
        file.write("}\n\n")
    file.close()

#------------------------------------------------------------------------------
def ce_write_mesh(file_path, objects, scale_uniform, operator_instance, armature = None, write_all_weights = False):
    # calculate scale and bounding sphere
    scale = (1.0 / ce_get_longest_extend(objects)) if scale_uniform else 1.0
    radius, center = ce_get_bounding_sphere(objects)
    radius *= scale

    # 1. write bounds
    file = open(file_path, 'wb')
    file.write(M_MESH_BOUNDS)
    byte_array = array('f', [center.x, center.z, -center.y] if armature == None else [center.x, center.y, center.z])
    byte_array.tofile(file)
    byte_array = array('f', [radius])
    byte_array.tofile(file)

    # gather all used materials for export of the material file
    used_materials = []
    for ob in objects:
        if ob.type == 'MESH' and len(ob.data.polygons) != 0:
            # triangulate mesh
            mesh = ob.data.copy()
            bm = bmesh.new()
            bm.from_mesh(mesh)
            bmesh.ops.triangulate(bm, faces=bm.faces[:], quad_method='BEAUTY', ngon_method='BEAUTY')
            bm.to_mesh(mesh)
            bm.free()

            # scaling and axis conversion
            if armature == None:
                mesh.transform((Matrix.Scale(scale, 4) @ axis_conversion(to_forward='-Z', to_up='Y').to_4x4()) @ ob.matrix_world)

            for index, mat_slot in enumerate(ob.material_slots):
                # 2. write submesh chunk id
                file.write(M_SUBMESH)

                # 3. write material name
                byte_array = array('B', [len(mat_slot.name)])
                byte_array.tofile(file)
                file.write(mat_slot.name.encode())
                used_materials.append(mat_slot.material)

                # gather all faces assigned to mat_slot
                faces_cur_mat = []
                for face in mesh.polygons:
                    if face.material_index == index:
                        faces_cur_mat.append(face)

                # 4. write vertex count
                byte_array = array('I', [len(faces_cur_mat) * 3])
                byte_array.tofile(file)

                # 5. write bytes per vertex
                if armature != None:
                    if write_all_weights:
                        byte_array = array('I', [BYTES_PER_VERTEX + 4 * len(armature.bones)])
                    else:
                        byte_array = array('I', [BYTES_PER_VERTEX + 32])
                else:
                    byte_array = array('I', [BYTES_PER_VERTEX])
                byte_array.tofile(file)

                # write vertices
                for face in faces_cur_mat:
                    for vert_idx, loop_idx in zip(face.vertices, face.loop_indices):
                        # 6. write vertex weights
                        if armature != None:
                            vertex_groups = mesh.vertices[vert_idx].groups
                            if write_all_weights:
                                # all weights
                                weights = []
                                for w in range(len(armature.bones)):
                                    weights.append(0.0)
                                for g in vertex_groups:
                                    weights[g.group] = g.weight
                                for weight in weights:
                                    byte_array = array('f', [weight])
                                    byte_array.tofile(file)
                            else:
                                if len(vertex_groups) > BONE_WEIGHTS_PER_VERTEX:
                                    operator_instance.report({'WARNING'}, "Vertex has more deform weights than allowed (limit is 4)")
                                sum_weights = 0.0
                                for w in vertex_groups:
                                    sum_weights += w.weight
                                weight_scale = 1.0
                                if not math.isclose(sum_weights, 1.0, rel_tol=0.02):
                                    weight_scale = 1.0 / sum_weights if sum_weights > 0.0 else 1.0
                                    operator_instance.report({'WARNING'}, "Bone weights will be normalised so they sum up to one")
                                weights = []
                                indices = []
                                for i in range(BONE_WEIGHTS_PER_VERTEX):
                                    if len(vertex_groups) > i:
                                        index = armature.bones.find(ob.vertex_groups[vertex_groups[i].group].name)
                                        if index >= 0:
                                            weights.append(vertex_groups[i].weight * weight_scale)
                                            indices.append(index)
                                        else:
                                            weights.append(0.0)
                                            indices.append(0)
                                    else:
                                        weights.append(0.0)
                                        indices.append(0)
                                for weight in weights:
                                    byte_array = array('f', [weight])
                                    byte_array.tofile(file)
                                for index in indices:
                                    byte_array = array('I', [index])
                                    byte_array.tofile(file)

                        # 7. write texture coordinates
                        byte_array = array('f', mesh.uv_layers.active.data[loop_idx].uv)
                        byte_array.tofile(file)
                        # 8. write color (just white for now)
                        byte_array = array('I', [0xffffffff])
                        byte_array.tofile(file)
                        # 9. write normal
                        byte_array = array('f', mesh.vertices[vert_idx].normal)
                        byte_array.tofile(file)
                        # 10. write position
                        byte_array = array('f', mesh.vertices[vert_idx].co)
                        byte_array.tofile(file)

    # write filename of skeleton definition
    if armature != None:
        file.write(M_MESH_SKELETON_FILE)
        skeleton_file_path = ce_export_path(file_path) + os.path.splitext(os.path.basename(file_path))[0] + ".skeleton"
        byte_array = array('B', [len(skeleton_file_path)])
        byte_array.tofile(file)
        file.write(skeleton_file_path.encode())

    file.close()

    return used_materials

#------------------------------------------------------------------------------
def ce_write_positions(file_path, objects, scale_uniform):
    # calculate scale
    scale = (1.0 / ce_get_longest_extend(objects)) if scale_uniform else 1.0

    file = open(file_path, 'wb')

    # write bounding box
    min_edge, max_edge = ce_get_bb_min_max_edges(objects)
    byte_array = array('f', [min_edge.x, min_edge.z, -min_edge.y, max_edge.x, max_edge.z, -max_edge.y])
    byte_array.tofile(file)

    for ob in objects:
        if ob.type == 'MESH' and len(ob.data.polygons) != 0:
            # triangulate mesh
            mesh = ob.data.copy()
            bm = bmesh.new()
            bm.from_mesh(mesh)
            bmesh.ops.triangulate(bm, faces=bm.faces[:], quad_method='BEAUTY', ngon_method='BEAUTY')
            bm.to_mesh(mesh)
            bm.free()

            # scaling and axis conversion
            mesh.transform((Matrix.Scale(scale, 4) @ axis_conversion(to_forward='-Z', to_up='Y').to_4x4()) @ ob.matrix_world)

            # write positions
            for face in mesh.polygons:
                for vert_idx in face.vertices:
                    byte_array = array('f', mesh.vertices[vert_idx].co)
                    byte_array.tofile(file)
    file.close()

#------------------------------------------------------------------------------
def ce_write_morph_animation(file_path):
    depsgraph = bpy.context.evaluated_depsgraph_get()

    # gather all used materials
    used_materials = []
    scene_frames = range(bpy.context.scene.frame_start, bpy.context.scene.frame_end + 1)
    for frame in scene_frames:
        bpy.context.scene.frame_set(frame, subframe = 0.0)
        for ob in bpy.context.scene.objects:
            ob_instance = ob.evaluated_get(depsgraph)
            if ob_instance.type == 'MESH' and len(ob_instance.data.polygons) != 0:
                for index, mat_slot in enumerate(ob_instance.material_slots):
                    used_materials.append(mat_slot.material)
    used_materials = list(dict.fromkeys(used_materials))

    file = open(file_path, 'wb')

    # 1. write bounds
    radius, center = ce_get_bounding_sphere(bpy.context.scene.objects)
    file.write(M_MESH_BOUNDS)
    byte_array = array('f', [center.x, center.z, -center.y])
    byte_array.tofile(file)
    byte_array = array('f', [radius])
    byte_array.tofile(file)

    # 2. write animation tag and length
    length = len(scene_frames) / bpy.context.scene.render.fps
    key_length = length / (len(scene_frames) - 1);
    file.write(M_ANIMATION)
    byte_array = array('f', [length])
    byte_array.tofile(file)

    for track, mat in enumerate(used_materials):
        # 3. write empty (0 vertices) submesh for animationtrack
        file.write(M_SUBMESH)
        byte_array = array('B', [len(mat.name)])
        byte_array.tofile(file)
        file.write(mat.name.encode())
        byte_array = array('I', [0])
        byte_array.tofile(file)
        byte_array = array('I', [BYTES_PER_VERTEX])
        byte_array.tofile(file)

        # 4. write animationtrack tag and handle
        file.write(M_ANIMATION_TRACK)
        byte_array = array('B', [track])
        byte_array.tofile(file)

        current_time = 0.0;

        for frame in scene_frames:
            # update state of objects to current frame
            bpy.context.scene.frame_set(frame, subframe = 0.0)
            for ob in bpy.context.scene.objects:
                # get object instance for this frame
                ob_instance = ob.evaluated_get(depsgraph)
                if ob_instance.type == 'MESH' and len(ob_instance.data.polygons) != 0:
                    # find mat_slot for material
                    for mat_index, mat_slot in enumerate(ob_instance.material_slots):
                        if mat_slot.material == mat:
                            # triangulate mesh
                            mesh = ob_instance.data.copy()
                            bm = bmesh.new()
                            bm.from_mesh(mesh)
                            bmesh.ops.triangulate(bm, faces=bm.faces[:], quad_method='FIXED', ngon_method='BEAUTY')
                            bm.to_mesh(mesh)
                            bm.free()

                            # scaling and axis conversion
                            mesh.transform((Matrix.Scale(1.0, 4) @ axis_conversion(to_forward='-Z', to_up='Y').to_4x4()) @ ob_instance.matrix_world)

                            # gather all faces assigned to track
                            faces_cur_mat = []
                            for face in mesh.polygons:
                                if face.material_index == mat_index:
                                    faces_cur_mat.append(face)

                            # 5. write key tag, keytime and vertexdata of the key
                            file.write(M_ANIMATION_MORPH_KEYFRAME)
                            byte_array = array('f', [current_time])
                            byte_array.tofile(file)
                            byte_array = array('I', [len(faces_cur_mat) * 3])
                            byte_array.tofile(file)
                            byte_array = array('I', [BYTES_PER_VERTEX])
                            byte_array.tofile(file)

                            current_time += key_length;

                            # write vertices
                            for face in faces_cur_mat:
                                for vert_idx, loop_idx in zip(face.vertices, face.loop_indices):
                                    # 6. write texture coordinates
                                    byte_array = array('f', mesh.uv_layers.active.data[loop_idx].uv)
                                    byte_array.tofile(file)
                                    # 7. write color (just white for now)
                                    byte_array = array('I', [0xffffffff])
                                    byte_array.tofile(file)
                                    # 8. write normal
                                    byte_array = array('f', mesh.vertices[vert_idx].normal)
                                    byte_array.tofile(file)
                                    # 9. write position
                                    byte_array = array('f', mesh.vertices[vert_idx].co)
                                    byte_array.tofile(file)
    file.close()

    return used_materials

#------------------------------------------------------------------------------
def ce_write_skeletal_animation(file_path, objects, write_all_weights, operator_instance):
    used_materials = []
    # find armature
    armature = None
    for ob in bpy.context.scene.objects:
        if ob.type == 'ARMATURE':
            armature = ob.data
    if armature == None:
        return used_materials

    # write restpose mesh
    used_materials = ce_write_mesh(file_path, objects, False, operator_instance, armature, write_all_weights)

    # write bones
    file = open(os.path.splitext(file_path)[0] + ".skeleton", 'wt')
    file.write("skeleton\n{\n")
    file.write("    num_bones %i\n"%(len(armature.bones)))
    for bone in armature.bones:
        file.write("    bone \"%s\"\n" % (bone.name))
        file.write("    {\n")
        file.write("        index %i\n"%(armature.bones.find(bone.name)))
        file.write("        parent %i\n"%(armature.bones.find(bone.parent.name) if bone.parent else -1))
        m = bone.parent.matrix_local.inverted() @ bone.matrix_local if bone.parent else bone.matrix_local
        file.write("        local_matrix %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n"%(\
        m[0][0], m[0][1], m[0][2], m[0][3], m[1][0], m[1][1], m[1][2], m[1][3], m[2][0], m[2][1], m[2][2], m[2][3], m[3][0], m[3][1], m[3][2], m[3][3]))
        m = bone.matrix_local.inverted()
        file.write("        inv_model_matrix %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n"%(\
        m[0][0], m[0][1], m[0][2], m[0][3], m[1][0], m[1][1], m[1][2], m[1][3], m[2][0], m[2][1], m[2][2], m[2][3], m[3][0], m[3][1], m[3][2], m[3][3]))
        file.write("    }\n")
    file.write("}\n")

    # write animations
    for action in bpy.data.actions:
        num_keyframes = 0
        animation_length = 0.0
        for fcu in action.fcurves:
            num_keyframes = len(fcu.keyframe_points)
            animation_length = fcu.keyframe_points[-1].co.x

        file.write("animation \"%s\"\n{\n"%(action.name))
        file.write("    length %f\n"%(animation_length / bpy.context.scene.render.fps))
        file.write("    num_keyframes %i\n"%(num_keyframes))
        for bone_index in range(len(armature.bones)):
            # track
            file.write("    track \"%s\"\n    {\n"%(armature.bones[bone_index].name))
            file.write("        index %i\n"%(bone_index))
            has_rotation_quaternion = False
            for key_index in range(num_keyframes):
                # keyframe
                file.write("        key\n        {\n")
                time = 0.0
                position = mathutils.Vector((0.0, 0.0, 0.0))
                orientation = mathutils.Quaternion((1.0, 0.0, 0.0, 0.0))
                for curve in action.fcurves:
                    for index, keyframe in enumerate(curve.keyframe_points):
                        bone_name = curve.data_path.split("[", maxsplit = 1)[1].split("]", maxsplit = 1)[0][1:-1]
                        if index == key_index and bone_name == armature.bones[bone_index].name:
                            if curve.data_path.find("location") != -1:
                                if curve.array_index == 0:
                                    time = keyframe.co.x / bpy.context.scene.render.fps
                                    position.x = keyframe.co.y
                                if curve.array_index == 1:
                                    position.y = keyframe.co.y
                                if curve.array_index == 2:
                                    position.z = keyframe.co.y
                            if curve.data_path.find("rotation_quaternion") != -1:
                                has_rotation_quaternion = True
                                if curve.array_index == 0:
                                    orientation.w = keyframe.co.y
                                if curve.array_index == 1:
                                    orientation.x = keyframe.co.y
                                if curve.array_index == 2:
                                    orientation.y = keyframe.co.y
                                if curve.array_index == 3:
                                    orientation.z = keyframe.co.y
                file.write("            time %f\n"%(time))
                m = orientation.to_matrix().to_4x4() @ mathutils.Matrix.Translation(position)
                file.write("            local_matrix %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n"%(\
                m[0][0], m[0][1], m[0][2], m[0][3], m[1][0], m[1][1], m[1][2], m[1][3], m[2][0], m[2][1], m[2][2], m[2][3], m[3][0], m[3][1], m[3][2], m[3][3]))
                file.write("        }\n")
            file.write("    }\n")
            if not has_rotation_quaternion:
                operator_instance.report({'WARNING'}, "Bone \"%s\" has to be in rotation mode \"Quaternion (WXYZ)\""%(armature.bones[bone_index].name))
        file.write("}\n")

    file.close()

    return used_materials

#------------------------------------------------------------------------------
class Export_ChrisslyEngineMesh(bpy.types.Operator, ExportHelper):
    bl_idname = "export_scene.chrisslyengine_mesh"
    bl_label = "Export ChrisslyEngine-Mesh"

    # ui elements/properties
    scale_uniform: bpy.props.BoolProperty(name="Scale to uniform size", description="Scale the exported mesh so it's longest extend has size of 1 unit", default=False)
    separate_objects: bpy.props.BoolProperty(name="Objects as separate files", description="Each object will be saved as separate file, material file is shared", default=False)
    selected_only: bpy.props.BoolProperty(name="Selected only", description="Only selected objects will be exported", default=False)
    position_only: bpy.props.BoolProperty(name="Position coordinates only", description="Only the aabb min and max followed by the vertex positions will be written to the file, can be used as collision geometry", default=False)
    export_morph_animation: bpy.props.BoolProperty(name="Export morph animation", description="Export scene frames as morph animation keyframes", default=False)
    export_skeletal_animation: bpy.props.BoolProperty(name="Export skeletal animation", description="Export restpose, skeleton and animations", default=False)
    export_all_bone_weights: bpy.props.BoolProperty(name="Export all bone weights", description="By default only the 4 most influencial bone weights and the index to their matrix are stored per vertex, enalbe this if you want to store all weights per vertex (mandatory on the PSP since there is no support for indexed vertex blending)", default=False)

    filename_ext = ".mesh"

    # ui layout description
    def draw(self, context):
        box = self.layout.box()
        box.label(text="Animation")
        box.prop(self, 'export_skeletal_animation')
        box.prop(self, 'export_all_bone_weights')
        box.prop(self, 'export_morph_animation')
        box = self.layout.box()
        box.label(text="Objects")
        box.prop(self, 'separate_objects')
        box.prop(self, 'selected_only')
        box = self.layout.box()
        box.label(text="Misc")
        box.prop(self, 'scale_uniform')
        box.prop(self, 'position_only')

    # export the mesh
    def execute(self, context):
        start_time = time.time()

        used_materials = []

        # selected only
        objects = bpy.context.selected_objects if self.selected_only else bpy.context.scene.objects

        if self.export_skeletal_animation:
            # export skeletal animation
            used_materials = ce_write_skeletal_animation(self.filepath, objects, self.export_all_bone_weights, self)
        elif self.export_morph_animation:
            # export morph animation
            used_materials += ce_write_morph_animation(self.filepath)
        else:
            # separate objects
            if self.separate_objects:
                for i, ob in enumerate(objects):
                    obj_arr = [ob]
                    if self.position_only:
                        ce_write_positions(os.path.splitext(self.filepath)[0] + str(i) + os.path.splitext(self.filepath)[1], obj_arr, self.scale_uniform)
                    else:
                        used_materials += ce_write_mesh(os.path.splitext(self.filepath)[0] + str(i) + os.path.splitext(self.filepath)[1], obj_arr, self.scale_uniform, self)
            else:
                if self.position_only:
                    ce_write_positions(self.filepath, objects, self.scale_uniform)
                else:
                    used_materials += ce_write_mesh(self.filepath, objects, self.scale_uniform, self)

        if len(used_materials) > 0:
            # assemble filename for material file and remove duplicates from material list
            ce_write_material(os.path.splitext(self.filepath)[0] + ".material", list(dict.fromkeys(used_materials)))

        self.report({'INFO'}, "Export \"%s\" finished, time taken %f sec"%(self.filepath, time.time() - start_time))

        return {'FINISHED'}

#------------------------------------------------------------------------------
# entry in the "File -> Export" list
def menu_func_export(self, context):
    self.layout.operator(Export_ChrisslyEngineMesh.bl_idname, text="ChrisslyEngine-Mesh (.mesh)")

# register and add to the file selector
def register():
    bpy.utils.register_class(Export_ChrisslyEngineMesh)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)

# unregister from file selector
def unregister():
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)
    bpy.utils.unregister_class(Export_ChrisslyEngineMesh)

if __name__ == "__main__":
    register()
