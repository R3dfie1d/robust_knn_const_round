//
// Created by kore on 9/8/21.
//

#ifndef kNN_Client_READDATA_H
#define kNN_Client_READDATA_H

#include <iostream>
#include <vector>
#include <fstream>

int reverse_int(int i);
void read_MNIST_data(bool train, std::vector<std::vector<double> > &vec, int& number_of_images, int& number_of_features, int NUM_IMAGES);
void read_MNIST_labels(bool train, std::vector<double> &vec, int NUM_IMAGES);
void read_LIVER_data_label(bool train, std::vector<std::vector<double> > &data, std::vector<double> &label, std::vector<std::vector<double> > &test_data, std::vector<double> &test_label, int& number_of_images, int& number_of_features, int NUM_IMAGES);
void read_csv_data_label(bool train, std::vector<std::vector<long> > &data, std::vector<long> &label, std::vector<std::vector<long> > &test_data, std::vector<long> &test_label, char path[]);
#endif //kNN_Client_READDATA_H
