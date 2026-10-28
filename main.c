
#include <stdio.h>   // Input and output functions
#include <stdlib.h>  // malloc(), free(), atoi()
#include <string.h>  // String handling functions
#include <ctype.h>   // tolower()

#define SIZE 100     // Maximum size of text fields

// Structure to store lost and found item details
typedef struct Item {
    int id;
    char name[SIZE];
    char category[SIZE];
    char color[SIZE];
    char description[SIZE];
    char location[SIZE];
    char date[SIZE];
    int returned;           // 0 = Available, 1 = Returned
    struct Item *next;      // Link to the next item
} Item;

// Structure to store a potential match
typedef struct Match {
    Item *lost;             // Pointer to lost item
    Item *found;            // Pointer to found item
    int score;              // Matching percentage
    struct Match *next;
} Match;

// Structure to store a claim
typedef struct Claim {
    int foundID;            // ID of the claimed item
    char claimant[SIZE];    // Name of the person claiming it
    struct Claim *next;
} Claim;

// Head pointers for lost and found item linked lists
Item *lostHead = NULL;
Item *foundHead = NULL;

// Head pointer for matches arranged by score
Match *priorityHead = NULL;

// Front and rear pointers for the claims queue
Claim *queueFront = NULL;
Claim *queueRear = NULL;

// Starting ID for newly registered items
int nextID = 101;

// Remove the newline character from user input
void removeNewline(char *str) {
    str[strcspn(str, "\n")] = '\0';
}

// Read a string from the user
void readString(char *message, char *value) {
    printf("%s", message);

    if (fgets(value, SIZE, stdin) != NULL)
        removeNewline(value);
}

// Read a number entered by the user
int readNumber(char *message) {
    char input[SIZE];
    readString(message, input);
    return atoi(input);
}

// Compare two strings without considering letter case
int sameText(char *a, char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) !=
            tolower((unsigned char)*b))
            return 0;

        a++;
        b++;
    }

    return *a == '\0' && *b == '\0';
}

// Find an item using its ID in a linked list
Item *findItem(Item *head, int id) {
    Item *temp = head;

    while (temp != NULL) {
        if (temp->id == id)
            return temp;

        temp = temp->next;
    }

    return NULL;
}

// Insert an item at the end of a linked list
void insertItem(Item **head, Item *item) {
    item->next = NULL;

    // If the list is empty, make this the first item
    if (*head == NULL) {
        *head = item;
        return;
    }

    // Traverse to the last node
    Item *temp = *head;

    while (temp->next != NULL)
        temp = temp->next;

    temp->next = item;
}

// Display all details of an item
void displayItem(Item *item) {
    printf("\nItem ID      : %d", item->id);
    printf("\nName         : %s", item->name);
    printf("\nCategory     : %s", item->category);
    printf("\nColour       : %s", item->color);
    printf("\nDescription  : %s", item->description);
    printf("\nLocation     : %s", item->location);
    printf("\nDate         : %s", item->date);

    printf("\nStatus       : %s\n",
           item->returned ? "Returned" : "Available");
}

// Register a new lost or found item
void registerItem(int isLost) {
    // Allocate memory dynamically for a new item
    Item *item = malloc(sizeof(Item));

    if (item == NULL) {
        printf("\nMemory allocation failed.\n");
        return;
    }

    item->id = nextID++;
    item->returned = 0;

    printf("\n--- Register %s Item ---\n",
           isLost ? "Lost" : "Found");

    // Collect item details
    readString("Enter item name: ", item->name);
    readString("Enter category: ", item->category);
    readString("Enter colour: ", item->color);
    readString("Enter description: ", item->description);
    readString("Enter location: ", item->location);
    readString("Enter date (DD-MM-YYYY): ", item->date);

    // Store the item in the appropriate linked list
    if (isLost)
        insertItem(&lostHead, item);
    else
        insertItem(&foundHead, item);

    printf("\nItem registered successfully!");
    printf("\nGenerated Item ID: %d\n", item->id);
}

// Display every item in a given linked list
void displayItems(Item *head, char *title) {
    Item *temp = head;

    printf("\n========== %s ==========\n", title);

    if (temp == NULL) {
        printf("No records available.\n");
        return;
    }

    // Traverse the list and display each item
    while (temp != NULL) {
        displayItem(temp);
        temp = temp->next;
    }
}

// Calculate a matching score between lost and found items
int calculateScore(Item *lost, Item *found) {
    int score = 0;

    // Each matching field contributes to the total score
    if (sameText(lost->name, found->name))
        score += 30;

    if (sameText(lost->category, found->category))
        score += 15;

    if (sameText(lost->color, found->color))
        score += 15;

    if (sameText(lost->location, found->location))
        score += 25;

    if (sameText(lost->date, found->date))
        score += 15;

    return score;
}

// Insert a match in descending order of matching score
void insertPriority(Match *match) {
    // Insert at the beginning if the score is highest
    if (priorityHead == NULL ||
        match->score > priorityHead->score) {
        match->next = priorityHead;
        priorityHead = match;
        return;
    }

    Match *temp = priorityHead;

    // Find the correct position for the new match
    while (temp->next != NULL &&
           temp->next->score >= match->score) {
        temp = temp->next;
    }

    match->next = temp->next;
    temp->next = match;
}

// Free all match nodes and reset the priority list
void clearMatches(void) {
    Match *temp;

    while (priorityHead != NULL) {
        temp = priorityHead;
        priorityHead = priorityHead->next;
        free(temp);
    }
}

// Compare all available lost items with found items
void findMatches(void) {
    Item *lost = lostHead;
    Item *found;
    Match *match;
    int count = 0;

    // Remove old results before generating new matches
    clearMatches();

    // Visit each lost item
    while (lost != NULL) {
        if (!lost->returned) {
            found = foundHead;

            // Compare it with every found item
            while (found != NULL) {
                if (!found->returned) {
                    int score = calculateScore(lost, found);

                    // Save pairs having at least one matching field
                    if (score > 0) {
                        match = malloc(sizeof(Match));

                        if (match == NULL) {
                            printf("Memory allocation failed.\n");
                            clearMatches();
                            return;
                        }

                        match->lost = lost;
                        match->found = found;
                        match->score = score;
                        match->next = NULL;

                        // Keep results sorted by score
                        insertPriority(match);
                    }
                }

                found = found->next;
            }
        }

        lost = lost->next;
    }

    printf("\n========== MATCHING RESULTS ==========\n");

    match = priorityHead;

    if (match == NULL) {
        printf("No potential matches found.\n");
        return;
    }

    // Display matches from highest score to lowest
    while (match != NULL) {
        printf("\nMatching Score: %d%%\n", match->score);

        printf("\nLost Item:");
        printf("\nID: %d | Name: %s | Colour: %s",
               match->lost->id,
               match->lost->name,
               match->lost->color);

        printf("\nLocation: %s", match->lost->location);

        printf("\n\nFound Item:");
        printf("\nID: %d | Name: %s | Colour: %s",
               match->found->id,
               match->found->name,
               match->found->color);

        printf("\nLocation: %s\n", match->found->location);

        // Classify the match based on its score
        if (match->score >= 70)
            printf("Result: Strong potential match\n");
        else if (match->score >= 40)
            printf("Result: Possible match\n");
        else
            printf("Result: Weak match\n");

        printf("--------------------------------------\n");

        count++;
        match = match->next;
    }

    printf("\nTotal potential matches: %d\n", count);
}

// Search lost and found items by name or category
void searchItems(void) {
    char key[SIZE];
    Item *temp;
    int found = 0;
    int choice;

    printf("\n1. Search by name");
    printf("\n2. Search by category\n");

    choice = readNumber("Enter choice: ");

    if (choice != 1 && choice != 2) {
        printf("Invalid choice.\n");
        return;
    }

    readString("Enter search text: ", key);

    printf("\n========== SEARCH RESULTS ==========\n");

    // Search the lost items list
    temp = lostHead;

    while (temp != NULL) {
        int matched = choice == 1
            ? sameText(temp->name, key)
            : sameText(temp->category, key);

        if (matched) {
            printf("\nType: Lost Item\n");
            displayItem(temp);
            found++;
        }

        temp = temp->next;
    }

    // Search the found items list
    temp = foundHead;

    while (temp != NULL) {
        int matched = choice == 1
            ? sameText(temp->name, key)
            : sameText(temp->category, key);

        if (matched) {
            printf("\nType: Found Item\n");
            displayItem(temp);
            found++;
        }

        temp = temp->next;
    }

    if (found == 0)
        printf("No matching items found.\n");
}

// Add a claim to the rear of the queue
void enqueueClaim(int id, char *name) {
    Claim *claim = malloc(sizeof(Claim));

    if (claim == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    claim->foundID = id;
    strcpy(claim->claimant, name);
    claim->next = NULL;

    // Initialize both pointers for the first claim
    if (queueRear == NULL) {
        queueFront = queueRear = claim;
    } else {
        // Attach the new claim at the rear
        queueRear->next = claim;
        queueRear = claim;
    }
}

// Accept a claim for an available found item
void submitClaim(void) {
    int id;
    Item *item;
    char name[SIZE];

    id = readNumber("Enter found item ID: ");

    // Check whether the entered ID exists
    item = findItem(foundHead, id);

    if (item == NULL) {
        printf("Found item ID does not exist.\n");
        return;
    }

    // Do not accept claims for returned items
    if (item->returned) {
        printf("This item has already been returned.\n");
        return;
    }

    readString("Enter claimant name: ", name);

    // Add the claim to the queue
    enqueueClaim(id, name);

    printf("\nClaim added to the queue successfully.\n");
}

// Process claims in First-In, First-Out order
void processClaims(void) {
    Claim *claim;
    Item *item;

    if (queueFront == NULL) {
        printf("\nNo pending claims.\n");
        return;
    }

    // Process claims starting from the front
    while (queueFront != NULL) {
        claim = queueFront;
        queueFront = queueFront->next;

        // Reset rear when the queue becomes empty
        if (queueFront == NULL)
            queueRear = NULL;

        printf("\nProcessing Claim...");
        printf("\nClaimant: %s", claim->claimant);
        printf("\nFound Item ID: %d", claim->foundID);

        item = findItem(foundHead, claim->foundID);

        if (item == NULL) {
            printf("\nClaim failed: Item not found.\n");
        } else if (item->returned) {
            // Reject a later claim for an already returned item
            printf("\nClaim rejected: Item already returned.\n");
        } else {
            // Mark the item as returned after processing
            item->returned = 1;
            printf("\nClaim processed successfully.");
            printf("\nItem marked as returned.\n");
        }

        printf("--------------------------------------\n");

        // Free the processed claim node
        free(claim);
    }

    // Remove old matching results after claim processing
    clearMatches();
}

// Display all items that have been returned
void displayReturnedItems(void) {
    Item *temp = foundHead;
    int count = 0;

    printf("\n========== RETURNED ITEMS ==========\n");

    while (temp != NULL) {
        if (temp->returned) {
            displayItem(temp);
            count++;
        }

        temp = temp->next;
    }

    if (count == 0)
        printf("No items have been returned yet.\n");
}

// Display claims waiting in the queue
void displayPendingClaims(void) {
    Claim *temp = queueFront;

    printf("\n========== PENDING CLAIMS ==========\n");

    if (temp == NULL) {
        printf("No pending claims.\n");
        return;
    }

    // Traverse the queue without removing claims
    while (temp != NULL) {
        printf("\nClaimant: %s", temp->claimant);
        printf("\nFound Item ID: %d\n", temp->foundID);

        temp = temp->next;
    }
}

// Free all nodes in an item linked list
void freeItems(Item *head) {
    Item *temp;

    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// Free any remaining claims in the queue
void freeClaims(void) {
    Claim *temp;

    while (queueFront != NULL) {
        temp = queueFront;
        queueFront = queueFront->next;
        free(temp);
    }

    queueRear = NULL;
}

// Main function: display menu and handle user choices
int main(void) {
    int choice;

    // Repeat until the user chooses Exit
    do {
        printf("\n\n");
        printf("========================================\n");
        printf("     SMART LOST & FOUND SYSTEM\n");
        printf("========================================\n");
        printf("1. Register Lost Item\n");
        printf("2. Register Found Item\n");
        printf("3. Find Matching Items\n");
        printf("4. Search Items\n");
        printf("5. View Lost Items\n");
        printf("6. View Found Items\n");
        printf("7. Submit Claim\n");
        printf("8. Process Claims\n");
        printf("9. View Returned Items\n");
        printf("10. View Pending Claims\n");
        printf("0. Exit\n");
        printf("========================================\n");

        choice = readNumber("Enter your choice: ");

        // Call the selected operation
        switch (choice) {
            case 1:
                registerItem(1);
                break;

            case 2:
                registerItem(0);
                break;

            case 3:
                findMatches();
                break;

            case 4:
                searchItems();
                break;

            case 5:
                displayItems(lostHead, "LOST ITEMS");
                break;

            case 6:
                displayItems(foundHead, "FOUND ITEMS");
                break;

            case 7:
                submitClaim();
                break;

            case 8:
                processClaims();
                break;

            case 9:
                displayReturnedItems();
                break;

            case 10:
                displayPendingClaims();
                break;

            case 0:
                printf("\nThank you for using the system!\n");
                break;

            default:
                printf("\nInvalid choice. Try again.\n");
        }

    } while (choice != 0);

    // Release dynamically allocated memory before exiting
    freeItems(lostHead);
    freeItems(foundHead);
    clearMatches();
    freeClaims();

    return 0;
}
