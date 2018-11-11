#include<cstdlib>
#include <fstream>
#include <string>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    if(argc!=3)
    {
        cout<<"please input grid size values in x and y\n";
        return 0;
    }
    int sizeX = atoi (argv[1]);
    int sizeY = atoi (argv[2]);

    ofstream myfile;
    //cout<<"printing to "<<iteration<<endl;
	srand (time(NULL));

    myfile.open("imag"+to_string(sizeX)+"*"+to_string(sizeY)+".pbm");
    myfile<<"P1\n";

    myfile<<sizeX<<" "<<sizeY<<endl;
    for(int a = 0; a < sizeY; a++)
    {
        for(int b = 0; b < sizeX; b++)
        {
            if(!(rand()%7))
            {
                myfile << "1";
            }
            else
            {
                myfile << "0";
            }
            if(b != sizeX-1)
            {
                myfile << " ";
            }
        }
        myfile<<endl;
    }

    return 0;
}