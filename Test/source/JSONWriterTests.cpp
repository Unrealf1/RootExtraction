//
// Created by fedor on 10/29/19.
//

#include <stack>
#include "test_main.hpp"

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

TEST(JSONWriter, Written_Balance1) {
    std::stringstream sstr;

    JSONWriter wr(sstr);

    const int N = 5;

    for (int i = 0; i < N; ++i) {
        wr.BeginBlock();
        wr.AddProperty("prop}", "value}}");
        wr.EndBlock();
        wr.AddProperty("prop}", "value}}");
        wr.BeginBlock("A");
        wr.AddProperty("prop}", "value}}");
        wr.EndBlock();
    }
    ASSERT_FALSE(sstr.eof());
    ASSERT_TRUE(checkBalance(sstr));
}
