#ifndef BITSTREAM_CPP
#define BITSTREAM_CPP
#endif

#pragma once
#include <string>
#include <cstdlib>
#define DEFAULT_BYTES_PER_LINE 4
#ifndef _WIN32
typedef char BitstreamByte;
typedef char* BitstreamByte_p;
#else
typedef unsigned char BitstreamByte;
typedef unsigned char* BitstreamByte_p;
#endif

typedef long long int int_l;

class bitstream {
private:
	BitstreamByte_p data = nullptr;
	int_l _size = 0;

public:

	// Get the size of the block of data
	int_l size();

	// Get the address of the first byte in the block of data
	BitstreamByte_p start();

	// Get the address of the first byte after the block of data.
	BitstreamByte_p end();

	// Delete all the data in the buffer
	bitstream& clear();

	// Serialize overload for a single element of data
	template <class Type>
	bitstream& serialize(Type data) {
		return serializeValue<Type>(data);
	}

	// Serialize overload for an array of elements of data
	template <class Type>
	bitstream& serialize(Type data[], int_l size) {
		return serializeArray<Type>(data, size);
	}

	bitstream& serialize_s(std::string data) {
		return serializeArray(data.c_str(), data.length() + 1); // Length + Null terminator
	}

	// // Serialize some value DEPRECATED
	template <class Val>
	bitstream& serializeValue(Val value) {
		int size = sizeof(value);

		// Create a buffer and allocate enough memory for it
		// Required because there is no dynamic memory allocation in the stack (easily)
		char* buffer = (char*)std::calloc(size, 1);

		// Begin serializing each byte
		for (int i = 0; i < size; i++) {
			BitstreamByte data = *((BitstreamByte_p)&value + i);

			buffer[i] = data;
		}

		// Now append the buffer to the bitstream data
		this->append(buffer, size);

		free(buffer);

		return *this;
	}

	// Serialize an array of some datatypes DEPRECATED
	template <class Val>
	bitstream& serializeArray(Val array[], int_l size) {
		for (int i = 0; i < size; i++) {
			BitstreamByte data = *((BitstreamByte_p)array + i);

			serializeValue(data);
		}

		return *this;
	}

	// Debug display the bitstream
	bitstream& display(unsigned short int = DEFAULT_BYTES_PER_LINE);

	// Constructor | Nullify data, prepare the class
	bitstream();
	bitstream(bitstream& stream);

	// Deconstructor | Free the block of data
	~bitstream();

	// Responsible for allocating space for data in the heap
	bitstream& append(char*, int_l);

	// Overloaded stream operators
	bitstream* operator<<(int);
	bitstream* operator<<(double);
	bitstream* operator<<(char);
	bitstream* operator<<(std::string);
	bitstream* operator<<(bool);
};