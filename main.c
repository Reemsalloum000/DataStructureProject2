#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define the structure for a Town
typedef struct Town {
    char district[50]; // Name of the district
    char town[50];     // Name of the town
    int population;    // Population of the town
    int elevation;     // Elevation of the town
    char hasMunicipality[4]; // "yes" or "no" indicating if the town has a municipality
    struct Town *left;  // Pointer to the left child in the AVL tree
    struct Town *right; // Pointer to the right child in the AVL tree
    int height;         // Height of the node in the AVL tree
} Town;

// Define the structure for a Hash Table
#define TABLE_SIZE 10 // Size of the hash table

typedef struct HashTableEntry {
    char town[50];     // Name of the town
    Town *townData;    // Pointer to the town data in the AVL tree
    int isOccupied;    // Flag to indicate if the slot is occupied
} HashTableEntry;

typedef struct HashTable {
    HashTableEntry *entries[TABLE_SIZE]; // Array of pointers to hash table entries
} HashTable;

// Function to get the height of a node in the AVL tree
int height(Town *node) {
    if (node == NULL)
        return 0;
    return node->height;
}

// Function to get the maximum of two integers
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Function to create a new town node
Town* newTown(char district[], char town[], int population, int elevation, char hasMunicipality[]) {
    Town* node = (Town*)malloc(sizeof(Town));
    strcpy(node->district, district);
    strcpy(node->town, town);
    node->population = population;
    node->elevation = elevation;
    strcpy(node->hasMunicipality, hasMunicipality);
    node->left = NULL;
    node->right = NULL;
    node->height = 1; // New node is initially added at leaf level (height = 1)
    return node;
}

// Function to perform a right rotation in the AVL tree
Town* rightRotate(Town *y) {
    Town *x = y->left;
    Town *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x; // New root after rotation
}

// Function to perform a left rotation in the AVL tree
Town* leftRotate(Town *x) {
    Town *y = x->right;
    Town *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y; // New root after rotation
}

// Function to get the balance factor of a node in the AVL tree
int getBalance(Town *node) {
    if (node == NULL)
        return 0;
    return height(node->left) - height(node->right);
}

// Function to insert a town into the AVL tree
Town* insert(Town* node, char district[], char town[], int population, int elevation, char hasMunicipality[]) {
    if (node == NULL)
        return newTown(district, town, population, elevation, hasMunicipality);

    // Perform standard BST insertion
    if (strcmp(town, node->town) < 0)
        node->left = insert(node->left, district, town, population, elevation, hasMunicipality);
    else if (strcmp(town, node->town) > 0)
        node->right = insert(node->right, district, town, population, elevation, hasMunicipality);
    else
        return node; // Duplicate towns are not allowed

    // Update height of the current node
    node->height = 1 + max(height(node->left), height(node->right));

    // Get the balance factor to check if the node became unbalanced
    int balance = getBalance(node);

    // Left Left Case
    if (balance > 1 && strcmp(town, node->left->town) < 0)
        return rightRotate(node);

    // Right Right Case
    if (balance < -1 && strcmp(town, node->right->town) > 0)
        return leftRotate(node);

    // Left Right Case
    if (balance > 1 && strcmp(town, node->left->town) > 0) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && strcmp(town, node->right->town) < 0) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node; // Return the unchanged node pointer
}

// Function to search for a town in the AVL tree
Town* findTown(Town* root, char town[]) {
    if (root == NULL || strcmp(root->town, town) == 0)
        return root;

    if (strcmp(root->town, town) < 0)
        return findTown(root->right, town);

    return findTown(root->left, town);
}

// Function to delete a town from the AVL tree
Town* deleteTown(Town* root, char town[]) {
    if (root == NULL) {
        printf("Town not found.\n");
        return root;
    }

    // Perform standard BST delete
    if (strcmp(town, root->town) < 0)
        root->left = deleteTown(root->left, town);
    else if (strcmp(town, root->town) > 0)
        root->right = deleteTown(root->right, town);
    else {
        // Node with only one child or no child
        if ((root->left == NULL) || (root->right == NULL)) {
            Town *temp = root->left ? root->left : root->right;

            // No child case
            if (temp == NULL) {
                temp = root;
                root = NULL;
            } else // One child case
                *root = *temp; // Copy the contents of the non-empty child

            free(temp);
        } else {
            // Node with two children: Get the inorder successor (smallest in the right subtree)
            Town *temp = root->right;
            while (temp->left != NULL)
                temp = temp->left;

            // Copy the inorder successor's data to this node
            strcpy(root->town, temp->town);
            strcpy(root->district, temp->district);
            root->population = temp->population;
            root->elevation = temp->elevation;
            strcpy(root->hasMunicipality, temp->hasMunicipality);

            // Delete the inorder successor
            root->right = deleteTown(root->right, temp->town);
        }
    }

    // If the tree had only one node, return
    if (root == NULL)
        return root;

    // Update height of the current node
    root->height = 1 + max(height(root->left), height(root->right));

    // Get the balance factor to check if the node became unbalanced
    int balance = getBalance(root);

    // Left Left Case
    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);

    // Left Right Case
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    // Right Right Case
    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);

    // Right Left Case
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root; // Return the balanced node
}

// Function to create a hash table
HashTable* createHashTable() {
    HashTable *table = (HashTable*)malloc(sizeof(HashTable));
    for (int i = 0; i < TABLE_SIZE; i++) {
        table->entries[i] = NULL; // Initialize all entries to NULL
    }
    return table;
}

// Hash function to compute the index for a town name
unsigned int hash(char *town) {
    unsigned long hashValue = 0;
    for (int i = 0; i < 5 && town[i] != '\0'; i++) {
        hashValue = hashValue * 31 + town[i]; // Use the first 5 characters for hashing
    }
    return hashValue % TABLE_SIZE; // Modulo operation to fit within table size
}

// Function to insert a town into the hash table
void insertIntoHashTable(HashTable *table, Town *townData) {
    unsigned int index = hash(townData->town); // Compute the hash index
    int originalIndex = index;

    // Linear probing to handle collisions
    while (table->entries[index] != NULL && table->entries[index]->isOccupied) {
        index = (index + 1) % TABLE_SIZE;
        if (index == originalIndex) {
            printf("Hash table is full!\n");
            return;
        }
    }

    // Allocate memory for the new entry
    HashTableEntry *entry = (HashTableEntry*)malloc(sizeof(HashTableEntry));
    if (entry == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    // Insert data into the hash table
    strcpy(entry->town, townData->town);
    entry->townData = townData;
    entry->isOccupied = 1; // Mark the slot as occupied

    table->entries[index] = entry; // Add the entry to the hash table
}

// Function to delete a town from the hash table
void deleteFromHashTable(HashTable *table, char *town) {
    unsigned int index = hash(town); // Compute the hash index
    int originalIndex = index;

    // Linear probing to find the town
    while (table->entries[index] != NULL) {
        if (table->entries[index]->isOccupied && strcmp(table->entries[index]->town, town) == 0) {
            // Free the memory allocated for the entry
            free(table->entries[index]);
            table->entries[index] = NULL; // Mark the slot as unoccupied
            printf("Town deleted successfully!\n");
            return;
        }
        index = (index + 1) % TABLE_SIZE;
        if (index == originalIndex) {
            break; // Entire table has been searched
        }
    }

    printf("Town not found.\n");
}

// Function to print towns in alphabetical order (in-order traversal of AVL tree)
void inOrder(Town* root) {
    if (root != NULL) {
        inOrder(root->left);
        printf("Town: %s, District: %s, Population: %d, Elevation: %d, Municipality: %s\n",
               root->town, root->district, root->population, root->elevation, root->hasMunicipality);
        inOrder(root->right);
    }
}

// Function to list towns with a population greater than a given number
void listTownsByPopulation(Town* root, int population) {
    if (root != NULL) {
        listTownsByPopulation(root->left, population);
        if (root->population > population) {
            printf("Town: %s, District: %s, Population: %d, Elevation: %d, Municipality: %s\n",
                   root->town, root->district, root->population, root->elevation, root->hasMunicipality);
        }
        listTownsByPopulation(root->right, population);
    }
}

// Function to list towns based on municipality status
void listTownsByMunicipality(Town* root, char hasMunicipality[]) {
    if (root != NULL) {
        listTownsByMunicipality(root->left, hasMunicipality);
        if (strcmp(root->hasMunicipality, hasMunicipality) == 0) {
            printf("Town: %s, District: %s, Population: %d, Elevation: %d, Municipality: %s\n",
                   root->town, root->district, root->population, root->elevation, root->hasMunicipality);
        }
        listTownsByMunicipality(root->right, hasMunicipality);
    }
}

// Function to save the AVL tree data to a file
void saveToFile(Town* root, FILE *file) {
    if (root != NULL) {
        saveToFile(root->left, file); // Traverse the left subtree
        fprintf(file, "%s:%s:%d:%d:%s\n", root->district, root->town, root->population, root->elevation, root->hasMunicipality); // Write data to file
        saveToFile(root->right, file); // Traverse the right subtree
    }
}

// Function to print the hash table
void printHashedTable(HashTable *table) {
    printf("Hashed Table:\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        printf("Index %d: ", i);
        HashTableEntry *entry = table->entries[i];
        if (entry == NULL) {
            printf("Empty\n");
        } else {
            printf("%s -> ", entry->town);
            printf("NULL\n");
        }
    }
}

// Function to print hash table statistics
void printHashTableStats(HashTable *table) {
    int count = 0;
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (table->entries[i] != NULL && table->entries[i]->isOccupied) {
            count++;
        }
    }
    printf("Size of hash table: %d\n", TABLE_SIZE);
    printf("Load factor: %.2f\n", (float)count / TABLE_SIZE);
}

// Function to search for a town in the hash table and print the number of collisions
void searchTownInHashTable(HashTable *table, char *town) {
    unsigned int index = hash(town);
    int collisions = 0;

    // Linear probing to find the town
    while (table->entries[index] != NULL) {
        if (table->entries[index]->isOccupied && strcmp(table->entries[index]->town, town) == 0) {
            printf("Town found: %s\n", table->entries[index]->town);
            printf("Number of collisions: %d\n", collisions);
            return;
        }
        collisions++;
        index = (index + 1) % TABLE_SIZE;
    }

    printf("Town not found.\n");
}

// Function to insert a new record into the hash table
void insertNewRecordIntoHashTable(HashTable *table, Town *root) {
    char town[50], district[50], hasMunicipality[4];
    int population, elevation;

    printf("Enter district: ");
    scanf("%s", district);
    printf("Enter town: ");
    scanf("%s", town);
    printf("Enter population: ");
    scanf("%d", &population);
    printf("Enter elevation: ");
    scanf("%d", &elevation);
    printf("Has municipality (yes/no): ");
    scanf("%s", hasMunicipality);

    // Insert the town into the AVL tree
    root = insert(root, district, town, population, elevation, hasMunicipality);

    // Insert the town into the hash table
    insertIntoHashTable(table, findTown(root, town));

    printf("New record inserted successfully!\n");
}

// Function to update the towns.txt file after deletion
void updateFileAfterDeletion(Town *root, char *town) {
    FILE *file = fopen("towns.txt", "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    // Helper function to write updated data to the file
    void writeToFile(Town *root) {
        if (root != NULL) {
            writeToFile(root->left);
            if (strcmp(root->town, town) != 0) { // Skip the deleted town
                fprintf(file, "%s:%s:%d:%d:%s\n", root->district, root->town, root->population, root->elevation, root->hasMunicipality);
            }
            writeToFile(root->right);
        }
    }

    writeToFile(root); // Write updated data to the file
    fclose(file);
    printf("File updated successfully!\n");
}

// Main function
int main() {
    FILE *file = fopen("districts.txt", "r");
    if (file == NULL) {
        printf("File not found. Creating a new file...\n");
        file = fopen("districts.txt", "w");
        if (file == NULL) {
            perror("Error creating file");
            return 1;
        }
        // Add sample data to the file
        fprintf(file, "AlQuds:Hezma:5000:550:yes\n");
        fprintf(file, "Nablus:Asira:8000:350:yes\n");
        fprintf(file, "AlQuds:Qatanna:3000:300:No\n");
        fprintf(file, "Jenin:Yabad:12000:200:yes\n");
        fclose(file);
        file = fopen("districts.txt", "r");
    }

    Town *root = NULL; // Initialize the AVL tree
    HashTable *hashTable = createHashTable(); // Initialize the hash table

    char district[50], town[50], hasMunicipality[4];
    int population, elevation;

    // Read data from the file and insert into the AVL tree and hash table
    while (fscanf(file, "%[^:]:%[^:]:%d:%d:%s\n", district, town, &population, &elevation, hasMunicipality) != EOF) {
        root = insert(root, district, town, population, elevation, hasMunicipality);
        insertIntoHashTable(hashTable, findTown(root, town));
    }

    fclose(file);

    int choice;
    do {
        printf("Main Menu\n");
        printf("1. Insert a new town\n");
        printf("2. Find and update a town\n");
        printf("3. List towns in alphabetical order\n");
        printf("4. List towns with population greater than a given number\n");
        printf("5. List towns based on municipality status\n");
        printf("6. Delete a town\n");
        printf("7. Save to file\n");
        printf("8. Print hash table\n");
        printf("9. Print hash table stats\n");
        printf("10. Print hashed table (including empty spots)\n");
        printf("11. Search for a town and print collisions\n");
        printf("12. Insert a new record into the hash table\n");
        printf("13. Delete a specific record from the hash table\n");
        printf("14. Save the data back to the file towns.txt\n");
        printf("15. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                printf("Enter district: ");
                scanf("%s", district);
                printf("Enter town: ");
                scanf("%s", town);
                printf("Enter population: ");
                scanf("%d", &population);
                printf("Enter elevation: ");
                scanf("%d", &elevation);
                printf("Has municipality (yes/no): ");
                scanf("%s", hasMunicipality);
                root = insert(root, district, town, population, elevation, hasMunicipality);
                insertIntoHashTable(hashTable, findTown(root, town));
                printf("Town added successfully!\n");
                break;
            }
            case 2: {
                printf("Enter town to find: ");
                scanf("%s", town);
                Town *foundTown = findTown(root, town);
                if (foundTown != NULL) {
                    printf("Town found: %s, District: %s, Population: %d, Elevation: %d, Municipality: %s\n",
                           foundTown->town, foundTown->district, foundTown->population, foundTown->elevation, foundTown->hasMunicipality);
                    printf("Enter new population: ");
                    scanf("%d", &foundTown->population);
                    printf("Enter new elevation: ");
                    scanf("%d", &foundTown->elevation);
                    printf("Has municipality (yes/no): ");
                    scanf("%s", foundTown->hasMunicipality);
                    printf("Town updated successfully!\n");
                } else {
                    printf("Town not found.\n");
                }
                break;
            }
            case 3: {
                printf("Towns in alphabetical order:\n");
                inOrder(root);
                break;
            }
            case 4: {
                int populationFilter;
                printf("Enter population filter: ");
                scanf("%d", &populationFilter);
                printf("Towns with population greater than %d:\n", populationFilter);
                listTownsByPopulation(root, populationFilter);
                break;
            }
            case 5: {
                char municipalityFilter[4];
                printf("Enter municipality filter (yes/no): ");
                scanf("%s", municipalityFilter);
                printf("Towns with municipality status '%s':\n", municipalityFilter);
                listTownsByMunicipality(root, municipalityFilter);
                break;
            }
            case 6: {
                printf("Enter town to delete: ");
                scanf("%s", town);
                root = deleteTown(root, town);
                deleteFromHashTable(hashTable, town);
                printf("Town deleted successfully!\n");
                break;
            }
            case 7: {
                FILE *outFile = fopen("towns.txt", "w");
                if (outFile == NULL) {
                    printf("Error opening file!\n");
                } else {
                    saveToFile(root, outFile);
                    fclose(outFile);
                    printf("Data saved to towns.txt successfully!\n");
                }
                break;
            }
            case 8: {
                printf("Hash table:\n");
                printHashedTable(hashTable);
                break;
            }
            case 9: {
                printf("Hash table stats:\n");
                printHashTableStats(hashTable);
                break;
            }
            case 10: {
                printf("Printing hashed table (including empty spots):\n");
                printHashedTable(hashTable);
                break;
            }
            case 11: {
                printf("Enter town to search: ");
                scanf("%s", town);
                searchTownInHashTable(hashTable, town);
                break;
            }
            case 12: {
                printf("Inserting a new record into the hash table...\n");
                insertNewRecordIntoHashTable(hashTable, root);
                break;
            }
            case 13: {
                printf("Enter town to delete: ");
                scanf("%s", town);
                root = deleteTown(root, town);
                deleteFromHashTable(hashTable, town);
                updateFileAfterDeletion(root, town);
                printf("Town deleted successfully!\n");
                break;
            }
            case 14: {
                FILE *outFile = fopen("towns.txt", "w");
                if (outFile == NULL) {
                    printf("Error opening file!\n");
                } else {
                    saveToFile(root, outFile);
                    fclose(outFile);
                    printf("Data saved to towns.txt successfully!\n");
                }
                break;
            }
            case 15: {
                printf("Exiting...\n");
                break;
            }
            default: {
                printf("Invalid choice. Please try again.\n");
                break;
            }
        }
    } while (choice != 15);

    return 0;
}
