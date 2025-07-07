//
// Created by talta on 21/06/2025.
//

#ifndef ASGN3_ALGORITHMREGISTRAR_H
#define ASGN3_ALGORITHMREGISTRAR_H

#include "../common/TankAlgorithm.h"
#include "../common/Player.h"

#include <cassert>
#include <vector>
#include <string>
#include "SOHandle.h"

class AlgorithmRegistrar {
public:
    class AlgorithmAndPlayerFactories {
        std::string so_name;
        std::unique_ptr<SOHandle> soHandle;
        TankAlgorithmFactory tankAlgorithmFactory;
        PlayerFactory playerFactory;
    public:
        AlgorithmAndPlayerFactories(const std::string& so_name) : so_name(so_name), soHandle(nullptr) {}
        void setTankAlgorithmFactory(TankAlgorithmFactory&& factory) {
            assert(tankAlgorithmFactory == nullptr);
            tankAlgorithmFactory = std::move(factory);
        }
        void setPlayerFactory(PlayerFactory&& factory) {
            assert(playerFactory == nullptr);
            playerFactory = std::move(factory);
        }
        const std::string& name() const { return so_name; }
        std::unique_ptr<Player> createPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells) const {
            return playerFactory(player_index, x, y, max_steps, num_shells);
        }
        void setSOHandle(const std::string& so_path) {
            assert(!soHandle);  // or: assert(!soHandle || !soHandle->isInitialized());
            soHandle = std::make_unique<SOHandle>(so_path);
        }
        const TankAlgorithmFactory& getTankAlgorithmFactory() const {
            return tankAlgorithmFactory;
        }
        bool hasPlayerFactory() const {
            return playerFactory != nullptr;
        }
        bool hasTankAlgorithmFactory() const {
            return tankAlgorithmFactory != nullptr;
        }
    };
private:
    std::vector<AlgorithmAndPlayerFactories> algorithms;
    static AlgorithmRegistrar registrar;
public:
    static AlgorithmRegistrar& getAlgorithmRegistrar();
    void createAlgorithmFactoryEntry(const std::string& name) {
        algorithms.emplace_back(name);
    }
    void loadAlgorithmSO(const std::string& name) {
        algorithms.back().setSOHandle(name);
    }
    void addPlayerFactoryToLastEntry(PlayerFactory&& factory) {
        algorithms.back().setPlayerFactory(std::move(factory));
    }
    void addTankAlgorithmFactoryToLastEntry(TankAlgorithmFactory&& factory) {
        algorithms.back().setTankAlgorithmFactory(std::move(factory));
    }
    struct BadRegistrationException {
        std::string name;
        bool hasName, hasPlayerFactory, hasTankAlgorithmFactory;
    };
    void validateLastRegistration() {
        const auto& last = algorithms.back();
        bool hasName = (last.name() != "");
        if(!hasName || !last.hasPlayerFactory() || !last.hasTankAlgorithmFactory() ) {
            throw BadRegistrationException{
                    .name = last.name(),
                    .hasName = hasName,
                    .hasPlayerFactory = last.hasPlayerFactory(),
                    .hasTankAlgorithmFactory = last.hasTankAlgorithmFactory()
            };
        }
    }
    void removeLast() {
        algorithms.pop_back();
    }
    auto begin() const {
        return algorithms.begin();
    }
    auto end() const {
        return algorithms.end();
    }
    std::size_t count() const { return algorithms.size(); }
    void clear() { algorithms.clear(); }
};



#endif //ASGN3_ALGORITHMREGISTRAR_H
