#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/****************************************************************
 **  Constants
 ****************************************************************/

// how many lines can a word appear on?
#define MAX_LINE_OCCURRENCES 4096
// how long can a line of text read in be?
#define MAX_LINE_LENGTH 16384
// what characters do we support within word tokens?
// you must include space
// ' is kinda funny: used as you'll, but also as 'as-is' notation
// - is kinda funny: used as hyphenation, but also as --- separators
#define TOKEN_CHARACTERS " abcdefghijklmnopqrstuvwxyz-'"

/****************************************************************
 **  Data structure section
 ****************************************************************/
struct node {
  char* word;
  struct node *left, *right;
  unsigned short instances;
  unsigned short lineNumbers[ MAX_LINE_OCCURRENCES ];
  unsigned short lineNumbersPos;
};

/* Node comparisons */
int nodeCmp (char *word, struct node *n) {
  return (strcmp(word, n->word));
}

bool isAfter (char *word, struct node *n) {
  return (nodeCmp(word,n) > 0);
}

bool isBefore (char *word, struct node *n) {
  return (nodeCmp(word,n) < 0);
}

bool isSame (char *word, struct node *n) {
  return (nodeCmp(word,n) == 0);
}

/* Allocate mem and create new node, and init */
struct node* newNode(char* w) {
  struct node* n = (struct node*) malloc(sizeof(struct node));
  n->word = (char*) malloc (sizeof (char) * strlen(w));
  strcpy(n->word, w);
  n->instances = 0;
  n->lineNumbersPos = 0;
  return n;
}

/* Splay routine rotations */
struct node* zig(struct node *n, struct node *p) {
  p->left = n->right;
  n->right = p;
  return n;
}

struct node* zag(struct node *n, struct node *p) {
  p->right = n->left;
  n->left = p;
  return n;
}

/*
   Move node with word w to root, or an adjacent node to where it will
   get created if it doesn't yet exist.
*/
struct node* splay(struct node* r, char* w) {
  if (r == NULL || strcmp(r->word,w)==0) {
    return r;
  }
  if (isBefore(w, r)) {
    if (r->left == NULL)
      return r;
    if (isBefore(w, r->left)) {
    // zig-zig
      r->left->left = splay(r->left->left, w);
      r = zig(r->left, r);
    } else if (isAfter(w, r->left)) {
    // zag-zig
      r->left->right = splay(r->left->right, w);
      if (r->left->right != NULL)
        r->left = zag(r->left->right, r->left);
    }
    return (r->left == NULL) ? r : zig(r->left, r);
  } else if (isAfter(w, r)) {
    if (r->right == NULL)
      return r;
    if (isBefore(w, r->right)) {
    // zig-zag
      r->right->left = splay(r->right->left, w);
      if (r->right->left != NULL)
        r->right = zig(r->right->left, r->right);
    } else if (isAfter(w, r->right)) {
    // zag-zag
      r->right->right = splay(r->right->right, w);
      r = zag(r->right, r);
    }
    return (r->right == NULL) ? r : zag(r->right, r);
  }
}

/*
   Either we're updating an existing node or creating a new one.
   The way Splay tree inserting works is you insert a node "in order"
   by first balancing the tree. That's what the play() does -- transforms
   the tree so we can insert our new node at the root, while having the
   tree in perfect order.
*/
struct node* insert(struct node* r, char* w, int lineNumber) {
  int size;
  if (r == NULL) {
    struct node* n = newNode(w);
    r = n;
  // Get node just before where this one will go
  } else {
    r = splay(r, w);
    if (! isSame(w, r)) {
      struct node* n = newNode(w);
      if (isBefore(w, r)) {
        // stick existing tree to the right
        n->right = r;
        n->left = r->left;
        r->left = NULL;
      } else if (isAfter(w, r)) {
        // stick existing tree to the left
        n->left = r;
        n->right = r->right;
        r->right = NULL;
      }
      r = n;
    }
  }
  // Now set the line text stuff, which shows the occurrences of the word
  if ((!r->lineNumbersPos) || r->lineNumbers[r->lineNumbersPos-1]!=lineNumber)
    // skip if we're on the same line number as the previous entry
    r->lineNumbers[r->lineNumbersPos++] = lineNumber;
  r->instances++;       // this is a bonus, that I needed for testing, and
                        // thought it added a nice touch.
  return r;
}

/*
   Here's the routine for traversing the tree and printing out the
   results in order. If you really wanna have a look "under the covers",
   uncomment out the commented line, and you'll see some pretty nerdy
   output, showing two child nodes. The recursion order is important,
   as left child is always before, and right child is always behind,
   order-wise.
*/
void printData(struct node* r) {
  if (r != NULL) {
    printData(r->left);
/*
    printf ("%s (%i) %s <<<%s - %s>>>\n", r->word, r->instances,
      r->left == NULL ? "NULL" : r->left->word,
      r->right == NULL ? "NULL" : r->right->word);
*/
    printf ("%s (%i)", r->word, r->instances);
    for (int i=0; i < r->lineNumbersPos; i++)
      printf (" %i", r->lineNumbers[i]);
    printf ("\n");
    printData(r->right);
  }
}


/****************************************************************
 **  Controller section
 ****************************************************************/

// globals
struct node* root;

/*
  filter screens out invalid characters from the input stream, prior to
  parsing for word tokens
*/

void filter(char *line) {
  char* valid = TOKEN_CHARACTERS ;
  char* newLine = line;
  for (char ch; *line; line++) {
    ch = *line;
    ch = tolower(ch);   // we can keep things case-insensitive
    if (strchr(valid, ch) != NULL) {
      *newLine++ = ch;
    }
  }
  *newLine='\0';        // close the string, nice and neat
}

/*
  scanLine will take in a line of input, parse each word (delimited by space)
  and send line number and word into updateWord array
*/
void scanLine (int lineNumber, char* line) {
  char *word;
  word = strtok(line, " ");     // grab first token, space-delimited
  while (word != NULL) {
    if (strcmp(word, "") != 0 && strcmp(word, " ") != 0) {
                                // make sure a word has some characters
      root = insert (root, word, lineNumber);
    }
    word = strtok(NULL, " ");   // grab next token
  }
}

void main() {
  int lineCount = 1;
  FILE *fp;
  char str[ MAX_LINE_LENGTH ];  // Wasn't specified how large the lines were
                        // 16K seems to work during my tests.
  char* line = (char*) malloc (sizeof(char) * MAX_LINE_LENGTH);
  fp = fopen("document.txt" , "r");
  while (fgets(str, MAX_LINE_LENGTH , fp)) {
    str[strlen(str)-1] = '\0';  // trim EOL
    strcpy(line, str);
    filter(line);
    if (strcmp(line, "") != 0) {
      // produce some output to show the scrubbed lines getting fed in
      printf("parsing line %i: %s\n", lineCount, line);
      scanLine(lineCount, line);
    }
    lineCount++;
  }
  fclose(fp);
  free(line);
  printData(root);      // yippee, we're done! now for the fun!
}
