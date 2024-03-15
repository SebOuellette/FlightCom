#include "EndDevice.hpp"

bitstream& EndDevice::send(int socket, bitstream& stream) {
	int n = ::send(socket, (char*)stream.start(), stream.size(), 0);

	if (RECV_ERR(n)) {
		//Get the error message ID, if any.
		DWORD errorMessageID = ::GetLastError();
		if (errorMessageID == 0) {
			return stream; //No error message has been recorded
		}

		LPSTR messageBuffer = nullptr;

		//Ask Win32 to give us the string version of that message ID.
		//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

		//Copy the error message into a std::string.
		std::string message(messageBuffer, size);

		//Free the Win32's string's buffer.
		LocalFree(messageBuffer);

		std::cout << "Failure: " << message << std::endl;

		fprintf(stderr, "Send Failure [%d]. I'm scared; disconnecting...", n);
		Protocol::logErr(n);
		Protocol::shutdownSocket(socket);

		

		exit(1);
	}

	return stream;
}

bitstream EndDevice::recv(int socket, int_l size) {
	BitstreamByte_p buffer2 = new BitstreamByte[size];
	bitstream stream;

	// Receive the message from the server
	int n = ::recv(socket, (char*)buffer2, size, IPPROTO_TCP);

	if (RECV_ERR(n)) {
		fprintf(stderr, "Receive Failure [%d]. I'm scared; disconnecting...", n);
		Protocol::logErr(n);
		Protocol::shutdownSocket(socket);

		exit(1);
	}

	// Display the message
	stream.serialize(buffer2, n);

	delete[] buffer2;

	return stream;
}

EndDevice::EndDevice() {
}