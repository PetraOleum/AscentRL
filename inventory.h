#ifndef INVENTORY_H
#define INVENTORY_H

#include "general.h"
#include <vector>

/// @brief Link item and number of items
using inventory_entry_t = std::pair<ItemType, unsigned int>;

/// @brief Translate char value to index
///
/// @param c The character
///
/// @return The index
inline int INV_chartoindex(const char& c) {
	if (c >= 'a' && c <= 'z')
		return c - 'a';
	if (c >= 'A' && c <= 'Z')
		return c - 'A' + 26;
	return -1;
}

/// @brief Translate index to char value
///
/// @param i The index
///
/// @return The char
inline char INV_indextochar(const int& i) {
	if (i < 0)
		return '\0';
	if (i < 26)
		return 'a' + i;
	if (i < 26 * 2)
		return 'A' + (i - 26);
	return '\0';
}

class Inventory {
	private:

		/// @brief The underlying array - 52 spots, a-z,A-Z.
		inventory_entry_t inv[26*2];


	public:
		Inventory();

		Inventory(const Inventory& other);

		Inventory& operator=(const Inventory& other);


		/// @brief Calculate the total number of all items
		///
		/// @return The total number of all items
		inline unsigned int total() const {
			unsigned int c = 0;
			for (int i = 0; i < 26 * 2; i++)
				c += inv[i].second;
			return c;
		}

		/// @brief Calculate total number of specified item in inventory
		///
		/// @param item The item
		///
		/// @return The total number of that item
		inline unsigned int total(ItemType item) const {
			unsigned int c = 0;
			for (int i = 0; i < 26 * 2; i++)
				if (inv[i].first == item)
					c += inv[i].second;
			return c;
		}

		/// @brief Whether the inventory contains an item
		///
		/// @param item The item
		/// @param count (Optional) minumum number
		///
		/// @return True if the number of item is greater than minimum
		inline bool has(ItemType item, unsigned int count = 1) const {
			return total(item) >= count;
		}

		/// @brief Add an item to the inventory
		///
		/// @param item The item
		/// @param count Count of the item
		///
		/// @return Whether it is successful
		bool add(ItemType item, unsigned int count = 1);

		/// @brief Remove an item from the inventory
		///
		/// @param item The item
		/// @param count Number of the item
		///
		/// @return Whether it is successful
		bool remove(ItemType item, unsigned int count = 1);

		inline inventory_entry_t& operator[](char index) {
			return inv[INV_chartoindex(index)];
		}

		/// @brief Expose inventory array
		///
		/// @param index Index of the array (as a char)
		///
		/// @return Reference to the array object
		inline const inventory_entry_t& operator[](char index) const {
			return inv[INV_chartoindex(index)];
		}
};


#endif
