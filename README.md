# Minesweeper in C

This is a small toy I made just for fun, when I was a high school student. And now, I'm trying to make it better(maybe).

## Revision History

### Features that the original version had

* Set rows/cols of the minefield before starting the game.
* The number of mines was also random (what a surprise).
* `termios`-based key handling.
* Support for 16 colors by using ANSI escape sequences.

### May 9th 2016

* Started using NCurses library for key handling and 256 colors support.

### May 10th 2016

* Converted the source codes into C++11. :tada:

### May 11th 2016

* Improve mine-planting algorithm
    * Now the number of mines is constant at every minefield at same dimension.
    
        The ratio of mines (R) is determined by following formula:<br>
        (n is the number of total cells, i.e., rows * cols)

        ![](http://www.sciweavers.org/tex2img.php?eq=R%28n%29%3D%5Clog_%7B2.15561%5Ctimes10%5E%7B9%7D%7D%28n%29-0.08101856&bc=Transparent&fc=Black&im=png&fs=18&ff=concmath&edit=0)

* The elapsed time is displayed in seconds when the game is over.

### May 14th 2016

* Display the leaderboard before/after the arcade mode games.
* Players can save their names if they cleared the arcade mode games with high scores.

### May 15th 2016

* Fancier leaderboards and the name input screen.

### May 19th 2016

* The score data file (`scores.dat`) is locked when a process is reading data from/writing data to that file.
