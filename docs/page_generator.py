import re

pages = ["index",
		 "features",
		 "roadmap",
		 "learn",
		 
		 "patterns",
		 "why",
		 
		 "learn/whatis",
		 "learn/getstarted",
		 "learn/structure",
		 "learn/scripting",
		 "learn/3dmodeling",
		 "learn/materials",
		 "learn/animations",
		 "learn/pingpong",
		 "learn/physics",
		 "learn/render",
		 "learn/world",
		 "learn/walking",
		 "learn/cameras",
		 "learn/platforms",
		 "learn/cake",
		 
		 "documentation",
		 "documentation/index",
		 "documentation/audio",
		 "documentation/components",
		 "documentation/components/animation",
		 "documentation/components/audio",
		 "documentation/components/controller",
		 "documentation/components/light",
		 "documentation/components/particle",
		 "documentation/components/physics",
		 "documentation/components/player",
		 "documentation/components/render",
		 "documentation/components/sprite",
		 "documentation/components/template",
		 "documentation/components/trigger",
		 
		 "documentation/entities",
		 "documentation/entities/crate",
		 "documentation/entities/decoration",
		 "documentation/entities/light",
		 "documentation/entities/marker",
		 "documentation/entities/player",
		 "documentation/entities/sound",
		 "documentation/entities/staticwobj",
		 "documentation/entities/trigger",
		 
		 "documentation/extensions",
		 "documentation/extensions/camera",
		 "documentation/extensions/menu",
		 "documentation/extensions/kitchensink",
		 "documentation/extensions/kitchensink/animationtable",
		 "documentation/extensions/kitchensink/soundtable",
		 "documentation/extensions/scripting",
		 
		 "documentation/framework",
		 "documentation/framework/async",
		 "documentation/framework/core",
		 "documentation/framework/entity",
		 "documentation/framework/entitycomponent",
		 "documentation/framework/event",
		 "documentation/framework/file",
		 "documentation/framework/graph",
		 "documentation/framework/gui",
		 "documentation/framework/language",
		 "documentation/framework/loader",
		 "documentation/framework/logging",
		 "documentation/framework/math",
		 "documentation/framework/message",
		 "documentation/framework/navmesh",
		 "documentation/framework/path",
		 "documentation/framework/resource",
		 "documentation/framework/script",
		 "documentation/framework/serialization",
		 "documentation/framework/settings",
		 "documentation/framework/signal",
		 "documentation/framework/stats",
		 "documentation/framework/system",
		 "documentation/framework/transition",
		 "documentation/framework/type",
		 "documentation/framework/ui",
		 "documentation/framework/uid",
		 "documentation/framework/worldcell",
		 "documentation/framework/value",
		 
		 "documentation/physics",
		 "documentation/physics/collisionmodel",
		 "documentation/physics/collisionshape",
		 
		 "documentation/platform",
		 
		 "documentation/render",
		 "documentation/render/aabb",
		 "documentation/render/animation",
		 "documentation/render/material",
		 "documentation/render/model",
		 "documentation/render/sprite",
		 
		 "documentation/tools/codegen",
		 
		 "documentation/plugin",
		 "documentation/leveleditor",
		 "documentation/entitydefinition",
		 ]
template = "template"

template_text = open("src/" + template + ".html", "r").read()

for page in pages:
	page_text = open("src/" + page + ".html", "r").read()
	
	page_level = page.count('/')
	page_level = "../" * page_level
	
	page_title = re.search("<!-- PAGE_TITLE: (.+?) -->", page_text)
	if page_title:
		page_title = page_title.group(1)
	else:
		page_title = "UNTITLED"
	
	page_templated = template_text
	page_templated = page_templated.replace("<!-- PAGE_TITLE -->", page_title).replace("<!-- PAGE_CONTENT -->", page_text)
	
	page_templated = page_templated.replace("<!-- PAGE_LEVEL -->", page_level)
	
	for link in re.findall("<!-- MENU_LINK:(.+?):(.+?) -->", page_templated):
		replace_match = "<!-- MENU_LINK:" + link[0] + ":" + link[1] + " -->"
		replace_string = "<a href=\"" + page_level + link[0] + ".html\" class=\"menu-link" + (" menu-open-page" if page == link[0] else "") + "\">" + link[1] + "</a><br/>"
		page_templated = page_templated.replace(replace_match, replace_string)
	
	file = open(page + ".html", "w")
	file.write(page_templated)
	file.close()
	
	print(page + " done")