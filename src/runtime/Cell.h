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

		// static objects are never freed during GC
		Static = 0x80,
	};

	uint8_t type;
	uint16_t size;
	char data[0];
	
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
	inline bool is_nil () const { return obj == nullptr; }
	inline bool is_object () const { return !is_nil() && !is_integer(); }

	// what kind of object, assuming is_object() has already been checked
	inline bool is_string () const    { return obj->type & Object::String; }
	inline bool is_bool () const      { return obj->type & Object::Bool; }
	inline bool is_datatype () const  { return obj->type & Object::Datatype; }
	inline bool is_array () const     { return obj->type & Object::Array; }
	inline bool is_instance () const  { return obj->type == 0x00; }
	inline bool is_static () const    { return obj->type & Object::Static; }
	inline bool has_children () const { return obj->type <= 0x1; }

	
	/* accessors */

	// when is_integer() is true
	Fixnum integer () const
	{
		return intptr_t(obj) >> 1;
	}
	
	// when is_string() is true
	boost::string_ref string () const
	{
		return boost::string_ref(obj->data, obj->size);
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
		return CellChildren(((Cell*) obj->data),
							((Cell*) obj->data) + (obj->size / sizeof(Cell)));
	}


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

	inline static constexpr Cell from_fixnum (Fixnum fx)
	{
		return Cell((Object*) ((fx << 1) | 1));
	}
};

}
