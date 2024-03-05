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
    for action in bpy.data.actions:
        
        full_path = filepath + action.name + ".anim"
        
        print("Saving animation to: ", full_path)
        f = open(full_path, 'w', encoding='utf-8')
        
        f.write("ANIMv1\n")
        
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
                kframe += str(group.channels[8].keyframe_points[i].co[1]) + "\n"
                f.write(kframe)
                i += 1
        f.close()

    return {'FINISHED'}



from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator

class ExportTramAnimation(bpy.types.Operator):
    bl_idname = "export_tram_animation.export_tram_anim"
    bl_label = "Export Tram Animation"
    bl_options = {'REGISTER'}

    directory: StringProperty(
        name="Outdir Path",
        description="Where I will save my stuff"
        )

    filter_folder: BoolProperty(
        default=True,
        options={"HIDDEN"}
        )

    def execute(self, context):
        write_tram_animation(context, self.directory, False)
        return {'FINISHED'}

    def invoke(self, context, event):
        context.window_manager.fileselect_add(self)
        return {'RUNNING_MODAL'}


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
