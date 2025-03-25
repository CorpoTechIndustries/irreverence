#include <util/sort.h>

#include <platform/memory.h>

// ------------------------------
// Sort Merge

static void MergeSort_Recursive(uint32_t* list, uint32_t* temp, sort_func_t fn, size_t l, size_t r)
{
	uint32_t m = (l + (r - 1)) / 2;
	
	if (l < r) { // Exit recurse, our job is done.
		MergeSort_Recursive(list, temp, fn, l, m); // Split's Left
		MergeSort_Recursive(list, temp, fn, m + 1, r); // Split's Right

		uint32_t lLength = m - l + 1;
		uint32_t rLength = r - m;

		uint32_t* lArray = temp;
		uint32_t* rArray = &temp[rLength];

		for (size_t i = 0; i < lLength; i++) {
			lArray[i] = list[l + i];
		}
		
		for (size_t i = 0; i < rLength; i++) {
			rArray[i] = list[m + i + 1];
		}

		size_t i = 0;
		size_t j = 0;
		for (size_t k = l; k <= r; k++) {
			// if i is in bound and left[i] is less than right[j] OR j is out of bounds then add left[i] else add right[j]
			if ((i < lLength) && (j >= rLength || fn(&lArray[i], &rArray[j])))
			{
				list[k] = lArray[i];
				i++;
			} else {
				list[k] = rArray[j];
				j++;
			}
		}
		
	}
}

void Sort_Merge(uint32_t* list, size_t count, sort_func_t fn)
{
	uint32_t* temp = Sys_Calloc(count * sizeof(uint32_t)); // Array that will save the overhead of continously allocating memory

	MergeSort_Recursive(list, temp, fn, 0, count - 1);

	Sys_Free(temp);
}
