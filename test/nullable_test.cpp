#include <gmock/gmock.h>

#include "sqlitewrapper/nullable.h"

using namespace testing;

TEST(Nullable, isNullWhenDefaultConstructed)
{
    EXPECT_THAT(bool(SqliteWrapper::Nullable<int>()), Eq(false));
}

TEST(Nullable, isNotNullWhenConstructedWithValue)
{
    EXPECT_THAT(bool(SqliteWrapper::Nullable<int>(42)), Eq(true));
}

TEST(Nullable, canBeSetToNull)
{
    SqliteWrapper::Nullable<int> answer(42);
    ASSERT_THAT(bool(answer), Eq(true));

    answer.setNull();
    EXPECT_THAT(bool(answer), Eq(false));
}

TEST(Nullable, canBeSetToValue)
{
    SqliteWrapper::Nullable<int> answer;
    ASSERT_THAT(bool(answer), Eq(false));

    answer.setValue(42);
    EXPECT_THAT(bool(answer), Eq(true));
    EXPECT_THAT(*answer, Eq(42));
}
