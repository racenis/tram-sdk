f = open("poland.spr")
lines = f.readlines()
f.close()

f = open("poland.spr2", "a")
f.write(lines[0])
f.write("\n\n")

for line_s in lines[2:]:
	line_s = line_s.split()

	line = ""
	
	line += line_s[0] + "\t"
	line += line_s[1] + "\t"
	line += line_s[2] + "\t"
	line += line_s[3] + "\t"
	line += str(int((int(line_s[2]) / 2))) + "\t"
	line += str(int((int(line_s[3]) / 2))) + "\t"
	line += "0" + "\t"
	line += str(int(int(line_s[4])/2)) + "\n"
	
	f.write(line)
f.close()
	
	
	
"""
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
"""