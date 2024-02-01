#pragma once

#include <string>

#include "../src/OrcaContext.h"

std::string replaceString(std::string &subject, const std::string &search,
                          const std::string &replace);

// Remove colors from string
std::string cleanString(std::string s);

typedef struct {
  std::string astStringWithoutTypes;
  std::string astStringWithTypes;
  std::string mainFunctionLLVMIr;
} RunResult;

RunResult runAndGetAstStrings(const std::string &filename);

static OrcaType *u8 = new OrcaType(OrcaType::u8);
static OrcaType *u16 = new OrcaType(OrcaType::u16);
static OrcaType *u32 = new OrcaType(OrcaType::u32);
static OrcaType *u64 = new OrcaType(OrcaType::u64);
static OrcaType *s8 = new OrcaType(OrcaType::s8);
static OrcaType *s16 = new OrcaType(OrcaType::s16);
static OrcaType *s32 = new OrcaType(OrcaType::s32);
static OrcaType *s64 = new OrcaType(OrcaType::s64);
static OrcaType *f32 = new OrcaType(OrcaType::f32);
static OrcaType *f64 = new OrcaType(OrcaType::f64);
static OrcaType *boolType = new OrcaType(OrcaType::boolean);
static OrcaType *voidType = new OrcaType(OrcaType::_void);
static OrcaType *charType = new OrcaType(OrcaType::_char);
