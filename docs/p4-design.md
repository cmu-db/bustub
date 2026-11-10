# Project 4 Design Document

<!--
A draft outline for your Project 4 Design Document. You should aim to provide an intuitive explanation of your thought process. It should be about 200-500 words.
-->

**Name:**

**Andrew ID / Email:**

## Watermark

What data structure does your `Watermark` use to track the lowest read timestamp, and why did you choose it? What are the time complexities of `AddTxn` and `RemoveTxn`?

## Version Chains

When a transaction modifies a tuple it has already modified, how do you update its undo log so that concurrent readers still see the correct version?

## Concurrency Control

If two transactions race to modify the same tuple, what guarantees that only one succeeds? While a transaction is committing, what stops a newly started transaction from seeing only some of its writes?

## Primary Key Index

What does your insert executor do when a key already exists in the index but its tuple has been deleted?

## Garbage Collection

How do you decide when a transaction can be removed? This project only tests stop-the-world garbage collection. What would have to change for it to run while transactions are active?

## Serializability

What anomaly does your serializable verification catch that snapshot isolation allows, and what information do you use to detect it?

## Leaderboard Optimizations

If you made changes aimed at the leaderboard, describe them. What kind of workloads would your optimizations be good at handling? What kind of workloads would your optimizations be ineffective for? How did they affect space per committed transaction? If you did not add any optimizations, say so.

## AI Usage

Disclose how you used AI tools on this project. Be specific about which tools and what you used them for: understanding the writeup, generating code you submitted, debugging, explaining an error, or reviewing your design. If AI produced any code you are submitting, say which parts. If you did not use AI at all, write "None".

## AI Reflection

What worked well when you used AI tools? What didn't work well? How did using AI tools affect your understanding of the code and the design process? Looking back on the semester, how did your use of AI tools change from Project 1 to Project 4, and what would you do differently on your next systems project?
