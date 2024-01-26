#include "../vendor/approvaltests/ApprovalTests.hpp"
#include <CommonTokenStream.h>
#include <gtest/gtest.h>
#include <string>
#include <utility>

#include "utils.h"

auto directoryDisposer =
    ApprovalTests::Approvals::useApprovalsSubdirectory("approval_tests");

TEST(Valid, MultiDigit) {
  auto astStrings =
      runAndGetAstStrings("../test/cases/stage1/valid/multi_digit.orca");

  {
    auto section =
        ApprovalTests::NamerFactory::appendToOutputFilename("WithoutTypes");
    ApprovalTests::Approvals::verify(astStrings.first);
  }
  {
    auto section =
        ApprovalTests::NamerFactory::appendToOutputFilename("WithTypes");
    ApprovalTests::Approvals::verify(astStrings.second);
  }
}

TEST(Valid, Newlines) {
  auto astStrings =
      runAndGetAstStrings("../test/cases/stage1/valid/newlines.orca");

  {
    auto section =
        ApprovalTests::NamerFactory::appendToOutputFilename("WithoutTypes");
    ApprovalTests::Approvals::verify(astStrings.first);
  }
  {
    auto section =
        ApprovalTests::NamerFactory::appendToOutputFilename("WithTypes");
    ApprovalTests::Approvals::verify(astStrings.second);
  }
}

TEST(Valid, NoNewlines) {
  auto astStrings =
      runAndGetAstStrings("../test/cases/stage1/valid/no_newlines.orca");

  {
    auto section =
        ApprovalTests::NamerFactory::appendToOutputFilename("WithoutTypes");
    ApprovalTests::Approvals::verify(astStrings.first);
  }
  {
    auto section =
        ApprovalTests::NamerFactory::appendToOutputFilename("WithTypes");
    ApprovalTests::Approvals::verify(astStrings.second);
  }
}

TEST(Valid, Return0) {
  auto astStrings =
      runAndGetAstStrings("../test/cases/stage1/valid/return_0.orca");

  {
    auto section =
        ApprovalTests::NamerFactory::appendToOutputFilename("WithoutTypes");
    ApprovalTests::Approvals::verify(astStrings.first);
  }
  {
    auto section =
        ApprovalTests::NamerFactory::appendToOutputFilename("WithTypes");
    ApprovalTests::Approvals::verify(astStrings.second);
  }
}

TEST(Valid, Return2) {
  auto astStrings =
      runAndGetAstStrings("../test/cases/stage1/valid/return_2.orca");

  {
    auto section =
        ApprovalTests::NamerFactory::appendToOutputFilename("WithoutTypes");
    ApprovalTests::Approvals::verify(astStrings.first);
  }
  {
    auto section =
        ApprovalTests::NamerFactory::appendToOutputFilename("WithTypes");
    ApprovalTests::Approvals::verify(astStrings.second);
  }
}

TEST(Valid, Spaces) {
  auto astStrings =
      runAndGetAstStrings("../test/cases/stage1/valid/spaces.orca");

  {
    auto section =
        ApprovalTests::NamerFactory::appendToOutputFilename("WithoutTypes");
    ApprovalTests::Approvals::verify(astStrings.first);
  }
  {
    auto section =
        ApprovalTests::NamerFactory::appendToOutputFilename("WithTypes");
    ApprovalTests::Approvals::verify(astStrings.second);
  }
}
