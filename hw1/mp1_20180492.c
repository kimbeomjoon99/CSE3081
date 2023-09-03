#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
int row, col;
int **arr;
int **arr_sum;
int *arr_dp;
#define inf 999999999

int func_1(){
    int ans = -inf, i, j, k, l, m, n;
    for (i = 0; i < row; i++)
        for (j = 0; j < col; j++)
            for (k = i; k < row; k++)
                for (l = j; l < col; l++){
                    int sum = 0;
                    for (m = i; m <= k; m++){
                        for (n = j; n <= l; n++){
                            sum += arr[m][n];
                        }
                    }
                    if (sum > ans) ans = sum;
                }
    return ans;
}

int func_2(){ 
    int ans = -inf, i, j, k, l;
    for (i = 0; i < row; i++)
        for (j = 0; j < col; j++)
            for (k = i; k < row; k++)
                for (l = j; l < col; l++){
                    int sum;
                    if (!k && !l) sum = arr_sum[k][l];
                    else if (!k) sum = arr_sum[k][l] - arr_sum[k][l - 1];
                    else if (!l) sum = arr_sum[k][l] - arr_sum[k - 1][l];
                    else if (i && j) sum = arr_sum[k][l] - arr_sum[i - 1][l] - arr_sum[k][j - 1] + arr_sum[i - 1][j - 1];
                    else sum = arr_sum[k][l];
                    if (sum > ans) ans = sum;
                }
    return ans;
}

int func_3(){
    int ans = -inf, i, j, k;
    for (i = 0; i < col; i++){
        for (j = 0; j < row; j++)
            arr_dp[j] = 0;
        for (j = i; j < col; j++){
            for (k = 0; k < row; k++)
                arr_dp[k] += arr[k][j];
            int sum = 0, Max = 0, posflag = 0;
            for (k = 0; k < row; k++){
                sum += arr_dp[k];
                if (sum > Max) Max = sum;
                else if (sum < 0){
                    sum = 0;
                    posflag++;
                }
            }
            if (!Max){
                for (k = 0; k < row; k++)
                    if (arr_dp[k] > ans) ans = arr_dp[k]; 
            }
            else if (Max > ans) ans = Max;
        }
    }
    return ans;
}

int main(int argc, char* argv[]){
    if (argc != 3){
        printf("usage: ./mp1_20180492 input_filename algorithm_index\n");
        return 1;
    }
    else if (argv[2][0] < 49 || argv[2][0] > 51){
        printf("algorithm index must be 1 or 2 or 3\n");
        return 1;
    }

    double start = (double)clock() / CLOCKS_PER_SEC;

    int i, j;
    int command = argv[2][0] - 48;
    FILE* input = fopen(argv[1], "r");
    char resfilename[10000] = "result_";
    strcat(resfilename, argv[1]);
    FILE* output = fopen(resfilename, "w");
    
    fscanf(input, "%d %d", &row, &col);
    arr = (int**)malloc(row * sizeof(int*));
    arr_sum = (int**)malloc(row * sizeof(int*));
    arr_dp = (int*)malloc(sizeof(int) * row);
    for (i = 0; i < row; i++){
        arr[i] = (int*)malloc(col * sizeof(int));
        arr_sum[i] = (int*)malloc(col * sizeof(int));
    }
    for (i = 0; i < row; i++)
        for (j = 0; j < col; j++)
            fscanf(input, "%d", &arr[i][j]);
    arr_sum[0][0] = arr[0][0];
    for (i = 0; i < row; i++){
        for (j = 0; j < col; j++){
            if (!i && !j) continue;
            if (!i) arr_sum[i][j] = arr[i][j] + arr_sum[i][j - 1];
            else if (!j) arr_sum[i][j] = arr[i][j] + arr_sum[i - 1][j];
            else arr_sum[i][j] = arr[i][j] + arr_sum[i - 1][j] + arr_sum[i][j - 1] - arr_sum[i - 1][j - 1];
        }
    }

    int res;
    
    if (command == 1) res = func_1();
    else if (command == 2) res = func_2();
    else res = func_3();
    double end = (double)clock() / CLOCKS_PER_SEC;
    double t = (double)(end - start) * (double)1000;

    fprintf(output, "%s\n", argv[1]);
    fprintf(output, "%d\n", command);
    fprintf(output, "%d\n", row);
    fprintf(output, "%d\n", col);
    fprintf(output, "%d\n", res);
    fprintf(output, "%.3lf\n", t);

    for (i = 0; i < row; i++){
        free(arr[i]);
        free(arr_sum[i]);
    }
    free(arr);
    free(arr_sum);
    free(arr_dp);

    fclose(input);
    fclose(output);

    return 0;
}
