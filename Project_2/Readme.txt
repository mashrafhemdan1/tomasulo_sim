Mohamed Hemdan, 900171923
Ahmed Hassan, 900171850

As of date July 22nd 2020 (Project 2):

Issues:
1- So many loops as we initially intended, has to take the format of instructions spaced exactly like the one in project description with the same commas exactly. 
2- The memory was not made of a single data_structure we split it to instruction memory and data memory since we needed one of them to hold string, instruction, and the other to hold integer, data.
3- The memory was not made to be 128KB because Visual studio kept giving errors about so we implemented a small memory overall we figured this wouldn't pose a great defect since we are mainly trying to simulate the tomasulo's algorithm and the memory is but a means not an end.

Assumptions
1- All instructions are entered in capital letters, because this is how we do the comparisons inside.

What works from the Risc?
The data hazard handling seems to be working quite well in all its three dependencies, in addition to control hazard handling.

What doesn't work?
We tried implementing the bonus of inspecting what is going on inside the Tomasulo simulation by providing an option of inspecting instruction processing; however, it is not web based so if you remove the fact that it isnot web-based it would be functional in the sense of the bonus
There is a problem with the RET instruction at the end. It takes more cycles than needed. Also, there is something about the negative branch predictor that affects the dependency (delays for additional cycles).

Code guidlines:
We tried following the chosen OOP approach as much as possible by employing setters and getters to most of the attributes of our classes; as well as employing inheritance and polymorphism to decrease the amount of code required to write different classes. Finally we tried using less space by pointing a parent pointer to many child objects which gave us the opportunity to store them all in one array instread of creating mutliple arrays each different kind of instruction. Finally the use of pointers throughout the program generally reduces the memory needed.