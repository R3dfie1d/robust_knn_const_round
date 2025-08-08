//
// Created by kore on 10/6/21.
//

#include "KNN_AC.h"
using namespace std;

void KNN_AC::Encypt(int number_of_test){

    cout << "Load testing data..." << endl;

    mpz_t num_tmp;
    mpz_init(num_tmp);


    mpz_t numw;
    mpz_init(numw);



    cout << "Loading training samples... " << endl;
    for (int i = 0; i < number_of_clients; i++)  //for each train sample
    {
        //cout << "Sample " << i << "," << endl;
        vector<hss_input_p> temp_vec1;
        vector<hss_input_p> temp_vec2;

        //cout << "Sample load:";
        for (int t = 0; t < attribute_number; t++) { //read a train samples t to Xi
            /*
            hss_input_p inum1;
            hss_input_p inum1s;
            mpz_set_ui(num_tmp, (*images1)[i][t]);
            //if((*images)[i][t] != (*images1)[test_number][t] ) cout << i << "," << t << "; " ;
            if((*images)[i][t] != 0 ) cout << i << "," << t << "; " ;
            input_paillier(&inum1, &setup->hpk, num_tmp);
            input_paillier(&inum1s, &setup->hpk, num_tmp);
            input_copy_pailliar(&inum1s, &inum1);

            Xw1->push_back(inum1);
            Xw2->push_back(inum1s);
*/

            hss_input_p inum2;
            hss_input_p inum2s;
            mpz_set_ui(num_tmp, (*images1)[i][t]);

            input_paillier(&inum2, &setup->hpk, num_tmp);
            input_paillier(&inum2s, &setup->hpk, num_tmp);
            //cout << "1" << endl;
            input_copy_pailliar(&inum2s, &inum2);
            //cout << "2" << endl;
            temp_vec1.push_back(inum2);
            temp_vec2.push_back(inum2s);
            //cout << "3" << endl;
        }
        //cout << "0" << endl;
        Xi1->push_back(temp_vec1);
        Xi2->push_back(temp_vec2); //?
        //cout << "1" << endl;
        //cout << "  (" << i << endl;
/*
        cout << "X: " << (*images)[i][780] << endl;
        cout << "X: " << (*images)[i][781] << endl;
        cout << "X: " << (*images)[i][782] << endl;
        cout << "X: " << (*images)[i][783] << endl;
*/

    }
}

int KNN_AC::WriteQuery(int index){
    unsigned int result_flag, sum_out = 0;

    FILE * fp_out = NULL;
    char * output_file_name = new char[1024];
    sprintf(output_file_name, "query_%d.data", index);
    //string file_name = "query_" + index + ".data";
    fp_out = fopen(output_file_name, "w+");


    for (int t = 0; t < attribute_number; t++) { //read a train samples t to Xi

        result_flag = mpz_out_raw(fp_out, (*Xi1)[index][t].encx);
        if(result_flag == 0){
            cerr << "Unable to write!" << endl;
            return -1;
        }else{
            sum_out += result_flag;
        }

        for(int j=0; j<setup->KeySize; j++){
            result_flag = mpz_out_raw(fp_out, (*Xi1)[index][t].encdx[j]);
            if(result_flag == 0){
                cerr << "Unable to write!" << endl;
                return -1;
            }else{
                sum_out += result_flag;
            }
        }

    }

    cout << "Result: Write " << sum_out << " bytes." << endl;
    fclose(fp_out);

    return 0;
}