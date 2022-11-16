#include <iostream>
#include <random>
#include <thread>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 

    // Lock the _queue
    std::unique_lock<std::mutex> lock(_mutex);
    // Apply wait condition if the queue isn't empty
    
    _condition.wait(lock, [this] { return !_queue.empty();});

    // Get the last element in the queue
    auto msg = std::move(_queue.back());
    // Remove the last element in the queue
    _queue.pop_back();

    // Return the element
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    
    // Lock the _queue
    std::lock_guard<std::mutex> lock(_mutex);
    // Add message to the queue
    _queue.push_back(std::move(msg));
    // Notify that the add operation has already been performed
    _condition.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    
    // Add infinite while-loop 
    while (true)
    {
        // Get the message and check if its green light
        if(_mesageQueue.receive() == TrafficLightPhase::green)
            return;
    }
    
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 

    int numberOfCycles = 0;
    std::chrono::time_point<std::chrono::system_clock> lastUpdate = std::chrono::system_clock::now();
    double cycleDuration = rand() % 4 - 6;

    // Create infinite loop
    while(true)
    {
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - lastUpdate).count();

        // Continue with the next loop if the cycleDuration time has not passed yet
        if (timeSinceLastUpdate >= cycleDuration)
        {
            // Check if the cycle counter is a multiple of 2
            if(numberOfCycles % 2)
            {
                // Change the color of the light to red
                _currentPhase = TrafficLightPhase::red;
                // Sleep the thread each two cycles
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            } else {
                // Change the color of the light to green
                _currentPhase = TrafficLightPhase::green;
            }

            // Send current TrafficLightPhase state
            _mesageQueue.send(std::move(_currentPhase));

            // Update values for the next loop
            cycleDuration = rand() % 4 - 6;
            lastUpdate = std::chrono::system_clock::now();
            numberOfCycles++;
        }
    }

}
