using namespace std;

#include <iostream>
#include <random>

int main()
{
    const int num = 6;

    int arr[2][num];
    int start[num];
    int end[num];
    bool war;
    int j = 0;

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < num; j++)
        {
            arr[i][j] = rand() % 100 + 1;
       
        }
       
    }


    for (int i = 0; i < num; i++)
    {
        war = false;
        while(!war)
        {
            if (j >= arr[0][i] && i==0)
            {
                start[i] = j;
                war = true;
            }
            if(j >= arr[0][i] && j >= end[i-1])
            {
                start[i] = j;
                war = true;
            }
            j++;
            
        }

        end[i] = start[i] + arr[1][i];

    }

    for (int i = 0; i < num; i++)
    {
        cout << arr[0][i] << "  " << arr[1][i] << endl;
    }
    cout << endl;
    for (int i = 0; i < num; i++)
    {
        cout << start[i] << " " << end[i] << endl;
    }
}

