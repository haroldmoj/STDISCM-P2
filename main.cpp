/*
    Name: MOJICA, Harold C.
    Section: STDISCM S11

    Specifications:
    a) There are only n instances that can be concurrently active. Thus, there can only be a maximum n number of parties that are currently in a dungeon.
    b) A standard party of 5 is: 1 tank, 1 healer, 3 DPS.
    c) The solution should not result in a deadlock.
    d) The solution should not result in starvation.
    e) It is assumed that the input values arrived at the same time.
    f) A time value (in seconds) t is randomly selected between t1 and t2. Where t1 represents the fastest clear time of a dungeon instance and t2 is the slowest clear time of a dungeon instance. For ease of testing t2 <= 15.

    Input:
    n - maximum number of concurrent instances
    t - number of tank players in the queue
    h - number of healer players in the queue
    d - number of DPS players in the queue
    t1 - minimum time before an instance is finished
    t2 - maximum time before an instance is finished 

    Output:
    a) Display the current status of all available instances (dungeons) everytime a party enters the dungeon or finishes a dungeon.
       - If there is a party in the instance, the status should say "active"
       - If the instance is empty, the status should say "empty"
    b) At the end of the execution there should be a summary of how many parties an instance have served and total time served.

    Additional Notes:
    - Proper input validation
    - Does not run perpetually, players are only queued and processed once.
    - Dungeons are reused.
    - Print out the left over players.

    Deliverables:
    1) Source code
    2) Video Demonstration
       - Provide your own test cases
    3) Source code + build/compilation steps
    4) Slides containing the following:
       - Possible deadlock and starvation explanation
       - Synchronization mechanisms used to solve the problem
*/

#include <iostream> 
#include <limits>
#include <string> 
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace std;

class DungeonManager {
    private:
        // Synchronization
        bool shutdown = false;
        std::mutex mtx;
        std::condition_variable cv;
    
        // Instance tracking
        struct DungeonInstance {
            bool active = false;
            int tank = 0;
            int healer = 0;
            int dps = 0;
            int partiesServed = 0;
            int clearTime = 0;
            double totalTimeServed = 0.0;
        };
    
        // Simulation parameters
        int maxInstances;
        int tankPlayers;
        int healerPlayers;
        int dpsPlayers;
        int minTime;
        int maxTime;
    
        // Instances
        std::vector<DungeonInstance> instances;
    
        // Random number generation
        std::random_device rd;
        std::mt19937 gen;
    
        // Display current status of all instances
        void displayInstanceStatus() {
            std::cout << "-----------------------------\n";
            std::cout << "[CURRENT STATUS]\n";
            for (size_t i = 0; i < instances.size(); ++i) {
                std::cout << "  Instance " << i + 1 << ": " 
                          << (instances[i].active ? "Active" : "Empty") 
                          << " (Tank: " << instances[i].tank 
                          << ", Healer: " << instances[i].healer 
                          << ", DPS: " << instances[i].dps << ") "
                          << "Clear Time: " << instances[i].clearTime << "\n";
            }
            std::cout << "-----------------------------\n\n";
        }
    
        // Wait for party members, THEN Enter dungeon, THEN Finish
        void runDungeon(int instanceId) {
            while (true) {
                std::unique_lock<std::mutex> lock(mtx);


                // Wait for a party or shutdown signal
                cv.wait(lock, [&]() {
                    return shutdown || 
                        (instances[instanceId].tank == 1 &&
                            instances[instanceId].healer == 1 &&
                            instances[instanceId].dps == 3);
                });

                // If shutdown is set and no active party, exit the thread
                if (shutdown && 
                    instances[instanceId].tank == 0 && 
                    instances[instanceId].healer == 0 && 
                    instances[instanceId].dps == 0) {
                    break;
                }
        
                // ENTER
                // Randomly generated clear time
                std::uniform_int_distribution<> timeDist(minTime, maxTime);
                instances[instanceId].clearTime = timeDist(gen);
                //instances[instanceId].active = true;

                std::cout << "Instance " << instanceId + 1 << " (PARTY ENTERED)\n";
                displayInstanceStatus();         

                // Simulate dungeon running
                lock.unlock();
                std::this_thread::sleep_for(std::chrono::seconds(instances[instanceId].clearTime));
                lock.lock();

                // FINISH
                instances[instanceId].partiesServed++;
                instances[instanceId].totalTimeServed += instances[instanceId].clearTime;
                instances[instanceId].tank--;
                instances[instanceId].healer--;
                instances[instanceId].dps -= 3;
                instances[instanceId].clearTime = 0;
                instances[instanceId].active = false;
        
                std::cout << "Instance " << instanceId + 1 << " (PARTY FINISHED)\n";
                displayInstanceStatus();
                
                // Notify potential waiting parties
                cv.notify_all();
            }
        }
    
    public:
        DungeonManager(int n, int tanks, int healers, int dpss, int t1, int t2) 
        : maxInstances(n), tankPlayers(tanks), healerPlayers(healers), 
          dpsPlayers(dpss), minTime(t1), maxTime(t2), gen(rd()) {
            
            // Initialize instances
            instances.resize(maxInstances);
        }

        void printSummary() {
            std::cout << "-----------------------------\n";
            std::cout << "[SUMMARY]\n";
            int totalPartiesServed = 0;
            double totalTimeServed = 0.0;
    
            for (size_t i = 0; i < instances.size(); ++i) {
                totalPartiesServed += instances[i].partiesServed;
                totalTimeServed += instances[i].totalTimeServed;
    
                std::cout << "  Instance " << i + 1 
                          << ": Parties Served = " << instances[i].partiesServed 
                          << ", Total Time = " << std::fixed << std::setprecision(2) 
                          << instances[i].totalTimeServed << " seconds\n";
            }
    
            // Print leftover players
            std::cout << "\n[LEFTOVER]\n";
            std::cout << "  Tanks: " << tankPlayers << "\n";
            std::cout << "  Healers: " << healerPlayers << "\n";
            std::cout << "  DPS: " << dpsPlayers << "\n";
            std::cout << "-----------------------------\n\n";
        }

        void processQueue() {
            std::vector<std::thread> dungeonThreads;
            for (int i = 0; i < maxInstances; i++) {
                dungeonThreads.emplace_back(&DungeonManager::runDungeon, this, i);
            }
        
            // Now distribute players to instances
            int z = 0;
            while (tankPlayers >= 1 && healerPlayers >= 1 && dpsPlayers >= 3) {
                // Find an inactive instance
                if (!instances[z].active) {
                    instances[z].tank = 1;
                    instances[z].healer = 1;
                    instances[z].dps = 3;
                    instances[z].active = true;
                    tankPlayers--;
                    healerPlayers--;
                    dpsPlayers -= 3;

                    // Notify that a party is available
                    cv.notify_all();
                }

                // Loop through instances
                z = (z + 1) % maxInstances;
            }

            // Shutdown
            {
                std::lock_guard<std::mutex> lock(mtx);
                shutdown = true;
                cv.notify_all();
            }

            // Keep threads alive
            for (auto& thread : dungeonThreads) {
                thread.join();
            }
        }
    };
    
    // Input validation function
    int getValidInteger(const std::string& prompt) {
        int integerInput;
        std::string input;

        // Keep repeating if input is invalid
        while (true) {
            std::cout << prompt;
            std::cin >> input;

            // Check if input is a valid whole number
            bool isValid = !input.empty();
            for (char c : input) {
                if (!std::isdigit(c)) {
                    isValid = false;
                    break;
                }
            }

            // Check if out-of-range
            if (isValid) {
                try {
                    std::size_t pos;
                    integerInput = std::stoi(input, &pos);
                    if (pos == input.length() && integerInput >= 0) {
                        std::cout << "\n";
                        return integerInput;
                    }
                } catch (...) {
                    // Prevent out-of-range errors
                }
            }
            std::cout << "[INVALID] Please enter a whole number.\n\n";
            std::cin.clear();  // Clear error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');   // Clear input
        }
    }


int main() {
    int n, t, h, d, t1, t2;

    // Get input from user
    n = getValidInteger("(n) Enter the maximum number of concurrent instances: ");
    t = getValidInteger("(t) Enter the number of tank players in the queue: ");
    h = getValidInteger("(h) Enter the number of healer players in the queue: ");
    d = getValidInteger("(d) Enter the number of DPS players in the queue: ");
    t1 = getValidInteger("(t1) Enter the minimum time before an instance is finished: ");
    t2 = getValidInteger("(t2) Enter the maximum time before an instance is finished: ");

    // Create and run dungeon manager
    DungeonManager manager(n, t, h, d, t1, t2);
    
    // Process the queue
    manager.processQueue();

    // Print summary
    manager.printSummary();

    return 0;
}
