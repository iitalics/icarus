#include "State.h"

namespace run {

Environment::Environment ()
{
}


Function* Environment::get_function (const std::string& name,
                                     bool create_if_not_found)
{
    auto map_iter = functions.find(name);
    if (map_iter == functions.end()) {
        if (create_if_not_found) {
            auto fn = new Function(name);
            functions[name] = std::unique_ptr<Function>(fn);
            return fn;
        }
        else
            return nullptr;
    }

    return map_iter->second.get();
}

FunctionImpl& Environment::impl_function (const std::string& name, size_t argc)
{
    auto fn = get_function(name, true);
    fn->implementations.emplace_back(argc);
    return fn->implementations.back();
}


State::State ()
{
    env.load_std_lib();
}

}
