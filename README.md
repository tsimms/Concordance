#Concordance

A C program that takes as input a sufficiently large text document (several are available online for testing; e.g. via Project Gutenberg), and produces as output (via stdout) an alphabetical listing of each unique word in the document (case insensitive and space separated, though be careful to consider hyphenated words), along with the lines from the input document that the word appears on. Each unique word (and the list of lines that it appears on) should be on a separate line in the output.

 
For example, taking the following text as input:

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

The program should be well structured, cleanly implemented and commented, and use a time (and space, but time is more important than space) efficient algorithm. Also submit along with your program a small analysis of the time/space complexity of the algorithm used. Feel free to use whatever resources you like (e.g. Google away!), but the code must be your own
