#include "editor/draggable.hpp"

/* Initialise cache that stores the object being dragged */
Draggable* Draggable::s_ObjectBeingDragged = nullptr;

/* Returns true if passed object can currently be dragged */
bool Draggable::CanDrag(Draggable* object)
{
	return (object == s_ObjectBeingDragged || s_ObjectBeingDragged == nullptr);
}

/* Returns an object pointer stored in the cache */
Draggable* Draggable::GetObjectBeingDragged()
{
	return s_ObjectBeingDragged;
}