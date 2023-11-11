import cv2
import numpy as np
import sys
from scipy.fftpack import fft, dct, idct, ifft
from math import log10, sqrt, exp

alpha = 0.1
block_size = 8
orig_file = 'data/kodim04.bmp'
payload_file = 'data/payload.txt'
# result_img_file = 'data/result.bmp'
result_img_file = 'error/1.bmp'
unpacked_file = 'data/unpacked_result.txt'

def PSNR(original, compressed):
	mse = np.mean((original - compressed) ** 2)
	if(mse == 0):  # MSE is zero means no noise is present in the signal .
		return 100
	max_pixel = 255.0
	psnr = 20 * log10(max_pixel / sqrt(mse))
	return psnr

def splitColorBlock(cblock):
	block_r = np.zeros((cblock.shape[0], cblock.shape[1]))
	block_g = np.zeros((cblock.shape[0], cblock.shape[1]))
	block_b = np.zeros((cblock.shape[0], cblock.shape[1]))
	for i in range(cblock.shape[0]):
		for j in range(cblock.shape[1]):
			block_r[i][j] = (cblock[i][j][0])
			block_g[i][j] = (cblock[i][j][1])
			block_b[i][j] = (cblock[i][j][2])
	return block_r, block_g, block_b


def splitOnBlocks(image):
	blocks = []
	for r in range(0,image.shape[0], block_size):
		for c in range(0,image.shape[1], block_size):
			for block in splitColorBlock(image[r:r+block_size,c:c+block_size]):
				blocks.append(block)
			# blocks.append(image[r:r+block_size,c:c+block_size])
	return blocks


def blocksToImage(blocks, image):
	b_ind = 0;
	for x in range(0, image.shape[0], block_size):
		for y in range(0, image.shape[1], block_size):
			blockToImage(x, y, blocks[b_ind], image,0)
			b_ind += 1
			blockToImage(x, y, blocks[b_ind], image,1)
			b_ind += 1
			blockToImage(x, y, blocks[b_ind], image,2)
			b_ind += 1

def blockToImage(x, y, block, image, component):
	for i in range(block_size):
		for j in range(block_size):
			image[x+i][y+j][component] = block[i][j]


def findMax(block):
	maximum = -999999.0
	index1 = 0
	index2 = 0
	for i in range(block.shape[0]):
		for j in range(block.shape[1]):
			if maximum < block[i][j]:
				maximum = block[i][j]
				index1 = i
				index2 = j
	return index1, index2


def payload_into_bs(fn : str) -> list:
    result = list()

    with open(fn, "rb") as f:
        data = f.read()

    size = len(data) * 8
    for i in range(31, -1, -1):
        result.append(((size & (1 << i)) != 0))

    print("packeted size: " + str(size))

    for b in data:
        for i in range(7, -1, -1):
            result.append(((b & (1 << i)) != 0))

    return result



def from_bs(bs : list) -> list:
    tmp = 0
    result = list()
    for i in range(len(bs)):
        if (i % 8) == 0:
            result.append(tmp)
            tmp = 0

        if bs[i]:
            tmp += 1 << (7 - (i % 8))
    return result


def pack(payload_str, image):
	blocks = splitOnBlocks(image)
	print("max packed size: " + str(len(blocks)))
	bs_payload = payload_into_bs(payload_str)
	for i in range(len(bs_payload)):
		dct_block = dct(blocks[i], norm = 'ortho')
		x, y = findMax(dct_block)
		if(bs_payload[i]):
			dct_block[x][y] *= exp(alpha)
		else:
			dct_block[x][y] *= exp(-alpha)
		blocks[i] = idct(dct_block, norm = 'ortho')
	blocksToImage(blocks, image)
	return image

def getRes(block, original_block):
	dct_block = dct(block, norm = 'ortho')
	dct_orginal_block = dct(original_block, norm = 'ortho')
	x, y = findMax(dct_orginal_block)
	return dct_block[x][y] >= dct_orginal_block[x][y]

def getSize(result):
	size = 0
	for i in range(len(result)):
		if result[i]:
			size += (1 << (31 - i))
	return size

def unpack(image, original_image):
	blocks = splitOnBlocks(image)
	original_blocks = splitOnBlocks(original_image)
	result = list()
	for i in range(32):
		result.append(getRes(blocks[i], original_blocks[i]))

	size = getSize(result)

	result = list()
	print("unpackeded size: " + str(size))
	for i in range(32, size+33):
		result.append(getRes(blocks[i], original_blocks[i]))		

	return from_bs(result)

def compare(orig, unpacked):
	full = max(len(orig), len(unpacked))
	minimum = min(len(orig), len(unpacked))
	error = full - minimum
	for i in range(minimum):
		if orig[i] != unpacked[i]:
			error += 1
	return error/full



if __name__ == "__main__":
	if sys.argv[1] == "pack":
		img = cv2.imread(orig_file)
		print(img.shape)
		res_img = pack(payload_file, img)
		cv2.imwrite(result_img_file, res_img)
	elif sys.argv[1] == "unpack":
		orig_img = cv2.imread(orig_file)
		print(orig_img.shape)
		packed_image = cv2.imread(result_img_file)
		payload = unpack(packed_image, orig_img)

		with open(unpacked_file, "wb") as f:
			f.write(bytes(bytearray(payload[1:])))

		with open(unpacked_file, "r") as f:
			print("\n" + f.read())
	elif sys.argv[1] == "psnr":
		img1 = cv2.imread(sys.argv[2])
		img2 = cv2.imread(sys.argv[3])
		print(PSNR(img1, img2))
	elif sys.argv[1] == "error":
		
		with open(payload_file, "rb") as f:
			orig = f.read()
		with open(unpacked_file, "rb") as f:
			unpacked = f.read()
		print(compare(orig, unpacked))