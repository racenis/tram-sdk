import bpy

bl_info = {
    "name": "Export Tram Static Model",
	"description": "uwu nyaa",
	"author": "JDambergs",
    "blender": (2, 83, 0),
	"support": "COMMUNITY",
    "category": "Import-Export",
}

def write_tram_static_model(context, filepath, use_some_setting):
    ob = bpy.context.active_object
    exp = {'uv':[],'lmap':[],'vert':[],'vnorm':[],'polys':[],'mat':[]}

    #get uv coords
    for uv in ob.data.uv_layers['UVMap'].data:
        exp['uv'].append([uv.uv[0], uv.uv[1]])
    #now do the same thing but for lightmap
    for uv in ob.data.uv_layers['LightMap'].data:
        exp['lmap'].append([uv.uv[0], uv.uv[1]])

    #get info of the loop vertices
    for mloop in ob.data.loops:
        vert = ob.data.vertices[mloop.vertex_index]
        exp['vert'].append([vert.co[0], vert.co[2], -vert.co[1]])
        exp['vnorm'].append([vert.normal[0], vert.normal[2], -vert.normal[1]])

    #get all polygons
    for poly in ob.data.polygons:
        for i in range(0, len(poly.loop_indices) - 2):
            exp['polys'].append([poly.loop_indices[0], poly.loop_indices[i+1], poly.loop_indices[i+2], poly.material_index])

    #remove duplicate material entries
    #exp['mat'] = list(dict.fromkeys(exp['mat']))


    #ok now to compile everything together and write to disk
    f = open(filepath, 'w', encoding='utf-8')

    info = ""
    info += str(len(exp['vert'])) + " "
    info += str(len(exp['polys'])) + " "
    info += str(len(ob.material_slots)) + "\n"
    f.write(info)


    for mat in ob.material_slots:
        f.write(mat.name  + "\n")
		
    vid = 0
    while vid < len(exp['vert']):
        line = ""
        line += str(exp['vert'][vid][0]) + " "
        line += str(exp['vert'][vid][1]) + " "
        line += str(exp['vert'][vid][2]) + " "
        line += str(exp['vnorm'][vid][0]) + " "
        line += str(exp['vnorm'][vid][1]) + " "
        line += str(exp['vnorm'][vid][2]) + " "
        line += str(exp['uv'][vid][0]) + " "
        line += str(exp['uv'][vid][1]) + " "
        line += str(exp['lmap'][vid][0]) + " "
        line += str(exp['lmap'][vid][1]) + "\n"
        f.write(line)
        vid += 1
        
    for poly in exp['polys']:
        line = ""
        line += str(poly[0]) + " "
        line += str(poly[1]) + " "
        line += str(poly[2]) + " "
        line += str(poly[3]) + "\n"
        f.write(line)

   
   
    f.close()

    return {'FINISHED'}


# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator


class ExportTramStaticObj(Operator, ExportHelper):
    """This appears in the tooltip of the operator and in the generated docs"""
    bl_idname = "export_tram_static_obj.export_static_tram"  # important since its how bpy.ops.import_test.some_data is constructed
    bl_label = "Export Tram Static Model"

    # ExportHelper mixin class uses this
    filename_ext = ".stmdl"

    filter_glob: StringProperty(
        default="*.stmdl",
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

    def execute(self, context):
        return write_tram_static_model(context, self.filepath, self.use_setting)


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportTramStaticObj.bl_idname, text="Tram Static Exporter")


def register():
    bpy.utils.register_class(ExportTramStaticObj)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportTramStaticObj)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()

    # test call
    #bpy.ops.export_tram_static_obj.export_static_tram('INVOKE_DEFAULT')
