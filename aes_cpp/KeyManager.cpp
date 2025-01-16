#include <iostream>
#include <string>
#include <unordered_map>
#include <ctime>
#include <random>
#include <openssl/evp.h>
#include <openssl/rand.h>

std::unordered_map<std::string, std::string> sessionKeys; // Maps SteamID to Session Key

// Generate a random session key
std::string generateSessionKey() {
    unsigned char key[32];
    RAND_bytes(key, sizeof(key)); // 32 bytes = 256 bits
    std::string sessionKey(reinterpret_cast<char*>(key), sizeof(key));
    return sessionKey;
}

// Store a session key for a specific SteamID
void storeSessionKey(const std::string& steamID, const std::string& sessionKey) {
    sessionKeys[steamID] = sessionKey;
}

// Validate a session key for a SteamID
bool validateSessionKey(const std::string& steamID, const std::string& receivedKey) {
    auto it = sessionKeys.find(steamID);
    if (it != sessionKeys.end() && it->second == receivedKey) {
        return true;
    }
    return false;
}

// Example main function to interact with the program
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: KeyManager <command> [args...]\n";
        std::cout << "Commands:\n";
        std::cout << "  generate <SteamID>      - Generate a new session key\n";
        std::cout << "  validate <SteamID> <Key> - Validate a session key\n";
        return 1;
    }

    std::string command = argv[1];
    if (command == "generate" && argc == 3) {
        std::string steamID = argv[2];
        std::string sessionKey = generateSessionKey();
        storeSessionKey(steamID, sessionKey);
        std::cout << "Generated Session Key: " << sessionKey << "\n";
    } else if (command == "validate" && argc == 4) {
        std::string steamID = argv[2];
        std::string receivedKey = argv[3];
        if (validateSessionKey(steamID, receivedKey)) {
            std::cout << "Key is valid!\n";
        } else {
            std::cout << "Invalid key.\n";
        }
    } else {
        std::cout << "Invalid command or arguments.\n";
    }

    return 0;
}
