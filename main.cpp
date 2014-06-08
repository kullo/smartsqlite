#include <gmock/gmock.h>

#include "sqlitewrapper.h"

int main(int argc, char *argv[])
{
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
