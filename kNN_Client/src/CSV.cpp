//
// Created by kore on 12/26/21.
//

#include "CSV.h"

void find(float a[],int m, float *min, float *max){
    (*min)=a[0];
    (*max)=a[0];

    for(int i=0;i<m;i++){
        if(a[i]>(*max)){
            (*max)=a[i];
            continue;
        }else if(a[i]<(*min)){
            (*min)=a[i];
            continue;
        }
    }
}

int CSV::pre_process(float (*data)[attribute_number]){
    float vec[train_number+100];
    float min_value = 0, max_value = 0;

    for(int i=0; i<attribute_number; i++){
        for(int j=0; j<train_number; j++){
            vec[j] = data[j][i];
        }
        find(vec, train_number, &min_value, &max_value);
        for(int j=0; j<train_number; j++){
            data[j][i] = (data[j][i] - min_value)/(max_value - min_value);
        }
        std::cout << min_value << ", " << max_value << std::endl;
    }
    return 0;
}

int CSV::pre_process_int(float (*data)[attribute_number]){
    //float vec[train_number+100];
    //float min_value = 0, max_value = 0;

    for(int i=0; i<attribute_number; i++){
        for(int j=0; j<train_number+test_number; j++){
            data[j][i] = (data[j][i] * 100);
        }
        //std::cout << min_value << ", " << max_value << std::endl;
    }
    return 0;
}