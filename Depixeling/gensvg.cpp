//
// gensvg.cpp
//

#include <iostream>

using namespace std;

// Constant declarations specifying the dimensions of the image and
// the rectangular bar.

const unsigned IMG_WIDTH = 100;
const unsigned IMG_HEIGHT = 30;
const unsigned IMG_MARGIN = 10;
const unsigned BAR_WIDTH = IMG_WIDTH - 2 * IMG_MARGIN;
const unsigned BAR_HEIGHT = IMG_HEIGHT - 2 * IMG_MARGIN;

// Prolog for the SVG image

void header() {
  cout << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" << endl
       << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.0//EN\"" << endl
       << " \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">" << endl 
       << "<svg width=\"" << IMG_WIDTH 
       << "\" height=\"" << IMG_HEIGHT << "\">" << endl
       << "<title>Bar</title>" << endl;
}

// Epilog for the SVG image

void footer() {
  cout << "</svg>" << endl;
}

// Generation of the two segments

void bar(unsigned m, unsigned n) {
  unsigned m_width = (unsigned) (((double) m) / ((double) m + n) * BAR_WIDTH);
  unsigned n_width = BAR_WIDTH - m_width;
  cout << "<rect x=\"" << IMG_MARGIN << "\" y=\"" << IMG_MARGIN << "\""
       << " width=\"" << m_width << "\" height=\"" << BAR_HEIGHT << "\""
       << " style=\"stroke: none; fill: red;\" />" << endl;
  cout << "<rect x=\"" << IMG_MARGIN + m_width << "\" y=\"" << IMG_MARGIN << "\""
       << " width=\"" << n_width << "\" height=\"" << BAR_HEIGHT << "\""
       << " style=\"stroke: none; fill: green;\" />" << endl;
}

// Read input and generate SVG image

int main() {
  unsigned m, n;

  cin >> m;
  cin >> n;

  header();
  bar(m, n);
  footer();

  return 0;
}