#pragma once

#include <vector>
#include <string>

#include "Packet/BinaryReader.hpp"
#include "Packet/BinaryWriter.hpp"

class IMessage {
    public:
        virtual ushort getId() = 0;
        virtual std::string getName() = 0;
        virtual void serialize(BinaryWriter &writer) = 0;
        virtual void deserialize(BinaryReader &reader) = 0;
};