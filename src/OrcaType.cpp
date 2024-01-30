#include "OrcaType.h"

OrcaIntegerType OrcaType::u8 = OrcaIntegerType(false, 8);
OrcaIntegerType OrcaType::u16 = OrcaIntegerType(false, 16);
OrcaIntegerType OrcaType::u32 = OrcaIntegerType(false, 32);
OrcaIntegerType OrcaType::u64 = OrcaIntegerType(false, 64);

OrcaIntegerType OrcaType::s8 = OrcaIntegerType(true, 8);
OrcaIntegerType OrcaType::s16 = OrcaIntegerType(true, 16);
OrcaIntegerType OrcaType::s32 = OrcaIntegerType(true, 32);
OrcaIntegerType OrcaType::s64 = OrcaIntegerType(true, 64);

OrcaFloatType OrcaType::f32 = OrcaFloatType(32);
OrcaFloatType OrcaType::f64 = OrcaFloatType(64);

OrcaBooleanType OrcaType::boolean = OrcaBooleanType();
OrcaCharType OrcaType::_char = OrcaCharType();
OrcaVoidType OrcaType::_void = OrcaVoidType();
