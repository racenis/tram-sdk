f = open("glyph_text_bold.info")
lines = f.readlines()
f.close()

allchars = dict.fromkeys(range(255))

for line in lines:
	line = line.split()
	
	if len(line) > 0:
		allchars[int(line[0])] = [line[1], line[2], line[3], line[4], line[5], "0", "1.0", "1.0"]
	
for char in allchars:
	if not allchars[char]:
		allchars[char] = ["0", "0", "0", "0", "0", "0", "1.0", "1.0"]
		
f = open("glyph.outputs", "a")
for char in allchars:
	line = ""
	for metric in allchars[char]:
		line += metric + "\t"
	line += "\n"
	f.write(line)
f.close()