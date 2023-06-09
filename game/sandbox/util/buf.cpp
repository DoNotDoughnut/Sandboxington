/**
 ByteBuffer
 ByteBuffer.cpp
 Copyright 2011 - 2013 Ramsey Kant

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 Modfied 2015 by Ashley Davis (SgtCoDFish)
 */

#include "buf.h"

#ifdef BB_USE_NS
namespace bb {
#endif

/**
 * ByteBuffer constructor
 * Reserves specified size in internal vector
 *
 * @param size Size (in bytes) of space to preallocate internally. Default is set in DEFAULT_SIZE
 */
ByteBuffer::ByteBuffer(size_t size) {
    buf.reserve(size);
    clear();
#ifdef BB_UTILITY
    name = "";
#endif
}

/**
 * ByteBuffer constructor
 * Consume an entire uint8_t array of length len in the ByteBuffer
 *
 * @param arr uint8_t array of data (should be of length len)
 * @param size Size of space to allocate
 */
ByteBuffer::ByteBuffer(uint8_t* arr, size_t size) {
    // If the provided array is NULL, allocate a blank buffer of the provided size
    if (arr == NULL) {
        buf.reserve(size);
        clear();
    } else { // Consume the provided array
        buf.reserve(size);
        clear();
        putBytes(arr, size);
    }

#ifdef BB_UTILITY
    name = "";
#endif
}

/**
 * Bytes Remaining
 * Returns the number of bytes from the current read position till the end of the buffer
 *
 * @return Number of bytes from rpos to the end (size())
 */
size_t ByteBuffer::bytesRemaining() {
    return size() - rpos;
}

/**
 * Clear
 * Clears out all data from the internal vector (original preallocated size remains), resets the positions to 0
 */
void ByteBuffer::clear() {
    rpos = 0;
    wpos = 0;
    buf.clear();
}

/**
 * Clone
 * Allocate an exact copy of the ByteBuffer on the heap and return a pointer
 *
 * @return A pointer to the newly cloned ByteBuffer. NULL if no more memory available
 */
std::unique_ptr<ByteBuffer> ByteBuffer::clone() {
    std::unique_ptr<ByteBuffer> ret = std::make_unique<ByteBuffer>(buf.size());

    // Copy data
    for (size_t i = 0; i < buf.size(); i++) {
        ret->putByte((uint8_t) getByteAt(i));
    }

    // Reset positions
    ret->setReadPos(0);
    ret->setWritePos(0);

    return ret;
}

/**
 * Equals, test for data equivilancy
 * Compare this ByteBuffer to another by looking at each byte in the internal buffers and making sure they are the same
 *
 * @param other A pointer to a ByteBuffer to compare to this one
 * @return True if the internal buffers match. False if otherwise
 */
bool ByteBuffer::equals(ByteBuffer* other) {
    // If sizes aren't equal, they can't be equal
    if (size() != other->size())
        return false;

    // Compare byte by byte
    size_t len = size();
    for (size_t i = 0; i < len; i++) {
        if ((uint8_t) getByteAt(i) != (uint8_t) other->getByteAt(i))
            return false;
    }

    return true;
}

/**
 * Resize
 * Reallocates memory for the internal buffer of size newSize. Read and write positions will also be reset
 *
 * @param newSize The amount of memory to allocate
 */
void ByteBuffer::resize(size_t newSize) {
    buf.resize(newSize);
    rpos = 0;
    wpos = 0;
}

/**
 * Size
 * Returns the size of the internal buffer...not necessarily the length of bytes used as data!
 *
 * @return size of the internal buffer
 */
size_t ByteBuffer::size() {
    return buf.size();
}

// Replacement

/**
 * Replace
 * Replace occurance of a particular uint8_t, key, with the uint8_t rep
 *
 * @param key uint8_t to find for replacement
 * @param rep uint8_t to replace the found key with
 * @param start Index to start from. By default, start is 0
 * @param firstOccuranceOnly If true, only replace the first occurance of the key. If false, replace all occurances. False by default
 */
void ByteBuffer::replace(uint8_t key, uint8_t rep, size_t start, bool firstOccuranceOnly) {
    size_t len = buf.size();
    for (size_t i = start; i < len; i++) {
        uint8_t data = read<uint8_t>(i);
        // Wasn't actually found, bounds of buffer were exceeded
        if ((key != 0) && (data == 0))
            break;

        // Key was found in array, perform replacement
        if (data == key) {
            buf[i] = rep;
            if (firstOccuranceOnly)
                return;
        }
    }
}

// Read Functions

uint8_t ByteBuffer::peek() const {
    return read<uint8_t>(rpos);
}

uint8_t ByteBuffer::getByte() const {
    return read<uint8_t>();
}

uint8_t ByteBuffer::getByteAt(size_t index) const {
    return read<uint8_t>(index);
}

void ByteBuffer::getBytes(uint8_t* buf, size_t len) const {
    for (size_t i = 0; i < len; i++) {
        buf[i] = read<uint8_t>();
    }
}

char ByteBuffer::getChar() const {
    return read<char>();
}

char ByteBuffer::getChar(size_t index) const {
    return read<char>(index);
}

double ByteBuffer::getDouble() const {
    return read<double>();
}

double ByteBuffer::getDouble(size_t index) const {
    return read<double>(index);
}

float ByteBuffer::getFloat() const {
    return read<float>();
}

float ByteBuffer::getFloat(size_t index) const {
    return read<float>(index);
}

uint32_t ByteBuffer::getUInt() const {
    return read<uint32_t>();
}

uint32_t ByteBuffer::getUInt(size_t index) const {
    return read<uint32_t>(index);
}

int32_t ByteBuffer::getSInt() const {
    return read<int32_t>();
}

int32_t ByteBuffer::getSInt(size_t index) const {
    return read<int32_t>(index);
}

uint64_t ByteBuffer::getLong() const {
    return read<uint64_t>();
}

uint64_t ByteBuffer::getLong(size_t index) const {
    return read<uint64_t>(index);
}

uint16_t ByteBuffer::getShort() const {
    return read<uint16_t>();
}

uint16_t ByteBuffer::getShort(size_t index) const {
    return read<uint16_t>(index);
}

// Write Functions

//template <typename T>
//void ByteBuffer::put(const T &value) {
//    append<T>(value);
//}

void ByteBuffer::putBuf(ByteBuffer* src) {
    size_t len = src->size();
    for (size_t i = 0; i < len; i++)
        append<uint8_t>(src->getByteAt(i));
}

void ByteBuffer::putByte(uint8_t b) {
    append<uint8_t>(b);
}

void ByteBuffer::putByteAt(uint8_t b, size_t index) {
    insert<uint8_t>(b, index);
}

void ByteBuffer::putBytes(uint8_t* b, size_t len) {
    // Insert the data one byte at a time into the internal buffer at position i+starting index
    for (size_t i = 0; i < len; i++)
        append<uint8_t>(b[i]);
}

void ByteBuffer::putBytes(uint8_t* b, size_t len, size_t index) {
    wpos = index;

    // Insert the data one byte at a time into the internal buffer at position i+starting index
    for (size_t i = 0; i < len; i++)
        append<uint8_t>(b[i]);
}

void ByteBuffer::putChar(char value) {
    append<char>(value);
}

void ByteBuffer::putChar(char value, size_t index) {
    insert<char>(value, index);
}

void ByteBuffer::putDouble(double value) {
    append<double>(value);
}

void ByteBuffer::putDouble(double value, size_t index) {
    insert<double>(value, index);
}

void ByteBuffer::putFloat(float value) {
    append<float>(value);
}


void ByteBuffer::putVec3(const glm::vec3 &value) {
    putFloat(value.x);
    putFloat(value.y);
    putFloat(value.z);
}

void ByteBuffer::putIVec3(const glm::ivec3 &value) {
    putSInt(value.x);
    putSInt(value.y);
    putSInt(value.z);
}

void ByteBuffer::putU8Vec3(const glm::u8vec3 &value) {
    putByte(value.x);
    putByte(value.y);
    putByte(value.z);
}

void ByteBuffer::putFloat(float value, size_t index) {
    insert<float>(value, index);
}

void ByteBuffer::putUInt(uint32_t value) {
    append<uint32_t>(value);
}

void ByteBuffer::putUInt(uint32_t value, size_t index) {
    insert<uint32_t>(value, index);
}

void ByteBuffer::putSInt(int32_t value) {
    append<int32_t>(value);
}

void ByteBuffer::putSInt(int32_t value, size_t index) {
    insert<int32_t>(value, index);
}

void ByteBuffer::putLong(uint64_t value) {
    append<uint64_t>(value);
}

void ByteBuffer::putLong(uint64_t value, size_t index) {
    insert<uint64_t>(value, index);
}

void ByteBuffer::putShort(uint16_t value) {
    append<uint16_t>(value);
}

void ByteBuffer::putShort(uint16_t value, size_t index) {
    insert<uint16_t>(value, index);
}

// Utility Functions
#ifdef BB_UTILITY
void ByteBuffer::setName(std::string n) {
	name = n;
}

std::string ByteBuffer::getName() {
	return name;
}

void ByteBuffer::printInfo() {
	size_t length = buf.size();
	std::cout << "ByteBuffer " << name.c_str() << " Length: " << length << ". Info Print" << std::endl;
}

void ByteBuffer::printAH() {
	size_t length = buf.size();
	std::cout << "ByteBuffer " << name.c_str() << " Length: " << length << ". ASCII & Hex Print" << std::endl;

	for (size_t i = 0; i < length; i++) {
		std::printf("0x%02x ", buf[i]);
	}

	std::printf("\n");
	for (size_t i = 0; i < length; i++) {
		std::printf("%c ", buf[i]);
	}

	std::printf("\n");
}

void ByteBuffer::printAscii() {
	size_t length = buf.size();
	std::cout << "ByteBuffer " << name.c_str() << " Length: " << length << ". ASCII Print" << std::endl;

	for (size_t i = 0; i < length; i++) {
		std::printf("%c ", buf[i]);
	}

	std::printf("\n");
}

void ByteBuffer::printHex() {
	size_t length = buf.size();
	std::cout << "ByteBuffer " << name.c_str() << " Length: " << length << ". Hex Print" << std::endl;

	for (size_t i = 0; i < length; i++) {
		std::printf("0x%02x ", buf[i]);
	}

	std::printf("\n");
}

void ByteBuffer::printPosition() {
	size_t length = buf.size();
	std::cout << "ByteBuffer " << name.c_str() << " Length: " << length << " Read Pos: " << rpos << ". Write Pos: "
	        << wpos << std::endl;
}

#ifdef BB_USE_NS
}
#endif

#endif
