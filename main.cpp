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
    a) Current status of all available instances
    b) If there is a party in the instance, the status should say "active"
    c) If the instance is empty, the status should say "empty"
    d) At the end of the execution there should be a summary of how many parties an instance have served and total time served.

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
using namespace std;

// Gets the input and checks if valid
int getValidInteger(const string& prompt) {
    int integerInput;
    string input;

    // Keep repeating if input is invalid
    while (true) {
        cout << prompt;
        cin >> input;

        // Check if input is a valid whole number
        bool isValid = !input.empty();
        for (char c : input) {
            if (!isdigit(c)) {
                isValid = false;
                break;
            }
        }

        // Check if out-of-range
        if (isValid) {
            try {
                size_t pos;
                integerInput = stoi(input, &pos);
                if (pos == input.length() && integerInput >= 0) {
                    cout << "\n";
                    return integerInput;
                }
            } catch (...) {
                // Prevent out-of-range errors
            }
        }

        cout << "[INVALID] Please enter a whole number.\n\n";
        cin.clear();  // Clear error flag
        cin.ignore(numeric_limits<streamsize>::max(), '\n');   // Clear input
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

    // For checking purposes
    cout << "n = " << n 
    << "\nt = " << t 
    << "\nh = " << h 
    << "\nd = " << d 
    << "\nt1 = " << t1 
    << "\nt2 = " << t2;
    
    return 0;
}
