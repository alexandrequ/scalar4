#include "GPS.h"


GPS::GPS(){
	latitude=0;
	longitude=0;
	height=0;
	usedSat=0;
	cout<<"ok"<<endl;
	
	uart0_filestream = open("/dev/serial0", O_RDONLY | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
	if (uart0_filestream == -1)
	{
		cout<<"Error - Unable to open UART."<<endl;
	}
	
	struct termios options;
	tcgetattr(uart0_filestream, &options);
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);
	
}

GPS::~GPS(){
	
}

void GPS::display(ostream& flux){
	flux<<"longitude:"<<longitude<<"	latitude:"<<latitude<<"		hauteur:"<<height<<endl;
	flux<<"nombre de satellite utilises:"<<usedSat<<endl;		
	
}

string GPS::readLine(){
	char readChar[1];
	string myString = string();
	do{
		int nbchar = read(this->uart0_filestream, (void*)readChar,1);
		if(nbChar>0){
			myString+=readChar[0];
		}
	}while(readChar[0]!=10);
	return myString;
}


void GPS::actualiseData(){
	string message=this->readLine();
	vector<string> splittedMsg=vector<string>();
	int i=0;
	int begin=0;
	int length=0;
	while(i<message.length()){
		if(message[i]==','){
			splittedMsg.push_back(message.substr(begin,length));
			begin=i+1;
			length=0;
		}
		else{
			length++;
		}
		i++;
	}
	splittedMsg.push_back(message.substr(begin,length));
	
	if(splittedMsg[0]=="b'$GPGGA"){
		if(splittedMsg[2]!=""){
			int NS=1;
			int EW=1;
			if(splittedMsg[3]=="S"){
				NS=-1;
			}
			if(splittedMsg[5]=="W"){
				EW=-1;
			}
			this->latitude  = NS*(stod(splittedMsg[2].substr(0,2))+stod(splittedMsg[2].substr(2))/60);
			this->longitude = EW*(stod(splittedMsg[4].substr(0,3))+stod(splittedMsg[4].substr(3,7))/60);
			this->height    = stod(splittedMsg[9]);
			this->usedSat   = stoi(splittedMsg[7]);
			
		}
	}
}


ostream& operator<<(ostream& flux, GPS& gps){
	gps.display(flux);
	return flux;
}

