#include <iostream>
#include <string>
#include "spectator_queue.hpp"
#include "match_scheduler.hpp"
#include "tournament_registration.hpp"
#include "performance_history.hpp"

int main() {
    PerformanceHistoryManager perfManager;
    SpectatorQueue spectatorQueue;
    MatchScheduler matchScheduler(&perfManager);
    TournamentRegistration registration;
    
    // Load initial data from files
    spectatorQueue.loadViewers("data/viewers.csv");
    spectatorQueue.loadSlots("data/stream_slots.csv");
    matchScheduler.loadTeams("data/teams.csv");
    matchScheduler.loadResults("data/results.csv");
    registration.loadTeams("data/teams.csv");

    bool running = true;
    while (running) {
        std::cout << "\n=== APUEC System Menu ===\n";
        std::cout << "1. Tournament Registration\n";
        std::cout << "2. Match Scheduling\n";
        std::cout << "3. Spectator Management\n";
        std::cout << "4. Performance History\n";
        std::cout << "5. End Tournament\n";
        std::cout << "Select an option: ";

        int choice;
        std::cin >> choice;
        switch (choice) {
            case 1: {
                bool registrationActive = true;
                while (registrationActive) {
                    std::cout << "\n=== Tournament Registration Menu ===\n";
                    std::cout << "1. Register Team\n";
                    std::cout << "2. Process Registrations\n";
                    std::cout << "3. Check-in Team\n";
                    std::cout << "4. Withdraw Team\n";
                    std::cout << "5. Display Registration Stats\n";
                    std::cout << "6. Back to Main Menu\n";
                    std::cout << "Select option: ";
                    
                    int regChoice;
                    std::cin >> regChoice;
                    switch (regChoice) {
                        case 1: {
                            std::string teamID, regType;
                            std::cout << "Enter Team ID: ";
                            std::cin >> teamID;
                            std::cout << "Enter Registration Type (early-bird/regular/wildcard): ";
                            std::cin >> regType;
                            registration.registerTeam(teamID, regType);
                            break;
                        }
                        case 2:
                            registration.processRegistrations();
                            break;
                        case 3: {
                            std::string teamID;
                            std::cout << "Enter Team ID: ";
                            std::cin >> teamID;
                            registration.checkInTeam(teamID);
                            break;
                        }
                        case 4: {
                            std::string teamID;
                            std::cout << "Enter Team ID: ";
                            std::cin >> teamID;
                            registration.withdrawTeam(teamID);
                            break;
                        }
                        case 5:
                            registration.displayStats();
                            break;
                        case 6:
                            registrationActive = false;
                            break;
                        default:
                            std::cout << "Invalid choice.\n";
                    }
                }
                break;
            }
            case 2: {
                bool schedulingActive = true;
                while (schedulingActive) {
                    std::cout << "\n=== Match Scheduling Menu ===\n";
                    std::cout << "1. Generate Match Pairings\n";
                    std::cout << "2. Get Next Match\n";
                    std::cout << "3. Record Match Result\n";
                    std::cout << "4. Display Tournament Bracket\n";
                    std::cout << "5. Display Match Schedule\n";
                    std::cout << "6. Display Player Statistics\n";
                    std::cout << "7. Back to Main Menu\n";
                    std::cout << "Select option: ";
                    
                    int schedChoice;
                    std::cin >> schedChoice;
                    switch (schedChoice) {
                        case 1:
                            matchScheduler.generateMatchPairings();
                            break;
                        case 2:
                            matchScheduler.getNextMatch();
                            break;
                        case 3: {
                            std::string matchID, winner;
                            std::cout << "Enter Match ID: ";
                            std::cin >> matchID;
                            std::cout << "Enter Winner Team ID: ";
                            std::cin >> winner;
                            matchScheduler.recordMatchResult(matchID, winner);
                            break;
                        }
                        case 4:
                            matchScheduler.displayTournamentBracket();
                            break;
                        case 5:
                            matchScheduler.displayMatchSchedule();
                            break;
                        case 6:
                            matchScheduler.displayPlayerStats();
                            break;
                        case 7:
                            schedulingActive = false;
                            break;
                        default:
                            std::cout << "Invalid choice.\n";
                    }
                }
                break;
            }
            case 3: {
                bool spectatorActive = true;
                while (spectatorActive) {
                    std::cout << "\n=== Spectator Management Menu ===\n";
                    std::cout << "1. Admit Next Spectator\n";
                    std::cout << "2. View Next Spectator\n";
                    std::cout << "3. View Spectator Queue\n";
                    std::cout << "4. View Available Stream Slots\n";
                    std::cout << "5. Add New Spectator\n";
                    std::cout << "6. Back to Main Menu\n";
                    std::cout << "Select option: ";
                    
                    int specChoice;
                    std::cin >> specChoice;
                    switch (specChoice) {
                        case 1: {
                            if (spectatorQueue.isEmpty()) {
                                std::cout << "No spectators waiting.\n";
                            } else {
                                Viewer* next = spectatorQueue.dequeueViewer();
                                if (next) {
                                    std::cout << "Admitted " << next->name << " (Priority " << next->priority << ")\n";
                                    delete next;
                                }
                            }
                            break;
                        }
                        case 2: {
                            Viewer* next = spectatorQueue.peekNext();
                            if (next) {
                                std::cout << "Next spectator: " << next->name << " (Priority " << next->priority << ")\n";
                            } else {
                                std::cout << "No spectators in queue.\n";
                            }
                            break;
                        }
                        case 3:
                            spectatorQueue.displayQueue();
                            break;
                        case 4:
                            spectatorQueue.displaySlots();
                            break;
                        case 5: {
                            std::string id, name;
                            int priority;
                            std::cout << "Enter Viewer ID: ";
                            std::cin >> id;
                            std::cout << "Enter Viewer Name: ";
                            std::cin.ignore();
                            std::getline(std::cin, name);
                            std::cout << "Enter Priority (1-VIP, 2-Influencer, 3-Usual): ";
                            std::cin >> priority;
                            if (priority >= 1 && priority <= 3) {
                                Viewer* v = new Viewer{id, name, priority, 0};
                                spectatorQueue.enqueueViewer(v);
                                std::cout << "Added " << name << " to queue.\n";
                            } else {
                                std::cout << "Invalid priority.\n";
                            }
                            break;
                        }
                        case 6:
                            spectatorActive = false;
                            break;
                        default:
                            std::cout << "Invalid choice.\n";
                    }
                }
                break;
            }
            case 4: {
                bool perfActive = true;
                while (perfActive) {
                    std::cout << "\n=== Performance History Menu ===\n";
                    std::cout << "1. Display All Player Stats\n";
                    std::cout << "2. Display Recent Matches\n";
                    std::cout << "3. Search Player Matches\n";
                    std::cout << "4. Get Top Players by Win Rate\n";
                    std::cout << "5. Get Top Players by Score\n";
                    std::cout << "6. Generate Tournament Summary\n";
                    std::cout << "7. Back to Main Menu\n";
                    std::cout << "Select option: ";
                    
                    int perfChoice;
                    std::cin >> perfChoice;
                    switch (perfChoice) {
                        case 1:
                            perfManager.displayAllPlayerStats();
                            break;
                        case 2:
                            perfManager.displayRecentMatches();
                            break;
                        case 3: {
                            std::string playerName;
                            std::cout << "Enter player name: ";
                            std::cin.ignore();
                            std::getline(std::cin, playerName);
                            perfManager.searchPlayerMatches(playerName);
                            break;
                        }
                        case 4:
                            perfManager.getTopPlayersByWinRate(5);
                            break;
                        case 5:
                            perfManager.getTopPlayersByScore(5);
                            break;
                        case 6:
                            perfManager.generateTournamentSummary();
                            break;
                        case 7:
                            perfActive = false;
                            break;
                        default:
                            std::cout << "Invalid choice.\n";
                    }
                }
                break;
            }
            case 5:
                std::cout << "Ending tournament. Clearing queues...\n";
                spectatorQueue.clearAll();
                matchScheduler.clearAll();
                running = false;
                break;
            default:
                std::cout << "Invalid choice.\n";
        }
    }

    std::cout << "Goodbye!\n";
    return 0;
}