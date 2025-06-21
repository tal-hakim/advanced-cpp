# Assignment \#1 - Advanced Topics in Programming

code by: \
Tal Hakim 322213836 \
Gal Shnitzer 212054837

### Compile & Run

To build and compile run:
```
cmake .
make
```

To run the game:
```
tanks_game <input_file>
```
- `<input file> ` should be either an absolute path or relative to the running directory of the game. \
- The output will be in the running directory of the program with the name `output_<input_filename>.txt` \
- Errors in the input file format will be outputted to `input_errors.txt`.

Sure! Here's a concise version of the **Input Format** and **Output Format**, without notes or extra explanation — just the essential structure:

---

### Input Format:

```
<map name or description>
MaxSteps = <NUM>
NumShells = <NUM>
Rows = <NUM>
Cols = <NUM>
<map layout> (starting from line 6)
```

**Map Characters:**

* `#` — Wall
* `@` — Mine
* `1` — Player 1 Tank (any digit `0–9`)
* `2` — Player 2 Tank (any digit `0–9`)
* (space or other) — Empty

**Example:**

```
Maze Map
MaxSteps = 100
NumShells = 10
Rows = 4
Cols = 10
#### @ @   
# 1    @   
#   2      
##########
```

---

### Output Format:

Each line: actions of all tanks, comma-separated, in board order (top-left to bottom-right).

If killed: append `(killed)` that round, then `killed` in future rounds.
If invalid: append `(ignored)`.

**Final line (one of):**

* `Player <X> won with <Y> tanks still alive`
* `Tie, both players have zero tanks`
* `Tie, reached max steps = <MAX>, player 1 has <X> tanks, player 2 has <Y> tanks`
* `Tie, both players have zero shells for <N> steps`

**Example:**

```
MoveForward, Shoot
RotateLeft90, MoveForward
MoveForward (killed), Shoot
killed, Shoot
Player 2 won with 1 tank still alive
```
