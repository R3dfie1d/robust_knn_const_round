//
// Created by kore on 12/26/21.
//

#ifndef KNN_CLIENT_CSV_H
#define KNN_CLIENT_CSV_H

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <math.h>
#include "crypto.h"

class CSV {
public:
    FILE *fp = NULL;
    char *line,*record;
    char buffer[20450];//20450这个数组大小也要根据自己文件的列数进行相应修改。

    CSV(const char * filename, float (*data)[attribute_number], int flag){
        if(flag == 10) {

            double db1;
            int arr_index_i = 0, arr_index_j = 0;

            if ((fp = fopen(filename, "r")) != NULL) {
                fseek(fp, 173, SEEK_SET);  //定位到第二行，每个英文字符大小为1，16425L这个参数根据自己文件的列数进行相应修改。

                while ((line = fgets(buffer, sizeof(buffer), fp)) != NULL)//当没有读取到文件末尾时循环继续
                {
                    record = strtok(line, ",");
                    arr_index_j = 0;
                    while (record != NULL)//读取每一行的数据
                    {
                        if (strcmp(record, "Male") == 0) {
                            db1 = 0;
                        } else if (strcmp(record, "Female") == 0) {
                            db1 = 1;
                        } else {
                            db1 = atof(record);
                        }
                        data[arr_index_i][arr_index_j] = db1;
                        printf("%f ", db1);//将读取到的每一个数据打印出来
                        record = strtok(NULL, ",");
                        arr_index_j++;
                    }
                    printf("\n");
                    arr_index_i++;
                    if (arr_index_i > train_number) break;
                }
                fclose(fp);
                fp = NULL;
            }
        }else{
            float db1;
            int arr_index_i = 0, arr_index_j = 0;

            if ((fp = fopen(filename, "r")) != NULL) {
                fseek(fp, 0, SEEK_SET);  //定位到第二行，每个英文字符大小为1，16425L这个参数根据自己文件的列数进行相应修改。

                while ((line = fgets(buffer, sizeof(buffer), fp)) != NULL)//当没有读取到文件末尾时循环继续
                {
                    record = strtok(line, ",");
                    arr_index_j = 0;
                    while (record != NULL)//读取每一行的数据
                    {
                        db1 = atof(record);
                        data[arr_index_i][arr_index_j] = db1;
                        printf("%f ", data[arr_index_i][arr_index_j]);//将读取到的每一个数据打印出来
                        record = strtok(NULL, ",");
                        arr_index_j++;
                    }
                    printf("\n");
                    arr_index_i++;
                    if (arr_index_i > train_number+test_number) break;
                }
                fclose(fp);
                fp = NULL;
            }
        }

    }
    int pre_process(float (*data)[attribute_number]);
    int pre_process_int(float (*data)[attribute_number]);
    };


#endif //KNN_CLIENT_CSV_H
