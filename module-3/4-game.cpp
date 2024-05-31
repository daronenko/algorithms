/* 
 * Написать алгоритм для решения игры в “пятнашки”. Решением задачи является 
 * приведение к виду: [ 1 2 3 4 ] [ 5 6 7 8 ] [ 9 10 11 12] [ 13 14 15 0 ],
 * где 0 задает пустую ячейку. Достаточно найти хотя бы какое-то решение.
 * Число перемещений костяшек не обязано быть минимальным.
 *
 * Формат ввода
 * Начальная расстановка.
 *
 * Формат вывода
 * Если вам удалось найти решение, то в первой строке файла выведите число
 * перемещений, которое требуется сделать в вашем решении. А во второй строке
 * выведите соответствующую последовательность ходов: L означает, что в результате
 * перемещения костяшка сдвинулась влево, R – вправо, U – вверх, D – вниз. Если же
 * выигрышная конфигурация недостижима, то выведите в выходной файл одно число −1. 
 */


#include <limits>
#include <iostream>
#include <sstream>
#include <cassert>
#include <cstdint>
#include <utility>
#include <array>
#include <set>
#include <algorithm>
#include <unordered_map>

// #define DEBUG


const std::uint8_t FIELD_SIZE = 16;
const std::uint8_t FIELD_SIDE = 4;

const std::array<std::uint8_t, FIELD_SIZE> FINSISH_STATE = {
     1,  2,  3,  4,
     5,  6,  7,  8,
     9, 10, 11, 12,
    13, 14, 15,  0,
};


class GameState {
 public:
    explicit GameState(const std::array<std::uint8_t, FIELD_SIZE>& field): field_(field) {
        empty_pos_ = std::numeric_limits<std::uint8_t>::max();
        for (std::uint8_t i = 0; i < FIELD_SIZE; ++i) {
            if (!field_[i]) {
                empty_pos_ = i;
            }
        }

        assert(empty_pos_ != std::numeric_limits<std::uint8_t>::max());
    }

    bool IsComplete() const {
        return field_ == FINSISH_STATE;
    }

    bool IsSolvable() const {
        auto empty_row = empty_pos_ / FIELD_SIDE + 1;
        return (getInvCount() + empty_row) % 2 == 0;
    }

    bool CanMoveLeft() const {
        return empty_pos_ % FIELD_SIDE != FIELD_SIDE - 1;
    }

    bool CanMoveRight() const {
        return empty_pos_ % FIELD_SIDE != 0;
    }

    bool CanMoveUp() const {
        return empty_pos_ < FIELD_SIDE * (FIELD_SIDE - 1);
    }

    bool CanMoveDown() const {
        return empty_pos_ > FIELD_SIDE - 1;
    }

    GameState MoveLeft() const {
        assert(CanMoveLeft());

        GameState new_state(*this);
        std::swap(new_state.field_[empty_pos_],
                  new_state.field_[empty_pos_ + 1]);
        ++new_state.empty_pos_;

        return new_state;
    }

    GameState MoveRight() const {
        assert(CanMoveRight());

        GameState new_state(*this);
        std::swap(new_state.field_[empty_pos_],
                  new_state.field_[empty_pos_ - 1]);
        --new_state.empty_pos_;

        return new_state;
    }

    GameState MoveUp() const {
        assert(CanMoveUp());

        GameState new_state(*this);
        std::swap(new_state.field_[empty_pos_],
                  new_state.field_[empty_pos_ + FIELD_SIDE]);
        new_state.empty_pos_ += FIELD_SIDE;

        return new_state;
    }

    GameState MoveDown() const {
        assert(CanMoveDown());

        GameState new_state(*this);
        std::swap(new_state.field_[empty_pos_],
                  new_state.field_[empty_pos_ - FIELD_SIDE]);
        new_state.empty_pos_ -= FIELD_SIDE;

        return new_state;
    }

    bool operator == (const GameState& other) const {
        return field_ == other.field_;
    }

    std::uint8_t GetPriority() const {
        std::uint8_t mismatched_count = 0;
        for (std::uint8_t i = 0; i < FIELD_SIZE; ++i) {
            if (field_[i] != FINSISH_STATE[i]) {
                ++mismatched_count;
            }
        }

        return mismatched_count;
    }

 private:
    std::size_t getInvCount() const {
        std::size_t inv_count = 0;
        for (std::uint8_t i = 0; i < FIELD_SIZE - 1; ++i) {
            for (std::uint8_t j = i + 1; j < FIELD_SIZE; ++j) {
                if (field_[i] > field_[j] && field_[i] && field_[j]) {
                    ++inv_count;
                }
            }
        }

        return inv_count;
    }

    std::array<std::uint8_t, FIELD_SIZE> field_;
    std::uint8_t empty_pos_;

    friend struct GameStateHasher;
    friend std::ostream& operator << (std::ostream& out, const GameState& state);
};

struct GameStateHasher {
 public:
    std::size_t operator () (const GameState& state) const {
        std::size_t hash = 0;
        for (uint8_t i = 1u; i <= 16u; i += 2u) {
            char val = (state.field_[i] << 4) + state.field_[i + 1u];
            hash <<= 8;
            hash += val;
        }

        return hash;
    }
};

std::ostream& operator << (std::ostream& out, const GameState& state) {
    for (std::uint8_t i = 0; i < FIELD_SIDE; ++i) {
        for (std::uint8_t j = 0; j < FIELD_SIDE; ++j) {
            out << static_cast<int>(state.field_[i * FIELD_SIDE + j]) << ' ';
        }
        out << std::endl;
    }

    return out;
}

struct GameStateComparator {
 public:
    bool operator () (const std::pair<std::uint8_t, GameState>& lhs,
                      const std::pair<std::uint8_t, GameState>& rhs) const {
        return lhs.first < rhs.first;
    }
};

std::string GetSolution(const std::array<std::uint8_t, FIELD_SIZE> &field) {
    GameState start_state(field);

    if (!start_state.IsSolvable()) {
        return "-1";
    }

    std::unordered_map<GameState, char, GameStateHasher> visited;
    visited[start_state] = 'S';

    std::set<std::pair<std::uint8_t, GameState>, GameStateComparator> queue;
    queue.emplace(start_state.GetPriority(), start_state);

    while (true) {
        auto [curr_priority, curr_state] = *queue.begin();
        queue.erase(queue.begin());

        if (curr_state.IsComplete()) {
            break;
        }

        if (curr_state.CanMoveLeft()) {
            GameState new_state = curr_state.MoveLeft();
            if (visited.find(new_state) == visited.end()) {
                visited[new_state] = 'L';
                auto priority = new_state.GetPriority();
                queue.emplace(priority, new_state);
            }
        }

        if (curr_state.CanMoveRight()) {
            GameState new_state = curr_state.MoveRight();
            if (visited.find(new_state) == visited.end()) {
                visited[new_state] = 'R';
                auto priority = new_state.GetPriority();
                queue.emplace(priority, new_state);
            }
        }

        if (curr_state.CanMoveUp()) {
            GameState new_state = curr_state.MoveUp();
            if (visited.find(new_state) == visited.end()) {
                visited[new_state] = 'U';
                auto priority = new_state.GetPriority();
                queue.emplace(priority, new_state);
            }
        }

        if (curr_state.CanMoveDown()) {
            GameState new_state = curr_state.MoveDown();
            if (visited.find(new_state) == visited.end()) {
                visited[new_state] = 'D';
                auto priority = new_state.GetPriority();
                queue.emplace(priority, new_state);
            }
        }
    }

    std::string path;
    GameState state(FINSISH_STATE);

    while (visited[state] != 'S') {
        char move = visited[state];
        switch (move) {
            case 'L': {
                state = state.MoveRight();
                path += 'L';
                break;
            }
            case 'R': {
                state = state.MoveLeft();
                path += 'R';
                break;
            }
            case 'D': {
                state = state.MoveUp();
                path += 'D';
                break;
            }
            case 'U': {
                state = state.MoveDown();
                path += 'U';
                break;
            }
        }
    }

    std::reverse(path.begin(), path.end());
    return path;
}


void Run(std::istream& input, std::ostream& output) {
    std::array<std::uint8_t, FIELD_SIZE> source_state;
    for (auto& cell: source_state) {
        int value;
        input >> value;
        cell = value;
    }

    auto solution = GetSolution(source_state);
    if (solution != "-1") {
        output << solution.size() << std::endl;
    }

    output << solution << std::endl;
}

#ifdef DEBUG

void TestRun() {
    {
        std::stringstream input, output;

        input << "1 2 3 4\n";
        input << "5 6 7 8\n";
        input << "9 10 11 0\n";
        input << "13 14 15 12\n";

        Run(input, output);
        const std::string EXPECTED = "1\nU\n";
        if (output.str() != EXPECTED) {
            std::cerr << "EXPECTED:\n" << EXPECTED << std::endl;
            std::cerr << "\nOBTAINED:\n" << output.str() << std::endl;
            throw;
        }
    }
    {
        std::stringstream input, output;

        input << "1 2 3 4\n";
        input << "5 6 7 8\n";
        input << "9 10 11 12\n";
        input << "13 15 14 0\n";

        Run(input, output);
        const std::string EXPECTED = "-1\n";
        if (output.str() != EXPECTED) {
            std::cerr << "EXPECTED:\n" << EXPECTED << std::endl;
            std::cerr << "\nOBTAINED:\n" << output.str() << std::endl;
            throw;
        }
    }
}

#endif  // DEBUG


int main() {
    #ifdef DEBUG
        TestRun();
    #else
        Run(std::cin, std::cout);
    #endif  // DEBUG
}


