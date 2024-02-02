#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>

class ShotTimer {
public:
    ShotTimer(size_t time, SpaceTraveler& shooter, std::vector<SpaceTraveler>& enemies) : time{ std::chrono::milliseconds{time} }, shooter{shooter}, enemies{enemies} {}
    ~ShotTimer() { wait_thread.join(); }

private:
    void wait_then_call()
    {
        std::unique_lock<std::mutex> lck{ mtx };
        for (int i{ 10 }; i > 0; --i) {
            std::cout << "Thread " << wait_thread.get_id() << " countdown at: " << '\t' << i << '\n';
            cv.wait_for(lck, time / 10);
        }
        shooter.shoot(enemies);
    }
    std::mutex mtx;
    std::condition_variable cv{};
    std::chrono::milliseconds time;
    SpaceTraveler& shooter;
    std::vector<SpaceTraveler>& enemies;
    std::thread wait_thread{ [this]() {wait_then_call(); } };
};