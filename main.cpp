#include<iostream>
#include<cstdlib>
#include <unistd.h>
#include <fstream>
#include <string>
#include <QTime>
#include<omp.h>
#include <QtGui/QImage>

using namespace std;
////home/theprogrammer/gameOfLife/imag0.pbm

//const int gridSize = 25;
void printGrid(bool **gridOne, int sizeX, int sizeY);
void determineState(bool **gridOne, int sizeX, int sizeY);
void readPBM(string fileName, bool **gridRead, int sizeX, int sizeY);
void printGridToFile(bool **gridOne, int sizeX, int sizeY, int iteration);
void readImageFile(char *fileName, bool **gridDynamic, int sizeXOriginal, int sizeYOriginal);
int main(int argc, char *argv[]){

    if(!(argc==5||argc==4))
    {
        cout<<"please input grid size values in x and y and number of threads and (m)ode\n";
        cout<<"type (i) to see the grid as it evolves\n";
        return 0;
    }
    int gridSizeX = atoi (argv[1]);
    int gridSizeY = atoi (argv[2]);
    int numThreads = atoi(argv[3]);
    char mode = '0';
    if(argc==5)
        mode = *argv[4];
    omp_set_num_threads(numThreads);
    //cout<<"threads sat\n";
    bool** gridDynamic = new bool*[gridSizeX+1];
//#pragma omp parallel for
    for(int i = 0; i < gridSizeX+1; ++i)
        gridDynamic[i] = new bool[gridSizeY+1];

    //readPBM("/home/theprogrammer/gameOfLife/imag0.pbm", gridDynamic, gridSizeX, gridSizeY);
    char fileName[]="imag1000*500.pbm";
    readImageFile(fileName, gridDynamic, gridSizeX, gridSizeY);
    //char start;
    //cout<<"start the game? (y)\n";
    //cin>>start;
    //if(start == 'y' || start == 'Y')
    {
        QTime time;
        time.start();
        if(mode=='i'||mode=='I')
        for (int i = 0;i<250;i++)
        {
            system("clear");
            printGrid(gridDynamic, gridSizeX, gridSizeY);
            determineState(gridDynamic, gridSizeX, gridSizeY);
            usleep(100000);
        }
        else
        for (int i = 0;i<=20000;i++)
        {
            //#pragma omp parallel sections default(none) shared(gridSizeX,gridSizeY,i) private(gridDynamic)
            {
            determineState(gridDynamic, gridSizeX, gridSizeY);
            if(i%1000==0)
                printGridToFile(gridDynamic, gridSizeX, gridSizeY, i);

            }
        }

        //printGrid(gridDynamic, gridSizeX, gridSizeY);
//#pragma omp parallel for
        for(int i = 0; i < gridSizeX+1; ++i) {
            delete[] gridDynamic[i];
    }
        delete[] gridDynamic;
        printf("%d",time.elapsed());
        //31660 single core
        //450 no sleep and no printing
    }

    return 0;

}

void readImageFile(char *fileName, bool **gridDynamic, int sizeXOriginal, int sizeYOriginal)
{
    QImage *img= new QImage(fileName);
    //img.load(fileName);
    //*
    if(!img)
    {
        cout<<"no image\n";
        return;
    }//*/
    int sizeX = img->width();
    int sizeY = img->height();

    if(sizeX!= sizeXOriginal||sizeY!= sizeYOriginal)
    {
        cout<<"error: size does not match\n";
        for(int i = 0; i < sizeX+1; ++i) {
            delete[] gridDynamic[i];
        }
        delete[] gridDynamic;
        exit(0);
    }
    //cout<<sizeX<<"  "<<sizeY<<endl;

     //cout<<qAlpha(img->pixel(0,0))<<endl;
     //cout<<qAlpha(img->pixel(5,0))<<endl;
    //bool** gridDynamic = new bool*[sizeX];


    for(int i =0;i<sizeX;i++)
    {
        for(int j = 0;j<sizeY;j++)
        {
            if(qGray(img->pixel(i,j))<127)
                gridDynamic[i+1][j+1] = true;
            else
                gridDynamic[i+1][j+1]=false;
        }
    }
    printGridToFile(gridDynamic,sizeX,sizeY,-1);
}


/*
                myfile.get(readtt);
                if(readtt=='\n')
                    break;//go to next line
                if(readtt=='O')
                    gridRead[a][b]=true;
                else if(readtt=='.')
                    gridRead[a][b]=false;
                else if(readtt!=' ')
                {
                    myfile.close();
                    printGrid(gridRead, sizeX, sizeY);
                    return;//end of file. anything other that the above.
                }
*/
void printGrid(bool **gridOne, int sizeX, int sizeY){
    for(int b = 1; b <sizeY; b++)
    {
        for(int a = 1; a <sizeX; a++)
        {
            if(gridOne[a][b] == true)
            {
                cout << " O ";
            }
            else
            {
                cout << " . ";
            }
        }
        cout<<endl;
    }
}

void printGridToFile(bool **gridOne, int sizeX, int sizeY, int iteration){

    ofstream myfile;
    //cout<<"printing to "<<iteration<<endl;
    myfile.open("iterationFolder/iteration_" + to_string(iteration)+".pbm");
    myfile<<"P1\n";

    myfile<<sizeX<<" "<<sizeY<<endl;
    for(int a = 0; a < sizeY; a++)
    {
        for(int b = 0; b < sizeX; b++)
        {
            if(gridOne[b+1][a+1] == true)
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
}


void copyGrid (bool **gridOne, bool **gridTwo, int sizeX, int sizeY)
{
#pragma omp parallel for collapse(1)
    for(int a =0; a < sizeX; a++)
    {
        for(int b = 0; b < sizeY; b++)
        {
            gridTwo[a][b] = gridOne[a][b];
        }
    }
}

void determineState(bool **gridOne, int sizeX, int sizeY){

    bool** gridTwo = new bool*[sizeX+1];
//#pragma omp parallel for
    for(int i = 0; i < sizeX+1; ++i)
        gridTwo[i] = new bool[sizeY+1];
    //as grid one will be changing during this opperation,
    //we need to copy it to calculate gridOne on its original state
    copyGrid(gridOne, gridTwo, sizeX, sizeY);

#pragma omp parallel for collapse(1) shared(gridOne, gridTwo, sizeX, sizeY) //schedule(static,8)
    for(int a = 1; a < sizeX; a++)
    {
        //#pragma omp parallel for
        for(int b = 1; b < sizeY; b++)
        {
            int alive = 0;
            //checking all neigbors
            if(gridTwo[a][b+1])     ++alive;
            if(gridTwo[a][b-1])     ++alive;
            if(gridTwo[a+1][b])     ++alive;
            if(gridTwo[a-1][b])     ++alive;
            if(gridTwo[a+1][b+1])   ++alive;
            if(gridTwo[a-1][b-1])   ++alive;
            if(gridTwo[a+1][b-1])   ++alive;
            if(gridTwo[a-1][b+1])   ++alive;

//#pragma omp atomic
            //#pragma omp critical

            if(alive < 2)
            {
                gridOne[a][b] = false;
            }
            else if(alive == 3)
            {
                gridOne[a][b] = true;
            }
            else if(alive > 3)
            {
                gridOne[a][b] = false;
            }

        }
    }

    //why this parallel changes every thing??
//#pragma omp parallel for
    for(int i = 0; i < sizeX+1; ++i) {
        delete[] gridTwo[i];
    }
    delete[] gridTwo;
}

/*
            for(int c = -1; c < 2; c++)
            {
                for(int d = -1; d < 2; d++)
                {
                    //to not check the cell itself
                    if(!(c == 0 && d == 0))
                    {
                        //check if the neigbor cells are alive
                        if(gridTwo[a+c][b+d])
                        {
                            ++alive;
                        }
                    }
                }
            }
            */
/*
void readPBM(string fileName, bool **gridRead, int sizeX, int sizeY)
{
    string line;
    ifstream myfile;
    myfile.open(fileName);
    //bool gridRead[gridSize+1][gridSize+1];
    char readtt[3];
    if (myfile.is_open())
    {
        for(int a = 1; a < sizeX; a++)
        {
            for(int b = 1; b < sizeY; b++)
            {
               //*
                if(myfile.read(readtt,3))
                {
                    line = line.assign(readtt);
                    if(!line.compare(" O "))
                        gridRead[a][b]=true;
                    else
                        gridRead[a][b]=false;
                }
                else
                    return;
                    //
            }
            myfile.read(readtt,1);//to skip new line
        }
        myfile.close();
        //printGrid(gridRead, sizeX, sizeY);


    }

    else
    {
        cout << "Unable to open file\n\n";
        exit(0);
    }
}*/
