#include <iostream>
using namespace std;

int main()
{
    int batteryLevel = 100;

    while (batteryLevel > 0)
    {
        cout << "Battery: " << batteryLevel << "%" << endl;

        if (batteryLevel <= 20)
        {
            cout << "Warning: Low Battery!" << endl;
        }

        batteryLevel -= 10;
    }

    return 0;
}