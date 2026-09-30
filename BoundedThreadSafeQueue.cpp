#include <iostream>
#include <thread>
#include <queue>
#include <array>
#include <condition_variable>

#define PRODUCER_THREAD_COUNT 20
#define CONSUMER_THREAD_COUNT 50

class BoundedThreadSafeQueue{

    public:
    BoundedThreadSafeQueue(int cap):capacity(cap) {};

    void push(int x){
        //Push only if q is not full - without synchronization
        //Acquire lock, wait until the queue is not full, push into queue, unlock , notify consumer
        std::unique_lock<std::mutex> lock(mtx);
        notFull.wait(lock,[this]{
            return q.size() < capacity;
        });
        q.push(x);
        lock.unlock();
        notEmpty.notify_one();
    }

    bool pop(int &val){
        //Acquire lock, wait until the queue is not empty, pop from queue, unlock, notify producer
        std::unique_lock<std::mutex> lock(mtx);
        notEmpty.wait(lock,[this]{
            return !q.empty() || stopped;
        });

        if(q.empty() && stopped){
            return false;
        }
        val = q.front();
        q.pop();
        lock.unlock();
        notFull.notify_one();
        return true;
    }

    int front(){
        std::unique_lock<std::mutex> lock(mtx);
        notEmpty.wait(lock,[this]{
            return !q.empty();
        });

        return q.front();
    }

    int size(){
        std::lock_guard<std::mutex> lock(mtx);
        return q.size();
    }

    bool empty(){
        std::lock_guard<std::mutex> lock(mtx);
        return q.empty();
    }

    void shutdown(){
        {
            std::cout << "Producers have finished producing items\n";
            std::lock_guard<std::mutex> lock(mtx);
            stopped = true;
        }
        notEmpty.notify_all();
    }

    private:
        int capacity;
        bool stopped = false;
        std::queue<int> q;
        std::mutex mtx;
        std::condition_variable notEmpty;
        std::condition_variable notFull;
};

BoundedThreadSafeQueue* boundedQ = new BoundedThreadSafeQueue(100);

void producer(){
    for(int i = 0; i < 1000; i++){
        boundedQ->push(i);
    }
}

void consumer(){
    int x = 0;
    while(boundedQ->pop(x)){
            std::cout << "[" << x << "] ";
    }
}

int main(){

    std::array<std::thread,PRODUCER_THREAD_COUNT> producers;
    std::array<std::thread,CONSUMER_THREAD_COUNT> consumers;

    for(int i = 0; i < producers.size(); i++){
        producers[i] = std::thread(producer);
    }

    for(int i = 0; i < consumers.size(); i++){
        consumers[i] = std::thread(consumer);
    }

    for(int i = 0; i < producers.size(); i++){
        producers[i].join();
    }
    boundedQ->shutdown();
    for(int i = 0; i < consumers.size(); i++){
        consumers[i].join();
    }

    return 0;
}