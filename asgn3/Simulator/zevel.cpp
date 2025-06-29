bool GameManager_322213836_212054837::readBoardHeader(std::ifstream& file, int& numShells, int& rows, int& cols) {
    std::string line;
    std::string mapName;

    // Read map name/description (Line 1)
    if (!std::getline(file, mapName)) {
        logger.logInputError("Empty file");
        return false;
    }

    // Helper function to remove spaces from a string
    auto removeSpaces = [](std::string& str) {
        str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    };

    // Read MaxSteps (Line 2)
    if (!std::getline(file, line)) {
        logger.logInputError("Missing MaxSteps line");
        return false;
    }
    removeSpaces(line);
    if (sscanf(line.c_str(), "MaxSteps=%d", &maxSteps) != 1) {
        logger.logInputError("Invalid MaxSteps format");
        return false;
    }

    // Read NumShells (Line 3)
    if (!std::getline(file, line)) {
        logger.logInputError("Missing NumShells line");
        return false;
    }
    removeSpaces(line);
    if (sscanf(line.c_str(), "NumShells=%d", &numShells) != 1) {
        logger.logInputError("Invalid NumShells format");
        return false;
    }

    // Read Rows (Line 4)
    if (!std::getline(file, line)) {
        logger.logInputError("Missing Rows line");
        return false;
    }
    removeSpaces(line);
    if (sscanf(line.c_str(), "Rows=%d", &rows) != 1) {
        logger.logInputError("Invalid Rows format");
        return false;
    }

    // Read Cols (Line 5)
    if (!std::getline(file, line)) {
        logger.logInputError("Missing Cols line");
        return false;
    }
    removeSpaces(line);
    if (sscanf(line.c_str(), "Cols=%d", &cols) != 1) {
        logger.logInputError("Invalid Cols format");
        return false;
    }

    // Validate dimensions
    if (rows <= 0 || cols <= 0) {
        logger.logInputError("Invalid dimensions");
        return false;
    }

    return true;
}

bool GameManager_322213836_212054837::processMapCell(char cell, const Position& pos, int numShells) {
    switch (cell) {
        case WALL: // Wall
            board.placeObject(std::make_shared<Wall>(pos));
            break;
        case MINE: // Mine
            board.placeObject(std::make_shared<Mine>(pos));
            break;
        case EMPTY: // Empty space
            break;
        default: // Tank or other character
            if (cell == PLAYER1_TANK || cell == PLAYER2_TANK) {  // Support for 2 players
                int playerId = cell - '0';

                // Create tank with appropriate direction
                Direction initialDir = (playerId == PLAYER_1_ID) ? Direction::L : Direction::R;
                auto tank = std::make_shared<Tank>(pos, initialDir, playerId, static_cast<int>(size(allTanks)), numShells);

                // Add to all tanks vector
                allTanks.push_back(tank);
                board.placeObject(tank);

                // Create and add algorithm for this tank
                int tankIdx = tankCountPerPlayer[playerId]++;
                auto algorithm = algorithmFactory.create(playerId, tankIdx);
                tank->setAlgorithm(std::move(algorithm));

                // Initialize alive tanks counter for this player
                aliveTanksPerPlayer[playerId]++;
            }
            break;
    }
    return true;
}

bool GameManager_322213836_212054837::readMapContent(std::ifstream& file, int rows, int cols, int numShells) {
    // Read the map content
    std::vector<std::string> mapLines;
    std::string line;
    while (std::getline(file, line)) {
        mapLines.push_back(line);
    }

    for (int y = 0; y < rows; y++) {
        std::string currentLine = (y < static_cast<int>(mapLines.size())) ? mapLines[y] : "";

        for (int x = 0; x < cols; x++) {
            char cell = (x < static_cast<int>(currentLine.length())) ? currentLine[x] : EMPTY;
            Position pos(x, y);

            if (!processMapCell(cell, pos, numShells)) {
                return false;
            }
        }
    }

    // Create players for each player ID that has tanks
    players.clear();
    players.resize(2);  // Resize to hold 2 players
    for (const auto& [playerId, tankCount] : aliveTanksPerPlayer) {
        players[playerId - 1] = playerFactory.create(playerId, cols, rows, maxSteps, numShells);
    }

    // Check if we have any players with tanks
    if (players.empty()) {
        logger.logInputError("No tanks found on the board");
        return false;
    }

    // If there's only one player with tanks, they win immediately
    if (players.size() == 1) {
        // Find which player exists (should be at globalIndex 0 or 1)
        int playerId = players[0] ? 1 : 2;
        logger.logResult("Player " + std::to_string(playerId) + " won with " + std::to_string(aliveTanksPerPlayer[playerId]) + " tanks still alive");
        return false;  // Don't start the game since it's already over
    }

    // Initialize total shells count
    totalShells = allTanks.size() * numShells;

    return true;
}

bool GameManager_322213836_212054837::readBoard(const std::string& inputFile) {
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        logger.logInputError("Could not open file " + inputFile);
        cerr << "Could not open file " + inputFile << endl;
        return false;
    }

    int numShells = 0;
    int rows = 0;
    int cols = 0;

    // Read and validate header information
    if (!readBoardHeader(file, numShells, rows, cols)) {
        return false;
    }

    // Initialize the board with the specified dimensions
    board = GameBoard(cols, rows);

    // Read and process map content
    if (!readMapContent(file, rows, cols, numShells)) {
        return false;
    }

    // If we got here, everything was successful
    validGame = true;
    return true;
}


/* v func that returns all filenames inside folder
 * v func that loads a .so
 * v func that goes over vec of files and loads .so
 * v CompareSim and CompetitiveSim derived classes
 * TODO: log results
 * TODO: log errors
 * TODO: main
 * TODO: check arg list
 * TODO: threads :(
 * TODO: move the timeStamp func to userCommon so it will we accessible from gameManager
 * TODO: change the folder names to lower case
 * v func that builds the list to run
 * v make class "gameContainer" that holds: relevant gameManager, map ..., GameResult. has func run */


/*
 * TODO: create unique log name (for verbose mode)
*/