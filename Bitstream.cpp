#include "Bitstream.hpp"
#include <cstring>

// Constructor | Nullify data to start with. When it is needed, data will be allocated.
bitstream::bitstream() {
	this->data = nullptr;
	this->_size = 0;
}

bitstream::bitstream(bitstream& stream) {
	// Allocate space for the new data
	this->data = (BitstreamByte_p)malloc(stream.size());

	// Copy the provided bitstream data into the current bitstream
	if (this->data != nullptr) {
		memcpy(this->data, stream.start(), stream.size());
	}
}

// Deconstructor | Free all data
bitstream::~bitstream() {
	if (!this->data) {
		free(this->data);
	}
}

// Return the size of the block of data
int_l bitstream::size() {
	return this->_size;
}

// Return the beginning of the block of data
BitstreamByte_p bitstream::start() {
	return this->data;
}

// Return the end of the block of data
BitstreamByte_p bitstream::end() {
	// Check if data exists first
	if (!this->data)
		return nullptr;

	// If it exists, add size to the start pointer to get the end
	return (BitstreamByte_p)((int_l)this->start() + this->size());
}

// Free all the allocated data. There is no need to reallocate since it will be allocated once required
bitstream& bitstream::clear() {
	if (!this->data) {
		this->data = nullptr;

		return *this; // It's already clear.. but now it's still clear
	}

	// Free the memory
	free(this->data);
	this->data = nullptr;

	// Reset the size
	this->_size = 0;

	return *this;
}

// Append some number of bytes (chars) to the end of the data block
// Will call std::realloc in order to reallocate a new block. All data is in one block.
bitstream& bitstream::append(char* byteArray, int_l count) {
	// Find the current length of data before appending this byte
	int_l currentSize = this->size();
	int_l newSize = currentSize + count;

	// (re)allocate space for the data
	this->data = (BitstreamByte_p)std::realloc(this->data, newSize);
	BitstreamByte_p dataEnd = this->start() + this->size();

	// end() points to the first byte after data. We offset the pointer by the number of bytes we would like to append,
	/// or by the number of available bytes, if we are appending too many.
	//int offset = count - 1;//> currentSize ? currentSize : count;

	// bitstream::end() returns the byte after the block of data, so - 1 to offset
	// Buuut we don't need an additional offset, because the smallest amount of data that can be added is 1 byte, so it cannot be 0 anyway
	//byte* dataEnd = this->end() - offset;

	// Set the newly allocated byte to the given byte
	std::memcpy(dataEnd, byteArray, count);
	//*dataEnd = b;

	// Increment stored size of data
	this->_size = newSize;

	return *this;
}

bitstream& bitstream::display(unsigned short int width) {
	for (int i = 0; i < this->size(); i++) {
		int byteCol = i % width;
		bool firstByte = byteCol == 0;
		bool finalByte = byteCol == (width - 1);

		BitstreamByte Byte = this->data[i];
		if (firstByte)
			printf("0x%12llx:\t", (int_l)this->data + i);

		for (int b = 0; b < 8; b++) {
			bool byteBreak = (b % 4) == 3;

			int mask = 0x80 >> b;
			printf("%d", (Byte & mask) > 0 ? 1 : 0);

			// Space between every N bits
			if (byteBreak)
				printf(" ");
		}

		// Space between bytes
		printf(" ");

		// Check if this is the final byte in the sequence
		if (!finalByte && i < this->size() - 1)
			continue;

		// If this byte line was cut short, add enough spaces for the characters
		for (int b = 1; b < (width - byteCol); b++) {
			printf("%11c", ' ');
		}

		printf("\t|");
		// Print the characters in plaintext
		for (int c = 0; c < width; c++) {
			int index = i / width * width + c; // bro what

			if (index >= this->size()) {
				printf(" ");
				continue;
			}

			char printable = this->data[index];
			printable = printable < 0x20 ? '.' : printable;

			printf("%c", printable);
		}

		printf("|\n");
	}

	printf("\n");
	return *this;
}

// Append a string
// Append a string
bitstream* bitstream::operator<<(std::string data) {
	this->append((char*)data.c_str(), data.size());

	return this;
}

// Append an integer
bitstream* bitstream::operator<<(int data) {
	char dat[sizeof(int)];

	//Copy over data
	memcpy(dat, &data, sizeof(int));

	this->append(dat, sizeof(int));

	return this;
}

// Append a double
bitstream* bitstream::operator<<(double data) {
	char dat[sizeof(double)];

	//Copy over data
	memcpy(dat, &data, sizeof(double));

	this->append(dat, sizeof(double));

	return this;
}

// Append a char
bitstream* bitstream::operator<<(char data) {

	this->append(&data, sizeof(char));

	return this;
}

// Append a boolean
bitstream* bitstream::operator<<(bool data) {
	char dat[sizeof(bool)];

	//Copy over data
	memcpy(dat, &data, sizeof(bool));

	this->append(dat, sizeof(bool));

	return this;
}