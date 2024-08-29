import csv
f = open("FontData.csv")
reader = csv.reader(f)

results = dict()

for line in reader:
	results[line[0]] = line[1]

f.close()

def find(xyz):
	return results[xyz]
			
f = open("Font.spr", "a")
f.write("SPRv2 ui/font_text\n\n")

for i in range(0, 256):
	x = (i % 16) * 16
	y = int(int(i / 16) * 16)
	
	w = find("Char "+str(i)+" Base Width")
	h = 16
	
	cx = 8
	cy = 8
	
	bh = 0
	bv = 0
	
	f.write(str(x) + "\t" + str(y) + "\t" + str(w) + "\t" + str(h) + "\t" + str(cx) + "\t" + str(cy) + "\t" + str(bh) + "\t" + str(bv) + "\n")

f.close()