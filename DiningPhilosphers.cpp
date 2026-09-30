#include <iostream>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <chrono>

int sushi_count = 5000;

void philospher(std::mutex &first_chopstick, std::mutex& second_chopstick){
    while(sushi_count > 0){
        first_chopstick.lock();
        second_chopstick.lock();
        if(sushi_count) sushi_count--;
        second_chopstick.unlock();
        first_chopstick.unlock();
    }
}

int main(){
    std::mutex chopstick_a,chopstick_b;
    std::thread p1(philospher,std::ref(chopstick_a),std::ref(chopstick_b));
    std::thread p2(philospher,std::ref(chopstick_a),std::ref(chopstick_b));
    p1.join();
    p2.join();
    std::cout << "The philosphers are done eating!\n";
}
