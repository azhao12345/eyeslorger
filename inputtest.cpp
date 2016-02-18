#include <iostream>
#include <stdio.h>
#include <sys/select.h>
#include "inputtest.h"

using namespace std;


int main(int argc, char *argv[])
{
    int reads = 0;
    int nonreads = 0;
    while(1)
    {
        string value;
        if(data_available())
        {
            cout << "reading in" << endl;
            getline(cin, value);
            reads++;
            cout << reads << endl;
        }
        else
        {
            nonreads++;
        }
        if(!(nonreads % 1000000))
        {
            cout << nonreads << endl;
        }
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
