NAMES: Arghir Monica-Andreea & Sindrilaru Catalina-Maria

GROUP: 312CA

# Simple Linux File System  - Third Homework

Before going through each task we had to implement, we created the file
tree by allocating memory for the root node, for the roots name and setting
all its characteristics to initial values. The content is initially NULL
because we will allocate memory for it when we're going to put something there.


## 1. TOUCH COMMAND
***   
When creating a file in the folder where we are at, we check first if the
content of the folder doesn't exist in order to allocate memory for it.

If the folder doesn't have any other children we have to allocate memory
for them and to create the node which contains the new file we want to insert
(which is also the first child of the folder). After copying the files name and
setting its parent, type and content, we check if there is any content given to
the command to copy it in the structure.
    
If the folder does have any other children, we go through them to see if a
file with the same name exists already and exit the function if it does. After
this verification we add the file to the beginning of the list of children in a
similar way as before.


## 2. LS COMMAND
***

Firstly we checked if there isn't an argument given to the command, in
which case we print the folders and files from the current folder. After
verifying if there are no invalid fields we went through every child of the
folder to display its name.

If we have arguments, we made a copy of the current node to come back to it
because we used a recursive function that finds the wanted folder or file and
puts it in the current node given. If we find the file we're looking for, we
reset the current node to the initial one and exit the function, but if we find
the folder we're looking for we go through all his children and display their
names. If we didn't find it we print a specific message. 


## 3. MKDIR COMMAND
***

We do almost the same thing as in the touch function, but in this case
we always set the content to NULL, until further data will be added in the
specific folder.


## 4. CD COMMAND
***

We copy the current node, to not lose it in the process, and the path
because we will separate the folders and files given in the path using a
function which goes directory by directory. After doing that we have in the
current node the node in which we have to go.


## 5. TREE COMMAND
***

For this task we keep the number of directories and files to print them
in the end, and also the level to know how many spaces we have to display for
each element in the folder for which we print its hierarchy.

As before we keep copies of the current folder and the path to be able to
get back to it, to print it. We use a recursive function that goes through all
the children and prints their names in a hierarchical order.

If there are no arguments given, we print the tree of the current folder,
but if we have a certain path we get it using the same helpful function as in
CD task and use that for displaying the tree.


## 6. PWD COMMAND
***

For printing the path of the current folder we put each parent, starting
from the one of the current folder, in an array of strings, which we print from
the end to the beginning and free in the end of the function.


## 7. RMDIR COMMAND
***

This function uses a recursive one that searches the folder that has to be
deleted and deletes it, by freeing all it's memory. It also sets a variable to
different values for different cases in which we have to print different
messages.


## 8. RM COMMAND
***

For the rm function we've also used a recursive function in order to find
a certain file in the current folder, proceeding similary as before.


## 9. RMREC COMMAND
***

For this recursive function that deletes everything, we used the helpful
functions that were used before for deleting a file, an empty folder or a
folder and all the data contained by it. We went through each folder/file from
the given one and deleted and freed all its data recursively.


## 10. CP COMMAND
***

When copying a file from a source to a destination we firstly got the
source file and put it in the current node. If the source was a folder we
simply exited the function and freed the memory used. After that we copy the
files name and content to be able to put them in the destination that we
extracted similary as the source. 

If the destination was a folder we used the touch function and gave as
arguments what we extracted before. If it's a file we overwrite the content.


## 11. MV COMMAND
***

In this function we proceeded similary as in the CP one, but if the
destination wasn't a file we created in the source folder a new folder
(a new child) with the data contained in the destination.

After creating the data in the destination we have to restore the
connections such that we've only moved the data from the source, not deleted
it.

***
    
When exiting the program, the function that frees all the memory used by
the file tree goes recursively using the rmrec function through all the
children of the root and frees everything. 
