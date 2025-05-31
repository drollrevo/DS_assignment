#include <iostream>
#include <string>
#include "spectator_queue.hpp"

/*
# To build and run your project manually, use the following command from the project root:
#
# g++ -std=c++11 -Iinclude src/main.cpp src/spectator_queue.cpp -o apuec_spectator
#
# Then run:
# ./apuec_spectator
#
# The Makefile is no longer needed and can be deleted.
*/

int main() {
    SpectatorQueue spectatorQueue;
    spectatorQueue.loadViewers("data/viewers.csv");
    spectatorQueue.loadSlots("data/stream_slots.csv");

    bool running = true;
    while (running) {
        std::cout << "\n=== APUEC System Menu ===\n";
        std::cout << "1) Registration (Task 2 - stub)\n";
        std::cout << "2) Scheduling (Task 1 - stub)\n";
        std::cout << "3) Logging Results (Task 4 - stub)\n";
        std::cout << "4) Admit Next Spectator (Task 3)\n";
        std::cout << "5) View Queue Size\n";
        std::cout << "6) End Tournament\n";
        std::cout << "Select an option: ";

        int choice;
        std::cin >> choice;
        switch (choice) {
            case 1:
                std::cout << "[Stub] Registration module not implemented.\n";
                break;
            case 2:
                std::cout << "[Stub] Scheduling module not implemented.\n";
                break;
            case 3:
                std::cout << "[Stub] Result logging module not implemented.\n";
                break;
            case 4: {
                if (spectatorQueue.isEmpty()) {
                    std::cout << "No spectators waiting.\n";
                } else {
                    Viewer* next = spectatorQueue.dequeueViewer();
                    if (next) {
                        std::cout << "Admitted " 
                                  << next->name 
                                  << " (Priority " 
                                  << next->priority 
                                  << ")\n";
                        // Free the admitted viewer's memory
                        delete next;
                    }
                }
                break;
            }
            case 5:
                std::cout << "Currently " 
                          << spectatorQueue.size() 
                          << " spectators in queue.\n";
                break;
            case 6:
                std::cout << "Ending tournament. Clearing queue...\n";
                spectatorQueue.clearAll();
                running = false;
                break;
            default:
                std::cout << "Invalid choice.\n";
        }
    }

    std::cout << "Goodbye!\n";
    return 0;
}
