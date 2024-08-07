import re

pages = ["index",
		 "features",
		 "roadmap",
		 "learn",
		 "documentation",
		 "documentation/index"
		 
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
		replace_string = "<a href=\"" + link[0] + ".html\" class=\"menu-link" + (" menu-open-page" if page == link[0] else "") + "\">" + link[1] + "</a><br/>"
		page_templated = page_templated.replace(replace_match, replace_string)
	
	file = open(page + ".html", "w")
	file.write(page_templated)
	file.close()
	
	print(page + " done")