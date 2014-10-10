# ifndef _COLOR_H
# define _COLOR_H

class Color {
	double red;
	double green;
	double blue;
public:
	Color();
	Color(double r, double g, double b);
	void setRed(double r);
	void setGreen(double g);
	void setBlue(double b);
	double getRed();
	double getGreen();
	double getBlue();
};
# endif