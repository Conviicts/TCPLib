#include "Packet/BinaryWriter.hpp"
#include "Packet/BinaryReader.hpp"

BinaryWriter::BinaryWriter(ByteArray& buffer) :
	_index(0),
	_buffer(buffer) { }

unsigned int BinaryWriter::index() {
	return _index;
}

void BinaryWriter::setIndex(uint index) {
	_index = index;
}

void BinaryWriter::writeByte(char data) {
	write(data);
}

void BinaryWriter::writeBytes(ByteArray data, bool writeSize) {
	write(data, writeSize);
}

void BinaryWriter::writeShort(short data) {
	write(data);
}

void BinaryWriter::writeUShort(ushort data) {
	write(data);
}

void BinaryWriter::writeUnsignedShort(ushort data) {
	write(data);
}

void BinaryWriter::writeInt(int data) {
	write(data);
}

void BinaryWriter::writeUInt(uint data) {
	write(data);
}

void BinaryWriter::writeUnsignedInt(uint data) {
	write(data);
}

void BinaryWriter::writeLong(long data) {
	write(data);
}

void BinaryWriter::writeULong(ulong data) {
	write(data);
}

void BinaryWriter::writeUnsignedLong(ulong data) {
	write(data);
}

void BinaryWriter::writeDouble(double data) {
	write(data);
}

void BinaryWriter::writeUTF(const char* data) {
	size_t length = strlen(data);
	writeUTFBytes(data, length);
}

void BinaryWriter::writeUTF(std::string data) {
	std::vector<char> vec(data.begin(), data.end());
	write(vec);
}

void BinaryWriter::writeUTFBytes(const char* data, size_t length) {
	std::string str(data, length);
	writeUTF(str);
}

void BinaryWriter::writeBool(bool data) {
	if (data)
		writeByte(1);
	else
		writeByte(0);
}

void BinaryWriter::writeVarInt(int data) {
	std::vector<char> byteArray;

	if ((data >= 0) && (data <= MASK_01111111)) {
		byteArray.push_back(static_cast<char>(data));
		writeBytes(byteArray);
		return;
	}

	int c = data;
	char byte;

	while (c != 0) {
		byte = static_cast<char>(c & MASK_01111111);
		c = static_cast<uint>(c >> CHUNK_BIT_SIZE);

		if (c > 0)
			byte = byte | MASK_10000000;
		writeByte(byte);
	}
}

void BinaryWriter::writeVarShort(short data) {
	if ((data > SHORT_MAX_VALUE) || (data < SHORT_MIN_VALUE)) {
		throw std::logic_error("Forbidden value");
	}

	std::vector<char> byteArray;

	if ((data >= 0) && (data <= MASK_01111111)) {
		writeByte(static_cast<char>(data));
		return;
	}

	int c = data & 0xFFFF;
	char byte;

	while (c != 0) {
		byte = static_cast<char>(c & MASK_01111111);
		c = static_cast<uint>(c >> CHUNK_BIT_SIZE);

		if (c > 0)
			byte = byte | MASK_10000000;
		writeByte(byte);
	}
}

void BinaryWriter::writeInt32(uint data) {
	while (true) {
		if (data < MASK_10000000) {
			writeByte(static_cast<char>(data));
			return;
		}
		writeByte(static_cast<char>((data & MASK_01111111) | MASK_10000000));
		data = static_cast<uint>(data >> 7);
	}
}

template<class T>
void BinaryWriter::write(const T& data) {
	const char* bytes = reinterpret_cast<const char*>(&data);
	ushort size = static_cast<ushort>(sizeof(T));

	if (bytes == nullptr || size == 0)
		return;
	if (_buffer.size() < _index + size)
		_buffer.resize(_index + size);
	memcpy(&_buffer[_index], bytes, size);
	_index += size;
}

template<class T>
void BinaryWriter::write(const std::vector<T>& data, bool writeSize) {
	const char* bytes = reinterpret_cast<const char*>(&data[0]);
	ushort size = static_cast<ushort>(data.size());

	if (writeSize)
		writeUShort(size);
	if (bytes == nullptr || size == 0)
		return;
	if (_buffer.size() < _index + size)
		_buffer.resize(_index + size);
	memcpy(&_buffer[_index], bytes, size);
	_index += size;
}