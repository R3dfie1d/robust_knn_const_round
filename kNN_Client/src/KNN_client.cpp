//
// Created by kore on 9/14/21.
//

#include "KNN_client.h"
#include <string>

using namespace std;


void KNN_client::Encypt(int predict_number, int number_of_train){

    cout << "Load testing data..." << endl;

    mpz_t num_tmp;
    mpz_init(num_tmp);
    mpz_set_ui(num_tmp, 0);



    mpz_t numw;
    mpz_init(numw);


    unsigned int result_flag, sum_out = 0;

    FILE * fp_out = NULL;
    fp_out = fopen("DO_HSS_Input_Data.data", "w+");

    std::cout << "Check digits size:" << this->setup->KeySize << std::endl;

    cout << "Loading training samples... " << endl;
    hss_input_p inum2;
    input_paillier(&inum2, &setup->hpk, num_tmp);

    for (int i = 0; i < train_number; i++)  //for each train sample
    {
        //cout << "Sample " << i << "," << endl;
        for (int t = 0; t < attribute_number-1; t++) { //read a train samples t to Xi

            mpz_set_ui(num_tmp, (*images)[i][t]);

            input_paillier_value(&inum2, &setup->hpk, num_tmp);


            if(i ==0 and t == 0){
                //check:
                cout << "Keysize: " << sizeOfDigits << endl;
                cout << "Check input values of [0][0]: " << endl;
                cout << "> " << inum2.encx << " <" <<endl;

                cout << "encdx:***" << endl;
                for (int j = 0; j < sizeOfDigits; j++) {
                    cout << inum2.encdx[j] << ", ";
                }
                cout << endl;

            }

            //////
            result_flag = mpz_out_raw(fp_out, inum2.encx);
            if(result_flag == 0){
                cerr << "Unable to write!" << endl;
                return;
            }else{
                sum_out += result_flag;
            }

            for(int j=0; j<(sizeOfDigits); j++){
                result_flag = mpz_out_raw(fp_out, inum2.encdx[j]);
                if(result_flag == 0){
                    cerr << "Unable to write!" << endl;
                    return;
                }else{
                    sum_out += result_flag;
                }
            }





        }

        mpz_set_ui(num_tmp, (int) (*labels)[i]);

        input_paillier_value(&inum2, &setup->hpk, num_tmp);  //can be upgrade

        //////
        result_flag = mpz_out_raw(fp_out, inum2.encx);
        if(result_flag == 0){
            cerr << "Unable to write!" << endl;
            return;
        }else{
            sum_out += result_flag;
        }

    }





    cout << "Result: Write " << sum_out << " bytes." << endl;
    fclose(fp_out);

    /*
    cout << "Check Xi0 encx" << endl;
    for(int k=0; k<10; k++){
        cout << (*Xi1)[0][k].encx << endl;
    }
     */
}

void KNN_client::Share(){
    //Output Xw1 Xw2 Xi1 Xi2
}

