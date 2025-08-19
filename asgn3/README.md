# Assignment #3 - Advanced Topics in Programming
code by: \
Tal Hakim 322213836 \
Gal Shnitzer 212054837

## Compile & Run

Build from the main directory:

```bash
cmake .
make
```

Then go into the simulator folder:

```bash
cd Simulator
```

**Comparative run:**

```bash
./simulator_322213836_212054837 -comparative game_map=<map> game_managers_folder=<gm_folder> algorithm1=<alg1.so> algorithm2=<alg2.so> [num_threads=<n>] [-verbose]
```

**Competition run:**

```bash
./simulator_322213836_212054837 -competition game_maps_folder=<maps_folder> game_manager=<gm.so> algorithms_folder=<algos_folder> [num_threads=<n>] [-verbose]
```
* The algorithm is compiled into `Algorithm_322213836_212054837.so` file under `Algorithm/`.
* The GameManager is compiled into `GameManager_322213836_212054837.so` under `GameManager/`.

---

## Logic

* Each turn, one tank asks for battle info in round-robin order, so we always stay updated.
* If we only have one tank, it asks once every 3 turns instead.

### Tank's Internal Logic

* We check for **shell threats** by simulating shell paths around the tank.
* If any shell is heading toward our position → we are in danger.
* If we are **out of ammo**, we also count the nearest enemy as danger (since we can’t fight back).

**If in danger (Evader mode):**

* Try to move out of the shell’s path or rotate to a safe direction.
* If already facing the threat and we have ammo, fire to intercept.

**If not in danger (Chaser mode):**

* Use BFS to plan a path toward the nearest enemy.
* Rotate and move step by step along the path.
* If the enemy enters our line of fire, shoot.

#### *Shooting safety

* Before firing, the tank scans forward cell by cell.
* If it sees a wall or one of our own tanks, it won’t fire.
* This prevents shooting ourselves or our teammate.

---

## Registration
We created `SimulatorRegistration.so` so that the simulator, the GameManager, and all algorithm .so files can use the same registrar.
