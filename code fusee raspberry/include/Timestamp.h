#include <sys/time.h>
#include <string>

class Timestamp{
	public:
	Timestamp();
	
	void set_zero();
	
	unsigned long timestamp();

	std::string timestamp_string();
	
	private:
	struct timeval StartTime;
};
