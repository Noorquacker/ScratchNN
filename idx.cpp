#include "idx.h"
#include <fstream>
#include <iostream>
#include <cstring>

int idx::unendian(int in) {
	return ((in & 255) << 24) + (((in >> 8) & 255) << 16) + (((in >> 16) & 255) << 8) + ((in >> 24) & 255);
}

idx::idx(char filename[]) {
	file.open(filename, std::ios::in | std::ios::binary);
	if(!file.good()) {
		std::cerr << "IDX: error opening file " << filename << "!\n";
	}
	file.read((char*)&magic, 4);
	magic = unendian(magic);
	file.read((char*)&elements, 4);
	elements = unendian(elements);
	itemrank = (magic & 255) - 1;
}

bool idx::getItem(int index, unsigned char buffer[]) {
	if(index >= elements || index < 0) {
		return false;
	}
	//everything is one byte so i can just yeet and skip dealing with multi byte items
	if(itemrank == 0) {
		//this is only for labels so ye
		file.seekg(8 + index);
		file.read((char*)&buffer[0], 1);
		return true;
	}
	else {
		//screw this imma just assume this is mnist and break compatibility to cut corners and save time
		file.seekg(16 + index*28*28);
		file.read((char*)buffer, 28*28);
		return true;
	}
	return false;
}

bool idx::good() {
	return file.good();
}
