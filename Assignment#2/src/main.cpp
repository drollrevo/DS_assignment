#include <iostream>
#include <string>
#include "spectator_queue.hpp"
#include "match_scheduler.hpp"
#include "tournament_registration.hpp"

/*
# To build and run your project manually, use the following command from the project root:
#
# g++ -std=c++11 -Iinclude src/main.cpp src/spectator_queue.cpp src/match_scheduler.cpp src/tournament_registration.cpp -o apuec_system
#
# Then run:
# ./apuec_system
*/

int main() {
    SpectatorQueue spectatorQueue;
    MatchScheduler matchScheduler;
    TournamentRegistration registration;
    
    // Load data
    spectatorQueue.loadViewers("data/viewers.csv");
    spectatorQueue.loadSlots("data/stream_slots.csv");
    
    matchScheduler.loadTeams("data/teams.csv");
    matchScheduler.loadPlayers("data/players.csv");
    matchScheduler.loadResults("data/results.csv");
    
    registration.loadTeams("data/teams.csv");

    bool running = true;
    while (running) {
        std::cout << "\n=== APUEC System Menu ===\n";
        std::cout << "1) Registration (Task 2)\n";
        std::cout << "2) Match Scheduling (Task 1)\n";
        std::cout << "3) Admit Next Spectator (Task 3)\n";
        std::cout << "4) View Queue Size\n";
        std::cout << "5) End Tournament\n";
        std::cout << "Select an option: ";

        int choice;
        std::cin >> choice;
        switch (choice) {
            case 1: {
                // Task 2 - Registration submenu
                bool registrationActive = true;
                while (registrationActive) {
                    std::cout << "\n=== Registration Menu ===\n";
                    std::cout << "1) Register Team\n";
                    std::cout << "2) Process Registrations\n";
                    std::cout << "3) Check-in Team\n";
                    std::cout << "4) Withdraw Team\n";
                    std::cout << "5) Display Registration Stats\n";
                    std::cout << "6) Back to Main Menu\n";
                    std::cout << "Select option: ";
                    
                    int regChoice;
                    std::cin >> regChoice;
                    
                    switch (regChoice) {
                        case 1: {
                            std::cout << "Enter Team ID: ";
                            std::string teamID;
                            std::cin >> teamID;
                            std::cout << "Enter Registration Type (early-bird/regular/wildcard): ";
                            std::string regType;
                            std::cin >> regType;
                            registration.registerTeam(teamID, regType);
                            break;
                        }
                        case 2: {
                            registration.processRegistrations();
                            break;
                        }
                        case 3: {
                            std::cout << "Enter Team ID: ";
                            std::string teamID;
                            std::cin >> teamID;
                            registration.checkInTeam(teamID);
                            break;
                        }
                        case 4: {
                            std::cout << "Enter Team ID: ";
                            std::string teamID;
                            std::cin >> teamID;
                            registration.withdrawTeam(teamID);
                            break;
                        }
                        case 5: {
                            registration.displayStats();
                            break;
                        }
                        case 6: {
                            registrationActive = false;
                            break;
                        }
                        default:
                            std::cout << "Invalid choice.\n";
                    }
                }
                break;
            }
            case 2: {
                // Task 1 - Match Scheduling submenu
                bool schedulingActive = true;
                while (schedulingActive) {
                    std::cout << "\n=== Match Scheduling Menu ===\n";
                    std::cout << "1) Generate Match Pairings\n";
                    std::cout << "2) Get Next Match\n";
                    std::cout << "3) Record Match Result\n";
                    std::cout << "4) Display Tournament Bracket\n";
                    std::cout << "5) Display Match Schedule\n";
                    std::cout << "6) Display Player Statistics\n";
                    std::cout << "7) Back to Main Menu\n";
                    std::cout << "Select option: ";
                    
                    int schedChoice;
                    std::cin >> schedChoice;
                    
                    switch (schedChoice) {
                        case 1: {
                            std::cout << "Enter stage (qualifier/group/knockout): ";
                            std::string stage;
                            std::cin >> stage;
                            matchScheduler.generateMatchPairings(stage);
                            break;
                        }
                        case 2: {
                            matchScheduler.getNextMatch();
                            break;
                        }
                        case 3: {
                            std::cout << "Enter Match ID: ";
                            std::string matchID;
                            std::cin >> matchID;
                            std::cout << "Enter Winner Team ID: ";
                            std::string winner;
                            std::cin >> winner;
                            matchScheduler.recordMatchResult(matchID, winner);
                            break;
                        }
                        case 4: {
                            matchScheduler.displayTournamentBracket();
                            break;
                        }
                        case 5: {
                            matchScheduler.displayMatchSchedule();
                            break;
                        }
                        case 6: {
                            matchScheduler.displayPlayerStats();
                            break;
                        }
                        case 7: {
                            schedulingActive = false;
                            break;
                        }
                        default:
                            std::cout << "Invalid choice.\n";
                    }
                }
                break;
            }
            case 3: {
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
                        delete next; // Free memory after admission
                    }
                }
                break;
            }
            case 4: {
                std::cout << "Currently " 
                          << spectatorQueue.size() 
                          << " spectators in queue.\n";
                std::cout << "Currently "
                          << matchScheduler.size()
                          << " matches scheduled.\n";
                break;
            }
            case 5: {
                std::cout << "Ending tournament. Clearing queues...\n";
                spectatorQueue.clearAll();
                matchScheduler.clearAll();
                running = false;
                break;
            }
            default:
                std::cout << "Invalid choice.\n";
        }
    }

    std::cout << "Goodbye!\n";
    return 0;
}