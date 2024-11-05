#include <iostream>
#include <random>
#include <future>
#include <chrono>

using namespace std;


bool multithread = true;

// l -left, m - middle, r - right
void merge(int* arr, int l, int m, int r)
{
    //Вычисляем размер левой и правой части массивов
    int nl = m - l + 1;
    int nr = r - m;

    //Создаем временные массивы
    int* left = new int[nl];
    int* right = new int[nr];

    //Копируем данные во временные массивы
    for (int i = 0; i < nl; i++)
        left[i] = arr[l + i];
    for (int j = 0; j < nr; j++)
        right[j] = arr[m + j + 1];

    //Инициализируем индексы

    int i = 0, j = 0;
    int k = l; //начало левой части

    //Записываем минимальные элементы обратно во входной массив
    while (i < nl && j < nr)
    {
        if (left[i] < right[j])
        {
            arr[k++] = left[i++];
        }
        else
        {
            arr[k++] = right[j++];
        }
    }

    //Записываем оставшиеся элементы левой части

    while (i < nl)
        arr[k++] = left[i++];

    //Записываем оставшиеся элементы правой части

    while (j < nr)
        arr[k++] = right[j++];

    delete[] left;
    delete[] right;
}

/*
* int* arr - сортируемый массив
* int l - начало сортируемой части
* int r - конец сортируемой части
*/

void mergeSort(int* arr, int l, int r)
{
    if (l >= r) return; //выход из рекурсии

    int m = (l + r - 1) / 2;
    if (multithread && (m - l > 10000))
    {

        // если элементов в левой части больше чем 10000
        // вызываем асинхронно рекурсию для правой части
        future<void> f = async(launch::async, [&]() {
            mergeSort(arr, l, m);
        });
        mergeSort(arr, m + 1, r);
        f.wait();

    }
    else
    {
        // запускаем обе части синхронно
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
    }
    merge(arr, l, m, r);
}

int main()
{
    srand(0);
    int arr_size = 10000000;

    int* arr = new int[arr_size];
    for (long i = 0; i < arr_size; i++)
    {
        arr[i] = rand() % 50000;
    }
    time_t start, end;
    time(&start);
    mergeSort(arr, 0, arr_size - 1);
    time(&end);
    double seconds = difftime(end, start);
    printf("Multithread: %f seconds\n", seconds);
    for(long i=0;i<arr_size-1; i++) {
        if (arr[i] > arr[i + 1]) {
            cout << "Unsorted" << endl;
            break;
        }
    }

    for(long i=0;i<arr_size; i++) {
        arr[i] = rand() % 50000;
    }

    multithread = false;
    time(&start);
    mergeSort(arr, 0, arr_size - 1);
    time(&end);
    seconds = difftime(end, start);
    printf("Single thread: %f seconds\n", seconds);
    delete[] arr;
    return 0;
}
