//
// Created by kore on 10/6/21.
//

#ifndef kNN_Client_KNN_AC_H
#define kNN_Client_KNN_AC_H

#include <iostream>
#include <vector>
#include <algorithm>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "SetupPhase.h"

class KNN_AC {
public:
    int n, d;

    SetupPhase* setup;
    MyTime* ktime;

    char const * file_path_his = "history_record.log";

    std::vector<double> *labels1;
    std::vector<std::vector<double> > *images1;//测试集

    std::vector<std::vector<hss_input_p> > *Xi1; //input
    std::vector<std::vector<hss_input_p> > *Xi2;

    int number_of_clients = 10;

    KNN_AC(std::vector<std::vector<double> >* testing_data, std::vector<double>* testing_labels,
            TrainingParams params, std::vector<std::vector<hss_input_p> >* input_Xi1, std::vector<std::vector<hss_input_p> >* input_Xi2,
            SetupPhase* input_setup, MyTime* input_ktime){
        this->n = params.n;
        this->d = params.d;

        this->images1 = testing_data;
        this->labels1 = testing_labels;

        this->Xi1 = input_Xi1;
        this->Xi2 = input_Xi2;


        //for(int i = 0; i < 8; i++)countOp[i]=0;

        for (int i = 0; i < (*images1).size(); i++) {
            for (int j = 0; j < (*images1)[0].size(); j++) {
                (*images1)[i][j] = ((*images1)[i][j] > 0) ? 1 : 0;
            }
        }


        std::cout << "Client Prepare Complet. " << std::endl;

        this->setup = input_setup;

        this->ktime = input_ktime;

        Encypt(test_number);
        //Share();
        std::cout << "Encrypt Complet. " << std::endl;

        for(int client_index=0; client_index < number_of_clients; client_index++){
            WriteQuery(client_index);
        }
        std::cout << "Write Complet. " << std::endl;

    }

    void Encypt(int number_of_test);
    //void Share(int index);

    int WriteQuery(int index);


};


#endif //kNN_Client_KNN_AC_H
