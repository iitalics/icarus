#include "Cell.h"

namespace run {

Cell Cell::null_object = nullptr;
Cell Cell::true_object = nullptr;
Cell Cell::false_object = nullptr;

Cell Cell::int_type = nullptr;
Cell Cell::bool_type = nullptr;
Cell Cell::string_type = nullptr;
Cell Cell::array_type = nullptr;
Cell Cell::type_type = nullptr;

namespace {
struct CellSingetonInitialize
{
    static inline Cell static_alloc (uint8_t type, uint16_t size = 0)
    {
        Object* obj = (Object*)
            new char[sizeof(Object) + size];

        obj->type = type | Object::Static;
        obj->size = size;

        return Cell(obj);
    }

    CellSingetonInitialize ()
    {
        Cell::true_object  = static_alloc(Object::True);
        Cell::false_object = static_alloc(Object::False);
        Cell::type_type   = static_alloc(Object::Datatype | Object::DatatypeNoInst);
        Cell::int_type    = static_alloc(Object::Datatype | Object::DatatypeNoInst);
        Cell::bool_type   = static_alloc(Object::Datatype | Object::DatatypeNoInst);
        Cell::string_type = static_alloc(Object::Datatype | Object::DatatypeNoInst);
        Cell::array_type  = static_alloc(Object::Datatype | Object::DatatypeNoInst);
    }
};
// slight hack to call constructor before main()
CellSingetonInitialize _;
}

Cell Cell::get_type () const
{
    if (is_null())
        return nullptr;

    if (is_integer())
        return int_type;

    switch (obj->type & ~(Object::Static | Object::DatatypeNoInst)) {
    case Object::Array:
        return array_type;

    case Object::True:
    case Object::False:
        return bool_type;

    case Object::String:
        return string_type;

    case Object::Datatype:
        return type_type;

    case Object::Instance:
    default:
        return children()[0];
    }
}


}
