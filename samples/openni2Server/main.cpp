/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#include "saiga/openni2/RGBDCameraInput.h"
#include "boost/asio.hpp"

using namespace Saiga;


int main(int argc, char *argv[]) {
    boost::asio::io_service io_service;
    boost::asio::ip::udp::socket socket(io_service);
    boost::asio::ip::udp::endpoint remote_endpoint;
    socket.open(boost::asio::ip::udp::v4());
    remote_endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 9000);

    boost::system::error_code err;


    TemplatedImage<ucvec4> colorImg(480,640);
    while(true)
    {

//        auto buf = boost::asio::buffer(colorImg.data(), colorImg.size());
//        socket.receive_from(buf, remote_endpoint, 0, err);

        cout << "wait" << endl;


        std::array<char, 128> recv_buf;

            size_t len = socket.receive_from(
                boost::asio::buffer(recv_buf), remote_endpoint);

            std::cout.write(recv_buf.data(), len);



//        cout << (int)colorImg(50,50)[0] << endl;

    }

    socket.close();
}
