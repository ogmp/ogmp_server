#include "reply.hpp"

namespace http {
namespace server {

namespace misc_strings {

const char name_value_separator[] = { ':', ' ' };
const char crlf[] = { '\r', '\n' };

} // namespace misc_strings

std::vector<boost::asio::const_buffer> reply::to_buffers() {
	std::vector<boost::asio::const_buffer> buffers;
	buffers.push_back(boost::asio::buffer(buffer));
	return buffers;
}

void reply::add_to_buffers(char* content, int size){
	// cout << "array size " << size << endl;
	for(int i = 0; i < size; i++){
		buffer.push_back(content[i]);
	}
}

void reply::add_to_buffers(request_handler::message_type content){
	buffer.push_back(content);
}

void reply::add_plain_text(std::string content){
	const char* byte_array = content.c_str();
	for(int i = 0; i < strlen(byte_array); i++){
		buffer.push_back(content[i]);
	}
}

void reply::add_to_buffers(char content){
	buffer.push_back(content);
}

void reply::add_to_buffers(float content){
	// std::cout << "Trying to add a float to the buffer " << content << std::endl;
	floatToByteArray(content);
}

void reply::add_to_buffers(std::string content){
	// std::cout << "Trying to add a string to the buffer " << content << std::endl;
	const char* byte_array = content.c_str();
	//The size of the string is send first.
	buffer.push_back(strlen(byte_array));
	for(int i = 0; i < strlen(byte_array); i++){
		// std::cout << (int)content[i] << " ";
		buffer.push_back(content[i]);
	}
	// std::cout << endl;
}

void reply::add_to_buffers(bool content){
	// std::cout << "Trying to add a boolean to the buffer " << content << std::endl;
	if(content){
		buffer.push_back(1);
	}else{
		buffer.push_back(0);
	}
}

void reply::add_to_buffers(int content){
	// std::cout << "Trying to add a integer to the buffer " << content << std::endl;
	buffer.push_back(content);
}

void reply::floatToByteArray(float f) {
	for(int i = 0; i < 4; i++){
		buffer.push_back(((char*)&f)[i]);
	}
}

void reply::add_size_byte(){
	vector<unsigned char> size = intToByteArray(buffer.size());
	buffer.insert(buffer.begin(), size.begin(), size.end());
}

vector<unsigned char> reply::intToByteArray(int value){
	vector<unsigned char> return_bytes(4);
	for (int i = 0; i < 4; i++){
		return_bytes[3 - i] = (value >> (i * 8));
	}
	return return_bytes;
}

} // namespace server
} // namespace http
