import bpy

from tram_static_obj_export import write_tram_static_model
from tram_dynamic_obj_export import write_tram_dynamic_model
from tram_collision_export import write_tram_collision_model
from tram_anim_export import write_tram_animation

path_prefix = "./"

print("Autoexport script begin")

# first we find all of the top-level objects
root_objects = []

for object in bpy.context.scene.objects:
    if object.parent is None:
        root_objects.append(object)

# then we process them
for object in root_objects:
    if object.type == 'MESH':
        
        export_name = object.name
        
        # check if allowed to export
        if object.name.startswith('&'):
            continue
        
        # determine if should be exported as dymdl or stmdl
        export_type = 'STMDL'
        
        def determine_type(prefix, type):
            global export_type
            global export_name
            
            if object.name.startswith(prefix):
                export_type = type
                export_name = export_name.strip(prefix)
        
        determine_type('stmdl?', 'STMDL')
        determine_type('s?', 'STMDL')
        
        determine_type('dymdl?', 'DYDML')
        determine_type('d?', 'DYDML')
        
        # export the mesh
        if export_type == 'STMDL':
            file_path = path_prefix + "data/models/" + export_name + '.stmdl'
            print("Exporting", object.name, "as", file_path)
            
            write_tram_static_model(bpy.context, object, file_path)
        else:
            file_path = path_prefix + "data/models/" + export_name + '.dymdl'
            print("Exporting", object.name, "as", file_path)
            
            write_tram_dynamic_model(bpy.context, object, file_path)
            
            
        # check if could be treated as collmdl
        if len(object.children) > 0:
            file_path = path_prefix + "data/models/" + export_name + '.collmdl'
            print("Exporting", object.name, "as", file_path)
            
            write_tram_collision_model(bpy.context, object, file_path)
    
    
    if object.type == 'ARMATURE':
        for subobj in object.children:
            
            # check if allowed to export
            if subobj.name.startswith('&'):
                continue
            
            # export as dymdl since has an armature
            file_path = path_prefix + "data/models/" + subobj.name + '.dymdl'
            print("Exporting", subobj.name, "as", file_path)
            
            write_tram_dynamic_model(bpy.context, subobj, file_path)
            
            # check if could be treated as collmdl
            if len(object.children) > 0:
                file_path = path_prefix + subobj.name + '.collmdl'
                print("Exporting", subobj.name, "as", file_path)
                
                write_tram_collision_model(bpy.context, subobj, file_path)
            
            
# finally we export animations
write_tram_animation(bpy.context, path_prefix + "data/animations")

print("Autoexport script finished")