//
// Created by talta on 21/06/2025.
//

#ifndef ASGN3_GAMEMANAGERREGISTRAR_H
#define ASGN3_GAMEMANAGERREGISTRAR_H


#include "../common/AbstractGameManager.h"
#include "SOHandle.h"
#include <cassert>
#include <vector>

class GameManagerRegistrar {
public:
    class GameManagerEntry {
        std::string so_name;
        std::unique_ptr<SOHandle> soHandle;
        GameManagerFactory gameManagerFactory;
    public:
        GameManagerEntry(const std::string& so_name) : so_name(so_name), soHandle(nullptr) {}
        void setGameManagerFactory(GameManagerFactory&& factory) {
            assert(gameManagerFactory == nullptr);
            gameManagerFactory = std::move(factory);
        }
        void setSOHandle(const std::string& so_path) {
            assert(!soHandle);  // or: assert(!soHandle || !soHandle->isInitialized());
            soHandle = std::make_unique<SOHandle>(so_path);
        }
        const std::string& name() const { return so_name; }
        std::unique_ptr<AbstractGameManager> createGameManager(bool verbose) const {
            return gameManagerFactory(verbose);
        }
        bool hasGameManagerFactory() const {
            return gameManagerFactory != nullptr;
        }
    };
private:
    std::vector<GameManagerEntry> managers;
    static GameManagerRegistrar registrar;
public:
    static GameManagerRegistrar& getGameManagerRegistrar();
    void createGameManagerFactoryEntry(const std::string& name) {
        managers.emplace_back(name);
    }

    void loadGameManagerSO(const std::string& name) {
        managers.back().setSOHandle(name);
    }

    void addGameManagerFactoryToLastEntry(GameManagerFactory&& factory) {
        managers.back().setGameManagerFactory(std::move(factory));
    }

    struct BadRegistrationException  {
        std::string name;
        bool hasName, hasGameManagerFactory;
    };
    void validateLastRegistration() {
        const auto& last = managers.back();
        bool hasName = (last.name() != "");
        if(!hasName || !last.hasGameManagerFactory() ) {
            throw BadRegistrationException{
                    .name = last.name(),
                    .hasName = hasName,
                    .hasGameManagerFactory = last.hasGameManagerFactory(),
            };
        }
    }
    void removeLast() {
        managers.pop_back();
    }
    auto begin() const {
        return managers.begin();
    }
    auto end() const {
        return managers.end();
    }
    std::size_t count() const { return managers.size(); }
    void clear() { managers.clear(); }
};

#endif //ASGN3_GAMEMANAGERREGISTRAR_H
