#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."
#define MAX_NUMBER_OF_STRINGS 50
#define MAX_LENGTH_OF_STRING 10


/* Useful macro for handling error codes */
#define DIE(assertion, call_description)                    \
    do {                                    				\
        if (assertion) {                       			    \
            fprintf(stderr, "(%s, %s, %d): ",               \
                    __FILE__, __FUNCTION__, __LINE__);      \
            perror(call_description);                       \
            exit(EXIT_FAILURE);                             \
        }                                                   \
    } while (0)

/* function that creates an empty list */
List *create_list()
{
	List *list = malloc(sizeof(List));
	DIE(!list, "Malloc failed\n");
	list->head = NULL;

	return list;
}

/* function that creates a FileTree structure  */
FileTree createFileTree(char* rootFolderName)
{
	FileTree file_tree;
	/* allocating memory for the root node */
	file_tree.root = malloc(sizeof(TreeNode));
	DIE(!file_tree.root, "Malloc failed\n");

	int length = strlen(rootFolderName);

	/* allocating memory for the name and setting it */
	file_tree.root->name = malloc(length + 1);
	DIE(!file_tree.root->name, "Malloc failed\n");

	memcpy(file_tree.root->name, rootFolderName, length + 1);

	/* setting the type to folder */
	file_tree.root->type = FOLDER_NODE;

	/* initializing the parent to NULL and its content until further changes*/
	file_tree.root->parent = NULL;
	file_tree.root->content = NULL;

	/* freeing the memory for the name that was allocated by strdup
	in the main function */
	free(rootFolderName);

	return file_tree;
}

/* function that eliberates all the memory used by the program */
void freeTree(FileTree fileTree)
{
	TreeNode *root = fileTree.root;

	/* going through roots content until it's all freed */
	if (root->content != NULL) {
		/* getting the list of its children and the first child of it */
		List *children = ((FolderContent *)root->content)->children;
		ListNode *child = children->head;

		/* going through all the children the root has */
		while (child != NULL) {
			/* recursive deleting and freeing all the memory for the child */
			rmrec(root, child->info->name);
			ListNode *aux = child;

			child = child->next;
			/* also freeing the memory for the structure of the child itself */
			free(aux);
		}
	}

	free(root->name);
	free(root);
}

/* function that searches a folder or a file in the given folder */
TreeNode *search_node_recursive(TreeNode *currentNode,
									char *arg, int *writted)
{
	/* verifying if the given folder and its content are valid */
	if (currentNode == NULL || currentNode->content == NULL) {
		return NULL;
	}

	/* getting the list of children it has */
	List *children = ((FolderContent *)currentNode->content)->children;

	/* getting the first child */
	ListNode *child = children->head;

	/* going through the kids */
	while (child != NULL) {
		/* getting the info that it holds */
		TreeNode *childs_info = child->info;

		/* verifying if the node it's a file and displaying its content */
		if (strcmp(childs_info->name, arg) == 0 &&
			childs_info->type == FILE_NODE) {
			printf("%s: %s\n", childs_info->name,
					((FileContent *)childs_info->content)->text);

			/* setting the value to one to know we found what
			we were looking for */
			*writted = 1;
			return NULL;
		}

		/* if it's not a file we return the info of the folder we found */
		if (strcmp(childs_info->name, arg) == 0) {
			return childs_info;
		}

		/* if it's not a file but neither what we're searching, we continue
		to look for it recursively */
		if (childs_info->type == FOLDER_NODE) {
			search_node_recursive(childs_info, arg, writted);
		}

		child = child->next;
	}
}

/* function that displays what is in the given node or
in the folder we're in */
void ls(TreeNode* currentNode, char* arg)
{
	List *children;

	/* treating the case in which we're not given a specific node and we have
	to display the content of the folder we're in */
	if (arg[0] == '\0' || strcmp(arg, currentNode->name) == 0) {
		/* verifying if the data we're given it's valid */
		if (currentNode == NULL || currentNode->content == NULL) {
			return;
		}

		children = ((FolderContent *)currentNode->content)->children;

		/* going through the children it has and displaying them all */
		ListNode *child = children->head;

		while (child != NULL) {
			TreeNode *childs_info = child->info;

			printf("%s\n", childs_info->name);

			child = child->next;
		}

		return;
	}

	/* copying the node we're in to know what to come back to */
	TreeNode *initial_node = currentNode;

	/* setting the value to 0 to know if we've found a file or not */
	int writted = 0;

	currentNode = search_node_recursive(currentNode, arg, &writted);

	/* if we've found a file we exit the function and reset the current node */
	if (writted == 1) {
		currentNode = initial_node;
		return;
	}

	/* if we didn't find nothing, we print a specific message
	and reset the current node */
	if (currentNode == NULL || currentNode->content == NULL) {
		printf("ls: cannot access '%s': No such file or directory\n", arg);
		currentNode = initial_node;
		return;
	}

	children = ((FolderContent *)currentNode->content)->children;

	/* displaying the children */
	ListNode *child = children->head;

	while (child != NULL) {
		TreeNode *childs_info = child->info;

		printf("%s\n", childs_info->name);

		child = child->next;
	}

	currentNode = initial_node;
}

/* function that shows where we're at in the file system */
void pwd(TreeNode* treeNode)
{
	if (strcmp(treeNode->name, "root") == 0) {
		printf("root\n");
		return;
	}

	/* allocating memory for a number of strings */
	char **strings = malloc(MAX_NUMBER_OF_STRINGS * sizeof(char *));
	DIE(!strings, "Malloc failed\n");

	for (int i = 0; i < MAX_NUMBER_OF_STRINGS; i++) {
		strings[i] = malloc(MAX_LENGTH_OF_STRING * sizeof(char));
		DIE(!strings[i], "Malloc failed\n");
	}

	/* counter for the number of folders we're going through */
	int cnt = 0;

	/* going through the given node until we reach the root and
	putting them in the strings array */
	while (strcmp(treeNode->name, "root") != 0) {
		memcpy(strings[cnt], treeNode->name, strlen(treeNode->name) + 1);

		cnt += 1;

		treeNode = treeNode->parent;
	}

	/* printing the path */
	printf("root");

	for (int i = cnt - 1; i >= 0; i--) {
		printf("/%s", strings[i]);
	}

	printf("\n");

	/* eliberating the memory used for the strings */
	for (int i = 0; i < MAX_NUMBER_OF_STRINGS; i++) {
		free(strings[i]);
	}

	free(strings);
}

/* function that moves the current node to the given node */
TreeNode *cd_one_arg(TreeNode *currentNode, char *p)
{
	/* checking if the given node it's a symbol for the parent
	of the current node */
	if (strcmp(p, "..") == 0) {
		return currentNode->parent;
	}

	/* verifying the validity of the node */
	if (currentNode == NULL || currentNode->content == NULL) {
		return NULL;
	}

	/* going through the children and returning the node we want */
	List *children = ((FolderContent *)currentNode->content)->children;

	ListNode *child = children->head;

	while (child != NULL) {
		TreeNode *childs_info = child->info;

		if (strcmp(childs_info->name, p) == 0) {
			return childs_info;
		}

		child = child->next;
	}

	return NULL;
}

/* function that moves the current node we're in, to the node
we want by a given path */
TreeNode* cd(TreeNode* currentNode, char* path)
{
	/* saving the initial node */
	TreeNode *initial_node = currentNode;

	/* making a copy of the path because we will modify the string */
	char *copy_path = malloc(strlen(path) + 1);
	DIE(!copy_path, "Malloc failed\n");
	memcpy(copy_path, path, strlen(path) + 1);

	/* getting the folders of the path one by one using
	the previous function */
	char *p = strtok(path, "/ ");

	while (p) {
		currentNode = cd_one_arg(currentNode, p);

		if (currentNode == NULL) {
			printf("cd: no such file or directory: %s\n", copy_path);

			currentNode = initial_node;

			free(copy_path);
			return currentNode;
		}

		p = strtok(NULL, "/ ");
	}

	/* eliberating the memory we don't need and returning the node we wanted */
	free(copy_path);

	return currentNode;
}

/* function that prints the whole data contained by the current folder */
void display_from_current_folder(TreeNode *currentNode, int *nr_dir,
									int *nr_files, int level)
{
	if (currentNode == NULL || currentNode->content == NULL) {
		return;
	}

	List *children = ((FolderContent *)currentNode->content)->children;

	ListNode *child = children->head;

	while (child != NULL) {
		TreeNode *childs_info = child->info;

		/* if the child is a folder */
		if (childs_info->type == FOLDER_NODE) {
			/* depending on which level the child is on, we print a
			certain number of spaces */
			for (int i = 0; i < level; i++) {
				printf("    ");
			}

			printf("%s\n", childs_info->name);

			*nr_dir += 1;

			/* recursive call of the function for the data of the child */
			display_from_current_folder(childs_info, nr_dir,
										nr_files, level + 1);

		} else {
			for (int i = 0; i < level; i++){
				printf("    ");
			}

			printf("%s\n", childs_info->name);

			*nr_files += 1;
		}

		child = child->next;
	}
}

/* function that displays a hierarchy of the file system of the current folder
or of the given folder */
void tree(TreeNode* currentNode, char* arg)
{
	/* keeping the number of directories, the files and the level */
	int nr_dir = 0, nr_files = 0, level = 0;

	/* if there isn't an argument given to the function */
	if (arg[0] == '\0') {
		display_from_current_folder(currentNode, &nr_dir, &nr_files, level);

		printf("%d directories, %d files\n", nr_dir, nr_files);

		return;
	}

	TreeNode *initial_node = currentNode;

	char *copy_path = malloc(strlen(arg) + 1);
	DIE(!copy_path, "Malloc failed\n");
	memcpy(copy_path, arg, strlen(arg) + 1);

	char *p = strtok(arg, "/ ");

	while (p) {
		currentNode = cd_one_arg(currentNode, p);

		if (currentNode == NULL || currentNode->type == FILE_NODE) {
			printf("%s [error opening dir]\n\n", copy_path);
			printf("%d directories, %d files\n", nr_dir, nr_files);

			currentNode = initial_node;

			free(copy_path);
			return;
		}

		p = strtok(NULL, "/ ");
	}

	/* after we changed the folder we are in */
	display_from_current_folder(currentNode, &nr_dir, &nr_files, level);
	printf("%d directories, %d files\n", nr_dir, nr_files);

	currentNode = initial_node;

	free(copy_path);
}

/* function that creates a new folder in the folder we're in */
void mkdir(TreeNode* currentNode, char* folderName)
{
	/* if the folders content hasn't been created yet */
	if (currentNode->content == NULL) {
		currentNode->content = malloc(sizeof(FolderContent));
		DIE(!currentNode->content, "Malloc failed\n");

		((FolderContent *)currentNode->content)->children =
			create_list();
	}

	List *children = ((FolderContent *)currentNode->content)->children;

	ListNode *child = children->head;

	/* if the folder doesn't have any children we create the first one*/
	if (child == NULL) {
		/* allocating memory for the child and its info */
		children->head = malloc(sizeof(ListNode));
		DIE(!children->head, "Malloc failed\n");

		TreeNode *new_childs_info = malloc(sizeof(TreeNode));
		DIE(!new_childs_info, "Malloc failed\n");

		/* setting the characteristics of the childs info */
		new_childs_info->parent = currentNode;

		new_childs_info->name = malloc(strlen(folderName) + 1);
		DIE(!new_childs_info->name, "Malloc failed\n");
		memcpy(new_childs_info->name, folderName, strlen(folderName) + 1);

		new_childs_info->type = FOLDER_NODE;
		new_childs_info->content = NULL;

		children->head->info = new_childs_info;
		children->head->next = NULL;

		free(folderName);

		return;
	}

	/* if there are existing children already we go through them to see
	if there is one with the same name */
	while (child != NULL) {
		TreeNode *childs_info = child->info;

		if (strcmp(childs_info->name, folderName) == 0) {
			printf("mkdir: cannot create directory '%s': File exists\n",
				folderName);

			free(folderName);

			return;
		}

		child = child->next;
	}

	/* adding the new folder to the front of the list of children
	of the current folder */
	ListNode *new_folder = malloc(sizeof(ListNode));
	DIE(!new_folder, "Malloc failed\n");

	TreeNode *new_folders_info = malloc(sizeof(TreeNode));
	DIE(!new_folders_info, "Malloc failed\n");

	new_folders_info->parent = currentNode;

	new_folders_info->name = malloc(strlen(folderName) + 1);
	DIE(!new_folders_info->name, "Malloc failed\n");
	memcpy(new_folders_info->name, folderName, strlen(folderName) + 1);

	new_folders_info->type = FOLDER_NODE;
	new_folders_info->content = NULL;

	new_folder->info = new_folders_info;

	new_folder->next = children->head;
	children->head = new_folder;

	free(folderName);
}

/* function that deletes the given file from the folder we're in */
void delete_the_file(TreeNode *currentNode, char *filename)
{
	List *children = ((FolderContent *)currentNode->content)->children;
	ListNode *child = children->head;

	/* if the first child is the one we look for */
	if (strcmp(child->info->name, filename) == 0) {
		/* changing the head of the list */
		children->head = children->head->next;

		/* freeing the content of it, the name and the info */
		free(child->info->name);

		if (((FileContent *)child->info->content) != NULL) {
			free(((FileContent *)child->info->content)->text);
			free((FileContent *)child->info->content);
		}

		free(child->info);

		/* if the list of children gets empty we also free its memory */
		if (children->head == NULL) {
			free(children->head);
			free(children);
			free((FolderContent *)currentNode->content);
		}

		return;
	}

	/* in the other case we go through all the children and
	re-do the connections */
	ListNode *prev;

	while (child != NULL && strcmp(child->info->name, filename) != 0) {
		prev = child;
		child = child->next;
	}

	prev->next = child->next;

	/* again freeing all the memory */
	free(child->info->name);

	if (((FileContent *)child->info->content) != NULL) {
		free(((FileContent *)child->info->content)->text);
		free((FileContent *)child->info->content);
	}

	free(child->info);
	free(child);
}

/* function that searches a file to delete it */
void search_file_recursiv(TreeNode *currentNode, char *fileName, int *find)
{
	if (currentNode == NULL || *find != 0 || currentNode->content == NULL) {
		return;
	}

	List *children = ((FolderContent *)currentNode->content)->children;

	/* going through the children and deleting the file we want
	when we find it */
	ListNode *child = children->head;

	while (child != NULL) {
		TreeNode *childs_info = child->info;

		if (strcmp(childs_info->name, fileName) == 0 &&
			childs_info->type == FILE_NODE) {
			delete_the_file(currentNode, fileName);

			*find = 1;
			return;
		}

		/* if we find a folder with the same name instead of a file we put
		a certain value in the find variable */
		if (strcmp(childs_info->name, fileName) == 0 &&
			childs_info->type == FOLDER_NODE) {
			*find = -1;
			return;
		}

		/* if we reach this we continue to look for the file */
		if (childs_info->type == FOLDER_NODE) {
			search_file_recursiv(childs_info, fileName, find);
		}

		child = child->next;
	}
}

/* function that deletes a file */
void rm(TreeNode* currentNode, char* fileName)
{
	/* we use the previous function to treat each case */
	int find = 0;
	search_file_recursiv(currentNode, fileName, &find);

	if (find == 0) {
		printf("rm: failed to remove '%s': No such file or directory\n",
			fileName);
	}

	if (find == -1) {
		printf("rm: cannot remove '%s': Is a directory\n", fileName);
	}
}

/* function that deletes an empty folder */
void delete_empty_folder(TreeNode *currentNode, char *folderName)
{
	if (currentNode == NULL || currentNode->content == NULL) {
		return;
	}

	List *children = ((FolderContent *)currentNode->content)->children;

	ListNode *child = children->head;

	/* if the folder we want to delete it's the first child
	of the current folder */
	if (strcmp(child->info->name, folderName) == 0) {
		children->head = children->head->next;

		/* freeing its memory */
		free(child->info->name);
		free(child->info);

		/* if there are no children left we also free the memory used*/
		if (children->head == NULL) {
			free(children->head);
			free(children);
			free((FolderContent *)currentNode->content);
		}

		return;
	}

	/* if it's not the first child we re-do its connections */
	ListNode *prev;

	while (child != NULL && strcmp(child->info->name, folderName) != 0) {
		prev = child;
		child = child->next;
	}

	prev->next = child->next;

	/* eliberating the memory */
	free(child->info->name);
	free(child->info);
	free(child);
}

/* function that searches a folder in order to delete it */
void search_folder(TreeNode *currentNode, char *folderName, int *find)
{
	if (currentNode == NULL || *find != 0 || currentNode->content == NULL) {
		return;
	}

	List *children = ((FolderContent *)currentNode->content)->children;

	ListNode *child = children->head;

	while (child != NULL) {
		TreeNode *childs_info = child->info;

		if (strcmp(childs_info->name, folderName) == 0 &&
			childs_info->type == FOLDER_NODE) {
			if (childs_info->content == NULL) {
				delete_empty_folder(currentNode, folderName);
				*find = 1;
			/* if the folder isn't empty we put a certain value
			in the find variable */
			} else {
				*find = -2;
			}
			return;
		}

		/* if we find a file with the same name we also put a
		certain value in the find variable */
		if (strcmp(childs_info->name, folderName) == 0 &&
			childs_info->type == FILE_NODE) {
			*find = -1;
			return;
		}

		/* if we find a folder we go and search again */
		if (childs_info->type == FOLDER_NODE) {
			search_folder(childs_info, folderName, find);
		}

		child = child->next;
	}
}

/* function that wants to delete a folder */
void rmdir(TreeNode* currentNode, char* folderName)
{
	/* based on the value of find we delete or print certain messages */
	int find = 0;
	search_folder(currentNode, folderName, &find);

	if (find == 0) {
		printf("rmdir: failed to remove '%s': No such file or directory\n",
			folderName);
	}

	if (find == -1) {
		printf("rmdir: failed to remove '%s': Not a directory\n",
			folderName);
	}

	if (find == -2) {
		printf("rmdir: failed to remove '%s': Directory not empty\n",
			folderName);
	}
}

/* function that returns the information of a certain folder or file */
TreeNode *find_folder(TreeNode *currentNode, char *resourceName)
{
	if (currentNode == NULL || currentNode->content == NULL) {
		return NULL;
	}

	List *children = ((FolderContent *)currentNode->content)->children;

	ListNode *child = children->head;

	while(child != NULL) {
		TreeNode *childs_info = child->info;

		if (strcmp(childs_info->name, resourceName) == 0) {
			return childs_info;
		}

		if (childs_info->type == FOLDER_NODE) {
			find_folder(childs_info, resourceName);
		}

		child = child->next;
	}
}

/* recursive function that deletes a folder and all the data contained by it
or a file */
void rmrec(TreeNode* currentNode, char* resourceName)
{
	/* getting the data that has to be deleted */
	TreeNode *childs_info = find_folder(currentNode, resourceName);

	/* verifying if it exits */
	if (childs_info == NULL) {
		printf("rmrec: failed to remove '%s': No such file or directory\n",
			resourceName);
		return;
	}

	/* if it's a folder that has to be deleted */
	if (childs_info->type == FOLDER_NODE) {
		/* if it doesn't have content */
		if (childs_info->content == NULL) {
			delete_empty_folder(currentNode, childs_info->name);

		/* if it has content we have to delete it too */
		} else {
			List *children = ((FolderContent *)childs_info->content)->children;
			ListNode *child = children->head;

			/* going through the children and checking which one is a folder
			and which one is a file */
			while (child != NULL) {
				/* if it's a folder we use the same function for it */
				if (child->info->type == FOLDER_NODE) {
					rmrec(childs_info, child->info->name);
					/* otherwise we only delete the file */
				} else {
					delete_the_file(childs_info, child->info->name);
				}

				ListNode *aux = child;

				child = child->next;
				free(aux);
			}
			/* after deleting the content we delete the empty folder
			remained */
			delete_empty_folder(childs_info->parent, childs_info->name);
		}
	} else {
		delete_the_file(childs_info->parent, childs_info->name);
	}
}

/* function that creates a new file in the current folder */
void touch(TreeNode* currentNode, char* fileName, char* fileContent)
{
	/* if the current folder doesn't have content we alloc memory for it */
	if (currentNode->content == NULL) {
		currentNode->content = malloc(sizeof(FolderContent));
		DIE(!currentNode->content, "Malloc failed\n");

		((FolderContent *)currentNode->content)->children = create_list();
	}

	List *children = ((FolderContent *)currentNode->content)->children;
	ListNode *child = children->head;

	/* if there are no other children we have to create them */
	if (child == NULL) {
		children->head = malloc(sizeof(ListNode));
		DIE(!children->head, "Malloc failed\n");

		TreeNode *new_childs_info = malloc(sizeof(TreeNode));
		DIE(!new_childs_info, "Malloc failed\n");

		new_childs_info->parent = currentNode;

		new_childs_info->name = malloc(strlen(fileName) + 1);
		DIE(!new_childs_info->name, "Malloc failed\n");
		memcpy(new_childs_info->name, fileName, strlen(fileName) + 1);

		new_childs_info->type = FILE_NODE;
		new_childs_info->content = NULL;

		children->head->info = new_childs_info;

		/* copying the content of the file (if it's given) which is a string */
		if (fileContent[0] != '\0') {
			new_childs_info->content = malloc(sizeof(FileContent));
			DIE(!new_childs_info->content, "Malloc failed\n");

			((FileContent *)new_childs_info->content)->text =
				malloc(strlen(fileContent) + 1);
			DIE(!((FileContent *)new_childs_info->content)->text,
				"Malloc failed\n");
			memcpy(((FileContent *)new_childs_info->content)->text,
				fileContent, strlen(fileContent) + 1);
		}

		children->head->next = NULL;

		free(fileContent);
		free(fileName);

		return;
	}

	while (child != NULL) {
		TreeNode *childs_info = child->info;

		/* if there already is a file with the same name */
		if (strcmp(childs_info->name, fileName) == 0) {
			free(fileName);
			free(fileContent);

			return;
		}

		child = child->next;
	}

	ListNode *new_file = malloc(sizeof(ListNode));
	DIE(!new_file, "Malloc failed\n");

	TreeNode *new_files_info = malloc(sizeof(TreeNode));
	DIE(!new_files_info, "Malloc failed\n");

	new_files_info->parent = currentNode;

	new_files_info->name = malloc(strlen(fileName) + 1);
	DIE(!new_files_info->name, "Malloc failed\n");
	memcpy(new_files_info->name, fileName, strlen(fileName) + 1);

	new_files_info->type = FILE_NODE;

	new_file->info = new_files_info;
	new_files_info->content = NULL;

	new_file->next = children->head;
	children->head = new_file;

	if (fileContent[0] != '\0') {
		new_files_info->content = malloc(sizeof(FileContent));
		((FileContent *)new_files_info->content)->text =
			malloc(strlen(fileContent) + 1);
		DIE(!((FileContent *)new_files_info->content)->text,
				"Malloc failed\n");
		memcpy(((FileContent *)new_files_info->content)->text, fileContent,
			strlen(fileContent) + 1);
	}

	free(fileContent);
	free(fileName);
}

/* function that copies a file from its source to a given destination */
void cp(TreeNode* currentNode, char* source, char* destination)
{
	/* saving the initial node to not lose it and making
	a copy of the source */
    TreeNode *initial_node = currentNode;

	char *copy_source = malloc(strlen(source) + 1);
	DIE(!copy_source, "Malloc failed\n");
	memcpy(copy_source, source, strlen(source) + 1);

	/* getting the file we want to copy */
	char *p = strtok(source, "/ ");

	while (p) {
		currentNode = cd_one_arg(currentNode, p);

		p = strtok(NULL, "/ ");
	}

	/* if the source is not a file we free the used memory,
	reset the current folder and exit the program */
	if (currentNode->type == FOLDER_NODE) {
		printf("cp: -r not specified; omitting directory '%s'\n", copy_source);

		currentNode = initial_node;

		free(copy_source);
		return;
	}

	/* copying the file name and its content */
	char *new_file_name = malloc(strlen(currentNode->name) + 1);
	DIE(!new_file_name, "Malloc failed\n");
	memcpy(new_file_name, currentNode->name, strlen(currentNode->name) + 1);

	char *new_file_content;

	/* if there is no content we only initialize the new file content
	with a string terminator */
	if (currentNode->content != NULL) {
		new_file_content =
			malloc(strlen(((FileContent *)currentNode->content)->text) + 1);
		DIE(!new_file_content, "Malloc failed\n");
		memcpy(new_file_content, ((FileContent *)currentNode->content)->text,
			strlen(((FileContent *)currentNode->content)->text) + 1);

	} else {
		new_file_content = strdup("\0");
		DIE(!new_file_content, "Malloc failed\n");
	}

	/* setting the current node to the initial one to be able to get
	the destination */ 
	currentNode = initial_node;

	char *copy_destination = malloc(strlen(destination) + 1);
	DIE(!copy_destination, "Malloc failed\n");
	memcpy(copy_destination, destination, strlen(destination) + 1);

	p = strtok(destination, "/ ");

	while (p) {
		currentNode = cd_one_arg(currentNode, p);

		/* if the destination doesn't exist we free all the use memory
		before existing the program */
		if (currentNode == NULL) {
			printf("cp: failed to access '%s': Not a directory\n",
				copy_destination);

			currentNode = initial_node;

			free(new_file_content);
			free(new_file_name);
			free(copy_source);
			free(copy_destination);

			return;
		}

		p = strtok(NULL, "/ ");
	}

	/* if the destination is a folder we use the touch function
	to copy the data */
	if (currentNode->type == FOLDER_NODE) {
		touch(currentNode, new_file_name, new_file_content);

	/* if the destination is a file and the content exists we free the
	content it has and put the new content in it */
	} else if (currentNode->type == FILE_NODE &&
				currentNode->content != NULL) {
		free(((FileContent *)currentNode->content)->text);

		((FileContent *)currentNode->content)->text =
			malloc(strlen(new_file_content) + 1);
		DIE(!((FileContent *)currentNode->content)->text, "Malloc failed\n");
		memcpy(((FileContent *)currentNode->content)->text, new_file_content,
			strlen(new_file_content) + 1);

		free(new_file_content);
		free(new_file_name);
	}

	currentNode = initial_node;

	free(copy_source);
	free(copy_destination);
}

/* function that creates a new child in a given folder */
void create_new_child(TreeNode* new_child, TreeNode* new_parent)
{
	if (new_parent->content == NULL) {
		new_parent->content = malloc(sizeof(FolderContent));
		((FolderContent *)new_parent->content)->children =
			malloc(sizeof(List));
		DIE(!new_parent->content, "Malloc failed\n");

		((FolderContent *)new_parent->content)->children->head = NULL;
	}

	List *children = ((FolderContent *)new_parent->content)->children;
	ListNode *child = children->head;

	/* if there aren't any children we allocate memory and
	do the connections */
	if (child == NULL) {
		children->head = malloc(sizeof(ListNode));
		DIE(!children->head, "Malloc failed\n");

		children->head->info = new_child;
		children->head->next = NULL;

		new_child->parent = new_parent;

		return;
	}

	/* adding the node to the front of the list */
	ListNode *new_folder = malloc(sizeof(ListNode));
	DIE(!new_folder, "Malloc failed\n");

	new_folder->info = new_child;

	new_folder->next = children->head;
	children->head = new_folder;

	new_child->parent = new_parent;
}

/* function that moves a file or a folder (with all it's data)
to a given destination */
void mv(TreeNode* currentNode, char* source, char* destination)
{
	/* saving the initial node to not lose it and making
	a copy of the destination */
    TreeNode *initial_node = currentNode;

	char *copy_destination = malloc(strlen(destination) + 1);
	DIE(!copy_destination, "Malloc failed\n");
	memcpy(copy_destination, destination, strlen(destination) + 1);

	/* getting the destination */
	char *p = strtok(destination, "/ ");

	while (p) {
		currentNode = cd_one_arg(currentNode, p);

		if (currentNode == NULL) {
			printf("mv: failed to access '%s': Not a directory\n",
				copy_destination);

			currentNode = initial_node;

			free(copy_destination);

			return;
		}

		p = strtok(NULL, "/ ");
	}

	/* getting the source */
	TreeNode *source_node = initial_node;

	p = strtok(source, "/ ");

	while (p) {
		source_node = cd_one_arg(source_node, p);
		p = strtok(NULL, "/ ");
	}

	/* if the destination is a file we changed its content
	with the one from the source */
	if (currentNode->type == FILE_NODE) {
		free(((FileContent *)currentNode->content)->text);

		char *new_file_content =
			malloc(strlen(((FileContent *)source_node->content)->text) + 1);
		DIE(!new_file_content, "Malloc failed\n");
		memcpy(new_file_content, ((FileContent *)source_node->content)->text,
			strlen(((FileContent *)source_node->content)->text) + 1);

		((FileContent *)currentNode->content)->text =
			malloc(strlen(new_file_content) + 1);
		DIE(!((FileContent *)currentNode->content)->text, "Malloc failed\n");
		memcpy(((FileContent *)currentNode->content)->text, new_file_content,
			strlen(new_file_content) + 1);

		/* going through the sources parent children and deletes the source file */
		List *children = ((FolderContent *)source_node->parent->content)->children;
		ListNode *child = children->head;

		while (child != NULL && strcmp(child->info->name, source_node->name) != 0) {
			child = child->next;
		}

		delete_the_file(source_node->parent, source_node->name);
		free(child);

		/* resetting the current folder and eliberating all the memory used */
		currentNode = initial_node;

		free(new_file_content);
		free(copy_destination);

		return;
	}

	/* if the destination isn't a file we create the child in it */
	create_new_child(source_node, currentNode);

	List *children = ((FolderContent *)initial_node->content)->children;
	ListNode *child = children->head;

	/* recreating the sources connections to delete the data */
	if (strcmp(child->info->name, source_node->name) == 0) {
		children->head = children->head->next;

		currentNode = initial_node;

		free(child);
		free(copy_destination);

		return;
	}

	ListNode *prev;

	while (child != NULL && strcmp(child->info->name,
		source_node->name) != 0) {
		prev = child;
		child = child->next;
	}

	prev->next = child->next;
	free(child);

	currentNode = initial_node;

	free(copy_destination);
}
