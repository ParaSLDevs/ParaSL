#pragma once

#include <memory>

#include "expressions.h"

class InitList {

};

class InputWithType : public InputExpr {
    std::unique_ptr<Type> type_;
};

class GlueWithFields {

};

class Range {

};

class BindMethodDef {

};
