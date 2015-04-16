#include "image.h"
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <setjmp.h>



static const int maxvals[9] = { 0, 1, 3, 7, 15, 31, 63, 127, 255 };

/*  val2bits												*/
/*  converts a double v into an integer in range [0,255].	*/
/*	rounding is done so as to ensure that only				*/
/*  only 2^b - 1 choices in the range [0,255] are possible.	*/

static inline unsigned char val2bits (double v, int b)
{

	return (unsigned char) (floor(v * maxvals[b] + 0.5) / maxvals[b] * 255);
}

Pixel::Pixel()
{
	col[0]=0;
	col[1]=0;
	col[2]=0;
}
Pixel::Pixel(double r, double g, double b)
{
	col[0]=r;
	col[1]=g;
	col[2]=b;
}



Pixel::~Pixel()
{
}

// copy constructor
Pixel::Pixel (const Pixel& toCopy)
{
	col[0] = toCopy.col[0];
	col[1] = toCopy.col[1];
	col[2] = toCopy.col[2];
}

// assignment operator
Pixel& Pixel::operator = (const Pixel& toCopy)
{

	if (this != &toCopy) {
		col[0] = toCopy.col[0];
		col[1] = toCopy.col[1];
		col[2] = toCopy.col[2];
	}
	return *this;
}


double Pixel::getColor(int chan)
{
	assert (chan>=0 && chan <=2);
	return col[chan];
}
void Pixel::setColor(int chan, double val)
{
	assert (chan>=0 && chan <=2);
	val=max(val,0);
	val=min(val,1);
	col[chan]=val;
}

// write
ostream &operator<<(ostream &out_file, Pixel& thePixel)
{
	out_file << thePixel.getColor(0) << ", " << thePixel.getColor(1) << ", " << thePixel.getColor(2);
	return out_file;
}


Image::Image ()
{
	width    = 0;
	height   = 0;
	bits     = 0;
	maxValue = 0;
	pixels   = NULL;
}


Image::Image (int width_, int height_)
{
	width    = width_;
	height   = height_;

	bits     = 8;
	maxValue = 255;

	// note we don't handle alpha channels
	assert(width > 0 && height > 0 &&
		bits > 0 && bits < 9
		);

	pixels   = new unsigned char[width*height*3];;
	memset(pixels, 0, width*height*3);
}


Image::Image (int width_, int height_, int bits_)
{
	width    = width_;
	height   = height_;
	bits     = bits_;
	maxValue = 255;

	assert(width > 0 && height > 0 &&
		bits > 0 && bits < 9
		);


	pixels   = new unsigned char[width*height*3];

	memset(pixels, 0, width*height*3);
}




Image::Image (const char* filename)
{
	width    = 0;
	height   = 0;
	bits     = 0;
	maxValue = 0;
	pixels   = NULL;

	read(filename);
}


Image::~Image ()
{
	if (pixels) delete[] pixels;
}


Image::Image (const Image& image)
{
	width    = image.width;
	height   = image.height;
	bits     = image.bits;
	maxValue = image.maxValue;
	pixels   = new unsigned char[width*height*3];

	for (int i = 0; i < width*height*3; ++i)
		pixels[i] = image.pixels[i];
}


Image& Image::operator= (const Image& image)
{
	if (&image == this) return *this;

	if (pixels) delete[] pixels;

	width    = image.width;
	height   = image.height;
	bits     = image.bits;
	maxValue = image.maxValue;
	pixels   = new unsigned char[width*height*3];

	for (int i = 0; i < width*height*3; ++i)
		pixels[i] = image.pixels[i];

	return *this;
}


bool Image::good ()
{
	return (width > 0 && height > 0 &&
		bits > 0 && bits < 9 && pixels);
}


bool Image::bad ()
{
	return !good();
}


void Image::clear ()
{
	memset(pixels, 0, width*height*3);
}


int Image::index (int x, int y, int c)
{
	return (((height - y - 1) * width + x) * 3 + c);
}


double Image::getPixel (int x, int y, int channel)
{
	assert(good());
	assert((x >= 0)       &&
		(x <  width)   &&
		(y >= 0)       &&
		(y <  height)  &&
		(channel >= 0) &&
		(channel < 3));

	return pixels[index(x,y,channel)] / 255.0;
}


double Image::getPixel_ (int x, int y, int channel)
{
	if (!good()        ||
		(x <  0)       ||
		(x >= width)   ||
		(y <  0)       ||
		(y >= height)  ||
		(channel < 0)  ||
		(channel >= 3))
		return 0.0;

	return getPixel(x,y,channel);
}


Pixel Image::getPixel (int x, int y)
{
	assert(good());
	assert((x >= 0)       &&
		(x <  width)   &&
		(y >= 0)       &&
		(y <  height));

	Pixel pixel;


	pixel.setColor(BLUE, pixels[index(x,y,BLUE)]  / 255.0);
	pixel.setColor(GREEN, pixels[index(x,y,GREEN)] / 255.0);
	pixel.setColor(RED,pixels[index(x,y,RED)]   / 255.0);

	return pixel;
}


Pixel Image::getPixel_ (int x, int y)
{
	if (!good()        ||
		(x <  0)       ||
		(x >= width)   ||
		(y <  0)       ||
		(y >= height))
	{
		Pixel pixel;
		memset(&pixel, 0, sizeof(Pixel));
		return pixel;
	}

	return getPixel(x,y);
}


Pixel& Image::getPixel (int x, int y, Pixel& pixel)
{
	assert(good());
	assert((x >= 0)       &&
		(x <  width)   &&
		(y >= 0)       &&
		(y <  height));


	pixel.setColor(BLUE,pixels[index(x,y,BLUE)]  / 255.0);
	pixel.setColor(GREEN,pixels[index(x,y,GREEN)] / 255.0);
	pixel.setColor(RED, pixels[index(x,y,RED)]   / 255.0);

	return pixel;
}


Pixel& Image::getPixel_ (int x, int y, Pixel& pixel)
{
	if (!good()        ||
		(x <  0)       ||
		(x >= width)   ||
		(y <  0)       ||
		(y >= height))
	{
		return pixel;
	}

	return getPixel(x,y,pixel);
}


/* set pixel value.  throw error if any problem with parameters */
/* including value outside of [0,1] */
void Image::setPixel (int x, int y, int channel, double value)
{
	assert(good());
	assert((x >= 0)       &&
		(x <  width)   &&
		(y >= 0)       &&
		(y <  height)  &&
		(channel >= 0) &&
		(channel < 3));
	assert((value >= 0.0) && 
		(value <= 1.0));  

	pixels[index(x,y,channel)] = val2bits(value, bits);
}


/*  set pixel clamps value clamping to [0,1]	*/
/*  if any problem with parameters it RETURNS	*/
void Image::setPixel_ (int x, int y, int channel, double value)
{
	if (!good()        ||
		(x <  0)       ||
		(x >= width)   ||
		(y <  0)       ||
		(y >= height)  ||
		(channel < 0)  ||
		(channel >= 3))
		return;
	value=clamp(value, 0, 1);
	setPixel(x,y,channel,value);
}



/* set pixel;  throws error if problems with parameters */
/* including pixel value outside of [0,1]			    */
void Image::setPixel (int x, int y, Pixel& pixel)
{
	assert(good());
	assert((x >= 0)       &&
		(x <  width)   &&
		(y >= 0)       &&
		(y <  height));


	assert((pixel.getColor(BLUE) >= 0.0) && 
		(pixel.getColor(BLUE)<= 1.0));  
	assert((pixel.getColor(GREEN) >= 0.0) && 
		(pixel.getColor(GREEN) <= 1.0));  
	pixels[index(x,y,BLUE)]  = val2bits(pixel.getColor(BLUE), bits);
	pixels[index(x,y,GREEN)] = val2bits(pixel.getColor(GREEN), bits);

	assert((pixel.getColor(RED) >= 0.0) && 
		(pixel.getColor(RED) <= 1.0));  
	pixels[index(x,y,RED)]   = val2bits(pixel.getColor(RED), bits);


}


/*  set pixel value, clamping to [0,1]    */
/*  if any problem with parameters RETURN */
void Image::setPixel_ (int x, int y, Pixel& pixel)
{
	if (!good()        ||
		(x <  0)       ||
		(x >= width)   ||
		(y <  0)       ||
		(y >= height))
		return;
	pixel.setColor(RED,clamp(pixel.getColor(RED),0,1));
	pixel.setColor(GREEN,clamp(pixel.getColor(GREEN),0,1));
	pixel.setColor(BLUE,clamp(pixel.getColor(BLUE),0,1));
	setPixel(x,y,pixel);
}


void Image::glReadPixelsWrapper ()
{
	assert(good());
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

}


void Image::glDrawPixelsWrapper ()
{
	assert(good());

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);


}


void Image::glTexImage2DWrapper ()
{
	assert(good());

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, pixels);

}


int Image::read (const char* filename)
{
	ifstream file(filename,ifstream::binary);

	if (file.bad())
	{
		cerr << "Couldn't open file " << filename << endl;
		return -1;
	}

	unsigned char type[2];
	file.read((char*) type, sizeof(type));

	if ((type[0] == 0x4D && type[1] == 0x42) ||
		(type[1] == 0x4D && type[0] == 0x42))
	{
		file.seekg(0, ifstream::beg);
		return readBMP(file);
	}


	else
	{
		cerr << "Unknown filetype!" << endl;
		file.close();
		return -1;
	}
}


int Image::write (const char* filename)
{
	int len = strlen(filename);
	const char* ext = &(filename[len-4]);

	if (strncmp(ext, ".bmp", 4) == 0)
	{
		cerr << "Writing BMP " << filename << endl;
		return writeBMP(filename);
	}
	else
		return -1;

}


/******************************************************************
* .BMP file  manipulation  
*/



int Image::readBMP (const char* filename)
{
	ifstream file(filename);

	if (file.good())
		return readBMP(file);
	else
		return -1;
}


int Image::writeBMP (const char* filename)
{
	ofstream file(filename, ios_base::binary);

	if (file.good())
	{
		if (writeBMP(file) == -1)
		{
			file.close();
			//unlink(filename);
			return -1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return -1;
	}
}


//#if !defined(WIN32) || defined(_CONSOLE)

typedef unsigned char BYTE;		/* 8 bits */
typedef unsigned short int WORD;	/* 16-bit unsigned integer. */
typedef unsigned int DWORD;		/* 32-bit unsigned integer */
typedef int LONG;			/* 32-bit signed integer */



typedef struct tagBITMAPFILEHEADER {
	WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
	WORD bfReserved2;
	DWORD bfOffBits;
} BITMAPFILEHEADER;



typedef struct tagBITMAPINFOHEADER {
	DWORD biSize;
	LONG biWidth;
	LONG biHeight;
	WORD biPlanes;
	WORD biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	LONG biXPelsPerMeter;
	LONG biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
} BITMAPINFOHEADER;



/* constants for the biCompression field */
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L



typedef struct tagRGBTRIPLE {
	BYTE rgbtBlue;
	BYTE rgbtGreen;
	BYTE rgbtRed;
} RGBTRIPLE;



typedef struct /*tagRGBQUAD*/ {
	BYTE rgbBlue;
	BYTE rgbGreen;
	BYTE rgbRed;
	BYTE rgbReserved;
} RGBQUAD;


//#endif // !defined(WIN32) || defined(_CONSOLE)


/* Some magic numbers */

#define BMP_BF_TYPE 0x4D42
/* word BM */

#define BMP_BF_OFF_BITS 54
/* 14 for file header + 40 for info header (not sizeof(), but packed size) */

#define BMP_BI_SIZE 40
/* packed size of info header */


/* Reads a WORD from a file in little endian format */
static WORD WordReadLE(ifstream& fp)
{
	WORD lsb, msb;

	lsb = fp.get();
	msb = fp.get();
	return (msb << 8) | lsb;
}



/* Writes a WORD to a file in little endian format */
static void WordWriteLE(WORD x, ofstream& fp)
{
	BYTE lsb, msb;

	lsb = (BYTE) (x & 0x00FF);
	msb = (BYTE) (x >> 8);
	fp.put(lsb);
	fp.put(msb);
}



/* Reads a DWORD word from a file in little endian format */
static DWORD DWordReadLE(ifstream& fp)
{
	DWORD b1, b2, b3, b4;

	b1 = fp.get();
	b2 = fp.get();
	b3 = fp.get();
	b4 = fp.get();
	return (b4 << 24) | (b3 << 16) | (b2 << 8) | b1;
}



/* Writes a DWORD to a file in little endian format */
static void DWordWriteLE(DWORD x, ofstream& fp)
{
	unsigned char b1, b2, b3, b4;

	b1 = (x & 0x000000FF);
	b2 = ((x >> 8) & 0x000000FF);
	b3 = ((x >> 16) & 0x000000FF);
	b4 = ((x >> 24) & 0x000000FF);
	fp.put(b1);
	fp.put(b2);
	fp.put(b3);
	fp.put(b4);
}



/* Reads a LONG word from a file in little endian format */
static LONG LongReadLE(ifstream& fp)
{
	LONG b1, b2, b3, b4;

	b1 = fp.get();
	b2 = fp.get();
	b3 = fp.get();
	b4 = fp.get();
	return (b4 << 24) | (b3 << 16) | (b2 << 8) | b1;
}



/* Writes a LONG to a file in little endian format */
static void LongWriteLE(LONG x, ofstream& fp)
{
	char b1, b2, b3, b4;

	b1 = (x & 0x000000FF);
	b2 = ((x >> 8) & 0x000000FF);
	b3 = ((x >> 16) & 0x000000FF);
	b4 = ((x >> 24) & 0x000000FF);
	fp.put(b1);
	fp.put(b2);
	fp.put(b3);
	fp.put(b4);
}


int bitcount (DWORD w)
{
	w = (0x55555555 & w) + (0x55555555 & (w>> 1));
	w = (0x33333333 & w) + (0x33333333 & (w>> 2));
	w = (0x0f0f0f0f & w) + (0x0f0f0f0f & (w>> 4));
	w = (0x00ff00ff & w) + (0x00ff00ff & (w>> 8));
	w = (0x0000ffff & w) + (0x0000ffff & (w>>16));
	return w;
}



int Image::readBMP (ifstream& fp)
{
	if (pixels) delete[] pixels;

	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;

	/* Read file header */
	bmfh.bfType = WordReadLE(fp);  // magic number should be 19778
	bmfh.bfSize = DWordReadLE(fp); // size file in bytes
	bmfh.bfReserved1 = WordReadLE(fp); // reserved, should be 0
	bmfh.bfReserved2 = WordReadLE(fp); // reserved, should be 0
	bmfh.bfOffBits = DWordReadLE(fp); //offset to bitmap data

	/* Debug file header  
	fprintf(stderr, "file header:\n");
	fprintf(stderr, "\tbfType: %x\n", (int) bmfh.bfType);
	fprintf(stderr, "\tbfSize: %d\n", (int) bmfh.bfSize);
	fprintf(stderr, "\tbfReserved1: %d\n", (int) bmfh.bfReserved1);
	fprintf(stderr, "\tbfReserved2: %d\n", (int) bmfh.bfReserved2);
	fprintf(stderr, "\tbfOffBits: %d\n", (int) bmfh.bfOffBits);*/

	/* Check file header */
	if (bmfh.bfType != BMP_BF_TYPE) 
	{
		cerr << "This file is not in BMP format." << endl;
		return -1;
	}

	/* Read info header */
	bmih.biSize = DWordReadLE(fp);			//size of info header in bytes
	bmih.biWidth = LongReadLE(fp);			//width of image in pixels
	bmih.biHeight = LongReadLE(fp);			// height of image in pixels
	bmih.biPlanes = WordReadLE(fp);			// number of planes
	bmih.biBitCount = WordReadLE(fp);		// number of bits per pixel
	bmih.biCompression = DWordReadLE(fp);	// type of compression, 0=none
	if (bmih.biCompression!=0) 
	{
		cerr << "We cannot handle compressed BMP files." << endl;
		return -1;
	}
	bmih.biSizeImage = DWordReadLE(fp);		// size of image in bytes
	// if no compression this field
	// might be 0
	// we are not going to support compression so the
	// image size is always width*height
	// because of the possibility that this is set to 0 above
	// we will recompute
	bmih.biSizeImage = bmih.biHeight*bmih.biWidth;  
	bmih.biXPelsPerMeter = LongReadLE(fp);	// we don't care about this
	bmih.biYPelsPerMeter = LongReadLE(fp);	// we don't care about this
	bmih.biClrUsed = DWordReadLE(fp);		// number of colors used in
	// bitmap, if 0 then the
	// #colors = 2^bitsperpixel
	bmih.biClrImportant = DWordReadLE(fp);	// we don't care about this

	/* Debug info header
	fprintf(stderr, "info header:\n");
	fprintf(stderr, "\tbiSize: %d\n", (int) bmih.biSize);
	fprintf(stderr, "\tbiWidth: %d\n", (int) bmih.biWidth);
	fprintf(stderr, "\tbiHeight: %d\n", (int) bmih.biHeight);
	fprintf(stderr, "\tbiPlanes: %d\n", (int) bmih.biPlanes);
	fprintf(stderr, "\tbiBitCount: %d\n", (int) bmih.biBitCount);
	fprintf(stderr, "\tbiCompression: %d\n", (int) bmih.biCompression);
	fprintf(stderr, "\tbiSizeImage: %d\n", (int) bmih.biSizeImage);
	fprintf(stderr, "\tbiXPelsPerMeter: %d\n", (int) bmih.biXPelsPerMeter);
	fprintf(stderr, "\tbiYPelsPerMeter: %d\n", (int) bmih.biYPelsPerMeter);
	fprintf(stderr, "\tbiClrUsed: %d\n", (int) bmih.biClrUsed);
	fprintf(stderr, "\tbiClrImportant: %d\n", (int) bmih.biClrImportant);*/

	// check info header
	if ((bmih.biWidth <= 0)  ||
		(bmih.biHeight <= 0) ||
		(bmih.biPlanes != 1) ||
		(bmih.biBitCount!=24 && bmih.biBitCount!=8)  )

	{
		cerr << "Something is wrong with the image or it is not in a format we can read.";
		return -1;
	}

	/* Creates the image */
	width    = bmih.biWidth;
	height   = bmih.biHeight;
	bits     = 8;
	maxValue = 255;
	pixels   = new unsigned char[width*height*3];
	memset(pixels, 0, width*height*3);

	// each row of the image is width*bmih.biBitCount bits
	// each row of the image is padded in the file so that each
	// row's length is a multiple of 32 bits (4 bytes)
	// scalinelength is the number of bytes in each row of the file
	int scanlinelength = (int) ceil((double) ((width * bmih.biBitCount)/32.0))*4;


	RGBQUAD		*palette;
	BYTE		*scanlineByte;
	RGBTRIPLE   *scanline24;
	Pixel       pixel;
	int			index;
	bool		oneChannel=false;

	// read all the color info / data
	switch (bmih.biBitCount)  // 
	{

	case 8:	// 8 bit - 256 color, index mode
		// we only support color indexing to handle
		// one channel images

		// read the palette 

		palette = new RGBQUAD[256];
		memset(palette, 0, 256 * sizeof(RGBQUAD));
		fp.read((char*) palette, 256 * sizeof(RGBQUAD));

		// check to be sure this is intended to be a one channel
		// image
		oneChannel=true;
		for (int i=0; i<256; ++i) 
		{
			if ((palette[i].rgbRed != palette[i].rgbGreen) ||
				(palette[i].rgbRed != palette[i].rgbBlue))
				oneChannel=false;
		}
		if (!oneChannel)
		{
			cerr << "Warning:  Color indexing is not supported except to handle one channel images." << endl;
		}

		// read the image
		scanlineByte = new BYTE[scanlinelength];
		fp.seekg((long) bmfh.bfOffBits, ifstream::beg);

		for (int y = 0; y < height; ++y) 
		{
			fp.read((char*) scanlineByte, scanlinelength);
			if (fp.fail())
			{	
				cerr << "unknown stream error!" << endl;
				delete[] palette;
				delete[] pixels;
				pixels = NULL;
				return -1;
			}

			// create row of image
			for (int x = 0; x < width; ++x)
			{
				index = scanlineByte[x];
				pixel.setColor(RED,palette[index].rgbRed   / 255.0);
				// blue and green channels aren't used in 8 bit mode
				setPixel(x, height - 1 - y, pixel);
			}
		} // end read
		delete[] scanlineByte;
		delete[] palette;

		break; // end of 8 bit

	default:  // default is 24 bit
		scanline24 = new RGBTRIPLE[scanlinelength];
		fp.seekg((long) bmfh.bfOffBits, ifstream::beg);

		for (int y = 0; y < height; ++y) 
		{
			fp.read((char*) scanline24, scanlinelength); 
			if (fp.fail())
			{
				cerr << "unknown stream error0!" << endl;
				delete[] pixels;
				pixels = NULL;
				return -1;
			}
			// create row of image
			for (int x = 0; x < width; ++x)
			{
				pixel.setColor(RED,scanline24[x].rgbtRed   / 255.0);
				pixel.setColor(GREEN,scanline24[x].rgbtGreen / 255.0);
				pixel.setColor(BLUE,scanline24[x].rgbtBlue  / 255.0);
				setPixel(x, height - 1 - y, pixel);
			}
		}// end read

	} // end switch

	return 0;
}



int Image::writeBMP (ofstream& fp)
{
	if (!good())
	{
		cerr << "Invalid image" << endl;
		return -1;
	}

	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;
	int lineLength;

	lineLength = width*3;

	//  for efficiency(?) we want to have line lengths that are multiples
	//  of 32 -- we'll pad as necessary to make that happen
	if ((lineLength % 4) != 0) 
		lineLength = (lineLength / 4 + 1) * 4;

	/* Write file header */

	bmfh.bfType = BMP_BF_TYPE;
	bmfh.bfSize = BMP_BF_OFF_BITS + lineLength * height;
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = BMP_BF_OFF_BITS;

	WordWriteLE(bmfh.bfType, fp);
	DWordWriteLE(bmfh.bfSize, fp);
	WordWriteLE(bmfh.bfReserved1, fp);
	WordWriteLE(bmfh.bfReserved2, fp);
	DWordWriteLE(bmfh.bfOffBits, fp);

	/* Write info header */

	bmih.biSize = BMP_BI_SIZE;
	bmih.biWidth = width;
	bmih.biHeight = height;
	bmih.biPlanes = 1;
	bmih.biBitCount = 24; 
	bmih.biCompression = BI_RGB;
	bmih.biSizeImage = lineLength * (DWORD) bmih.biHeight;
	bmih.biXPelsPerMeter = 2925;
	bmih.biYPelsPerMeter = 2925;
	bmih.biClrUsed =  0;
	bmih.biClrImportant = 0;

	DWordWriteLE(bmih.biSize, fp);
	LongWriteLE(bmih.biWidth, fp);
	LongWriteLE(bmih.biHeight, fp);
	WordWriteLE(bmih.biPlanes, fp);
	WordWriteLE(bmih.biBitCount, fp);
	DWordWriteLE(bmih.biCompression, fp);
	DWordWriteLE(bmih.biSizeImage, fp);
	LongWriteLE(bmih.biXPelsPerMeter, fp);
	LongWriteLE(bmih.biYPelsPerMeter, fp);
	DWordWriteLE(bmih.biClrUsed, fp);
	DWordWriteLE(bmih.biClrImportant, fp);

	/* Write pixels */

	Pixel pixel;



	for (int y = 0; y < height; ++y) 
	{
		int nbytes = 0;
		for (int x = 0; x < width; ++x) 
		{
			getPixel(x, height - y - 1, pixel);
			fp.put((unsigned char) (pixel.getColor(BLUE) * 255)), nbytes++;
			fp.put((unsigned char) (pixel.getColor(GREEN) * 255)), nbytes++;
			fp.put((unsigned char) (pixel.getColor(RED) * 255)), nbytes++;
		}

		while ((nbytes % 4) != 0) 
		{
			fp.put((unsigned char) 0);  
			nbytes++;
		}

	}


	if (fp.fail())
	{
		cerr << "unknown stream error" << endl;
		return -1;
	}

	return 0;
}
