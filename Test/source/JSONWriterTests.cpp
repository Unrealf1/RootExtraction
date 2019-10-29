//
// Created by fedor on 10/29/19.
//

#include "test_main.hpp"

class NullBuffer : public std::streambuf
{
public:
    int overflow(int c) { return c; }
};

TEST(JSONWriter, Balance_zero_sequence) {
    NullBuffer null_buffer;
    std::ostream null_stream(&null_buffer);

    JSONWriter wr(null_stream);

    const int N = 5;

    for (int i = 0; i < N; ++i) {
        wr.BeginBlock();
    }
    ASSERT_EQ(wr.GetDepth(), N);

    for (int i = 0; i < N; ++i) {
        wr.EndBlock();
    }
    ASSERT_EQ(wr.GetDepth(), 0);

    for (int i = 0; i < N; ++i) {
        wr.BeginBlock("TEST");
    }
    ASSERT_EQ(wr.GetDepth(), N);

    for (int i = 0; i < N; ++i) {
        wr.EndBlock();
    }
    ASSERT_EQ(wr.GetDepth(), 0);
}

TEST(JSONWriter, JSONWriter_Balance_zero) {
    NullBuffer null_buffer;
    std::ostream null_stream(&null_buffer);

    JSONWriter wr(null_stream);

    const int N = 5;

    for (int i = 0; i < N; ++i) {
        wr.BeginBlock();
        wr.EndBlock();

        wr.BeginBlock("A");
        wr.EndBlock();
    }
    ASSERT_EQ(wr.GetDepth(), 0);

}
