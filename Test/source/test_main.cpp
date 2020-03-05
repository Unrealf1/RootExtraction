#include "test_main.hpp"

int NullBuffer::overflow(int c) { return c; }

bool checkBalance(std::istream& str) {
    
    if (str.bad()) {
        return false;
    }
    
    int32_t common_counter = 0;
    int32_t square_counter = 0;
    uint64_t counter = 0;
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
            ++common_counter;
        } else if (c == '}') {
            --common_counter;
        } else if (c == '[') {
            ++square_counter;
        } else if (c == ']') {
            --square_counter;
        }
        if (common_counter < 0 || square_counter < 0) {
            return false;
        }
        if (!(counter % 15000000)) {
            //std::cerr << counter/1000000 << " millions symbols read" << std::endl;
        }
        ++counter;
    }
    return (common_counter == 0) && (square_counter == 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
