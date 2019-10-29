#include "test_main.hpp"

class NulStreambuf : public std::streambuf
{
    char                dummyBuffer[ 64 ];
protected:
    virtual int         overflow( int c )
    {
        setp( dummyBuffer, dummyBuffer + sizeof( dummyBuffer ) );
        return (c == traits_type::eof()) ? '\0' : c;
    }
};

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
