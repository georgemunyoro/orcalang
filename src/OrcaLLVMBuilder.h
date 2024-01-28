
#pragma once

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/NoFolder.h"

typedef llvm::IRBuilder<llvm::NoFolder> OrcaLLVMBuilder;
