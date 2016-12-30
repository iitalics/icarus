#include "GC.h"
#include <cstring>

namespace run {

GC::GC ()
{
}


/*** Allocation ***/

Cell GC::alloc_ (uint8_t type, uint16_t size)
{
    Object* obj = reinterpret_cast<Object*>(new char[sizeof(Object) + size]);
    obj->type = type;
    obj->gc_status = GCStatus::NewlyAllocated;
    obj->size = size;
    return Cell(obj);
}

Cell GC::make_array (size_t nelems)
{
    auto obj_arr = alloc_(Object::Array, nelems * sizeof(Cell));
    for (auto& child : obj_arr.children())
        child = Cell::nil();
    return obj_arr;
}

Cell GC::make_string (boost::string_ref s)
{
    auto len = s.size();
    auto obj_str = alloc_(Object::String, len + 1);
    std::memcpy(obj_str.obj->data, s.data(), len);
    obj_str.obj->data[len] = '\0'; // a NULL terminator, for good luck.
    return obj_str;
}

Cell GC::make_datatype (Cell::DatatypeFields field_names)
{
    size_t total_size = sizeof(Object::DatatypeDesc);
    for (auto field_name : field_names) {
        total_size += field_name.size() + 1;
        total_size += sizeof(boost::string_ref);
    }

    /* allocate Datatype object */
    auto obj_dt = alloc_(Object::Datatype, total_size);
    auto& desc = *obj_dt.obj->data_as_datatype_desc();
    desc.count = field_names.size();

    /* begin storing strings at memory located
       directly after desc.fields */
    char* chars = reinterpret_cast<char*>(desc.fields + desc.count);

    /* copy each string and assign fields */
    size_t i = 0;
    for (auto field_name : field_names) {
        /* copy string contents */
        size_t len = field_name.size();
        std::memcpy(chars, field_name.data(), len);
        chars[len] = '\0'; // a NULL terminator, for good luck.
        desc.fields[i++] = boost::string_ref(chars, len);

        /* advance `chars' buffer */
        chars += len + 1;
    }
    return obj_dt;
}

Cell GC::make_instance (Cell datatype, Cell* argv)
{
    auto dt_desc = *datatype.obj->data_as_datatype_desc();
    size_t num_fields = dt_desc.count;

    auto obj_inst = alloc_(0, num_fields * sizeof(Cell));
    auto children = obj_inst.children();

    /* first child is the datatype */
    children[0] = datatype;
    /* rest are the fields */
    for (size_t i = 0; i < num_fields; i++) {
        children[i + 1] = argv[i];
    }
    return obj_inst;
}




/*** Collection ***/

void GC::collect (State* state)
{
}

void GC::traverse (State* state, Cell v) {
    if (!v.is_object() || (v.obj->gc_status & GCStatus::Marked)) {
        /* ignore non-collectables, or already-marked objects */
        return;
    }

    v.obj->gc_status = Marked;
    if (v.has_children()) {
        for (auto& child : v.children()) {
            traverse(state, child);
        }
    }
}


}
