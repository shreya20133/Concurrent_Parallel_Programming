#include <thread>
#include <chrono>
#include <iostream>
#include <unistd.h>
using namespace std;
void cpu_waster(){
    cout << "CPU Waster process id: " << getpid() << endl;
    cout << "CPU Waster thread id: " << this_thread::get_id() << endl;
    while(true){
        continue; // Waste CPU cycles
    }
}

int main(){
    cout << "Main Process id: " << getpid() << endl;
    cout << "Main thread id: " << this_thread::get_id() << endl;
    for(int i = 0; i < 16; i++){
        thread t(cpu_waster);
        t.detach(); // Detach the thread to allow it to run independently
    }

    while(true){ //Keep the main thread alive to prevent the program from exiting
        this_thread::sleep_for(chrono::milliseconds(100));
    }   
}