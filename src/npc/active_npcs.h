/*
 * TheXTech - A platform game engine ported from old source code for VB6
 *
 * Copyright (c) 2009-2011 Andrew Spinks, original VB6 code
 * Copyright (c) 2020-2022 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#ifndef ACTIVE_NPCS_H
#define ACTIVE_NPCS_H

#include <cstdint>
#include "global_constants.h"

// active NPCs are stored in a sparse sorted doubly linked list
class ActiveNPCs_t
{
	static constexpr uint16_t NONE = 0;
	static constexpr uint16_t BEGIN = 0;
	static constexpr uint16_t END = (uint16_t)-1;

	struct Entry
	{
		uint16_t prev;
		uint16_t next;
	};

	Entry m_data[maxNPCs+2];

public:
	class iterator
	{
	private:
		const ActiveNPCs_t& parent;
		uint16_t i;
	public:
		inline iterator(const ActiveNPCs_t& parent, uint16_t i): parent(parent), i(i) {}
		inline iterator operator++()
		{
			this->i = parent.m_data[this->i].next;
			return *this;
		}
		inline bool operator!=(const iterator& other) const
		{
			return this->i != other.i;
		}
		inline uint16_t operator*() const
		{
			return this->i;
		}
	};

    // inline ActiveNPCs_t()
    // {
    //     reset();
    // }

	inline iterator begin() const
	{
		return ++iterator(*this, BEGIN);
	}

	inline iterator end() const
	{
		return iterator(*this, END);
	}

	inline void reset()
	{
		m_data[BEGIN].next = END;
		for(uint16_t i = 1; i < maxNPCs+2; i++)
			m_data[i].next = NONE;
	}

	inline void insert(uint16_t index)
	{
		// invalid
		if(index == 0 || index >= maxNPCs+2)
			return;

		// already inserted
		if(m_data[index].next != NONE)
			return;

		uint16_t prev = index - 1;
		while(prev > 0 && !m_data[prev].next)
			prev--;

		uint16_t next = m_data[prev].next;
		m_data[prev].next = index;

		m_data[index].prev = prev;
		m_data[index].next = next;

		if(next != END)
			m_data[next].prev = index;
	}

	inline void remove(uint16_t index)
	{
		// invalid
		if(index == 0 || index >= maxNPCs+2)
			return;

		// already removed
		if(m_data[index].next == NONE)
			return;

		uint16_t prev = m_data[index].prev;
		uint16_t next = m_data[index].next;
		m_data[index].next = NONE;

		m_data[prev].next = next;
		if(next != END)
			m_data[next].prev = prev;
	}
};

extern ActiveNPCs_t ActiveNPCs;

#endif // #ifndef ACTIVE_NPCS_H