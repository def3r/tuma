# Turing Machine Simulator
*`tuma`* is a Turing Machine Simulator with a single tape and a single R/W head written in C using Raylib.

<p align="center">
  <img src="https://github.com/user-attachments/assets/a8ea5e20-a94f-402d-94d2-f0d4eb5f61c8" alt="tuma demo"/>
  <br/>
  <i>Example: Turing machine for binary increment</i>
</p>

## Build
The primamry build method is `Makefile` which is only *linux compatible*.
However, [Raylib](https://github.com/raysan5/raylib) is corss-platform and one can
[build raylib](https://github.com/raysan5/raylib?tab=readme-ov-file#build-and-installation) and link
this project on any other platform.

## Input file
The information of the turing machine is stored in a `.toml` file and is to be *drag n drop*ped to be loaded.

> [!WARNING]
>  toml parsing is not strict. Although it's strict enough to enfore basic toml syntax.

Example `input.toml` file for a language acceptor turing machine:
```toml
# turing machine for language not containing 1
# States start from index 0
[turingmachine]
numofstates = 2
initialstate = 0              # optional; default is 0
finalstates = [0]
inputsymbols = ['0', '1']
tapesymbols = ['0', '1', 'B']
blanksymbol = 'B'             # optional; default is B
tape = '000100'               # optional; default is empty string

# Directions for transition
# LEFT=-1, NOMOVE=0, RIGHT=1, SAME (as prev)
# Head direction defaults RIGHT if no dir provided in the init state

# State 0
[[transition]] # optional; no transition for any state halts machine
cur = 0       # current state 0
head = '0'    # optional; read val at head
              # if not provided its considered a mandatory move
write = ''    # optional;
dir = 'RIGHT'
next = 0      # next state

[[transition]]
cur = 0
head = '1'
dir = 'RIGHT'
next = 1

# Optional for dead state to have transition
# State 1
[[transition]]
cur = 1
halt = true # optional;
```

## Controls

| Key | Action |
|-----|--------|
| *`Drag n Drop`* | Load input file |
| *`Q or Esc`* | Quit |
| *`Ctrl+R`* | Reload the current input file |
| *`Space`* | Next state |
| *`Enter`* | Run the turing machine |
| *`R`* | Reset machine to original state |
| *`P`* | Pause the turing machine |
| *`E`* | Execute turing machine and display last state |

## Examples
The [`examples/`](https://github.com/def3r/tuma/tree/main/examples) directory contains some turing machine `toml` files.
