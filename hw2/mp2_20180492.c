#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int *arr;
int size;

void s_sort(){
    int i, j;
    for (i = 0; i < size - 1; i++)
        for (j = i + 1; j < size; j++){
            if (arr[i] > arr[j]){
                int tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
}

void q_sort(int low, int high){
    if (low > high) return;
    int i = low - 1, j = low, p = arr[high];
    for ( ; j < high; j++){
        if (arr[j] <= p){
            i++;
            int tmp = arr[j];
            arr[j] = arr[i];
            arr[i] = tmp;
        }
    }
    int tmp = arr[++i];
    arr[i] = arr[high];
    arr[high] = tmp;
    q_sort(low, i - 1);
    q_sort(i + 1, high);
}

void m_sort(int low, int high){
    if (low >= high) return;
    int mid = (low + high) / 2;
    m_sort(low, mid);
    m_sort(mid + 1, high);
    int len_l = mid - low + 1;
    int len_r = high - mid;
    int *arr_l = (int*)malloc(sizeof(int) * len_l);
    int *arr_r = (int*)malloc(sizeof(int) * len_r);
    int i, j = 0, w = 0;
    for (i = 0; i < len_l; i++)
        arr_l[i] = arr[low + i];
    for (i = 0; i < len_r; i++)
        arr_r[i] = arr[mid + 1 + i];
    i = 0;
    while(i < len_l && j < len_r){
        if (arr_l[i] <= arr_r[j]){
            arr[low + i + j] = arr_l[i];
            i++;
        }
        else{
            arr[low + i + j] = arr_r[j];
            j++;
        }
    }
    if (i == len_l)
        while(j < len_r){
            arr[low + i + j] = arr_r[j];
            j++;
        }
    else
        while(i < len_l){
            arr[low + i + j] = arr_l[i];
            i++;
        }
    free(arr_l);
    free(arr_r);
}

void fq_sort(int low, int high){
    if (low >= high) return;
    if (high - low + 1 <= 15){
        for (int i = low + 1; i <= high; i++){
            int key = arr[i];
            int j = i - 1;
            while(j >= low && arr[j] > key){
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
    }
    else{
        int i = low - 1, j = low, p = arr[(low + high) / 2];
        int tmp = arr[high];
        arr[high] = arr[(low + high) / 2];
        arr[(low + high) / 2] = tmp;
        for ( ; j < high; j++){
            if (arr[j] <= p){
                i++;
                tmp = arr[j];
                arr[j] = arr[i];
                arr[i] = tmp;
            }
        }
        tmp = arr[++i];
        arr[i] = arr[high];
        arr[high] = tmp;
        fq_sort(low, i - 1);
        fq_sort(i + 1, high);
    }
}

int main(int argc, char *argv[]){
    if (argc != 3){
        printf("usage: ./mp2_20180492 input_filename algorithm_index\n");
        return 1;
    }
    else if (argv[2][0] < 49 || argv[2][0] > 52){
        printf("algorithm index must be 1 or 2 or 3 or 4\n");
        return 1;
    }

    int i;
    FILE* input = fopen(argv[1], "r");
    char resfilename[10000] = "result_";
    strcat(resfilename, argv[2]);
    strcat(resfilename, "_");
    strcat(resfilename, argv[1]);
    FILE* output = fopen(resfilename, "w");

    fscanf(input, "%d", &size);
    arr = (int*)malloc(sizeof(int) * size);
    for (i = 0; i < size; i++){
        fscanf(input, "%d", &arr[i]);
    }

    double start = (double)clock() / CLOCKS_PER_SEC;
    if (argv[2][0] == '1') s_sort();
    else if (argv[2][0] == '2') q_sort(0, size - 1);
    else if (argv[2][0] == '3') m_sort(0, size - 1);
    else if (argv[2][0] == '4') fq_sort(0, size - 1);
    double end = (double)clock() / CLOCKS_PER_SEC;
    double t = (double)(end - start);

    fprintf(output, "%s\n", argv[1]);
    fprintf(output, "%c\n", argv[2][0]);
    fprintf(output, "%d\n", size);
    fprintf(output, "%.6lf\n", t);
    for (i = 0; i < size; i++)
        fprintf(output, "%d ", arr[i]);

    fclose(input);
    fclose(output);

    free(arr);

    return 0;
}
