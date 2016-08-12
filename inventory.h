#ifndef INVENTORY_H
#define INVENTORY_H

#include "general.h"
#include <vector>

using inventory_entry_t = std::pair<ItemType, unsigned int>;

class Inventory {
	private:

		inventory_entry_t inv[26*2];

		inline int chartoindex(const char& c) const {
			if (c >= 'a' && c <= 'z')
				return c - 'a';
			if (c >= 'A' && c <= 'Z')
				return c - 'A' + 26;
			return -1;
		}

		inline char indextochar(const int& i) const {
			if (i < 0)
				return '\0';
			if (i < 26)
				return 'a' + i;
			if (i < 26 * 2)
				return 'A' + (i - 26);
			return '\0';
		}

	public:
		Inventory();

		Inventory(const Inventory& other);

		Inventory& operator=(const Inventory& other);


		inline unsigned int total() const {
			unsigned int c = 0;
			for (int i = 0; i < 26 * 2; i++)
				c += inv[i].second;
			return c;
		}

		inline unsigned int total(ItemType item) const {
			unsigned int c = 0;
			for (int i = 0; i < 26 * 2; i++)
				if (inv[i].first == item)
					c += inv[i].second;
			return c;
		}

		inline bool has(ItemType item, unsigned int count = 1) const {
			return total(item) >= count;
		}

		bool add(ItemType item, unsigned int count = 1);

		bool remove(ItemType item, unsigned int count = 1);
};


#endif
