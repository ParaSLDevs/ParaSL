#pragma once

#include <memory>

#include "expressions.h"

class InitList {

};

class InputWithType : public expressions::InputExpr {
    std::unique_ptr<types::Type> type_;
};

class GlueWithFields {

};

class Range {

};

class BindMethodDef {

};
