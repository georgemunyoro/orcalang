#define APPROVALS_GOOGLETEST
#include "../vendor/approvaltests/ApprovalTests.hpp"

auto directoryDisposer =
    ApprovalTests::Approvals::useApprovalsSubdirectory("approval_tests");

bool dropTestCaseNamesWithIgnoreThis(const std::string &,
                                     const std::string &testCaseName) {
  return ApprovalTests::StringUtils::contains(testCaseName, "Fixture");
}

auto ignoreNames =
    ApprovalTests::GoogleConfiguration::addTestCaseNameRedundancyCheck(
        dropTestCaseNamesWithIgnoreThis);
