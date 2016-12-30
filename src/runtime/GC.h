#include "Cell.h"
#include <vector>

namespace run {

struct State;

struct GC
{
    GC ();

    Cell make_array (size_t nelems);
    Cell make_string (boost::string_ref s);
    Cell make_datatype (Cell::DatatypeFields field_names);
    Cell make_instance (Cell datatype, Cell* args);

    void collect (State* state);
    void traverse (State* state, Cell x);

private:
    Cell alloc_ (uint8_t type, uint16_t size);
};

enum GCStatus {
    Marked = 0x01,
    NewlyAllocated = 0x02,
};

}
