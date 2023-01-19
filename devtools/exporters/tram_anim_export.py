import bpy

bl_info = {
    "name": "Export Tram Animation",
    "description": "uwu nyaa ;33",
    "author": "JDambergs",
    "blender": (2, 83, 0),
    "support": "COMMUNITY",
    "category": "Import-Export",
}

def write_tram_animation(context, filepath, use_some_setting):
    f = open(filepath, 'w', encoding='utf-8')
    
    f.write(bpy.path.display_name_from_filepath(bpy.context.blend_data.filepath) + " " + str(len(bpy.data.actions)) + "\n")
    for action in bpy.data.actions:
        f.write(action.name + " " + str(len(action.groups)) + "\n")
        for group in action.groups:
            frames = len(group.channels[0].keyframe_points)
            
            f.write(group.name + " " + str(frames) + "\n")
            i = 0
            while (i < frames):
                kframe = ""
                kframe += str(group.channels[0].keyframe_points[i].co[0]) + " "
                kframe += str(group.channels[0].keyframe_points[i].co[1]) + " "
                kframe += str(group.channels[2].keyframe_points[i].co[1]) + " "
                kframe += str(-group.channels[1].keyframe_points[i].co[1]) + " "
                #kframe += str(group.channels[3].keyframe_points[i].co[1]) + " "
                #kframe += str(group.channels[4].keyframe_points[i].co[1]) + " "
                #kframe += str(group.channels[5].keyframe_points[i].co[1]) + " "
                #kframe += str(group.channels[6].keyframe_points[i].co[1]) + " "
                kframe += str(group.channels[4].keyframe_points[i].co[1]) + " "
                kframe += str(group.channels[6].keyframe_points[i].co[1]) + " "
                kframe += str(-group.channels[5].keyframe_points[i].co[1]) + " "
                kframe += str(group.channels[3].keyframe_points[i].co[1]) + " "
                kframe += str(group.channels[7].keyframe_points[i].co[1]) + " "
                kframe += str(group.channels[9].keyframe_points[i].co[1]) + " "
                kframe += str(-group.channels[8].keyframe_points[i].co[1]) + "\n"
                f.write(kframe)
                i += 1
    
    
    f.close()

    return {'FINISHED'}


# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator


class ExportTramAnimation(Operator, ExportHelper):
    """This appears in the tooltip of the operator and in the generated docs"""
    bl_idname = "export_tram_animation.export_tram_anim"  # important since its how bpy.ops.import_test.some_data is constructed
    bl_label = "Export Tram Animation"

    # ExportHelper mixin class uses this
    filename_ext = ".anim"

    filter_glob: StringProperty(
        default="*.anim",
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
        return write_tram_animation(context, self.filepath, self.use_setting)


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportTramAnimation.bl_idname, text="Tram Animation Exporter")


def register():
    bpy.utils.register_class(ExportTramAnimation)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportTramAnimation)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()
