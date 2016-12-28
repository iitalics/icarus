#pragma once
#include "../datatypes.h"
#include <boost/utility/string_ref.hpp>

namespace run {

/*
data forms:
  - null
  - boolean
  - integer
  - string
  - array
  - datatype
  - instance

 */

struct Cell;

struct Object
{
	enum {
		Instance =  0x00,
		Array =     0x01,
		Bool =      0x06,  True = 0x02, False = 0x04,
		String =    0x08,
		Datatype =  0x10,
        DatatypeNoInst = 0x20,

		// static objects are never freed during GC
		Static = 0x80,
	};

	uint8_t type;
	uint16_t size;
	char data[0];

    inline Cell* data_as_cells () const { return (Cell*) data; }
};

struct Cell
{
	inline constexpr Cell (Object* o = nullptr)
		: obj(o)
	{}

	Object* obj;


	/* predicates */

	// what kind of representation?
	inline bool is_integer () const { return uintptr_t(obj) & 1; }
	inline bool is_null () const { return obj == nullptr; }
	inline bool is_object () const { return !is_null() && !is_integer(); }

	// what kind of object, assuming is_object() has already been checked
	inline bool is_string () const    { return obj->type & Object::String; }
	inline bool is_bool () const      { return obj->type & Object::Bool; }
	inline bool is_datatype () const  { return obj->type & Object::Datatype; }
	inline bool is_array () const     { return obj->type & Object::Array; }
	inline bool is_instance () const  { return obj->type == 0x00 || obj->type == Object::Static; }
	inline bool is_static () const    { return obj->type & Object::Static; }
	inline bool has_children () const { return obj->type <= 0x1; }
    inline bool can_make_instances () const
    {
        return (obj->type & Object::Datatype)
            && !(obj->type & Object::DatatypeNoInst);
    }

	/* accessors */

	// when is_integer() is true
	inline Fixnum integer () const
	{
		return intptr_t(obj) >> 1;
	}

	// when is_string() is true
	inline boost::string_ref string () const
	{
		return boost::string_ref(obj->data, obj->size);
	}

    // whenever
    inline bool boolean () const
    {
        return obj != false_object.obj;
    }

	// when is_object() and has_children() are both true
	// object type for iterating over & accessing object's children
	struct CellChildren
	{
		CellChildren ()
			: begin_(nullptr), end_(nullptr)
		{}
		CellChildren (Cell* b, Cell* e)
			: begin_(b)
			, end_(e)
		{}
		inline Cell* begin () const { return begin_; }
		inline Cell* end () const { return end_; }
		inline size_t size () const { return end_ - begin_; }
		inline Cell operator[] (size_t i) const { return begin_[i]; }
	private:
		Cell* begin_;
		Cell* end_;
	};
	inline CellChildren children () const
	{
        auto size = obj->size / sizeof(Cell);
		return CellChildren(obj->data_as_cells(),
							obj->data_as_cells() + size);
	}

    Cell get_type () const;


	/* singletons */
	static Cell null_object;
	static Cell true_object;
	static Cell false_object;

	// universal datatypes
	static Cell int_type;
	static Cell bool_type;
	static Cell string_type;
	static Cell array_type;
	static Cell type_type;


    /* misc */

    inline static Cell nil () { return Cell(); }

	inline static Cell from_fixnum (Fixnum fx)
	{
		return Cell((Object*) ((fx << 1) | 1));
	}
};


}
