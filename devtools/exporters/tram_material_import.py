import bpy
import os

bl_info = {
    "name": "Import Tram Material",
    "description": "Imports .mat files for Tramway SDK",
    "author": "racenis",
    "blender": (2, 83, 0),
    "support": "COMMUNITY",
    "category": "Import-Export",
}

def import_tram_material(context, path_prefix):
    material_data = {}

    texture_prefix = os.path.join(path_prefix, "data", "textures")

    # scan in .png files from data/textures
    for root, subdirs, files in os.walk(texture_prefix):
        for file in files:
            name, extension = os.path.splitext(os.path.join(root, file))
            
            if extension != '.png':
                continue
            
            if name.endswith('.nrm') or name.endswith('.normal')
                continue
            
            name = os.path.relpath(name, texture_prefix)
            
            
            new_material = {
                "name" : name,
                "path" : os.path.join(root, file),
                
                "type" : "flat",
                "filter" : "nearest",
                "texture_type" : "same"
            }
            
            material_data[name] = new_material
            
    # read in material definition, if there is one
    file_path = os.path.join(path_prefix, "data", "material.list")
    file = open(file_path, 'r')

    if file.closed:
        print("Can't open file", file_path)
    else:
        first_line = file.readline().strip()
        if first_line != "MATv6":
            print("File", file_path, "is not a MATv6!")
        else:
            for line in file:
                stripped_line = line.strip()

                if not stripped_line or stripped_line.startswith('#'):
                    continue

                tokens = stripped_line.split()
                
                material_name = tokens[0]
                
                if material_name in material_data:
                    data = material_data[material_name]
                else:
                    data = {
                        "name" : material_name,
                        "path" : ""
                    }
                    
                data["type"] = tokens[1]
                data["filter"] = tokens[2]
                data["property"] = tokens[3]
                data["color"] = [float(tokens[4]), float(tokens[5]), float(tokens[6])]
                data["specular_weight"] = tokens[7]
                data["specular_exponent"] = tokens[8]
                data["specular_transparency"] = tokens[9]
                data["texture_type"] = tokens[10]
                    
                material_data[material_name] = data
                

    def get_texture(material):
        if not material in material_data:
            return ""
        data = material_data[material]
        if data["texture_type"] == "same":
            return data["path"]
        if data["texture_type"] == "none":
            return ""
        return get_texture(data["texture_type"])

    # create and/or update blender materials    
    for key, data in material_data.items():
        
        # check if material exists
        material_index = bpy.data.materials.find(data["name"])
        
        if material_index == -1:
            # create a new material
            material = bpy.data.materials.new(data["name"])
        else:
            # take the existing one
            material = bpy.data.materials[material_index]
        
        material.use_nodes = True
        
        if "color" in data:
            color = data["color"]
        else:
            color = [1.0, 1.0, 1.0]
        
        bsdf = material.node_tree.nodes["Principled BSDF"]
        bsdf.inputs["Base Color"].default_value = (color[0], color[1], color[2], 1.0)

        texture = get_texture(key)
        
        if not texture == "":
            texImage = material.node_tree.nodes.new('ShaderNodeTexImage')
            texImage.image = bpy.data.images.load(texture)
            material.node_tree.links.new(bsdf.inputs['Base Color'], texImage.outputs['Color'])



from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator

class ImportTramMaterial(bpy.types.Operator):
    bl_idname = "import_tram_material.import_tram_mat"
    bl_label = "Select Project Directory"
    bl_options = {'REGISTER'}

    directory: StringProperty(
        name="Indir Path",
        description="Where I will load my stuff"
    )

    filter_folder: BoolProperty(
        default=True,
        options={"HIDDEN"}
    )

    def execute(self, context):
        import_tram_material(context, self.directory)
        return {'FINISHED'}

    def invoke(self, context, event):
        context.window_manager.fileselect_add(self)
        return {'RUNNING_MODAL'}


def menu_func_import(self, context):
    self.layout.operator(ImportTramMaterial.bl_idname, text="Tram Material Importer")


def register():
    bpy.utils.register_class(ImportTramMaterial)
    bpy.types.TOPBAR_MT_file_import.append(menu_func_import)


def unregister():
    bpy.utils.unregister_class(ImportTramMaterial)
    bpy.types.TOPBAR_MT_file_import.remove(menu_func_import)


if __name__ == "__main__":
    register()
