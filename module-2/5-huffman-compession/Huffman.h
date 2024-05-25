#pragma once

#include <vector>


using byte_t = unsigned char;


class IInputStream {
 public:
    virtual bool Read(byte_t& value) = 0;
};

class IOutputStream {
 public:
    virtual void Write(byte_t value) = 0;
};


class VectorStream : public IInputStream, public IOutputStream {
    friend bool operator == (const VectorStream& lhs, const VectorStream& rhs);

 public:
    VectorStream(): cursor_(0) {
    }

    explicit VectorStream(const std::vector<byte_t>& buffer): buffer_(buffer), cursor_(0) {
    }

    bool Read(byte_t& value) final {
        if (cursor_ >= buffer_.size()) {
            return false;
        }

        value = buffer_[cursor_++];
        return true;
    }

    void Write(byte_t value) final {
        buffer_.push_back(value);
    }

 private:
    std::vector<byte_t> buffer_;
    std::size_t cursor_;
};

bool operator == (const VectorStream& lhs, const VectorStream& rhs) {
    return lhs.buffer_ == rhs.buffer_;
}


void Encode(IInputStream& original, IOutputStream& compressed);

void Decode(IInputStream& compressed, IOutputStream& original);
