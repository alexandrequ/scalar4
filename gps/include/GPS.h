#ifndef GPS_H
#define GPS_H

#include <string>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <vector>

using namespace std;

class GPS{
	public:
	
	GPS();
	~GPS();
	void actualiseData();
	void display(ostream& flux);
	string readLine();
	
	private:
	
	double latitude;
	double longitude;
	double height;
	int usedSat;
	int uart0_filestream;
};

ostream& operator<<(ostream& flux, GPS& gps);
#endif
