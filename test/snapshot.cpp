#include "../vendor/approvaltests/ApprovalTests.hpp"
#include "gtest/gtest.h"
#include <CommonTokenStream.h>
#include <filesystem>
#include <gtest/gtest.h>
#include <string>
#include <tuple>

namespace fs = std::filesystem;

#include "utils.h"

class Fixture
    : public ::testing::TestWithParam<std::tuple<std::string, std::string>> {};

TEST_P(Fixture, ast_typed) {
  std::string stage, testCaseName;
  std::tie(stage, testCaseName) = GetParam();

  {
    auto section =
        ApprovalTests::NamerFactory::appendToOutputFilename(testCaseName);
    auto astStrings = runAndGetAstStrings("../test/cases/stage" + stage +
                                          "/valid/" + testCaseName + ".orca");
    ApprovalTests::Approvals::verify(astStrings.astStringWithTypes);
  }
}

TEST_P(Fixture, ast_untyped) {
  std::string stage, testCaseName;
  std::tie(stage, testCaseName) = GetParam();

  {
    auto section =
        ApprovalTests::NamerFactory::appendToOutputFilename(testCaseName);
    auto astStrings = runAndGetAstStrings("../test/cases/stage" + stage +
                                          "/valid/" + testCaseName + ".orca");
    ApprovalTests::Approvals::verify(astStrings.astStringWithoutTypes);
  }
}

TEST_P(Fixture, llvm_ir) {
  std::string stage, testCaseName;
  std::tie(stage, testCaseName) = GetParam();

  {
    auto section =
        ApprovalTests::NamerFactory::appendToOutputFilename(testCaseName);
    auto astStrings = runAndGetAstStrings("../test/cases/stage" + stage +
                                          "/valid/" + testCaseName + ".orca");
    ApprovalTests::Approvals::verify(astStrings.mainFunctionLLVMIr);
  }
}

auto GetData(const std::string &stage, const std::string &type) {
  std::vector<std::tuple<std::string, std::string>> data;
  for (const auto &expr :
       fs::directory_iterator("../test/cases/stage" + stage + "/" + type)) {
    auto testCaseName = expr.path().filename().string();
    testCaseName = testCaseName.substr(0, testCaseName.find('.'));
    data.emplace_back(stage, testCaseName);
  }
  return ::testing::ValuesIn(data);
}

INSTANTIATE_TEST_SUITE_P(stage_1, Fixture, GetData("1", "valid"));
INSTANTIATE_TEST_SUITE_P(stage_2, Fixture, GetData("2", "valid"));
INSTANTIATE_TEST_SUITE_P(stage_3, Fixture, GetData("3", "valid"));
