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
#include <tuple>
#include <cassert>

 #define DEBUG


using byte_t = unsigned char;

namespace std {

namespace {

template <class T>
inline void hash_combine(std::size_t& seed, T const& v) {
    seed ^= hash<T>()(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
struct HashValueImpl {
    static void apply(size_t& seed, Tuple const& tuple) {
        HashValueImpl<Tuple, Index-1>::apply(seed, tuple);
        hash_combine(seed, get<Index>(tuple));
    }
};

template <class Tuple>
struct HashValueImpl<Tuple,0> {
    static void apply(size_t& seed, Tuple const& tuple) {
        hash_combine(seed, get<0>(tuple));
    }
};

}

template <typename ... TT>
struct hash<std::tuple<TT...>> {
    size_t operator()(std::tuple<TT...> const& tt) const {
        size_t seed = 0;
        HashValueImpl<std::tuple<TT...> >::apply(seed, tt);
        return seed;
    }
};

}

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

    explicit Node(std::tuple<byte_t, byte_t, byte_t> value)
            : content(value), left(nullptr), right(nullptr) {
    }

    Node(std::tuple<byte_t, byte_t, byte_t> value, Node *left, Node *right)
            : content(value), left(left), right(right) {
    }

    Node(Node *left, Node *right, std::size_t frequency)
            : left(left), right(right), frequency(frequency) {
    }

    Node(Node *left, Node *right)
            : left(left), right(right) {
    }

    Node(std::tuple<byte_t, byte_t, byte_t> value, Node *left, Node *right, std::size_t frequency)
            : content(value), left(left), right(right), frequency(frequency) {
    }

    Node(const Node &other) {
        content = other.content;
        frequency = other.frequency;
        is_full = other.is_full;

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
        is_full = other.is_full;

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
        is_full = other.is_full;

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
        is_full = other.is_full;

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

    std::optional<std::tuple<byte_t, byte_t, byte_t>> content;
    Node *left;
    Node *right;
    std::optional<std::size_t> frequency;
    bool is_full{true};
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

std::unordered_map<std::tuple<byte_t, byte_t, byte_t>, std::size_t> GroupFrequency(
        const std::unordered_map<byte_t, std::size_t>& frequencies,
        byte_t& last_node_size
) {
    std::unordered_map<std::tuple<byte_t, byte_t, byte_t>, std::size_t> grouped_frequency;

    std::vector<std::pair<byte_t, std::size_t>> sorted_frequencies(frequencies.begin(), frequencies.end());
    std::sort(
        sorted_frequencies.begin(),
        sorted_frequencies.end(),
        [](const std::pair<byte_t, std::size_t>& lhs, const std::pair<byte_t, std::size_t>& rhs){
            return lhs.second > rhs.second;
        }
    );

    byte_t tuple_index = 0;
    byte_t current_tuple[3];
    std::size_t current_frequency = 0;
    for (const auto& [key, value] : sorted_frequencies) {
        current_tuple[tuple_index++] = key;
        current_frequency += value;

        if (tuple_index == 3) {
            grouped_frequency[std::make_tuple(current_tuple[0], current_tuple[1], current_tuple[2])] = current_frequency;
//            std::cout << current_tuple[0] << ' ' << current_tuple[1] << ' ' << current_tuple[2] << std::endl;
            tuple_index = 0;
            current_frequency = 0;
        }
    }

    if (tuple_index == 1) {
        grouped_frequency[std::make_tuple(current_tuple[0], 0, 0)] = current_frequency;
    } else if (tuple_index == 2) {
        grouped_frequency[std::make_tuple(current_tuple[0], current_tuple[1], 0)] = current_frequency;
    }

    if (!tuple_index) {
        last_node_size = 3;
    } else {
        last_node_size = tuple_index;
    }

    return grouped_frequency;
}

void FlushByte(byte_t byte, const std::unordered_map<std::tuple<byte_t, byte_t, byte_t>, std::vector<bool>> &table, BitsWriter &buffer) {
    std::tuple<byte_t, byte_t, byte_t> target_key;
    byte_t tuple_index;

    for (const auto& [key, _] : table) {
        if (std::get<0>(key) == byte) {
            target_key = key;
            tuple_index = 0;
            break;
        } else if (std::get<1>(key) == byte) {
            target_key = key;
            tuple_index = 1;
            break;
        } else if (std::get<2>(key) == byte) {
            target_key = key;
            tuple_index = 2;
            break;
        }
    }

    if (tuple_index == 1) {
        buffer.WriteBit(0);
    } else if (tuple_index == 2) {
        buffer.WriteBit(1);
    }

//    std::cout << std::get<0>(target_key) << ' ' << std::get<1>(target_key) << ' ' << std::get<2>(target_key) << std::endl;
    for (byte_t bit: table.at(target_key)) {
        buffer.WriteBit(bit);
    }
}

void FlushLastNodeSize(byte_t last_node_size, BitsWriter &buffer) {
    assert(last_node_size <= 3);

    for (byte_t i = 0; i < 2; ++i) {
        buffer.WriteBit((last_node_size >> (1 - i)) & 1);
    }
}

void FlushTree(Node *root, BitsWriter &buffer, byte_t last_node_size, std::size_t& tree_section_len, bool is_right_tree = 0) {
    if (!root) {
        return;
    }

    if (root->content.has_value()) {
        buffer.WriteBit(1);
        tree_section_len += 1;
//        buffer.WriteByte(root->content.value());
        if (is_right_tree && last_node_size < 3) {
            tree_section_len += 8;
            buffer.WriteByte(std::get<0>(root->content.value()));
            if (last_node_size > 1) {
                tree_section_len += 8;
                buffer.WriteByte(std::get<1>(root->content.value()));
            }
        } else {
            tree_section_len += 24;
            buffer.WriteByte(std::get<0>(root->content.value()));
            buffer.WriteByte(std::get<1>(root->content.value()));
            buffer.WriteByte(std::get<2>(root->content.value()));
        }

    } else {
        buffer.WriteBit(0);
        tree_section_len += 1;
        FlushTree(root->left, buffer, last_node_size, tree_section_len, 0);
        FlushTree(root->right, buffer, last_node_size, tree_section_len, 1);
    }
}

void GetTableInternal(
        huffman::Node *root,
        std::unordered_map<std::tuple<byte_t, byte_t, byte_t>, std::vector<bool>> &table,
        std::vector<bool>& current_path
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

std::unordered_map<std::tuple<byte_t, byte_t, byte_t>, std::vector<bool>> GetTable(huffman::Node *root) {
    std::unordered_map<std::tuple<byte_t, byte_t, byte_t>, std::vector<bool>> table;

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

        auto new_node = Node(node_2, node_1, node_1->frequency.value() + node_2->frequency.value());
        heap.Push(new_node);
    }

    auto tree = new Node(std::move(heap.Top()));
    heap.Pop();

    return tree;
}

//Node *ReadTree(BitsReader &reader) {
//    byte_t bit, byte;
//    reader.ReadBit(bit);
//    if (bit) {
//        reader.ReadByte(byte);
//        auto node = new Node(byte);
//        return node;
//    } else {
//        auto left_node = ReadTree(reader);
//        auto right_node = ReadTree(reader);
//        return new Node(left_node, right_node);
//    }
//}

//void DecodeData(Node *tree, BitsReader &reader, BitsWriter &buffer) {
//    byte_t bit;
//    Node *current_node = tree;
//    while (reader.ReadBit(bit)) {
//        if (bit && current_node->right) {
//            current_node = current_node->right;
//        }
//
//        if (!bit && current_node->left) {
//            current_node = current_node->left;
//        }
//
//        if (!current_node->left && !current_node->right) {
//            buffer.WriteByte(current_node->content.value());
//            current_node = tree;
//        }
//    }
//}

void FlushLastByteLen(byte_t last_byte_length, BitsWriter &buffer) {
    for (byte_t i = 0; i < 3; ++i) {
        buffer.WriteBit((last_byte_length >> (2 - i)) & 1);
    }
}

//byte_t ReadLastByteLen(BitsReader &reader) {
//    byte_t bit;
//
//    byte_t last_byte_length = 0;
//    for (std::size_t i = 0; i < 3; ++i) {
//        reader.ReadBit(bit);
//        last_byte_length |= (bit << (2 - i));
//    }
//
//    return last_byte_length;
//}

byte_t CalculateDataSectionLen(
        const std::unordered_map<std::tuple<byte_t, byte_t, byte_t>, std::vector<bool>> &code_table,
        const std::unordered_map<byte_t, std::size_t> &frequencies,
        byte_t last_node_size
) {
    std::vector<std::pair<std::tuple<byte_t, byte_t, byte_t>, std::vector<bool>>> sorted_codes(code_table.begin(), code_table.end());
    std::sort(
            sorted_codes.begin(),
            sorted_codes.end(),
            [](const std::pair<std::tuple<byte_t, byte_t, byte_t>, std::vector<bool>>& lhs, const std::pair<std::tuple<byte_t, byte_t, byte_t>, std::vector<bool>>& rhs){
                return lhs.second < rhs.second;
            }
    );

    std::size_t data_section_length = 0;
    for (const auto& [key, code] : sorted_codes) {
        byte_t node_size = (key == sorted_codes.back().first ? last_node_size : 3);
        data_section_length += frequencies.at(std::get<0>(key)) * code.size();
        if (node_size > 1) {
            data_section_length += frequencies.at(std::get<1>(key)) * (code.size() + 1);
        }
        if (node_size > 2) {
            data_section_length += frequencies.at(std::get<2>(key)) * (code.size() + 1);
        }
    }

    return data_section_length;
}

Heap<Node> BuildHeap(const std::unordered_map<std::tuple<byte_t, byte_t, byte_t>, std::size_t>& frequencies) {
    Heap<huffman::Node> heap;

    std::vector<std::pair<std::tuple<byte_t, byte_t, byte_t>, std::size_t>> sorted_frequencies(frequencies.begin(), frequencies.end());
    std::sort(
            sorted_frequencies.begin(),
            sorted_frequencies.end(),
            [](const std::pair<std::tuple<byte_t, byte_t, byte_t>, std::size_t>& lhs, const std::pair<std::tuple<byte_t, byte_t, byte_t>, std::size_t>& rhs){
                return lhs.second < rhs.second;
            }
    );

    for (auto [key, frequency]: sorted_frequencies) {
        auto new_node = huffman::Node(key, nullptr, nullptr, frequency);
        heap.Push(std::move(new_node));
    }

    return heap;
}

void CompressData(
        BitsReader &reader,
        const std::unordered_map<std::tuple<byte_t, byte_t, byte_t>, std::vector<bool>> &code_table,
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

    byte_t last_node_size;
    const auto grouped_frequencies = huffman::GroupFrequency(frequencies, last_node_size);

    auto heap = huffman::BuildHeap(grouped_frequencies);
    const auto tree = huffman::BuildTree(heap);
    const auto code_table = huffman::GetTable(tree);
    const auto data_section_len = huffman::CalculateDataSectionLen(code_table, frequencies, last_node_size);

    BitsWriter buffer;

    huffman::FlushLastNodeSize(last_node_size, buffer);

    std::size_t tree_section_len = 0;
    huffman::FlushTree(tree, buffer, last_node_size, tree_section_len);

    byte_t last_byte_len = (2 + tree_section_len + 3 + data_section_len) % 8;
    huffman::FlushLastByteLen(last_byte_len, buffer);

    BitsReader original_reader(bytes);
    huffman::CompressData(original_reader, code_table, buffer);

    buffer.Flush(compressed);

    std::cout << "Compressed:\t\t" << buffer << std::endl;

    delete tree;
}


void Decode(IInputStream& compressed, IOutputStream& original) {
//    const auto bytes = GetBytes(compressed);
//
//    BitsReader compressed_reader(bytes);
//    const auto tree = huffman::ReadTree(compressed_reader);
//
//    const auto last_byte_length = huffman::ReadLastByteLen(compressed_reader);
//    compressed_reader.SetLastByteLen(last_byte_length);
//
//    BitsWriter buffer;
//    huffman::DecodeData(tree, compressed_reader, buffer);
//    buffer.Flush(original);
//
//    delete tree;
}


#ifdef DEBUG

#include <cassert>

namespace tests {

void TestCompression() {
//    {
//        auto original_buffer = std::vector<byte_t>{'a', 'b', 'r', 'a', 'c', 'a', 'd', 'a', 'b', 'r', 'a'};
//        VectorStream original(original_buffer);
//        VectorStream compressed;
//
//        Encode(original, compressed);
//
//        VectorStream uncompressed;
//        Decode(compressed, uncompressed);
//
//        assert(original == uncompressed);
//    }
//    {
//        auto original_buffer = std::vector<byte_t>{'a'};
//        VectorStream original(original_buffer);
//        VectorStream compressed;
//
//        Encode(original, compressed);
//
//        VectorStream uncompressed;
//        Decode(compressed, uncompressed);
//
//        assert(original == uncompressed);
//    }
//    {
//        auto original_buffer = std::vector<byte_t>{'I', 'E', 'E', 'E', 'C', 'O', 'M', 'P', 'U', 'T', 'A', 'T', 'I', 'O', 'N', 'A', 'L', 'I', 'N', 'T', 'E', 'L', 'L', 'I', 'G', 'E', 'N', 'C', 'E'};
//        VectorStream original(original_buffer);
//        VectorStream compressed;
//
//        Encode(original, compressed);
//
//        VectorStream uncompressed;
//        Decode(compressed, uncompressed);
//
//        assert(original == uncompressed);
//    }
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
