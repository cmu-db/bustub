# Project 1 Design Document

<!--
Replace every placeholder below with your own writing. About one page total
(250-900 words). This document is part of your Project 1 submission and is
included automatically when you run `make submit-p1`.
-->

**Name:**
**Andrew ID / Email:**

## Replacer

Describe the design of your `ArcReplacer`. What state do you keep, and why? How
do the two lists and their ghost lists interact, and what makes your adaptation
of the target size correct? Say what you tried that did not work, if anything.

## Buffer Pool Manager

Describe how your buffer pool is latched. What does each latch protect, in what
order are they acquired, and why can two threads not deadlock against each
other? Explain how a page is brought in and evicted, and where the page guards
fit in.

## Memory Footprint

State how much memory your buffer pool uses beyond the frames themselves, and
what it is spent on. A buffer pool configured with N frames is expected to hold
roughly N pages of data: if you keep per-page metadata that grows with the size
of the database rather than with the frame count, say so here and give the size
per page. The autograder checks peak memory against a budget derived from your
frame count, so this section should match what it measures.

## Leaderboard Optimizations

If you made changes aimed at the leaderboard, describe them and what they bought
you. If you did not, say so.
