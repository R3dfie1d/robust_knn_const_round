//
// Created by kore on 9/12/21.
//

#ifndef kNN_Client_KNN_SINGLE_H
#define kNN_Client_KNN_SINGLE_H


#include <iostream>
#include <vector>
#include <algorithm>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "SetupPhase.h"








class KNN_single {
public:
    int n, d;

    int server_id;


    //SetupPhase* setup;
    hss_men_p * mof11;
    hss_pk_p *hpk;
    hss_ek_p* hek0;
    MyTime* ktime;


/*
    std::vector<double>labels;
    std::vector<std::vector<double> >images;//训练集
    std::vector<double>labels1;
    std::vector<std::vector<double> >images1;//测试集
*/
    std::vector<hss_input_p> *Xw1; //input
    std::vector<std::vector<hss_input_p> > *Xi1; //input

    std::vector<hss_men_p> Xw1m; //men
    std::vector<hss_men_p> Xi1m; //men

    mpz_t (*Yw1)[train_number];
    //mpz_t Yw2[train_number];

    struct node qP[train_number+100], qP_recover[train_number+100];

    struct node_paillier (*qX1)[train_number+100];


    int a[20] = {0};
    int a_r[20] = {0};
    int flag_see = 1;

    KNN_single(std::vector<std::vector<hss_input_p> >* training_data_s, std::vector<hss_input_p>* testing_data, mpz_t (*input_Yw1)[train_number],
               hss_men_p * input_Mof1, hss_pk_p *input_hpk, hss_ek_p* input_ek, node_paillier (*input_qX1)[train_number+100], TrainingParams params, MyTime* ktime, int server_id){
        //hss_men_p* Mof1, hss_input_p* Ix, hss_ek_p* ek, paillier_public_key* pub, int serverId
        this->n = params.n;
        this->d = params.d;

        this->server_id = server_id;

        this->Xi1 = training_data_s;
        this->Xw1 = testing_data;

        //this->setup = setup;
        this->mof11 = input_Mof1;
        this->hpk = input_hpk;
        this->hek0 = input_ek;

        this->ktime = new MyTime();

        this->qX1 = input_qX1;

        this->Yw1 = input_Yw1;

        mpz_t num_tmp;
        mpz_init(num_tmp);

        for(int t=0;t<attribute_number;t++){


            //ktime->Time_begin(12);

            hss_men_p xnum1;
            men_share_init(&xnum1);  //men
            convert_shares_paillier(&xnum1, mof11, &(*Xw1)[t], hek0, hpk->pk, server_id);
            Xw1m.push_back(xnum1);
            //&setup->mof11, &setup->mof12, &setup->hpk, &setup->hek0, &setup->hek1,
            if(flag_see == 1){
                std::cout << "size of Xw1 is " << sizeof((*Xw1)[t]) << std::endl;
                std::cout << "size of mpz_t is " << sizeof(num_tmp) << std::endl;
                flag_see = 0;
            }


            //ktime->Time_begin(12);
            hss_men_p ynum1;
            men_share_init(&ynum1);  //men
            //men_share_init(&ynum2); //men of sum
            //ktime->Time_end(12);

            //fromINTtoMEN(&ynum1, &ynum2, &setup->mof11, &setup->mof12, &setup->hpk, &setup->hek0, &setup->hek1, num_tmp, ktime);
            //Xi1m.push_back(ynum1);
            convert_shares_paillier(&ynum1, mof11, &(*Xi1)[0][t], hek0, hpk->pk, server_id);
            Xi1m.push_back(ynum1);
            //Xi2m.push_back(ynum2);

        }

        //std::cout << "The element number of Xw1m is " << Xw1m.size() << std::endl;

        start_knn_origin();
        //newKNN();

        //send distances to another server

        //Time_result();
    }

    void start_knn();
    void start_knn_origin();
    int newKNN(int number);
    //void Time_result();

};


#endif //kNN_Client_KNN_SINGLE_H
