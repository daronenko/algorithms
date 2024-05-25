/*
 * Напишите две функции для создания архива из одного файла и извлечения файла из архива.
 * В архиве сохраняйте дерево Хаффмана и код Хаффмана от исходных данных. Дерево Хаффмана
 * требуется хранить эффективно - не более 10 бит на каждый 8-битный символ.
 */

#include "Huffman.h"

#include <iostream>
#include <vector>
#include <bitset>
#include <functional>
#include <unordered_map>
#include <stack>
#include <optional>

 #define DEBUG


using byte_t = unsigned char;

class BitsWriter {
    friend std::ostream& operator << (std::ostream& output, const BitsWriter& bits_buffer);

 public:
    BitsWriter(): bits_cursor_(0) {
    }

    void WriteBit(byte_t bit) {
        if (bits_cursor_ % 8 == 0) {
            buffer_.push_back(0);
        }

        if (bit) {
            buffer_[bits_cursor_ / 8] |= 1 << (7 - bits_cursor_ % 8);
        }

        ++bits_cursor_;
    }

    void WriteByte(byte_t byte) {
        if (bits_cursor_ % 8 == 0) {
            buffer_.push_back(byte);
        } else {
            std::size_t offset = bits_cursor_ % 8;
            buffer_[bits_cursor_ / 8] |= byte >> offset;
            buffer_.push_back(byte << (8 - offset));
        }

        bits_cursor_ += 8;
    }

    void Flush(IOutputStream& buffer) {
        std::cout << bits_cursor_ << std::endl;
        for (byte_t byte : buffer_) {
            buffer.Write(byte);
        }
    }

    [[nodiscard]] const std::vector<byte_t>& GetBuffer() const {
        return buffer_;
    }

    [[nodiscard]] std::size_t GetBitCount() const {
        return bits_cursor_;
    }

 private:
    std::vector<byte_t> buffer_;
    std::size_t bits_cursor_;
};

std::ostream& operator << (std::ostream& output, const BitsWriter& bits_buffer) {
    for (const auto& byte : bits_buffer.buffer_) {
        output << std::bitset<8>(byte) << ' ';
    }

    return output;
}


class BitsReader {
    friend std::ostream& operator << (std::ostream& output, const BitsReader& bits_buffer);

 public:
    explicit BitsReader(const std::vector<byte_t>& buffer)
            : buffer_(buffer), bits_cursor_(0) {
    }

    explicit BitsReader(const BitsWriter& writer)
            : buffer_(writer.GetBuffer()), bits_cursor_(0) {
        SetLastByteLen(writer.GetBitCount() % 8);
    }

    bool ReadBit(byte_t& bit) {
        std::size_t byte_index = bits_cursor_ / 8;
        std::size_t bit_offset = bits_cursor_ % 8;

        if (byte_index >= buffer_.size()) {
            return false;
        }

        if (last_byte_length_ > 0 && byte_index == buffer_.size() - 1 && bit_offset >= last_byte_length_) {
            return false;
        }

        byte_t byte = buffer_[byte_index];

        bit = (byte >> (7 - bit_offset)) & 1;
        ++bits_cursor_;

        return true;
    }

    bool ReadByte(byte_t& byte) {
        byte_t result_byte = 0;

        for (int i = 0; i < 8; ++i) {
            byte_t bit;
            if (ReadBit(bit)) {
                result_byte |= (bit << (7 - i));
            } else {
                if (i == 0) {
                    return false;
                } else {
                    break;
                }
            }
        }

        byte = result_byte;

        return true;
    }

    void SetLastByteLen(byte_t length) {
        last_byte_length_ = length;
    }

    [[nodiscard]] const std::vector<byte_t>& GetBuffer() const {
        return buffer_;
    }

    [[nodiscard]] std::size_t GetBitCount() const {
        return bits_cursor_;
    }

 private:
    std::vector<byte_t> buffer_;
    std::size_t bits_cursor_;
    std::size_t last_byte_length_{0};
};

std::ostream& operator << (std::ostream& output, const BitsReader& bits_buffer) {
    for (const auto& byte : bits_buffer.buffer_) {
        output << std::bitset<8>(byte) << ' ';
    }

    return output;
}


template <typename T, typename Comparator = std::less<T>>
class Heap {
 public:
    explicit Heap(Comparator compare = Comparator{})
            : capacity_(4), size_(0), buffer_(new T[capacity_]), compare_(compare) {
    }

    explicit Heap(std::size_t capacity, Comparator compare = Comparator{})
            : capacity_(capacity), size_(0), buffer_(new T[capacity]), compare_(compare) {
    }

    Heap& operator = (const Heap& rhs) {
        if (this == rhs) {
            return *this;
        }

        delete[] buffer_;
        size_ = rhs.size_;
        capacity_ = rhs.capacity_;
        buffer_ = new T[capacity_];
        std::copy(rhs.buffer_, rhs.buffer_ + size_, buffer_);

        return *this;
    }

    ~Heap() {
        delete[] buffer_;
    }

    [[nodiscard]] std::size_t Size() const noexcept {
        return size_;
    }

    [[nodiscard]] bool Empty() const noexcept {
        return !size_;
    }

    [[nodiscard]] const T& Top() const {
        return buffer_[0];
    }

    T& Top() {
        return buffer_[0];
    }

    void Pop() {
        if (Empty()) {
            throw std::length_error("cannot pop content from empty heap");
        }

        std::swap(buffer_[0], buffer_[--size_]);
        (buffer_ + size_)->~T();
        SiftDown_(0);
    }

    void Push(const T& value) {
        if (size_ == capacity_) {
            Reserve_(2 * capacity_);
        }

        buffer_[size_++] = value;
        SiftUp_(size_ - 1);
    }

    void Push(T&& value) {
        if (size_ == capacity_) {
            Reserve_(2 * capacity_);
        }

        buffer_[size_++] = std::move(value);
        SiftUp_(size_ - 1);
    }

 private:
    void Reserve_(std::size_t new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }

        T* new_buffer = new T[new_capacity];
        for (std::size_t i = 0; i < Size(); ++i) {
            new_buffer[i] = buffer_[i];
        }

        delete[] buffer_;
        buffer_ = new_buffer;
        capacity_ = new_capacity;
    }

    void SiftUp_(std::size_t current_node) {
        std::size_t parent_node = (current_node - 1) / 2;
        while (current_node > 0 && compare_(buffer_[current_node], buffer_[parent_node])) {
            std::swap(buffer_[current_node], buffer_[parent_node]);
            current_node = parent_node;
            parent_node = (current_node - 1) / 2;
        }
    }

    void SiftDown_(std::size_t current_node) {
        while (2 * current_node + 1 < size_) {
            std::size_t left_child = 2 * current_node + 1;
            std::size_t right_child = 2 * current_node + 2;

            std::size_t target_node = left_child;
            if (right_child < size_ && compare_(buffer_[right_child], buffer_[left_child])) {
                target_node = right_child;
            }

            if (compare_(buffer_[current_node], buffer_[target_node])) {
                break;
            }

            std::swap(buffer_[current_node], buffer_[target_node]);
            current_node = target_node;
        }
    }

    std::size_t capacity_;
    std::size_t size_;
    T* buffer_{nullptr};

    Comparator compare_;
};


std::vector<byte_t> GetBytes(IInputStream& input) {
    std::vector<byte_t> bytes;

    byte_t byte;
    while (input.Read(byte)) {
        bytes.push_back(byte);
    }

    return bytes;
}


namespace huffman {

struct Node {
    Node()
            : left(nullptr), right(nullptr) {
    }

    explicit Node(byte_t value)
            : content(value), left(nullptr), right(nullptr) {
    }

    Node(byte_t value, Node *left, Node *right)
            : content(value), left(left), right(right) {
    }

    Node(Node *left, Node *right, std::size_t frequency)
            : left(left), right(right), frequency(frequency) {
    }

    Node(Node *left, Node *right)
            : left(left), right(right) {
    }

    Node(byte_t value, Node *left, Node *right, std::size_t frequency)
            : content(value), left(left), right(right), frequency(frequency) {
    }

    Node(const Node &other) {
        content = other.content;
        frequency = other.frequency;

        if (other.left) {
            left = new Node(*other.left);
        } else {
            left = nullptr;
        }

        if (other.right) {
            right = new Node(*other.right);
        } else {
            right = nullptr;
        }
    }

    Node(Node &&other) {
        content = other.content;
        frequency = other.frequency;

        left = other.left;
        other.left = nullptr;

        right = other.right;
        other.right = nullptr;
    }

    Node &operator=(const Node &other) {
        if (this == &other) {
            return *this;
        }

        content = other.content;
        frequency = other.frequency;

        if (other.left) {
            left = new Node(*other.left);
        } else {
            left = nullptr;
        }

        if (other.right) {
            right = new Node(*other.right);
        } else {
            right = nullptr;
        }

        return *this;
    }

    Node &operator=(Node &&other) noexcept {
        if (this == &other) {
            return *this;
        }

        content = other.content;
        frequency = other.frequency;

        left = other.left;
        other.left = nullptr;

        right = other.right;
        other.right = nullptr;

        return *this;
    }

    ~Node() {
        delete left;
        delete right;
    }

    std::optional<byte_t> content;
    Node *left;
    Node *right;
    std::optional<std::size_t> frequency;
    std::optional<bool> code;
};

bool operator<(const Node &lhs, const Node &rhs) {
    return lhs.frequency < rhs.frequency;
}

std::unordered_map<byte_t, std::size_t> CountFrequency(BitsReader &bits_reader) {
    std::unordered_map<byte_t, std::size_t> frequencies;

    byte_t byte;
    while (bits_reader.ReadByte(byte)) {
        frequencies[byte] += 1;
    }

    return frequencies;
}

void FlushByte(byte_t byte, const std::unordered_map<byte_t, std::vector<bool>> &table, BitsWriter &buffer) {
    for (byte_t bit: table.at(byte)) {
        buffer.WriteBit(bit);
    }
}

void FlushTree(Node *root, BitsWriter &buffer) {
    if (!root) {
        return;
    }

    if (root->content.has_value()) {
        buffer.WriteBit(1);
        buffer.WriteByte(root->content.value());
    } else {
        buffer.WriteBit(0);
        FlushTree(root->left, buffer);
        FlushTree(root->right, buffer);
    }
}

void GetTableInternal(
        huffman::Node *root, std::unordered_map<byte_t, std::vector<bool>> &table,
        std::vector<bool> &current_path
) {
    if (!root) {
        return;
    }

    if (!root->left && !root->right) {
        table[root->content.value()] = current_path;
        return;
    }

    current_path.push_back(0);
    GetTableInternal(root->left, table, current_path);
    current_path.pop_back();

    current_path.push_back(1);
    GetTableInternal(root->right, table, current_path);
    current_path.pop_back();
}

std::unordered_map<byte_t, std::vector<bool>> GetTable(huffman::Node *root) {
    std::unordered_map<byte_t, std::vector<bool>> table;

    if (!root->left && !root->right) {
        table[root->content.value()] = {0};
        return table;
    }

    std::vector<bool> current_path;
    GetTableInternal(root, table, current_path);
    return table;
}

Node *BuildTree(Heap<Node> &heap) {
    while (heap.Size() > 1) {
        auto node_1 = new Node(std::move(heap.Top()));
        heap.Pop();

        auto node_2 = new Node(std::move(heap.Top()));
        heap.Pop();

        auto new_node = Node(node_1, node_2, node_1->frequency.value() + node_2->frequency.value());
        heap.Push(new_node);
    }

    auto tree = new Node(std::move(heap.Top()));
    heap.Pop();

    return tree;
}

Node *ReadTree(BitsReader &reader) {
    byte_t bit, byte;
    reader.ReadBit(bit);
    if (bit) {
        reader.ReadByte(byte);
        auto node = new Node(byte);
        return node;
    } else {
        auto left_node = ReadTree(reader);
        auto right_node = ReadTree(reader);
        return new Node(left_node, right_node);
    }
}

void DecodeData(Node *tree, BitsReader &reader, BitsWriter &buffer) {
    byte_t bit;
    Node *current_node = tree;
    while (reader.ReadBit(bit)) {
        if (bit && current_node->right) {
            current_node = current_node->right;
        }

        if (!bit && current_node->left) {
            current_node = current_node->left;
        }

        if (!current_node->left && !current_node->right) {
            buffer.WriteByte(current_node->content.value());
            current_node = tree;
        }
    }
}

void FlushLastByteLen(byte_t last_byte_length, BitsWriter &buffer) {
    for (byte_t i = 0; i < 3; ++i) {
        buffer.WriteBit((last_byte_length >> (2 - i)) & 1);
    }
}

byte_t ReadLastByteLen(BitsReader &reader) {
    byte_t bit;

    byte_t last_byte_length = 0;
    for (std::size_t i = 0; i < 3; ++i) {
        reader.ReadBit(bit);
        last_byte_length |= (bit << (2 - i));
    }

    return last_byte_length;
}

byte_t CalculateLastByteLen(
        const std::unordered_map<byte_t, std::vector<bool>> &code_table,
        const std::unordered_map<byte_t, std::size_t> &frequencies
) {
    std::size_t data_section_length = 0;
    for (const auto &[key, code]: code_table) {
        data_section_length += code.size() * frequencies.at(key);
    }

    return (10 * code_table.size() - 1 + 3 + data_section_length) % 8;
}

Heap<Node> BuildHeap(const std::unordered_map<byte_t, std::size_t> &frequencies) {
    Heap<huffman::Node> heap;

    for (auto [key, frequency]: frequencies) {
        auto new_node = huffman::Node(key, nullptr, nullptr, frequency);
        heap.Push(std::move(new_node));
    }

    return heap;
}

void CompressData(
        BitsReader &reader,
        const std::unordered_map<byte_t, std::vector<bool>> &code_table,
        BitsWriter &buffer
) {
    byte_t byte;
    while (reader.ReadByte(byte)) {
        huffman::FlushByte(byte, code_table, buffer);
    }
}

}


void Encode(IInputStream& original, IOutputStream& compressed) {
    const auto bytes = GetBytes(original);

    BitsReader frequency_reader(bytes);
    const auto frequencies = huffman::CountFrequency(frequency_reader);

    auto heap = huffman::BuildHeap(frequencies);
    const auto tree = huffman::BuildTree(heap);
    const auto code_table = huffman::GetTable(tree);
    const auto last_byte_length = huffman::CalculateLastByteLen(code_table, frequencies);

    BitsWriter buffer;
    huffman::FlushTree(tree, buffer);
    huffman::FlushLastByteLen(last_byte_length, buffer);

    BitsReader original_reader(bytes);
    huffman::CompressData(original_reader, code_table, buffer);

    buffer.Flush(compressed);

    std::cout << "Compressed:\t\t" << buffer << std::endl;

    delete tree;
}


void Decode(IInputStream& compressed, IOutputStream& original) {
    const auto bytes = GetBytes(compressed);

    BitsReader compressed_reader(bytes);
    const auto tree = huffman::ReadTree(compressed_reader);

    const auto last_byte_length = huffman::ReadLastByteLen(compressed_reader);
    compressed_reader.SetLastByteLen(last_byte_length);

    BitsWriter buffer;
    huffman::DecodeData(tree, compressed_reader, buffer);
    buffer.Flush(original);

    delete tree;
}


#ifdef DEBUG

#include <cassert>

namespace tests {

void TestCompression() {
    {
        auto original_buffer = std::vector<byte_t>{'a', 'b', 'r', 'a', 'c', 'a', 'd', 'a', 'b', 'r', 'a'};
        VectorStream original(original_buffer);
        VectorStream compressed;

        Encode(original, compressed);

        VectorStream uncompressed;
        Decode(compressed, uncompressed);

        assert(original == uncompressed);
    }
    {
        auto original_buffer = std::vector<byte_t>{'a'};
        VectorStream original(original_buffer);
        VectorStream compressed;

        Encode(original, compressed);

        VectorStream uncompressed;
        Decode(compressed, uncompressed);

        assert(original == uncompressed);
    }
    {
        auto original_buffer = std::vector<byte_t>{'I', 'E', 'E', 'E', 'C', 'O', 'M', 'P', 'U', 'T', 'A', 'T', 'I', 'O', 'N', 'A', 'L', 'I', 'N', 'T', 'E', 'L', 'L', 'I', 'G', 'E', 'N', 'C', 'E'};
        VectorStream original(original_buffer);
        VectorStream compressed;

        Encode(original, compressed);

        VectorStream uncompressed;
        Decode(compressed, uncompressed);

        assert(original == uncompressed);
    }
}

void Run() {
    tests::TestCompression();
}

}

int main() {
    tests::Run();
}

#endif
