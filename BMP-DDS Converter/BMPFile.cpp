#define _CRT_SECURE_NO_DEPRECATE
#include "BMPFile.h"

#include <iostream>
#include <fstream>
#include "MyException.h"


BMPFile::BMPFile(const std::string& location)
{
	uint8_t* dataBuffer[2] = { nullptr, nullptr }; // Header buffers
	m_pixels = nullptr;

	m_pBmpHeader = nullptr;
	m_pBmpInfoHeader = nullptr;

	std::ifstream file(location, std::ios::binary);
	if (!file) 
		builderFailed(nullptr, NULL, "Failure to open bitmap file from location: " + location);

	// Allocate byte memory that will hold the two headers
	dataBuffer[0] = new uint8_t[sizeof(BITMAPFILEHEADER)];
	dataBuffer[1] = new uint8_t[sizeof(BITMAPINFOHEADER)];
	file.read((char*)dataBuffer[0], sizeof(BITMAPFILEHEADER));
	file.read((char*)dataBuffer[1], sizeof(BITMAPINFOHEADER));

	// Construct the values from the buffers
	m_pBmpHeader = (BITMAPFILEHEADER*)dataBuffer[0];
	m_pBmpInfoHeader = (BITMAPINFOHEADER*)dataBuffer[1];

	// Check if the file is a BMP file
	if (m_pBmpHeader->bfType != BF_TYPE_MB) 
		builderFailed(dataBuffer, 2, "File " + location + " isn't a bitmap file");

	//Check if file is 24 bits per pixel
	if (m_pBmpInfoHeader->biBitCount != BIT_COUNT_24) 
		builderFailed(dataBuffer, 2, "File " + location + " isn't a 24bpp file");

	if (m_pBmpInfoHeader->biWidth * 3 % 4 != 0 || m_pBmpInfoHeader->biHeight * 3 % 4 != 0)
		builderFailed(dataBuffer, 2, "File " + location + " does not have width or height divisible by 4");

	unsigned int imageSize = m_pBmpInfoHeader->biSizeImage;

	//Check if the header shows the size of the image
	//If the size in header is zero, use calculated value based on offset to data and file size
	if (m_pBmpInfoHeader->biSizeImage == 0) {
		std::cerr << "Warning! File " << location << " has invalid header, doesn't show size of the file" << std::endl;
		imageSize = m_pBmpHeader->bfSize - m_pBmpHeader->bfOffBits;
	}

	// Allocate pixel memory
	m_pixels = new uint8_t[imageSize];

	// Go to where image data starts
	file.seekg(m_pBmpHeader->bfOffBits);

	//read in image data
	//Supports only divisible by 4 width and height so no padding is needed
	file.read((char*)m_pixels, imageSize);

	//BGR format to RGB
	for (unsigned long i = 0; i < imageSize; i += 3) { //3 bytes per pixel
		uint8_t tmpRGB = 0;
		tmpRGB = m_pixels[i];
		m_pixels[i] = m_pixels[i + 2];
		m_pixels[i + 2] = tmpRGB;
		std::cout << (int)m_pixels[i] << " " << (int)m_pixels[i + 1] << " " << (int)m_pixels[i + 2] << " " << (int)m_pixels[i + 3] << std::endl;
	}
	delete[] dataBuffer[0];
	delete[] dataBuffer[1];
}


BMPFile::~BMPFile()
{
	delete m_pixels;
	delete m_pBmpHeader;
	delete m_pBmpInfoHeader;
}

void BMPFile::builderFailed(uint8_t * dataBuffer[], int arraySize, std::string & cause) const
{
	if (dataBuffer != nullptr) {
		for (int i = 0; i < arraySize; ++i) {
			delete dataBuffer[i];
		}
	}
	throw MyException(cause);
}
