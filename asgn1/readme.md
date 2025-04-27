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

### Input Format:
A .txt file in the format of:

```
<height> <width>
<board layout>
```

- **height** and **width**: Dimensions of the game board.
- **board layout**: Characters representing the game environment:
    - `#` = Wall
    - `@` = Mine
    - `1` = Player 1's Tank
    - `2` = Player 2's Tank
    - ` ` = Empty space

#### Example Input:

```
5 5
#####
#1 2#
#   #
# @ #
#####
```

### Output Format:

```
Game Step #<step_number>
Player 1 - <action_1>
Player 2 - <action_2>
```

- **step_number**: Current game step.
- **action_1** and **action_2**: Actions of Player 1 and Player 2. Possible actions:
    - `Move Forward`
    - `Move Backwards`
    - `Shoot`
    - `RotateLeft 1/8`
    - `RotateRight 1/8`
    - `None`

Final result:

```
====== Game Over ======
<Player_Number> wins - <reason>
```
or
```
====== Game Over ======
Tie - <reason>
```

#### Example Output:

```
====== Game Start ======

Game Step #1
Player 1 - RotateRight 1/4
Player 2 - None

Game Step #2
Player 1 - RotateRight 1/4
Player 2 - None

Game Step #3
Player 1 - Shoot
Player 2 - None

====== Game Over ====== 
Player 1 wins - Player 2 tank destroyed by Shell
```
