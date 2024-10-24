import bpy
import os

bl_info = {
    "name": "Export Tram Collision Model",
    "description": "uwu nyaa ;33",
    "author": "JDambergs",
    "blender": (2, 83, 0),
    "support": "COMMUNITY",
    "category": "Import-Export",
}

def object_to_collision_mesh(c):
    out = ""
    
    exported_poly_count = 0
            
    poly_str = ""
    
    for poly in c.data.polygons:
        for i in range(0, len(poly.loop_indices) - 2):
            exported_poly_count += 1
            poly_str += str(c.data.vertices[poly.vertices[0]].co[0]) + " "
            poly_str += str(c.data.vertices[poly.vertices[0]].co[2]) + " "
            poly_str += str(-c.data.vertices[poly.vertices[0]].co[1]) + " "
            poly_str += str(c.data.vertices[poly.vertices[i+1]].co[0]) + " "
            poly_str += str(c.data.vertices[poly.vertices[i+1]].co[2]) + " "
            poly_str += str(-c.data.vertices[poly.vertices[i+1]].co[1]) + " "
            poly_str += str(c.data.vertices[poly.vertices[i+2]].co[0]) + " "
            poly_str += str(c.data.vertices[poly.vertices[i+2]].co[2]) + " "
            poly_str += str(-c.data.vertices[poly.vertices[i+2]].co[1]) + "\n"
    
    out += "mesh " + str(exported_poly_count) + "\n" + poly_str
    return out


def write_tram_collision_model(context, filepath, use_some_setting):
    ob = bpy.context.active_object

    out = ""

    for c in ob.children:
        if ("Cube") in c.name:
            out += "box " + str(c.location[0]) + " " + str(c.location[2]) + " " + str(-c.location[1]) + " "
            out += str(c.rotation_euler[0]) + " " + str(c.rotation_euler[2]) + " " + str(-c.rotation_euler[1]) + " "
            out += str(c.scale[0]) + " " + str(c.scale[2]) + " " + str(c.scale[1]) + "\n"
        if ("Cylinder") in c.name:
            out += "cylinder " + str(c.location[0]) + " " + str(c.location[2]) + " " + str(-c.location[1]) + " "
            out += str(c.rotation_euler[0]) + " " + str(c.rotation_euler[2]) + " " + str(-c.rotation_euler[1]) + " "
            out += str(c.scale[0]) + " " + str(c.scale[2]) + " " + str(c.scale[1]) + "\n"
        if ("Capsule") in c.name:
            out += "capsule " + str(c.location[0]) + " " + str(c.location[2]) + " " + str(-c.location[1]) + " "
            out += str(c.rotation_euler[0]) + " " + str(c.rotation_euler[2]) + " " + str(-c.rotation_euler[1]) + " "
            out += str((c.scale[0] + c.scale[2]) / 2) + " " + str(c.scale[1]) + "\n"
        if ("Cone") in c.name:
            out += "cone " + str(c.location[0]) + " " + str(c.location[2]) + " " + str(-c.location[1]) + " "
            out += str(c.rotation_euler[0]) + " " + str(c.rotation_euler[2]) + " " + str(-c.rotation_euler[1]) + " "
            out += str((c.scale[0] + c.scale[2]) / 2) + " " + str(c.scale[1]) + "\n"
        if ("Sphere") in c.name:
            out += "sphere " + str(c.location[0]) + " " + str(c.location[2]) + " " + str(-c.location[1]) + " "
            out += str(c.rotation_euler[0]) + " " + str(c.rotation_euler[2]) + " " + str(-c.rotation_euler[1]) + " "
            out += str((c.scale[0] + c.scale[1] + c.scale[2]) / 3) + "\n"
        if ("Cloud") in c.name:
            out += "cloud " + str(len(c.data.vertices)) + "\n"
            
            for v in c.data.vertices:
                out += str(v.co[0]) + " " + str(v.co[2]) + " " + str(-v.co[1]) + "\n"
        if ("Mesh") in c.name:            
            out += object_to_collision_mesh(c)
                
    if len(ob.children) == 0:
        out = object_to_collision_mesh(ob)
            
   


    f = open(filepath, 'w', encoding='utf-8')

    f.write(out)
   
    f.close()

    return {'FINISHED'}


# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator


class ExportTramCollisionObj(Operator, ExportHelper):
    """This appears in the tooltip of the operator and in the generated docs"""
    bl_idname = "export_tram_collision_obj.export_dynamic_tram"  # important since its how bpy.ops.import_test.some_data is constructed
    bl_label = "Export Tram Collision Model"

    # ExportHelper mixin class uses this
    filename_ext = ".collmdl"

    filter_glob: StringProperty(
        default="*.collmdl",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )

    # List of operator properties, the attributes will be assigned
    # to the class instance from the operator settings before calling.
    use_setting: BoolProperty(
        name="Example Boolean",
        description="Example Tooltip",
        default=True,
    )

    type: EnumProperty(
        name="Example Enum",
        description="Choose between two items",
        items=(
            ('OPT_A', "First Option", "Description one"),
            ('OPT_B', "Second Option", "Description two"),
        ),
        default='OPT_A',
    )

    def invoke(self, context, _event):
        if not self.filepath or True:
            blend_filepath = context.blend_data.filepath
            if not blend_filepath:
                blend_filepath = bpy.context.object.name
            else:
                path_dir = os.path.dirname(blend_filepath)
                blend_filepath = os.path.join(path_dir, bpy.context.object.name)

            self.filepath = blend_filepath + self.filename_ext

        context.window_manager.fileselect_add(self)
        return {'RUNNING_MODAL'}

    def execute(self, context):
        return write_tram_collision_model(context, self.filepath, self.use_setting)


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportTramCollisionObj.bl_idname, text="Tram Collision Exporter")


def register():
    bpy.utils.register_class(ExportTramCollisionObj)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportTramCollisionObj)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()

    # test call
    #bpy.ops.export_tram_static_obj.export_static_tram('INVOKE_DEFAULT')
