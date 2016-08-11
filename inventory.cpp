#include "inventory.h"

Inventory::Inventory() {
	for (int i = 0; i < 26*2; i++)
		inv[i] = std::make_pair(ItemType::NONE, 0);
}

Inventory::Inventory(const Inventory& other) {
	for (int i = 0; i < 26*2; i++)
		this->inv[i] = other.inv[i];
}

Inventory& Inventory::operator=(const Inventory& other) {
	if (this != &other) {
		for (int i = 0; i < 26*2; i++)
			this->inv[i] = other.inv[i];
	}

	return *this;
}

bool Inventory::add(ItemType item, unsigned int count) {
	for (int i = 0; i < 26 * 2; i++)
		if (inv[i].first == item){
			inv[i].second += count;
			return true;
		}
	for (int i = 0; i < 26 * 2; i++)
		if (inv[i].first == ItemType::NONE || inv[i].second == 0) {
			inv[i].first = item;
			inv[i].second = count;
			return true;
		}
	return false;
}

bool Inventory::remove(ItemType item, unsigned int count) {
	// If there aren't enough, return false
	if (total(item) < count)
		return false;
	for (int i = 0; i < 26 * 2; i++) 
		if (inv[i].first == item) {
			if (inv[i].second < count) {
				// Should only happen if inventory is fragmented
				count -= inv[i].second;
				inv[i].second = 0;
			} else {
				inv[i].second -= count;
				// Should exit here if there were enough
				return true;
			}
		}
	// This should never happen
	fprintf(stderr, "Something funny happened when calling Inventory::remove.\nitem = %d; count = %d\n", (int)item, count);

	return true;
}
