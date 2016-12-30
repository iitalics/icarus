#include "../runtime/State.h"

namespace run {

namespace {

Cell proc_add (State* s, Cell* args)
{
    if (args[0].is_integer() && args[1].is_integer()) {
        Fixnum fx = args[0].integer() + args[1].integer();
        return Cell::from_fixnum(fx);
    }
    else
        return Cell::nil();
}

Cell proc_sub (State* s, Cell* args)
{
    if (args[0].is_integer() && args[1].is_integer()) {
        Fixnum fx = args[0].integer() - args[1].integer();
        return Cell::from_fixnum(fx);
    }
    else
        return Cell::nil();
}

Cell proc_less (State* s, Cell* args)
{
    if (args[0].is_integer() && args[1].is_integer()) {
        bool cond = args[0].integer() < args[1].integer();
        return cond ? Cell::true_object : Cell::false_object;
    }
    else
        return Cell::nil();
}




inline void impl (Environment* env,
                  const std::string& name,
                  size_t argc,
                  NativeFnPtr nfptr)
{
    env->impl_function(name, argc).native_fn_ptr = nfptr;
}

}

void Environment::load_std_lib ()
{
    impl(this, "+", 2, proc_add);
    impl(this, "-", 2, proc_sub);
    impl(this, "<", 2, proc_less);
}

}
