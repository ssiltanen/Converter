#pragma once

#include "Interfaces.h"

#include <string>
#include <stdint.h>


//Without the pragma, the short fields are being padded to 4 bytes. 
//The size of BITMAPFILEHEADER (w/o pragma) is 20, but in the file it is written sequentially as 14 bytes.
#pragma pack(2)

typedef struct {                     /**** BMP file header structure ****/
	unsigned short bfType;           // Number for file
	unsigned int   bfSize;           // Size of file
	unsigned short bfReserved1;      // Reserved
	unsigned short bfReserved2;      
	unsigned int   bfOffBits;        // Offset to bitmap data
} BITMAPFILEHEADER;

#pragma pack()

#define BF_TYPE_MB 0x4D42          // "MB"

typedef struct {                     /**** BMP file info structure ****/
	unsigned int   biSize;           // Size of info header
	int            biWidth;          // Width of image
	int            biHeight;         // Height of image
	unsigned short biPlanes;         // Number of color planes
	unsigned short biBitCount;       // Number of bits per pixel
	unsigned int   biCompression;    // Type of compression to use
	unsigned int   biSizeImage;      // Size of image data
	int            biXPelsPerMeter;  // X pixels per meter
	int            biYPelsPerMeter;  // Y pixels per meter
	unsigned int   biClrUsed;        // Number of colors used
	unsigned int   biClrImportant;   // Number of important colors
} BITMAPINFOHEADER;

#define BI_RGB       0             // No compression - straight BGR data
#define BI_RLE8      1             // 8-bit run-length compression
#define BI_RLE4      2             // 4-bit run-length compression
#define BI_BITFIELDS 3             // RGB bitmap with RGB masks

typedef struct {                     /**** Colormap entry structure ****/
	unsigned char  rgbBlue;          // Blue value
	unsigned char  rgbGreen;         // Green value
	unsigned char  rgbRed;           // Red value
	unsigned char  rgbReserved;      // Reserved
} RGBQUAD;

class BMPFile : public IFiletype {
public:
	BMPFile(const std::string& location);
	virtual ~BMPFile();

private:
	BITMAPFILEHEADER* m_pBmpHeader;
	BITMAPINFOHEADER* m_pBmpInfoHeader;
	uint8_t* m_pixels;
};

