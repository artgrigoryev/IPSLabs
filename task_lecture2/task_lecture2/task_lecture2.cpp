#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_max.h>
#include <cilk/reducer_min.h>
#include <cilk/reducer_vector.h>
#include <chrono>

using namespace std::chrono;

/// ‘ункци€ ReducerMaxTest() определ€ет максимальный элемент массива,
/// переданного ей в качестве аргумента, и его позицию
/// mass_pointer - указатель исходный массив целых чисел
/// size - количество элементов в массиве
void ReducerMaxTest(int *mass_pointer, const long size)
{
	cilk::reducer<cilk::op_max_index<long, int>> maximum;
	cilk_for(long i = 0; i < size; ++i)
	{
		maximum->calc_max(i, mass_pointer[i]);
	}
	printf("\tMaximal element = %d has index = %d\n",
		maximum->get_reference(), maximum->get_index_reference());
}

void ReducerMinTest(int *mass_pointer, const long size)
{
	cilk::reducer<cilk::op_min_index<long, int>> minimum;
	cilk_for(long i = 0; i < size; ++i)
	{
		minimum->calc_min(i, mass_pointer[i]);
	}
	printf("\tMinimal element = %d has index = %d\n",
		minimum->get_reference(), minimum->get_index_reference());
}


/// ‘ункци€ ParallelSort() сортирует массив в пор€дке возрастани€
/// begin - указатель на первый элемент исходного массива
/// end - указатель на последний элемент исходного массива
void ParallelSort(int *begin, int *end)
{
	if (begin != end)
	{
		--end;
		int *middle = std::partition(begin, end, std::bind2nd(std::less<int>(), *end));
		std::swap(*end, *middle);
		cilk_spawn ParallelSort(begin, middle);
		ParallelSort(++middle, ++end);
		cilk_sync;
	}
}


void CompareForAndCilk_For(size_t& size)
{
	std::vector<int> vec;
	cilk::reducer<cilk::op_vector<int>>red_vec;

	printf("Number of elements is %d\n", size);

	high_resolution_clock::time_point t1_for = high_resolution_clock::now();

	for (size_t i = 0; i < size; ++i)
	{
		vec.push_back(rand() % 200000 + 1);
	}

	high_resolution_clock::time_point t2_for = high_resolution_clock::now();
	duration<double> duration_for = (t2_for - t1_for);
	printf("\tElapsed time for cycle FOR is: %lf seconds\n", duration_for.count());

	vec.clear();
	vec.shrink_to_fit();

	high_resolution_clock::time_point t1_cilk_for = high_resolution_clock::now();

	cilk_for(size_t k = 0; k < size; ++k)
	{
		red_vec->push_back(rand() % 200000 + 1);
	}

	high_resolution_clock::time_point t2_cilk_for = high_resolution_clock::now();
	duration<double> duration_cilk_for = (t2_cilk_for - t1_cilk_for);
	printf("\tElapsed time for cycle CILK_FOR is: %lf seconds\n\n", duration_cilk_for.count());
}

int main()
{
	srand((unsigned)time(0));

	// устанавливаем количество работающих потоков = 4
	__cilkrts_set_param("nworkers", "4");

/*
	long i;
	const long mass_size = 1000000;
	int *mass_begin, *mass_end;
	int *mass = new int[mass_size];

	for (i = 0; i < mass_size; ++i)
	{
		mass[i] = (rand() % 25000) + 1;
	}

	mass_begin = mass;
	mass_end = mass_begin + mass_size;

	printf("Number of element in massive is %d\n", mass_size);

	printf("Before ParallelSort()\n");
	ReducerMaxTest(mass, mass_size);
	ReducerMinTest(mass, mass_size);

	// timing ParallelSort()
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	ParallelSort(mass_begin, mass_end);
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double> duration = (t2 - t1);

	printf("\nElapsed time for sorting is: %lf seconds\n", duration.count());
	
	printf("\nAfter ParallelSort()\n");
	ReducerMaxTest(mass, mass_size);
	ReducerMinTest(mass, mass_size);

	delete[]mass;
*/

	size_t numbers[8] = { 1000000, 100000, 10000, 1000, 500, 100, 50, 10 };

	for (int j = 0; j < 8; ++j)
	{
		CompareForAndCilk_For(numbers[j]);
	}
	
	return 0;
}