Overview -
In this project, we implemented the required classes and adapted the game to support multiple tanks for each player.
Additionally, we added some bonus features: logging, configuration, and tests.
The main code is located in the /src directory. For bonus features, see the file bonus.txt.

Implementatins -
SatelliteView - We implemented SimpleSatelliteView, which maintains a matrix of characters received from the GameManager and uses it to implement the getObjectAt function.

    BattleInfo - We implemented SimpleBattleInfo, which stores location vectors for: walls, mines, shells, tanks of player 1, tanks of player 2, and the position of the calling tank (i.e., the tank requesting the battle info).

    Players - We implemented two types of players:
        SimplePlayer: When the GameManager calls updateTankWithBattleInfo(), this player creates a SimpleBattleInfo containing *all* data from the SatelliteView, and then passes it to the corresponding tank algorithm via updateBattleInfo().

        RadiusPlayer: Also implements updateTankWithBattleInfo(), but only provides data within a specified radius from the requesting tank. This avoids sending irrelevant data from distant parts of the board, simplifying communication and improving efficiency.

    Both players share some common functionality, so they inherit from the PlayerImp class.

    TankAlgorithm - We implemented two tank algorithms:
        BFSTankAlgorithm: Every three turns, it requests battle info, then uses it to locate the nearest enemy and runs a BFS algorithm to determine the best next two moves to shoot at that enemy. It then executes these moves in the next two turns before requesting info again.

        RotatingTankAlgorithm: A simple algorithm used mainly for testing. It only rotates in place and typically loses unless the opponent makes critical mistakes.

    Factories - 
        SimpleTankAlgorithmFactory: Creates a BFSTankAlgorithm for player 1 and a RotatingTankAlgorithm for player 2.
        SimplePlayerFactory: Creates a player instance based on the type specified in the configuration file. If no type is specified, the default is SimplePlayer.