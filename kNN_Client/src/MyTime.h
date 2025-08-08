//
// Created by kore on 9/11/21.
//

#ifndef kNN_Client_MYTIME_H
#define kNN_Client_MYTIME_H

#include <iostream>
#include <ctime>
#include <string>
#include <fstream>

class MyTime {
public:

    clock_t this_second, server_time_0=0, server_time_1=0, clients_time=0, controller_time=0;

    char * time_s = new char[1024];

    MyTime(){
        time_t now;
        this->this_second = clock();
        time_s = ctime(&(now));
    }



    int TimeRecorded(std::string info, char const * file_path);

    int TimeRecorded(std::string info, char const * file_path, int type);

    int Time_begin(int role);

    int Time_end(int role);

    int Time_report();

};


#endif //kNN_Client_MYTIME_H
