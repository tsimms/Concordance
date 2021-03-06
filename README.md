# Concordance

## The Challenge
A C program that takes as input a sufficiently large text document (several are available online for testing; e.g. via Project Gutenberg), and produces as output (via stdout) an alphabetical listing of each unique word in the document (case insensitive and space separated, though be careful to consider hyphenated words), along with the lines from the input document that the word appears on. Each unique word (and the list of lines that it appears on) should be on a separate line in the output.
 
For example, taking the following text as input:
```
This is
some kind OF text it
Is an example of text

The following would be the output:
an 3
example 3
is 1 3
it 2
kind 2
of 2 3
some 2
text 2 3
this 1
```

The program should be well structured, cleanly implemented and commented, and use a time (and space, but time is more important than space) efficient algorithm. Also submit along with your program a small analysis of the time/space complexity of the algorithm used. Feel free to use whatever resources you like (e.g. Google away!), but the code must be your own

## The Solution

Okay, so this was a good project, giving me a chance to recall some good ol' CompSci memories. I chose to implement the data structure using a Splay tree. It's actually a data structure I fell in love with years ago (it works really well as a queuing system, which I had written for a music processing engine), and I thought this was a great application to show off its capabilities.

A Splay tree stores data in order, and is continually transformed as searches are performed. One of the great things about that is that lookups of frequent keys are less expensive, because they remain closer to the top. In this way, we sorta "cheat" the performance with some common word ('a', 'and', 'of', 'the', etc.) optimization. Splay trees, on average, are documented to get O(log _n_) for both inserts and lookups (lookups are a part of inserts in fact). Traversing the tree is also fast, as is the case with other binary trees.

On the space side, I ran into some complications, which reminded me why I dislike coding in C (thanks for that)! Dynamic space allocation is a beast, and I don't have much control over the size of documents being read into the system. I got each node down to a little over 8KB of space (because ANDs and THEs are so frequently used and on most lines). Sure, I could have used a linked list or some other heap-style allocation, but my array implementation is at least a little fancier than string storage/appending.

Overall, I'm pretty happy with the results. I even implemented a little occurrence counter, that you can see how many total occurrences of the word show up. It was simply a carry-over from my testing, that I thought would be helpful.

## Running It

Make sure you have a `document.txt` file in the same folder. My compiling was simply:
```
gcc -o concordance main.c
```
...and then to run, simply:
```
./concordance
```
Compiled/Tested on `Ubuntu 16.04.1 LTS`.
Test `document.txt` in repo.

## Considerations

I like disclosing the areas where the system will have trouble. It's mainly due to some non-standard notations that I could have accommodated, admitedly, but I found to be rather loose exceptions, and didn't think it was important to spend the time implementing. Nonetheless, here's a list of some of those situations:

1. Periods. So, we strip periods as separating punctuation, but periods are rather important for notating website addresses.
2. Hyphens. &mdash; is used to separate phrases, but some people write them improperly (myself included) as double hyphens ("--"), or use hyphens as separators.
3. Numbers. We stripped out numbers, but numbers used as words are commonplace (1st, 2nd, 3rd, etc.).
4. Apostrphes. People use them in weird ways outside of normal conjunctions, including 'as-is' (I guess that's like double-finger air quotes?)

Of course all of this could be easily handled with a regexp parser -- it's just a real shame (oh look, I just did it myself! ...tho in my defense, at least I space around it!) that C doesn't have a great regexp engine with which I'm familiar! Anyhow, these were considerations that in a real-world implementation would need to be handled.
