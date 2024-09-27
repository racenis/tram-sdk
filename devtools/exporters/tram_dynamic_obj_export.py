import bpy

bl_info = {
    "name": "Export Tram Dynamic Model",
    "description": "uwu nyaa ;33",
    "author": "JDambergs",
    "blender": (2, 83, 0),
    "support": "COMMUNITY",
    "category": "Import-Export",
}

def write_tram_dynamic_model(context, filepath, use_some_setting):
    ob = bpy.context.active_object
    exp = {'uv':[],'vert':[],'vnorm':[],'polys':[],'vweight':[],'bones':[]}

    #get uv coords
    for uv in ob.data.uv_layers['UVMap'].data:
        exp['uv'].append([uv.uv[0], uv.uv[1]])
        
    #get info of the loop vertices
    for mloop in ob.data.loops:
        vert = ob.data.vertices[mloop.vertex_index]
        exp['vert'].append([vert.co[0], vert.co[2], -vert.co[1]])
        exp['vnorm'].append([vert.normal[0], vert.normal[2], -vert.normal[1]])
        
        weights = []
        for w in vert.groups:
            weights.append([w.weight, w.group])
        
        # if vertex has no weights, then just bind it to root
        if len(weights) < 1:
            weights.append([1.0, 0])
            
        # if vertex has less than the minimum of 4 weights, then just
        # add some blank weights to the root
        while len(weights) < 4:
            weights.append([0.0, 0])
        
        # if vertex has more than 4 weights, then find the weights
        # that are the most significant and discard the rest
        weights.sort(reverse=True)
        weights = weights[:4]
        
        # TODO: normalize weights? maybe?
        
        exp['vweight'].append(weights)

    # get all polygons
    for poly in ob.data.polygons:
        for i in range(0, len(poly.loop_indices) - 2):
            exp['polys'].append([poly.loop_indices[0], poly.loop_indices[i+1], poly.loop_indices[i+2], poly.material_index])

    # collect bones
    if ob.parent is None:
        
        # if the object has no armature attached, then we will just
        # assume that it has a single bone
        boning = []
        boning.append("Root")
        boning.append(0.0)
        boning.append(0.0)
        boning.append(0.0)
        boning.append(0.0)
        boning.append(1.0)
        boning.append(0.0)
        boning.append(0.0)
        boning.append(-1)
        
        exp['bones'].append(boning)
        
    else:
        
        # otherwise, just add the armature bones
        for bone in ob.parent.data.bones:
            boning = []
            boning.append(bone.name)
            boning.append(bone.head_local[0])
            boning.append(bone.head_local[2])
            boning.append(-bone.head_local[1])
            boning.append(bone.tail_local[0])
            boning.append(bone.tail_local[2])
            boning.append(-bone.tail_local[1])
            boning.append(-bone.matrix.to_euler().z)
            
            if len(bone.parent_recursive) > 0:
                boning.append(ob.parent.data.bones.find(bone.parent_recursive[0].name))
            else:
                boning.append(-1)

            exp['bones'].append(boning)





    
    #ok now to compile everything together
    f = open(filepath, 'w', encoding='utf-8')


    info = "DYMDLv1 "
    info += str(len(exp['vert'])) + " "
    info += str(len(exp['polys'])) + " "
    info += str(len(ob.material_slots)) + " "
    info += str(len(exp['bones'])) + " "
    info += str(len(ob.vertex_groups)) + "\n"
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
        line += str(exp['vweight'][vid][0][1]) + " "
        line += str(exp['vweight'][vid][0][0]) + " "
        line += str(exp['vweight'][vid][1][1]) + " "
        line += str(exp['vweight'][vid][1][0]) + " "
        line += str(exp['vweight'][vid][2][1]) + " "
        line += str(exp['vweight'][vid][2][0]) + " "
        line += str(exp['vweight'][vid][3][1]) + " "
        line += str(exp['vweight'][vid][3][0]) + "\n"
        f.write(line)
        vid += 1
        
    for poly in exp['polys']:
        line = ""
        line += str(poly[0]) + " "
        line += str(poly[1]) + " "
        line += str(poly[2]) + " "
        line += str(poly[3]) + "\n"
        f.write(line)
        
    for bone in exp['bones']:
        line = ""
        line += str(bone[0]) + " "
        line += str(bone[8]) + " "
        line += str(bone[1]) + " "
        line += str(bone[2]) + " "
        line += str(bone[3]) + " "
        line += str(bone[4]) + " "
        line += str(bone[5]) + " "
        line += str(bone[6]) + " "
        line += str(bone[7]) + "\n"
        f.write(line)
        
    for group in ob.vertex_groups:
        f.write(group.name + "\n")
   
    f.close()

    return {'FINISHED'}


# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator


class ExportTramDynamicObj(Operator, ExportHelper):
    """This appears in the tooltip of the operator and in the generated docs"""
    bl_idname = "export_tram_dynamic_obj.export_dynamic_tram"  # important since its how bpy.ops.import_test.some_data is constructed
    bl_label = "Export Tram Dynamic Model"

    # ExportHelper mixin class uses this
    filename_ext = ".dymdl"

    filter_glob: StringProperty(
        default="*.dymdl",
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
        return write_tram_dynamic_model(context, self.filepath, self.use_setting)


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportTramDynamicObj.bl_idname, text="Tram Dynamic Exporter")


def register():
    bpy.utils.register_class(ExportTramDynamicObj)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportTramDynamicObj)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()

    # test call
    #bpy.ops.export_tram_static_obj.export_static_tram('INVOKE_DEFAULT')
