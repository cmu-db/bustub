# Project 3 Design Document

<!--
A draft outline for your Project 3 Design Document. You should aim to provide an intuitive explanation of your thought process. It should be about 200-500 words. >
-->

**Name:**

**Andrew ID / Email:**

## IntermediateResultPage

How did you design the format for IntermediateResultPage? How are tuples represented and stored? What factors did you consider while designing? (e.g. space complexity, time complexity).

## Pipeline breakers

Which executors were pipeline breakers? How does being a pipeline breaker change the implementation of `Init` and `Next`?

## External Mergesort

For the external mergesort, you needed to work with the buffer pool. Briefly describe how your implementation interacts with the buffer pool, and how it "garbage collects" unused buffer pool pages. 

This semester, we are only testing two-way external mergesort. If your implementation already supports general k-way mergesort, briefly explain how. If it doesn't, briefly describe how you would modify it to support k-way mergesort.

## Leaderboard Optimizations

If you made changes aimed at the leaderboard, describe them. What kind of queries would your optimizations be good at handling? What kind of queries would your optimizations be ineffective for? If you did not add any optimizations, say so.

## AI Usage

Disclose how you used AI tools on this project. Be specific about which tools and what you used them for: understanding the writeup, generating code you submitted, debugging, explaining an error, or reviewing your design. If AI produced any code you are submitting, say which parts. If you did not use AI at all, write "None".