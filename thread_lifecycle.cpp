#include<iostream>
#include<thread>
#include<chrono>
using namespace std;

void helperFunc(){
    cout << "Executing helper func "<<endl;
    this_thread::sleep_for(chrono::seconds(3));
    cout << "Finished executing helper func " <<endl;
}

int main(){
    cout << "Main thread needs help of helper thread" << endl;
    thread helperThread(helperFunc);
    cout << "Main thread continues execution..." <<endl;
    cout << " Is helperThread alive ? " << (helperThread.joinable() ? "YES" : "NO") << "\n";
    this_thread::sleep_for(chrono::seconds(1));
    cout << "Main thread waits for helper thread to finish execution " << endl;
    helperThread.join();
    cout << " Is helperThread alive ? " << (helperThread.joinable() ? "YES" : "NO") << "\n";
    cout << "Both Main and helper threads finish execution " << endl;
}