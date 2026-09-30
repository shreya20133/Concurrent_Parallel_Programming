#include <iostream>
#include <thread>
#include <array>
#include <mutex>

int sushi_count = 100000;

void philospher(std::mutex& chopsticks){
    int sushi_eaten = 0;
    while(sushi_count > 0){
        std::scoped_lock lock(chopsticks);
        if(sushi_count){
            sushi_count--;
            sushi_eaten++;
        }
    }
    std::cout << "Sushi eaten by thread id: "<<std::this_thread::get_id()<<" is : "<<sushi_eaten<<"\n";
}

int main(){
    std::mutex chopsticks;
    std::array<std::thread,200> philosphers;
    for(size_t i = 0; i < philosphers.size(); i++){
        philosphers[i] = std::thread(philospher,std::ref(chopsticks));
    }
    for(size_t i = 0; i < philosphers.size(); i++){
        philosphers[i].join();
    }
    std::cout << "The philosphers are done eating\n";
}