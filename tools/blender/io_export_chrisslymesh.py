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

import os
import bpy
import bmesh
import mathutils
from mathutils import Vector, Matrix
from array import array
from bpy_extras.io_utils import axis_conversion

#------------------------------------------------------------------------------
M_SUBMESH = b'\x01'
M_MESH_BOUNDS = b'\x02'
M_ANIMATION = b'\x03'
M_ANIMATION_TRACK = b'\x04'
M_ANIMATION_MORPH_KEYFRAME = b'\x05'

PF_R8G8B8A8 = b'\x05'

#------------------------------------------------------------------------------
def ce_get_bounding_radius(objects):
    bounding_radius = 0.0
    for ob in objects:
        if ob.type == 'MESH' and len(ob.data.polygons) != 0:
            for corner in ob.bound_box: # relative to object origin
                p = mathutils.Vector(corner)
                if p.length > bounding_radius:
                    bounding_radius = p.length
    return bounding_radius

#------------------------------------------------------------------------------
def ce_get_longest_extend(objects):
    min_edge = mathutils.Vector((0.0, 0.0, 0.0))
    max_edge = mathutils.Vector((0.0, 0.0, 0.0))
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
    for p in image.pixels:
        byte_array = array('B', [int(p * 255.0)])
        byte_array.tofile(file)
    file.close()

#------------------------------------------------------------------------------
def ce_write_material(file_path, materials):
    file = open(file_path, 'wt')
    for mat in materials:
        file.write("material %s\n" % (mat.name))
        file.write("{\n")
        file.write("    pass\n")
        file.write("    {\n")
        file.write("        lighting on\n")
        if mat.blend_method == 'BLEND':
            file.write("        scene_blend src_alpha one_minus_src_alpha\n")
        file.write("        diffuse %f %f %f %f\n" % (mat.diffuse_color[0], mat.diffuse_color[1], mat.diffuse_color[2], mat.diffuse_color[3]))
        file.write("        specular %f %f %f 1.0 %f\n" % (mat.specular_color[0], mat.specular_color[1], mat.specular_color[2], mat.specular_intensity * 64.0))
        file.write("        cull_hardware %s\n" % ('clockwise' if mat.use_backface_culling else 'none'))
        file.write("        roughness %f\n" % (mat.roughness))
        file.write("        metallic %f\n" % (mat.metallic))
        for tex in mat.node_tree.nodes:
            if tex.type == 'TEX_IMAGE':
                file.write("        texture_unit\n")
                file.write("        {\n")
                file.write("            texture %s\n" % (os.path.splitext(tex.image.name)[0] + '.tex'))
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
        file.write("    }\n")
        file.write("}\n\n")
    file.close()

#------------------------------------------------------------------------------
def ce_write_mesh(file_path, objects, scale_uniform):
    # calculate scale and bounding radius
    scale = (1.0 / ce_get_longest_extend(objects)) if scale_uniform else 1.0
    radius = ce_get_bounding_radius(objects) * scale

    # 1. write bounds
    file = open(file_path, 'wb')
    file.write(M_MESH_BOUNDS)
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
                uv_vtx_map = []
                for face in mesh.polygons:
                    if face.material_index == index:
                        faces_cur_mat.append(face)

                # 4. write vertex count
                byte_array = array('I', [len(faces_cur_mat) * 3])
                byte_array.tofile(file)

                # write vertices
                for face in faces_cur_mat:
                    for vert_idx, loop_idx in zip(face.vertices, face.loop_indices):
                        # 5. write texture coordinates
                        byte_array = array('f', mesh.uv_layers.active.data[loop_idx].uv)
                        byte_array.tofile(file)
                        # 6. write color (just white for now)
                        byte_array = array('I', [0xffffffff])
                        byte_array.tofile(file)
                        # 7. write normal
                        byte_array = array('f', mesh.vertices[vert_idx].normal)
                        byte_array.tofile(file)
                        # 8. write position
                        byte_array = array('f', mesh.vertices[vert_idx].co)
                        byte_array.tofile(file)
    file.close()

    return used_materials

#------------------------------------------------------------------------------
class Export_ChrisslyEngineMesh(bpy.types.Operator):
    bl_idname = "export_scene.chrisslyengine_mesh"
    bl_label = "Export ChrisslyEngine-Mesh"

    # ui elements/properties
    scale_uniform: bpy.props.BoolProperty(name="Scale to uniform size", description="Scale the exported mesh so it's longest extend has size of 1 unit", default=True)
    separate_objects: bpy.props.BoolProperty(name="Objects as separate files", description="Each object will be saved as separate file, material file is shared", default=False)

    filepath: bpy.props.StringProperty(subtype='FILE_PATH')

    # export the mesh
    def execute(self, context):
        used_materials = []
        if self.separate_objects:
            for i, ob in enumerate(bpy.context.scene.objects):
                obj_arr = [ob]
                used_materials += ce_write_mesh(os.path.splitext(self.filepath)[0] + str(i) + os.path.splitext(self.filepath)[1], obj_arr, self.scale_uniform)
        else:
            used_materials += ce_write_mesh(self.filepath, bpy.context.scene.objects, self.scale_uniform)
        # assemble filename for material file and remove duplicates from material list
        ce_write_material(os.path.splitext(self.filepath)[0] + ".material", list(dict.fromkeys(used_materials)))
        return {'FINISHED'}

    # open the file selector, pressing "Export ChrisslyEngine-Mesh" then calls execute
    def invoke(self, context, event):
        context.window_manager.fileselect_add(self)
        return {'RUNNING_MODAL'}

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