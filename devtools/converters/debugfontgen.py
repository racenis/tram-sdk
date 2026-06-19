from PIL import Image
import sys

def pixel_index(r, g, b, a):
	# transparent
	if a < 128:
		return 0
	if r > 200 and b < 100 and b > 200:
		return 0
	
	# white
	if r > 200 and g > 200 and b > 200:
		return 1

	# gray
	if r > 100 and g > 100 and b > 100:
		return 2

	# black
	return 3

if len(sys.argv) != 2:
	print("Need to specify argument that is the input image!")
	print("Like this: python3 fontgen.py my_fancy_image.png")
	sys.exit()
	
img = Image.open(sys.argv[1]).convert("RGBA")
img = img.transpose(Image.FLIP_TOP_BOTTOM)

pixels = list(img.getdata())
packed = []

for i in range(0, len(pixels), 4):
	pix1 = pixels[i + 0]
	pix2 = pixels[i + 1]
	pix3 = pixels[i + 2]
	pix4 = pixels[i + 3]
	
	index1 = pixel_index(pix1[0], pix1[1], pix1[2], pix1[3])
	index2 = pixel_index(pix2[0], pix2[1], pix2[2], pix2[3])
	index3 = pixel_index(pix3[0], pix3[1], pix3[2], pix3[3])
	index4 = pixel_index(pix4[0], pix4[1], pix4[2], pix4[3])

	packed.append((index1 << 6) | (index2 << 4) | (index3 << 2) | index4)
	
nums = 0
for num in packed:
	nums = nums + 1
	if nums == 64:
		nums = 0
		print(f"{num},")
	else:
		print(f"{num}, ", end='')
