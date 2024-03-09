#include "EndDevice.hpp"

bitstream EndDevice::send(int socket, bitstream& stream) {
    int n = ::send(socket, stream.start(), stream.size(), IPPROTO_TCP);

    if (RECV_ERR(n)) {
        fprintf(stderr, "Send Failure [%d]. I'm scared; disconnecting...", n);
        Protocol::logErr(n);
        Protocol::shutdownSocket(socket);

        exit(1);
    }

    return stream;
}

bitstream EndDevice::recv(int socket, int_l size) {
    byte* buffer2 = new byte[size];
    bitstream stream;

    // Receive the message from the server
    int n = ::recv(socket, buffer2, size, IPPROTO_TCP);

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