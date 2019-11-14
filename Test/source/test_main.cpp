#include "test_main.hpp"

int NullBuffer::overflow(int c) { return c; }

bool checkBalance(std::istream& str) {
    int32_t counter = 0;
    bool in_string = false;
    while (!str.eof()) {
        char c = str.get();
        // This is not entirely correct!
        // Cases like \" would not be found
        if (c == '\"') {
            in_string = !in_string;
        } else if(in_string) {
            continue;
        } else if (c == '{') {
            ++counter;
        } else if (c == '}') {
            --counter;
        }
        if (counter < 0) {
            return false;
        }
    }
    return (counter == 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
