This was one of the assignments for a low-level Systems course that I took.  While this code's formatting & documentation was not my finest work, it is functional and demonstrates an understanding of computer systems and a proficiency with C/C++.

*  ackerman.h / ackerman.c : These files were provided with the mathematical Ackerman function which was used to exercise the actual allocator.  The external function "ackerman_main" was called from the main program.

*  my_allocator.h / my_allocator.c : These files contain the actual implementations of my functions written between the comment sections as defined by the course.  Essentially, I replaced a call for the function "malloc" with my own allocator.

*  memtest.c : This file calls the "ackerman_main" function to exercise my allocator.

*  makefile : This file tells 'make' how to compile everything together.







