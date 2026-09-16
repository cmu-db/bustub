# Project 2 Design Document

<!--
A draft outline for you Project 2 Design Document. About one page total
(250-900 words). This document is part of your Project 2 submission and is
included automatically when you run `make submit-p2`.

If you make any modifications to your design for the previous projects, please update the related
design documents as well. All design documents up to Project 2 will be included in your submission.
-->

**Name:**

**Andrew ID / Email:**

## Tombstones

Briefly describe how you store tombstones in the leaf and when a tombstone becomes a
real removal.

## B+Tree Operations (Insertion, Deletion, and Point Search)

Briefly describe when you split, coalesce or redistribute, and which sibling you pick.
Say what happens when the root splits or collapses.

## Index Iterator

Briefly describe how your iterator moves from one leaf to the next, and which latches it
holds while doing so.

## Concurrency Control

Briefly describe the order in which you acquire and release latches, and when it becomes
safe to release an ancestor.

## Leaderboard Optimizations

If you made changes aimed at the leaderboard, describe them and what they bought
you. If you did not, say so.

## AI Usage

Disclose how you used AI tools on this project. Be specific about which tools and
what you used them for: understanding the writeup, generating code you submitted,
debugging, explaining an error, or reviewing your design. If AI produced any code
you are submitting, say which parts. If you did not use AI at all, write "None".
