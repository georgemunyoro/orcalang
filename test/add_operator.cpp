#include "utils.h"
#include <gtest/gtest.h>

static orca::AddOperator addOperator = orca::AddOperator();

TEST(add_operator, unsigned_ints_of_same_size) {
  auto res = addOperator.getResultingType(u32, u32);

  ASSERT_EQ(res->getIntegerType().getIsSigned(), false);
  ASSERT_EQ(res->getIntegerType().getBits(), 32);
}

TEST(add_operator, unsigned_ints_of_different_size) {
  auto res = addOperator.getResultingType(u32, u64);
  ASSERT_EQ(res->getIntegerType().getIsSigned(), false);
  ASSERT_EQ(res->getIntegerType().getBits(), 64);

  res = addOperator.getResultingType(u64, u32);
  ASSERT_EQ(res->getIntegerType().getIsSigned(), false);
  ASSERT_EQ(res->getIntegerType().getBits(), 64);
}

TEST(add_operator, signed_ints_of_same_size) {
  auto res = addOperator.getResultingType(s32, s32);

  ASSERT_EQ(res->getIntegerType().getIsSigned(), true);
  ASSERT_EQ(res->getIntegerType().getBits(), 32);
}

TEST(add_operator, signed_ints_of_different_size) {
  auto res = addOperator.getResultingType(s32, s64);
  ASSERT_EQ(res->getIntegerType().getIsSigned(), true);
  ASSERT_EQ(res->getIntegerType().getBits(), 64);

  res = addOperator.getResultingType(s64, s32);
  ASSERT_EQ(res->getIntegerType().getIsSigned(), true);
  ASSERT_EQ(res->getIntegerType().getBits(), 64);
}

TEST(add_operator, signed_and_unsigned_ints_of_same_size) {
  auto res = addOperator.getResultingType(s32, u32);
  ASSERT_EQ(res->getIntegerType().getIsSigned(), true);
  ASSERT_EQ(res->getIntegerType().getBits(), 32);

  res = addOperator.getResultingType(u32, s32);
  ASSERT_EQ(res->getIntegerType().getIsSigned(), true);
  ASSERT_EQ(res->getIntegerType().getBits(), 32);
}

TEST(add_operator, signed_and_unsigned_ints_of_different_size) {
  auto res = addOperator.getResultingType(s32, u64);
  ASSERT_EQ(res->getIntegerType().getIsSigned(), true);
  ASSERT_EQ(res->getIntegerType().getBits(), 64);

  res = addOperator.getResultingType(u64, s32);
  ASSERT_EQ(res->getIntegerType().getIsSigned(), true);
  ASSERT_EQ(res->getIntegerType().getBits(), 64);
}

TEST(add_operator, floats_of_same_size) {
  auto res = addOperator.getResultingType(f32, f32);

  ASSERT_EQ(res->getFloatType().getBits(), 32);
}

TEST(add_operator, floats_of_different_size) {
  auto res = addOperator.getResultingType(f32, f64);
  ASSERT_EQ(res->getFloatType().getBits(), 64);

  res = addOperator.getResultingType(f64, f32);
  ASSERT_EQ(res->getFloatType().getBits(), 64);
}

TEST(add_operator, non_ints_or_floats_should_fail) {
  ASSERT_THROW(addOperator.getResultingType(s32, boolType), std::string);
  ASSERT_THROW(addOperator.getResultingType(f32, charType), std::string);
  ASSERT_THROW(addOperator.getResultingType(f32, voidType), std::string);
  ASSERT_THROW(addOperator.getResultingType(voidType, boolType), std::string);
  ASSERT_THROW(addOperator.getResultingType(boolType, boolType), std::string);
}
