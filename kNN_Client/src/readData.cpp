//
// Created by kore on 9/8/21.
//

#include "readData.h"
#include "CSV.h"

int reverse_int(int i){
    unsigned char ch1, ch2, ch3, ch4;
    ch1 = i & 255;
    ch2 = (i >> 8) & 255;
    ch3 = (i >> 16) & 255;
    ch4 = (i >> 24) & 255;
    return ((int) ch1 << 24) + ((int) ch2 << 16) + ((int) ch3 << 8) + ch4;
}



//template <typename T>
void read_MNIST_data(bool train, std::vector<std::vector<double> > &vec, int& number_of_images, int& number_of_features, int NUM_IMAGES){
    std::ifstream file;

    double check1, check2;

    if (train == true)
        file.open("../Datasets/train-images-idx3-ubyte",std::ios::binary);
    else
        file.open("../Datasets/t10k-images-idx3-ubyte",std::ios::binary);



    if(!file){
        std::cout<<"Unable to open file";
        return;
    }
    if (file.is_open()){
        int magic_number = 0;
        int n_rows = 0;
        int n_cols = 0;
        file.read((char*) &magic_number, sizeof(magic_number));
        magic_number = reverse_int(magic_number);
        file.read((char*) &number_of_images, sizeof(number_of_images));
        number_of_images = reverse_int(number_of_images)/10;

        //number_of_images = (int) (number_of_images /5);

        if(train == true)
            number_of_images = NUM_IMAGES;
        file.read((char*) &n_rows, sizeof(n_rows));
        n_rows = reverse_int(n_rows);
        file.read((char*) &n_cols, sizeof(n_cols));
        n_cols = reverse_int(n_cols);
        number_of_features = n_rows * n_cols;
        std::cout << "Number of Images: " << number_of_images << std::endl;
        std::cout << "Number of Features: " << number_of_features << std::endl;
        int ft=1;
        for(int i = 0; i < number_of_images; ++i){
            std::vector<double> tp;
            for(int r = 0; r < n_rows; ++r)
                for(int c = 0; c < n_cols; ++c){
                    unsigned char temp = 0;
                    file.read((char*) &temp, sizeof(temp));
                    tp.push_back(((double) temp));
/*
                    check1 = (double) temp;
                    check2 = (double) ((int) temp);

                    if(check1 != check2)cout<< check1 << endl;*/
                    //if(ft){
                    //    cout<< check1 << endl;

                    //}
                }
            //ft = 0;
            vec.push_back(tp);
        }
    }
}

//template <typename T>
void read_MNIST_labels(bool train, std::vector<double> &vec, int NUM_IMAGES){
    std::ifstream file;
    if (train == true)
        file.open("../Datasets/train-labels-idx1-ubyte",std::ios::binary);
    else
        file.open("../Datasets/t10k-labels-idx1-ubyte", std::ios::binary);
    if(!file){
        std::cout << "Unable to open file";
        return;
    }
    if (file.is_open()){
        int magic_number = 0;
        int number_of_images = 0;
        file.read((char*) &magic_number, sizeof(magic_number));
        magic_number = reverse_int(magic_number);
        file.read((char*) &number_of_images, sizeof(number_of_images));
        number_of_images = reverse_int(number_of_images)/10;
        if(train == true)
            number_of_images = NUM_IMAGES;
        for(int i = 0; i < number_of_images; ++i){
            unsigned char temp = 0;
            file.read((char*) &temp, sizeof(temp));
            //vec.push_back((T) temp);
            //cout << (int) temp <<endl;

            if((double) temp == 2)
                vec.push_back((double) 0);
                //else if((T) temp == 9)
                //    vec.push_back((T) 1);
            else
                vec.push_back((double) 1);
        }
    }
}

//template <typename T>
void read_LIVER_data_label(bool train, std::vector<std::vector<double> > &data, std::vector<double> &label, std::vector<std::vector<double> > &test_data, std::vector<double> &test_label, int& number_of_images, int& number_of_features, int NUM_IMAGES){
    float data_array[train_number + 100][attribute_number];
    const char * csv_file= "ILPD.csv";
    CSV * liver_csv = new CSV(csv_file, data_array, 10);
    liver_csv->pre_process(data_array);

    for(int i=0; i<train_number; i++) {
        std::vector<double> tmp_vec;
        for (int j = 0; j < attribute_number-1; j++) {
            tmp_vec.push_back((int) 10000 * data_array[i][j]);
        }
        data.push_back(tmp_vec);
        label.push_back((int) data_array[i][attribute_number-1]);
        std::cout << std::endl;
    }

    for(int i=train_number-100; i<train_number; i++) {
        std::vector<double> tmp_vec;
        for (int j = 0; j < attribute_number-1; j++) {
            tmp_vec.push_back((int) 10000 * data_array[i][j]);
        }
        test_data.push_back(tmp_vec);
        test_label.push_back((int) data_array[i][attribute_number-1]);
        std::cout << std::endl;
    }
}

void read_csv_data_label(bool train, std::vector<std::vector<long> > &data, std::vector<long> &label, std::vector<std::vector<long> > &test_data, std::vector<long> &test_label, char path[]){
    float data_array[train_number + 100][attribute_number];
    CSV * csv = new CSV(path, data_array, 1);
    csv->pre_process_int(data_array);

    for(int i=0; i<train_number; i++) {
        std::vector<long> tmp_vec;
        for (int j = 0; j < attribute_number-1; j++) {
            //std::cout << data_array[i][j] << "; ";
            tmp_vec.push_back((long) data_array[i][j]);
        }
        //std::cout << std::endl;
        data.push_back(tmp_vec);
        //std::cout << data_array[i][attribute_number] << std::endl;
        label.push_back((long) data_array[i][attribute_number-1]);
        //std::cout << std::endl;
    }

    for(int i=train_number; i<train_number + test_number; i++) {
        std::vector<long> tmp_vec;
        for (int j = 0; j < attribute_number-1; j++) {
            tmp_vec.push_back((long) data_array[i][j]);
        }
        test_data.push_back(tmp_vec);
        test_label.push_back((long) data_array[i][attribute_number-1]);
        //std::cout << std::endl;
    }
}