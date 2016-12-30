#include "Function.h"
#include "../syntax/AST.h"

namespace run {

Cell FunctionImpl::call (State* state, Cell* args)
{
    if (native_fn_ptr) {
        return native_fn_ptr(state, args);
    }
    else {
        throw std::runtime_error("missing implementation of function");
    }
}

}
