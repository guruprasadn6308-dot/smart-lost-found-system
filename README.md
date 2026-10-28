# smart-lost-found-system
Smart Lost and Found System using C
Smart Lost & Found System

A menu-driven C application that manages lost and found items, identifies potential matches, and processes item claims using data structures.

Features

- Register lost items
- Register found items
- Automatic matching of lost and found items based on item details
- Matching score calculation using name, category, colour, location, and date
- Search items by name or category
- View all lost and found items
- Submit claims for found items
- Process pending claims in First-In, First-Out (FIFO) order
- View pending claims
- Track returned items

Data Structures Used

- Singly Linked Lists: Store lost items and found items.
- Priority-Ordered Linked List: Display potential matches in descending order of matching score.
- Queue: Process item claims in First-In, First-Out (FIFO) order.
- Structures and Pointers: Organize item, match, and claim records.
- Dynamic Memory Allocation: Allocate memory for records at runtime.

Requirements

- GCC compiler
- Windows, Linux, or another compatible C environment

How to Compile and Run

Compile using GCC:

gcc main.c -o lost_found

On Windows, run:

lost_found.exe

On Linux:

./lost_found

Matching System

The application calculates a matching score out of 100 based on the following criteria:

- Item name: 30 points
- Category: 15 points
- Colour: 15 points
- Location: 25 points
- Date: 15 points

Potential matches are classified as:

- Strong Match: 70% and above
- Possible Match: 40% to 69%
- Weak Match: Below 40%

Claim Processing

Users can submit claims using a found item's ID. Claims are placed in a queue and processed in FIFO order. If the item is available when its claim is processed, it is marked as returned.

Data Persistence

The current application stores item and claim records in linked lists in memory while the program is running. Records are not automatically saved to files, so they are lost when the application exits.

Disclaimer

This is an educational project developed to demonstrate data structures and algorithms in C. Matching scores indicate potential matches only and do not verify item ownership.

Technologies Used

- C programming language
- Data Structures and Algorithms
- Singly Linked Lists
- Priority Queue concept
- Queue
- Dynamic Memory Allocation
- GCC Compiler
