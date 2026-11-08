# Project 4 Design Document

<!--
A draft outline for your Project 4 Design Document. You should aim to provide an intuitive explanation of your thought process. It should be about 200-500 words.
-->

**Name:**

**Andrew ID / Email:**

## Watermark and Garbage Collection

How does your `Watermark` track the lowest read timestamp, and what are the time complexities of `AddTxn` and `RemoveTxn`? How does `GarbageCollection` use the watermark to decide which undo logs are no longer needed and which transactions can be removed?

## Version Chains

How do `CollectUndoLogs` and `ReconstructTuple` work together to produce the version of a tuple that a transaction should see? What changes when a transaction modifies a tuple it has already modified?

## Write-Write Conflicts and the Primary Key Index

How do your executors detect write-write conflicts, and if two transactions race to modify the same tuple, what guarantees that only one succeeds? What does your insert executor do when a key already exists in the index but its tuple has been deleted? How do you handle an update that changes the primary key?

## Abort and Serializability

How does `Abort` revert the changes a transaction made, and how does that affect the version chains? How do you verify serializability when a `SERIALIZABLE` transaction commits?

## Leaderboard Optimizations

If you made changes aimed at the leaderboard, describe them. What kind of workloads would your optimizations be good at handling? What kind of workloads would your optimizations be ineffective for? If you did not add any optimizations, say so.

## AI Usage

Disclose how you used AI tools on this project. Be specific about which tools and what you used them for: understanding the writeup, generating code you submitted, debugging, explaining an error, or reviewing your design. If AI produced any code you are submitting, say which parts. If you did not use AI at all, write "None".

## AI Reflection

What worked well when you used AI tools? What didn't work well? How did using AI tools affect your understanding of the code and the design process? Looking back on the semester, how did your use of AI tools change from Project 1 to Project 4?
