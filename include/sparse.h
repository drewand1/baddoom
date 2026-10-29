#pragma once

#include <vector>

namespace kE {
	template <typename T>
	class SparseSet {
	public:
		explicit SparseSet(int maxSize) {
			if (maxSize <= 0)
				return;

			idToI.assign(maxSize, -1);
			iToId.reserve(maxSize);
			data.reserve(maxSize);
		}

		int idToIndex(int id) const {
			if (id < 0 || id > idToI.size() - 1)
				return -1;

			return idToI[id];
		}

		int indexToId(int index) const {
			if (index > data.size() - 1)
				return -1;

			return iToId[index];
		}

		size_t getSize() const {
			return data.size();
		}

		T& getFromId(int id) {
			return data[idToI[id]];
		}

		T& getFromIndex(int index) {
			return data[index];
		}

		T& add(int id, const T& item) {
			//if (idToI[id] != -1)
				//return 0;

			iToId.push_back(id);
			data.push_back(item);
			idToI[id] = (int)data.size() - 1;

			return data[data.size() - 1];
		}

		void removeId(int id) {
			if (id > idToI.size() - 1)
				return;

			removeIndex(idToI[id]);
		}

		void removeIndex(int index) {
			int id = iToId[index];

			idToI[id] = -1;
			iToId.erase(iToId.begin() + index);
			data.erase(data.begin() + index);

			for (int i = index; i < iToId.size(); i++)
				idToI[iToId[i]]--;
		}

	private:
		std::vector<int> idToI;
		std::vector<int> iToId;
		std::vector<T> data;
	};
}