#include "EndDevice.hpp"

bitstream EndDevice::send(int socket, bitstream& stream) {
	int n = ::send(socket, (char*)stream.start(), stream.size(), IPPROTO_TCP);

	if (RECV_ERR(n)) {
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