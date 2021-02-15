#include "FileInterface.h"


#define BASE_NAME "./"

bool FileInterface::fexist(const std::string& filename){
	std::ifstream ifile(filename.c_str());
	return (bool)ifile;
}

std::string FileInterface::generateIndex(uint8_t i){
	if(i < 10)
		return "00"+std::to_string(i);
	else if(i<100)
		return "0"+std::to_string(i);
	else
		return std::to_string(i);
}

FileInterface::FileInterface(){
	
}

void FileInterface::init(std::string base_name){
	uint8_t i = 0;
	
	while(fexist(base_name + generateIndex(i)))
		i++;
	
	std::string filename = base_name + generateIndex(i);
	file = std::ofstream(filename.c_str());
}

void FileInterface::write(std::string s){
	file << s;
}

void FileInterface::write_line(std::string line){
	file << line << std::endl;
}


void FileInterface::write_canframe(struct can_frame frame, unsigned long timestamp){
	file << std::to_string(timestamp) << " ";
	file << std::to_string(frame.can_id) << " " << std::to_string(frame.can_dlc);
	for(int i = 0; i<frame.can_dlc; i++)
		file << " " << std::to_string(frame.data[i]);
	file << std::endl;
}

void FileInterface::write_array(uint8_t array[], int len){
	for(int i = 0; i<len; i++)
		file << array[i] << " ";
	file << std::endl;
}

std::string FileInterface::canframe_string(struct can_frame frame, unsigned long timestamp){
	std::string result = std::to_string(timestamp) +","+ std::to_string(frame.can_id) +","+ std::to_string(frame.can_dlc);
	for(int i = 0; i<frame.can_dlc; i++)
		result += "," + std::to_string(frame.data[i]);
	return result;
}

std::string FileInterface::array_string(uint8_t array[], int len, unsigned long timestamp){
	std::string result = std::to_string(timestamp);
	for(int i = 0; i<len; i++)
		result += "," + std::to_string(array[i]);
	return result;
}
