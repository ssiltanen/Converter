#define _CRT_SECURE_NO_DEPRECATE
#include "BMPFile.h"

#include <functional>
#include <iostream>
#include <fstream>


BMPFile::BMPFile(const std::string& location)
{
	uint8_t* dataBuffer[2] = { nullptr, nullptr }; // Header buffers
	m_pixels = nullptr;

	m_pBmpHeader = nullptr;
	m_pBmpInfoHeader = nullptr;

	std::ifstream file(location, std::ios::binary);
	if (!file) {
		std::cerr << "Failure to open bitmap file from location: " << location << std::endl;
		throw std::bad_function_call();
	}

	// Allocate byte memory that will hold the two headers
	dataBuffer[0] = new uint8_t[sizeof(BITMAPFILEHEADER)];
	dataBuffer[1] = new uint8_t[sizeof(BITMAPINFOHEADER)];
	file.read((char*)dataBuffer[0], sizeof(BITMAPFILEHEADER));
	file.read((char*)dataBuffer[1], sizeof(BITMAPINFOHEADER));

	// Construct the values from the buffers
	m_pBmpHeader = (BITMAPFILEHEADER*)dataBuffer[0];
	m_pBmpInfoHeader = (BITMAPINFOHEADER*)dataBuffer[1];

	// Check if the file is a BMP file
	if (m_pBmpHeader->bfType != BF_TYPE_MB) {
		std::cerr << "File " << location << " isn't a bitmap file" << std::endl;
		throw std::bad_function_call();
	}

	// First allocate pixel memory
	m_pixels = new uint8_t[m_pBmpInfoHeader->biSizeImage];

	// Go to where image data starts
	file.seekg(m_pBmpHeader->bfOffBits + 1);
	//read in image data
	file.read((char*)m_pixels, m_pBmpInfoHeader->biSizeImage);

	//BGR format to RGB
	for (unsigned long i = 0; i < m_pBmpInfoHeader->biSizeImage; i += 4) {
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