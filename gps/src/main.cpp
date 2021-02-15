#include <iostream>
#include <string>
#include "GPS.h"

using namespace std;

int main(int argc, char** argv){
	GPS gps=GPS();
	cout<<gps<<endl;
	while(1){
		gps.actualiseData();
		cout<<gps<<endl;
	}
	return 0;
}
