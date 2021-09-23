#ifndef IDX_H
#define IDX_H
#include <fstream>


class idx {
	public:
		idx(char filename[]);
		bool getItem(int index, unsigned char buffer[]);
		bool good();
		unsigned int magic; //please do not write to this or else idx will not be happy with you
		int images;
		static int unendian(int in);
	private:
		std::ifstream file;
		int elements;
		int itemrank;
};

#endif // IDX_H
