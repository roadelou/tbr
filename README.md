# tbr
TBR is a small tool with a very poorly inspired name. I use it as a small 
verilog package manager for my personnal use, but I am afraid it wouldn't be 
suitable for anyone else.

Among the limitations, it is decided at compiled time what your home folder 
should be (and the default value is /home/roadelou of course), so unless you 
are me you won't be able to use the tool.

The project is fairly small and straightforward to compile :
 - Use "make" to get the tbr.elf executable.
 - Use "make install" to install it. Well, that step will only work if you are 
me for now ... Hopefully I will correct that in the future.

TODO :
Create a initialization and finalization routine for the tbr.c implementation.
The use of an hash table to avoid including the same dependancy multiple times does not work, there is a bug somewhere.
