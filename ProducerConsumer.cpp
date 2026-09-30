#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

class ServingLine {

    public:
        void serve_soup(int i){
            // std::unique_lock<std::mutex> ladle_lock(ladle);
            // soup_queue.push(i);
            // ladle_lock.unlock();
            {
                std::lock_guard<std::mutex> ladle_lock(ladle);
                soup_queue.push(i);
            }
            soup_served.notify_one();
        }
        int take_soup(){
            std::unique_lock<std::mutex> ladle_lock(ladle);
            soup_served.wait(ladle_lock,[&] {return !soup_queue.empty();});
            int bowl = soup_queue.front();
            soup_queue.pop();
            return bowl;
        }
    private:
        std::queue<int> soup_queue;
        std::mutex ladle;
        std::condition_variable soup_served;
};

ServingLine servingline = ServingLine();

void soup_producer(){
    for(int i = 0; i < 10000; i++){ //serve 10000 bowls of soup with soup id = 1
        servingline.serve_soup(1);
    }
    servingline.serve_soup(-1);//indicates no more soup
    std::cout << "Produce is done serving soup\n";
}

void soup_consumer(){
    int soup_eaten = 0;
    while(true){
        int bowl = servingline.take_soup();
        if(bowl == -1){ //no more soups left
            std::cout << " Consumer ate " << soup_eaten << " bowls\n";
            servingline.serve_soup(-1); // put the last bowl for other consumers to take
            return;
        }
        else{
            soup_eaten += bowl;
        }
    }
}

int main(){
    std::thread producer(soup_producer);
    std::thread consumer1(soup_consumer);
    std::thread consumer2(soup_consumer);
    producer.join();
    consumer1.join();
    consumer2.join();
    return 0;
}