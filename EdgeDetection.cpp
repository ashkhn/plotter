#include <cstdlib>
#include <iostream>
#include <vector>
#include "EasyBMP.h"
using namespace std;

int filter(int p1, int p2, int thresh) {
  if (p1 - p2 > thresh || p2 - p1 > thresh) {
    return 255;
  }
  else {
    return 0;
  }
}


int main( int argc, char* argv[] ) {
  const int width = 3, thresh = 5;

  BMFH bmfh = GetBMFH(argv[1]);
  if (bmfh.bfType != BMP::BMP_FILE_TYPE) { return -1; }

  BMP inImage;
  inImage.ReadFromFile(argv[1]);
  BMP outImage;
  outImage.SetSize(inImage.TellWidth(), inImage.TellHeight());
  outImage.SetBitDepth(24);

  const int numNbr = (2 * width + 1) * (2 * width + 1) - 1;
  for( int i=width ; i < inImage.TellWidth()-width ; i++) {
    for( int j=width ; j < inImage.TellHeight()-width ; j++) {
      // find average of neighboring input pixels
      int rPij = 0, gPij = 0, bPij = 0;
      for (int di = -width; di <= width; ++di) {
	for (int dj = -width; dj <= width; ++dj) {
	  if (di == 0 && dj == 0) continue; 
	  rPij += inImage.redPixel(i+di, j+dj);
	  gPij += inImage.greenPixel(i+di, j+dj);
	  bPij += inImage.bluePixel(i+di, j+dj);
	}
      }
      outImage.redPixel(i,j) = 
	filter(inImage.redPixel(i,j), rPij / numNbr, thresh);
      outImage.greenPixel(i,j) = 
	filter(inImage.greenPixel(i,j), gPij / numNbr, thresh);
      outImage.bluePixel(i,j) = 
	filter(inImage.bluePixel(i,j), bPij / numNbr, thresh);
    }
  }

  outImage.WriteToFile(argv[2]);
  return 0;
}
