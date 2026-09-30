#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <array>

#define PRODUCER_THREAD_COUNT 2
#define CONSUMER_THREAD_COUNT 5

class ThreadSafeQueue{
    public:

        void push(int i)
        {
            {
                std::lock_guard<std::mutex> lock(mtx);
                q.push(i);
            }
            notEmpty.notify_one();
        }

        int pop() 
        {
            std::unique_lock<std::mutex> lock(mtx);
            notEmpty.wait(lock,[this]{
                return !q.empty() || stopped;
            });

            if(q.empty() && stopped)    return false;
            int elem = q.front();
            q.pop();
            return elem;
        }

        int front()
        {
            std::unique_lock<std::mutex> lock(mtx);
            notEmpty.wait(lock,[this]{
                return !q.empty();
            });
            return q.front();
        }

        size_t size()
        {
            std::lock_guard<std::mutex> lock(mtx);
            return q.size();
        }

        bool empty()
        {
            std::lock_guard<std::mutex> lock(mtx);
            return q.empty();
        }

        void shutdown()//required for when there are more consumers than producers and we need to signal the consumers that no more items are being produced
        {
            {
                std::lock_guard<std::mutex> lock(mtx);
                stopped = true;
            }
            notEmpty.notify_all();
        }

    private:
        std::queue<int> q;
        std::mutex mtx;
        std::condition_variable notEmpty;
        bool stopped = false;
};

ThreadSafeQueue threadSafeQueue;

void producer(){
    for(int i = 1; i <= 10; i++){
        threadSafeQueue.push(i);
    }
}

void consumer(){
    for(int i = 1; i <= 10; i++){
        std::cout << "[" << threadSafeQueue.pop() << "]\n";
    }
}

int main()
{
    std::array<std::thread,PRODUCER_THREAD_COUNT> producers;
    std::array<std::thread,CONSUMER_THREAD_COUNT> consumers;

    for(int i = 0; i < PRODUCER_THREAD_COUNT; i++){
        producers[i] = std::thread(producer);
    }

    for(int i = 0; i < CONSUMER_THREAD_COUNT; i++){
        consumers[i] = std::thread(consumer);
    }

    for(int i = 0; i < PRODUCER_THREAD_COUNT; i++){
        producers[i].join();
    }

    threadSafeQueue.shutdown();
    for(int i = 0; i < CONSUMER_THREAD_COUNT; i++){
        consumers[i].join();
    }


    return 0;
}