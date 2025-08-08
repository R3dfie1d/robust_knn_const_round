//
// Created by kore on 9/14/21.
//

#ifndef kNN_Client_KNN_CLIENT_H
#define kNN_Client_KNN_CLIENT_H

#include <iostream>
#include <vector>
#include <algorithm>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "SetupPhase.h"




class KNN_client {
public:
    int n, d;

    SetupPhase* setup;
    MyTime* ktime;

    int sizeOfDigits;

    char const * file_path_his = "history_record.log";

    std::vector<long> *labels;
    std::vector<std::vector<long> > *images;//训练集
    std::vector<long> *labels1;
    std::vector<std::vector<long> > *images1;//测试集



    KNN_client(std::vector<std::vector<long> >* training_data, std::vector<long>* training_labels,
            TrainingParams params,
               SetupPhase* input_client, MyTime* input_ktime){
        this->n = params.n;
        this->d = params.d;

        this->images = training_data;
        this->labels = training_labels;


        this->sizeOfDigits = (input_client->KeySize);

        //for(int i = 0; i < 8; i++)countOp[i]=0;

//delete
/*
        for (int i = 0; i < (*images).size(); i++) {
            for (int j = 0; j < (*images)[0].size(); j++) {
                (*images)[i][j] = ((*images)[i][j] > 0) ? 3 : 1;
            }
        }
*/
        std::cout << "Client Prepare Complet. " << std::endl;

        //this->t = (params.n)/BATCH_SIZE;
        //this->alpha = params.alpha;

        this->setup = input_client;

        this->ktime = input_ktime;
/*
        mpz_t num_tmp;
        mpz_init(num_tmp);

        for (int t = 0; t < 784; t++) {

            hss_input_p inum1;
            hss_input_p inum1s;
            mpz_set_ui(num_tmp, 0);

            input_paillier(&inum1, &setup->hpk, num_tmp);
            input_paillier(&inum1s, &setup->hpk, num_tmp);
            input_copy_pailliar(&inum1s, &inum1);

            Xw1.push_back(inum1);
            Xw2.push_back(inum1s);





            hss_input_p inum2;
            hss_input_p inum2s;
            mpz_set_ui(num_tmp, 0);
            input_paillier(&inum2, &setup->hpk, num_tmp);
            input_paillier(&inum2s, &setup->hpk, num_tmp);
            input_copy_pailliar(&inum2s, &inum2);

            Xi1.push_back(inum2);
            Xi2.push_back(inum2s);


        }
    */








        Encypt(1, train_number);
        Share();
    }

    void Encypt(int predict_number, int number_of_train);
    void Share();



};


#endif //kNN_Client_KNN_CLIENT_H
