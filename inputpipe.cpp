#include <iostream>
#include <stdio.h>
#include <sys/select.h>
#include "inputpipe.h"
#include <cstring>
#include <cstdlib>

using namespace std;


/*
int main(int argc, char *argv[])
{
    float pos[3];
    float rot[3];
    while(1)
    {
        if(read_marker_data(pos, rot))
        {
            cout << pos[0] << endl;
        }
    }
}
*/
int read_marker_data(float* position, float* rotation)
{
    if(data_available())
    {
        cout << "tryina slurp" << endl;
        string data;
        getline(cin, data);
        
        const char* inputString = data.c_str();
        //copy string
        char* copy = new char[data.length() + 1];
        strcpy(copy, inputString);
        //now we can strtok this
        
        const char* token = " ";
        //gross unsafe slorgs
        position[0] = atof(strtok(copy, token));
        position[1] = atof(strtok(NULL, token));
        position[2] = atof(strtok(NULL, token));
        
        rotation[0] = atof(strtok(NULL, token));
        rotation[1] = atof(strtok(NULL, token));
        rotation[2] = atof(strtok(NULL, token));

        //delete the copy space stuff
        delete copy;

        return 1;
    }
    else
    {
        return 0;
    }
}


timeval select_timeout;

int data_available()
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds); 
    select(1, &fds, NULL, NULL, &select_timeout);
    return FD_ISSET(0, &fds);
}
