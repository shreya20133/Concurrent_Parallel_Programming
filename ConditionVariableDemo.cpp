#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <array>
#include <condition_variable>

#define NO_OF_THREADS 10
int soup_servings = 10;
std::mutex slow_cooker_lid;
std::condition_variable soup_taken;

void hungry_person(int id){
    int put_lid_back = 0; //represents the busy wait checking of thread to identify if its turn has come
    while(soup_servings > 0){
        std::unique_lock<std::mutex> lid_lock(slow_cooker_lid);
        while((id != soup_servings % NO_OF_THREADS) && (soup_servings > 0)){
            put_lid_back++;
            soup_taken.wait(lid_lock);
        }
        if(soup_servings > 0){
             soup_servings--;
             std::cout << "Person " << id << " took " << soup_servings << " servings\n";
             lid_lock.unlock();
             soup_taken.notify_all();
        }
    }
    std::cout << "Person " << id << " put the lid back " << put_lid_back << " times \n";
}

int main(){

    std::array<std::thread,NO_OF_THREADS> threads;
    for(int i = 0; i < NO_OF_THREADS; i++)
        threads[i] = std::thread(hungry_person,i);
    
    for(int i = 0; i < NO_OF_THREADS; i++)
        threads[i].join();
    
    return 0;
}