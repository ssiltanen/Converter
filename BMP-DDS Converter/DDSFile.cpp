#include "DDSFile.h"
#include "MyException.h"

#include <fstream>
#include <iostream>
#include <algorithm>

DDSFile::DDSFile()
{
	m_pDdsHeader = nullptr;
	m_mainData = nullptr;
}

DDSFile::~DDSFile()
{
	delete m_pDdsHeader;
	delete[] m_mainData;
}

void DDSFile::VInitializeFromFile(const std::string & location)
{
	uint8_t* dataBuffer = nullptr;

	m_mainData = nullptr;
	m_pDdsHeader = nullptr;

	std::ifstream file(location, std::ios::binary);
	if (!file)
		throw MyException("Failure to open dds file from location: " + location);

	char dwMagic[4];
	file.read(dwMagic, 4);

	//Check if file is DDS
	if (strncmp(dwMagic, "DDS ", 4) != 0)
		throw MyException("File " + location + " isn't a dds file");

	// Allocate byte memory that will hold the header
	dataBuffer = new uint8_t[sizeof(DDS_HEADER)];
	file.read((char*)dataBuffer, sizeof(DDS_HEADER));

	// Construct the values from the buffers
	m_pDdsHeader = (DDS_HEADER*)dataBuffer;

	//Check valid structure sizes
	if (m_pDdsHeader->dwSize != 124 || m_pDdsHeader->ddspf.dwsize != 32)
		throw MyException("File " + location + " has invalid header(s)");

	//Check is width and height divisible by 4
	if (m_pDdsHeader->dwHeight % 4 != 0 || m_pDdsHeader->dwWidth % 4 != 0)
		throw MyException("File " + location + " does not have width or height divisible by 4");

	//Check if file contains compressed RGB data
	if (m_pDdsHeader->ddspf.dwflags != DDPF_FOURCC)
		throw MyException("File " + location + " texture does not contain compressed RGB data");

	//Check if file is DXT1 compressed
	if (m_pDdsHeader->ddspf.dwfourCC != DXT1_FOURCC)
		throw MyException("File " + location + " is not DXT1 compressed DDS file");

	//Calculate main image size, (blocksize = 8 and packing = 4 for DXT1)
	unsigned int mainImageSize = std::max((unsigned int)1, (m_pDdsHeader->dwWidth + 3) / 4) * std::max((unsigned int)1, (m_pDdsHeader->dwHeight + 3) / 4) * 8;
	if (mainImageSize != m_pDdsHeader->dwPitchOrLinearSize)
		std::cerr << "Warning! Calculated main image size differ from header value" << std::endl;

	//Read the main data
	m_mainData = new uint8_t[mainImageSize];
	file.read((char*)m_mainData, mainImageSize);
}

void DDSFile::VConversionInitialize(uint8_t * uncompressedImageData, unsigned int imageSize, unsigned int width, unsigned int height)
{
	m_mainData = DXT1Compress(uncompressedImageData);
	delete uncompressedImageData;

	//Create header
	m_pDdsHeader = new DDS_HEADER();
	m_pDdsHeader->dwSize = HEADER_SIZE;
	m_pDdsHeader->dwFlags = FLAGS;
	m_pDdsHeader->dwHeight = height;
	m_pDdsHeader->dwWidth = width;
	m_pDdsHeader->dwPitchOrLinearSize = std::max((unsigned int)1, (m_pDdsHeader->dwWidth + 3) / 4) * std::max((unsigned int)1, (m_pDdsHeader->dwHeight + 3) / 4) * 8;
	m_pDdsHeader->dwDepth = 0;
	m_pDdsHeader->dwMipMapCount = 0;
	m_pDdsHeader->dwReserved1[11];

	//Create pixel format
	DDS_PIXELFORMAT pixelFormat;
	pixelFormat.dwsize = INFO_SIZE;
	pixelFormat.dwflags = DDPF_FOURCC;
	pixelFormat.dwfourCC = DXT1_FOURCC;
	pixelFormat.dwRGBBitCount = 0;
	pixelFormat.dwRBitMask = 0;
	pixelFormat.dwGBitMask = 0;
	pixelFormat.dwBBitMask = 0;
	pixelFormat.dwABitMask = 0;

	m_pDdsHeader->ddspf = pixelFormat;
	m_pDdsHeader->dwCaps = DDSCAPS_TEXTURE;
	m_pDdsHeader->dwCaps2 = 0;
	m_pDdsHeader->dwCaps3 = 0;
	m_pDdsHeader->dwCaps4 = 0;
	m_pDdsHeader->dwReserved2 = 0;
}

void DDSFile::VCreateFile(std::basic_ofstream<uint8_t>& outputFile) const
{
	outputFile.write((uint8_t*)"DDS ", 4);
	outputFile.write((uint8_t*)&m_pDdsHeader->dwSize, 4);
	outputFile.write((uint8_t*)&m_pDdsHeader->dwFlags, 4);
	outputFile.write((uint8_t*)&m_pDdsHeader->dwHeight, 4);
	outputFile.write((uint8_t*)&m_pDdsHeader->dwWidth, 4);
	outputFile.write((uint8_t*)&m_pDdsHeader->dwPitchOrLinearSize, 4);
	outputFile.write((uint8_t*)&m_pDdsHeader->dwDepth, 4);
	outputFile.write((uint8_t*)&m_pDdsHeader->dwMipMapCount, 4);
	outputFile.write((uint8_t*)&m_pDdsHeader->dwReserved1, 44);

	outputFile.write((uint8_t*)&m_pDdsHeader->ddspf.dwsize, 4);
	outputFile.write((uint8_t*)&m_pDdsHeader->ddspf.dwflags, 4);
	outputFile.write((uint8_t*)&m_pDdsHeader->ddspf.dwfourCC, 4);
	outputFile.write((uint8_t*)&m_pDdsHeader->ddspf.dwRGBBitCount, 4);
	outputFile.write((uint8_t*)&m_pDdsHeader->ddspf.dwRBitMask, 4);
	outputFile.write((uint8_t*)&m_pDdsHeader->ddspf.dwGBitMask, 4);
	outputFile.write((uint8_t*)&m_pDdsHeader->ddspf.dwBBitMask, 4);
	outputFile.write((uint8_t*)&m_pDdsHeader->ddspf.dwABitMask, 4);

	outputFile.write((uint8_t*)&m_pDdsHeader->dwCaps, 4);
	outputFile.write((uint8_t*)&m_pDdsHeader->dwCaps2, 4);
	outputFile.write((uint8_t*)&m_pDdsHeader->dwDepth, 4);
	outputFile.write((uint8_t*)&m_pDdsHeader->dwCaps3, 4);
	outputFile.write((uint8_t*)&m_pDdsHeader->dwReserved2, 4);

	//write pixels
	unsigned int arraySize = m_pDdsHeader->dwPitchOrLinearSize;
	for (unsigned int i = 0; i < arraySize; ++i) {
		outputFile.write((uint8_t*)&m_mainData[i], 1);
	}
	outputFile.close();
}

unsigned int DDSFile::VGetWidth() const
{
	if (m_pDdsHeader == nullptr)
		return 0;
	return m_pDdsHeader->dwWidth;
}

unsigned int DDSFile::VGetHeight() const
{
	if (m_pDdsHeader == nullptr)
		return 0;
	return m_pDdsHeader->dwHeight;
}

unsigned int DDSFile::VGetImageByteSize() const
{
	if (m_pDdsHeader == nullptr)
		return 0;
	return m_pDdsHeader->dwWidth * m_pDdsHeader->dwHeight * 3;
}

uint8_t * DDSFile::VGetUncompressedImageData() const
{
	if (m_mainData == nullptr)
		return nullptr;

	
	const unsigned int width = m_pDdsHeader->dwWidth;
	const unsigned int height = m_pDdsHeader->dwHeight;
	uint8_t* bytesBuffer = new uint8_t[width * height * 3];


	int i = 0;
	for (unsigned int y = 0; y < height; y += 4) {
		for (unsigned int x = 0; x < width; x += 4) {
			//Get bytes of 2 reference colors and 4x4 blocks of texels
			uint8_t c0_lo = m_mainData[i++];
			uint8_t c0_hi = m_mainData[i++];
			uint8_t c1_lo = m_mainData[i++];
			uint8_t c1_hi = m_mainData[i++];
			uint8_t bits_0 = m_mainData[i++];
			uint8_t bits_1 = m_mainData[i++];
			uint8_t bits_2 = m_mainData[i++];
			uint8_t bits_3 = m_mainData[i++];

			//Calculate pixels into one 32 bit variable
			uint32_t bits = bits_0 + 256 * (bits_1 + 256 * (bits_2 + 256 * bits_3));

			//Calculate the four colors used
			uint16_t color0 = (c0_lo + c0_hi * 256);
			uint16_t color1 = (c1_lo + c1_hi * 256);
			uint16_t color2 = color0;
			uint16_t color3 = color0;

			//Define the order of colors
			if (color0 > color1) {
				color2 = ((2 * color0 + color1) / 3);
				color3 = ((color0 + 2 * color1) / 3);
			}
			else {
				color2 = ((color0 + color1) / 2);
				color3 = 0; //transparent black  
			}

			//Cut texel bits to bytes
			uint8_t* texesBytes = toBytes(bits);

			//Decode texel bits (2bits per pixel) and then get the decoded rgb value to that pixel
			for (int yi = 0; yi < 4; ++yi) {
				for (int xi = 0, maskCounter = 3; xi < 4; ++xi, maskCounter *= 4) {
					uint8_t* block = nullptr;
					uint8_t code = (texesBytes[yi] & maskCounter) >> 2 * xi;
					switch (code) {
					case 0:
						block = decodeRGB(color0);
						break;
					case 1:
						block = decodeRGB(color1);
						break;
					case 2:
						block = decodeRGB(color2);
						break;
					case 3:
						block = decodeRGB(color3);
						break;
					default:
						std::cerr << "Unknown value for texel color code" << std::endl;
					}
					//Calculate the color byte positions in rgb buffer
					unsigned int redOrderInBuffer = ((y + yi) * width + x * 4 + xi) * 3;
					unsigned int greenOrderInBuffer = 1 + ((y + yi) * width + x * 4 + xi) * 3;
					unsigned int blueOrderInBuffer = 2 + ((y + yi) * width + x * 4 + xi) * 3;
					bytesBuffer[redOrderInBuffer] = block[0];
					bytesBuffer[greenOrderInBuffer] = block[1];
					bytesBuffer[blueOrderInBuffer] = block[2];
					delete[] block;
				}
			}
			delete[] texesBytes;
		}
	}
	return bytesBuffer;
}

uint8_t * DDSFile::DXT1Compress(const uint8_t* const uncompressedData) const
{


	return nullptr;
}

uint8_t* DDSFile::toBytes(uint32_t bits) const
{
	uint8_t* byteArray = new uint8_t[4];

	byteArray[0] = (bits >> 24) & 0xFF;
	byteArray[1] = (bits >> 16) & 0xFF;
	byteArray[2] = (bits >> 8) & 0xFF;
	byteArray[3] = bits & 0xFF;

	return byteArray;
}

uint8_t* DDSFile::decodeRGB(uint16_t color) const {
	uint8_t* rgb = new uint8_t[3];
	rgb[0] = (color & RED_MASK) >> RED_OFFSET;
	rgb[1] = (color & GREEN_MASK) >> GREEN_OFFSET;
	rgb[2] = (color & BLUE_MASK) << BLUE_OFFSET;
	//for (unsigned long i = 0; i < imageSize; i += 3) { //3 bytes per pixel
		std::cout << (unsigned int)rgb[0] << (unsigned int)rgb[1] << (unsigned int)rgb[2] << std::endl;
	//}
	return rgb;
}