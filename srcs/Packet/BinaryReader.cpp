#include "Packet/BinaryReader.hpp"

BinaryReader::BinaryReader(const char* buffer, int length) :
	_buffer(buffer, buffer + length),
	_index(0) { }

BinaryReader::BinaryReader(const ByteArray buffer) :
	_buffer(buffer),
	_index(0) { }

BinaryReader::BinaryReader(const std::deque<char> buffer) :
	_index(0) {
	_buffer.reserve(buffer.size());
	std::copy(buffer.begin(), buffer.end(), std::back_inserter(_buffer));
}

bool BinaryReader::setIndex(uint index) {
	if (index <= _buffer.size()) {
		_index = index;
		return true;
	}
	return false;
}

unsigned int BinaryReader::index() {
	return _index;
}

uint BinaryReader::bytesAvailable() {
	return static_cast<int>(_buffer.size() - _index);
}

void BinaryReader::reverseBytes(uchar* bytes, size_t count) {
	for (size_t lo = 0, hi = count - 1; hi > lo; ++lo, --hi) {
		auto temp = bytes[hi];
		bytes[hi] = bytes[lo];
		bytes[lo] = temp;
	}
}

void BinaryReader::readBytes(uchar* bytes, size_t count) {
	memcpy(bytes, &_buffer[_index], count);
	_index += count;
}

short BinaryReader::readShort() {
	short value = 0;
	read(value);
	return value;
}

ushort BinaryReader::readUShort() {
	ushort value = 0;
	read(value);
	return value;
}

int BinaryReader::readInt() {
	int value = 0;
	read(value);
	return value;
}

uint BinaryReader::readUInt() {
	uint value = 0;
	read(value);
	return value;
}

long BinaryReader::readLong() {
	long value = 0;
	read(value);
	return value;
}

ulong BinaryReader::readULong() {
	ulong value = 0;
	read(value);
	return value;
}

char BinaryReader::readByte() {
	char value = _buffer[_index];

	_index++;
	return value;
}

uchar BinaryReader::readUByte() {
	uchar value = static_cast<uchar>(_buffer[_index]);

	_index++;
	return value;
}

ByteArray BinaryReader::readBytes() {
	int length = readUShort();

	ByteArray value;

	for (int i = 0; i < length; i++)
		value.push_back(_buffer[_index + i]);

	value.push_back(0);
	_index += length;
	return value;
}

ByteArray BinaryReader::readBytes(int length) {
	std::vector<char> value;

	for (int i = 0; i < length; i++)
		value.push_back(_buffer[_index + i]);
	_index += length;
	return value;
}

std::string BinaryReader::readUTF() {
	ushort len = readUShort();
	std::vector<char> data = readBytes(len);
	return std::string(data.begin(), data.end());
}

std::string BinaryReader::readUTFBytes(uint length) {
	ByteArray data = readBytes(length);
	return std::string(data.begin(), data.end());
}

bool BinaryReader::readBool() {
	return readByte() == 1;
}

template<class T>
void BinaryReader::read(T &val) {
	readBytes(reinterpret_cast<uchar*>(&val), sizeof(T));
	reverseBytes(reinterpret_cast<uchar*>(&val), sizeof(T));
}

template<class T>
void BinaryReader::read(std::vector<T> &val) {
	uint16_t size;
	read(size);
	val.resize(size);
	readBytes(reinterpret_cast<uint8_t*>(&val[0]), size);
}