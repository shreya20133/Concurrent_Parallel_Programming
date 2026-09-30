#include <iostream>
#include <thread>
#include <mutex>
#include <string>

int sushi_count = 5000;

void philospher(const std::string& name,std::mutex& chopsticks){
    while(sushi_count > 0){
        //chopsticks.lock();
        std::scoped_lock lock(chopsticks); //scoped lock will release mutex when the scope in which this lock is declared ends either abruptly or gracefully, guaranteeing no deadlock
        if(sushi_count) sushi_count--;
        if(sushi_count == 10){//if only 10 sushis left, break before unlocking
            std::cout << name << " philospher has had enough\n";
            break;

        }
        //chopsticks.unlock();
    }
}

int main(){
    std::mutex chopsticks;
    std::thread t1(philospher,"Philospher1",std::ref(chopsticks));
    std::thread t2(philospher,"Philospher2",std::ref(chopsticks));
    t1.join();
    t2.join();
    std::cout << "Philosphers are done eating!\n";
}