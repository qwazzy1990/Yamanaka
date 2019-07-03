#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "Color.h"
#define CHECK 1000000000

// Structures
// A vertex
typedef struct vertex
{
  struct vertex *up;
  struct vertex *down;
  struct vertex *left;
  struct vertex *right;
  int line, routeNum; // line number and route number
  int vals[2];
} vertex;

// A bar
typedef struct bar
{
  struct vertex *left;
  struct vertex *right;
  struct vertex *activeBar;
  struct bar *next;
  struct bar *prev;
} bar;


static void swapVals(int* a, int * b)
{
    int c = *a;
    int d = *b;
    int temp;

    temp = c;
    c = d;
    d = temp;

    *a = c;
    *b = d;
}
//
// Global variables
//
int perm[20];
int n;
vertex *upper; // The upper endpoints of lines
vertex *lower; // The lower endpoints of lines
int *startLines;
vertex *activeBar = NULL; // Current active bar
double temp;
int depth = 0;

vertex *vs[1000];
int countOne = 0;
int countTwo = 0;

// Stack of bars
bar *head = NULL;
bar *tail = NULL;

double count = 0;

int withPrint = 0;

//
// Prototypes
//
inline void init();
void simplePrint();

inline void makeRoot();
void setLine(vertex **vs, int currNum, int *perm);
int *resizePerm(int *perm, int kValue, int size);

void findAllChildren(int);
void leftswap(vertex *, vertex *);
void rightswap(vertex *, vertex *);
int isLeftswappable(vertex *, vertex *);
int isRightswappable(vertex *, vertex *);

inline void insertBar(vertex *, vertex *, vertex *, vertex *, vertex *, vertex *);

inline void push(bar *);
inline bar *pop();

void print();

char *printVertex(void *v);
char *printBar(void *b);

/*
  Main routine
 */
int main(int argc, char *argv[])
{
  int i;
  clock_t time[2];

  //  if (*argv[1] == 'p') {
  if (argv[1][0] == '-')
  {
    if (argv[1][1] == 'p')
    {

      withPrint = 1;
      n = argc - 2;
      printf("withPrint = 1\n");
      printf("n=%d\n", n);
      for (i = 1; i <= n; i++)
      {
        perm[i] = atoi(argv[i + 1]);
      }
    }
  }
  else
  {

    withPrint = 0;
    n = argc - 1;

    printf("n = %d\n", n);

    for (i = 1; i <= n; i++)
    {
      perm[i] = atoi(argv[i]);
    }
  }

  printf("\nThe input permutation:\n");
  for (i = 1; i <= n; i++)
  {
    printf("%d ", perm[i]);
  }
  printf("\n");

  time[0] = clock();
  init();
  makeRoot();
  setLine(vs, n, perm);
  count++;
  findAllChildren(1);
  time[1] = clock();

  printf("Count = %.0f\n", count);
  printf("Time to enumerate: %.2f sec\n",
         (double)(time[1] - time[0]) / CLOCKS_PER_SEC);
}

/*
  Initialize above[], below[], startLines[].
*/
void init()
{
  int i;

  upper = (vertex *)malloc(sizeof(vertex[n + 1]));
  lower = (vertex *)malloc(sizeof(vertex[n + 1]));

  // An error ?
  for (i = n; i >= 1; i--)
  {
    upper[i].down = &lower[i];
    upper[i].up = upper[i].left = upper[i].right = NULL;
    upper[i].line = i;
    upper[i].routeNum = 0;
    lower[i].up = &upper[i];
    lower[i].down = lower[i].left = lower[i].right = NULL;
    lower[i].line = i;
    lower[i].routeNum = 0;
  }

  startLines = (int *)malloc(sizeof(int[n + 1]));
}

/*
  Simple print method
 */
void simplePrint()
{
  int i;
  vertex *curr;

  for (i = n; i >= 1; i--)
  {
    curr = &upper[i];
    printf("%d: ", i);
    while (curr != NULL)
    {
      printf("%d ", curr->line);
      curr = curr->down;
    }
    printf("\n");
  }
  printf("\n");
}

/*
 Construct root amida
*/
void makeRoot()
{
  int currNum; // current number
  int startLine = 1, restartLine;
  int i, diff;
  vertex *currVertex, *upperVertex, *lowerVertex;
  vertex *leftNewVertex, *rightNewVertex;

  for (currNum = n; currNum >= 1; currNum--)
  {

    // Calc of the zigzag-path for currNum.
    while (perm[startLine] != currNum)
      startLine++;
    //printf("startLine = %d, ",startLine);

    // Construct startLines[]
    startLines[currNum] = startLine;

    // Move to down
    currVertex = &upper[startLine];
    restartLine = startLine;
    while (currVertex->down != NULL)
    {
      if (currVertex->left != NULL)
      {
        currVertex = currVertex->left;
        restartLine--;
      }
      else
      {
        currVertex = currVertex->down;
      }
    }

    // Make zigzag-path for currNum.
    // We insert a new vertex between upperVertex and lowerVertex.
    upperVertex = currVertex->up;
    lowerVertex = currVertex;

    for (i = restartLine; i < currNum; i++)
    {
      leftNewVertex = (vertex *)malloc(sizeof(vertex));
      rightNewVertex = (vertex *)malloc(sizeof(vertex));

      leftNewVertex->line = i;
      leftNewVertex->routeNum = currNum;
      rightNewVertex->line = i + 1;
      rightNewVertex->routeNum = currNum;

      vs[countOne] = leftNewVertex;
      //printf("1: Route num is %d\n", vs[countOne]->routeNum);
      countOne++;

      vs[countOne] = rightNewVertex;
      //printf("2: Route num is %d\n", vs[countOne]->routeNum);

      countOne++;

      //setLine(vs, currNum);
      //setLine(vs, currNum);

      insertBar(upperVertex, lowerVertex, lower[i + 1].up,
                &lower[i + 1], leftNewVertex, rightNewVertex);

      // Update of upperVertex and lowerVertex.
      upperVertex = rightNewVertex;
      lowerVertex = &lower[i + 1];
    }

    startLine = 1;
    printf("\n");
  }

  printf("\n\n");
}

void setLine(vertex **vs, int currNum, int *perm)
{

  int count = 1;
  int numVs = countOne;
  int size = n;

  vertex *vss[1000][1000];

  int rowNum = 0;
  int colNum = 0;
  int numRows = 1;

  int colNumbers[10000];
  int countCols = 0;
  for (int i = 0; i < numVs; i++)
  {
    vertex *v = vs[i];

    if (i == 0)
    {
      vss[rowNum][colNum] = v;
      colNum++;
      continue;
    }
    vertex *prevVertex = vs[i - 1];
    if (prevVertex->routeNum == v->routeNum)
    {
      vss[rowNum][colNum] = v;
      colNum++;
    }
    else
    {
      colNumbers[countCols] = colNum;
      countCols++;
      rowNum++;
      colNum = 0;
      vss[rowNum][colNum] = v;
      numRows++;
      colNum++;
    }
  }
  colNumbers[countCols] = colNum;

  count = 0;

  int kValue = -1;
  for (int i = 0; i < numRows; i++)
  {
    int *tempPerm = calloc(1000, sizeof(int));
    int x = 0;
    for (int i = 1; i < n + 1; i++)
    {
      tempPerm[x] = perm[i];
      x++;
    }
    int size = n;
    x = 0;
    for (int j = 0; j < colNumbers[i]; j += 2)
    {
      vertex *v1 = vss[i][j];
      vertex *v2 = vss[i][j + 1];
      for (int k = 0; k < size; k++)
      {
        if (tempPerm[k] < v1->routeNum)
        {
          v1->vals[0] = v1->routeNum;
          v1->vals[1] = tempPerm[k];

          v2->vals[0] = v2->routeNum;
          v2->vals[1] = tempPerm[k];

          kValue = tempPerm[k];
          break;
        }
      }
      //free(tempPerm);
      tempPerm = resizePerm(tempPerm, kValue, size);
      size--;
    }
  }
}

int *resizePerm(int *perm, int kValue, int size)
{
  int *newPerm = calloc(1000, sizeof(int));
  int x = 0;
  for (int i = 0; i < size; i++)
  {
    if (perm[i] != kValue)
    {
      newPerm[x] = perm[i];
      x++;
    }
  }
  return newPerm;
}

void insertBar(vertex *upperleft, vertex *lowerleft, vertex *upperright,
               vertex *lowerright, vertex *leftend, vertex *rightend)
{
  upperleft->down = leftend;
  lowerleft->up = leftend;

  upperright->down = rightend;
  lowerright->up = rightend;

  leftend->up = upperleft;
  leftend->left = NULL;
  leftend->down = lowerleft;
  leftend->right = rightend;

  rightend->up = upperright;
  rightend->left = leftend;
  rightend->down = lowerright;
  rightend->right = NULL;
}

/*
  Find all children
 */
void findAllChildren(int cleanLevel)
{
  vertex *x, *y, *currVertex, *upperleft, *lowerleft;
  int i, state, route, currCleanLevel = cleanLevel;
  bar b, *b2;

  // Omit for efficiency @ 8th.Apr.2009.
  if (withPrint == 1)
  {
    //printf("%.0f-th amida, cleanLv = %d:\n",count,cleanLevel);
    printf("%.0f-th amida, depth = %d\n", count, depth);
    print(); // Count & Print
  }

  // Turn bar children
  for (i = n; i >= currCleanLevel - 1; i--)
  {
    // find turn bar for route of i.
    // state = 0: go to lowerleft
    // state = 1: stop state

    // If cleanLv = 1 then, we have error case: i = 0!.
    if (i == 0)
      continue;

    currVertex = &upper[startLines[i]];
    state = 0; // 0: Go to lower-left, 1: stop(find turn bar)

    while (state != 1)
    {
      currVertex = currVertex->down;
      if (currVertex->left != NULL)
      {
        currVertex = currVertex->left;
      }
      else
        state = 1;
    }
    if (currVertex->right == NULL)
    {
      continue;
    }
    // We have just found the turn bar of route i.

    while (currVertex->line != i)
    {

      lowerleft = currVertex->down; // Find lower-left vertex of current vertex

      if (lowerleft->right == NULL)
      {
        currVertex = currVertex->right;
        currVertex = currVertex->down;
        continue; // increment & continue (skip recursive call)
      }
      else if (isRightswappable(lowerleft, lowerleft->right))
      {

        if (i == cleanLevel - 1)
        {

          if (lowerleft->line + 2 < activeBar->line)
            ; //break;
          else
          {

            route = currVertex->routeNum;
            b.left = lowerleft;
            b.right = lowerleft->right;
            b.next = b.prev = NULL;
            b.activeBar = activeBar;
            
            //printf("Left: %d %d\nRight:%d %d\n:Active Bar %d %d\n", b.left->routeNum, b.left->vals[1], b.right->routeNum, b.right->vals[1], b.activeBar->routeNum, b.activeBar->vals[1]);

            push(&b);
            //swapVals(&(lowerleft->right->up->vals[1]), &(lowerleft->up->vals[1]));
            rightswap(lowerleft, lowerleft->right);

            activeBar = lowerleft; // Update of active bar
            count++;               // Count up

            depth++;

            // Recursive call
            findAllChildren(route + 1);

            depth--;

            b2 = pop();

            leftswap(b2->left, b2->right); // Return to the parent
            activeBar = b2->activeBar;     // Return to the parent
          }
        }
        else
        { // Case of i >= cleanLevel-1

          route = currVertex->routeNum;
          b.left = lowerleft;
          b.right = lowerleft->right;
          b.next = b.prev = NULL;
          b.activeBar = activeBar;

          //printf("Left: %d %d\nRight:%d %d\n:Active Bar %d %d\n", b.left->routeNum, b.left->vals[1], b.right->routeNum, b.right->vals[1], b.activeBar->routeNum, b.activeBar->vals[1]);

          push(&b);
          //char *s = printVertex(lowerleft);
          //printf("Lower Left\n%s\n\n", s);
          //free(s);
          //s = printVertex(lowerleft->right);
          //printf("Lower left->righ\n%s\n\n", s);
          //free(s);
          //count++;
          //printf("Right Swapping\n\n");

          //s = printVertex(lowerleft->up);
          //printf("Up of lower left is %s\n", s);
          //free(s);

          //s = printVertex(lowerleft->right->up);
          //printf("up of lowerleft->right is %s\n", s);
          //free(s);

          //swapVals(&(lowerleft->right->up->vals[1]), &(lowerleft->up->vals[1]));
          rightswap(lowerleft, lowerleft->right);

          activeBar = lowerleft; // Update of active bar
          //s = printVertex(activeBar);
          //printf("%s\n", s);
          //free(s);
          count++; // Count up

          depth++;

          // Recursive call
          findAllChildren(route + 1);

          depth--;

          b2 = pop();

          leftswap(b2->left, b2->right); // Return to the parent
          activeBar = b2->activeBar;     // Return to the parent
        }
      }

      // increment currVertex
      currVertex = currVertex->right;
      currVertex = currVertex->down;
    } // while

  } // for loop (biggest loop in this subroutine.)
}

/*
  rightswap and leftswap
 */
void rightswap(vertex *left, vertex *right)
{
  vertex *a, *b, *c, *d, *e, *f, *g, *h;

  a = left->up;  //left->up
  c = right->up; //right->up
  b = c->up;     //up from rights up - right->up->up
  d = c->right;  //right->up->right

  e = left->down;  // left->down
  f = right->down; //down from right
  g = b->up;       //right->up->up->up
  h = d->up;       //right->up->right->up

  // Remove left and right.
  e->up = a; //left->down->up->left->up
  a->down = e;
  f->up = c;
  c->down = f;

  // Add left and right to new places.
  b->up = left;
  left->down = b;
  left->up = g;
  g->down = left;

  d->up = right;
  right->down = d;
  right->up = h;
  h->down = right;


  //TEST ME: IF I REMOVE THESE 2 LINES DO I GENERATE MORE UNIQUE CHILDREN?
  left->line = left->line + 1;
  right->line = right->line + 1;
}

void leftswap(vertex *left, vertex *right)
{
  vertex *a, *b, *c, *d, *e, *f, *g, *h;

  e = left->up;
  f = right->up;

  b = left->down;
  a = b->left;

  c = b->down;
  d = right->down;

  g = a->down;
  h = c->down;

  // remove left and right
  e->down = b;
  b->up = e;
  f->down = d;
  d->up = f;

  // add left and right
  c->down = right;
  right->up = c;
  right->down = h;
  h->up = right;

  a->down = left;
  left->up = a;
  left->down = g;
  g->up = left;

  //TEST ME: IF I REMOVE THESE 2 LINES DO I GENERATE MORE UNIQUE CHILDREN?

  left->line = left->line - 1;
  right->line = right->line - 1;
}

/*
  isRightswappable and is Leftswappable
 */
int isRightswappable(vertex *left, vertex *right)
{
  vertex *leftup = left->up;
  vertex *rightup = right->up;

  if (leftup->up == NULL)
    return 0;
  else if (right->up == NULL)
    return 0;
  else if ((leftup->left == NULL) && (rightup->left == NULL) && (leftup->right == rightup->up))
    return 1;
  else
    return 0;
}

int isLeftswappable(vertex *left, vertex *right)
{
  vertex *leftdown = left->down;
  vertex *rightdown = right->down;

  if (leftdown->down == NULL)
    return 0;
  else if (rightdown->down == NULL)
    return 0;
  else if ((leftdown->right == NULL) && (rightdown->right == NULL) &&
           (leftdown->down == rightdown->left))
    return 1;
  else
    return 0;
}

/*
  Operations for Stack
 */
void push(bar *b)
{
  if ((head == NULL) && (tail == NULL))
  {
    head = tail = b;
    b->next = NULL;
    b->prev = NULL;
  }
  else if (head == tail)
  {
    head = b;
    b->next = tail;
    tail->prev = b;
  }
  else
  {
    head->prev = b;
    b->next = head;
    b->prev = NULL;
    head = b;
  }
}

bar *pop()
{
  bar *temp;

  if ((head == NULL) && (tail == NULL))
  {
    return NULL;
  }
  else if (head == tail)
  {
    temp = head;
    head = tail = NULL;
    return temp;
  }
  else
  {
    temp = head;
    head = head->next;
    head->prev = NULL;
    return temp;
  }
}

/*
  Print amidas
*/
void print()
{
  vertex **current;
  int i, j;
  int wasPrinted[n + 1]; // 0: not printed in the previous loop.
                         // 1: surely printed in the previous loop.
  int final = 0;

  current = (vertex **)malloc(sizeof(vertex * [n + 1]));

  // Initialization
  for (i = 1; i <= n; i++)
  {
    current[i] = &upper[i];
    wasPrinted[i] = 1;
  }

  while (final != n)
  {

    final = 0;

    // 1st: Go down phase.
    for (i = 1; i <= n; i++)
    {
      if (wasPrinted[i] == 1)
      {
        if (current[i]->down != NULL)
        {
          current[i] = current[i]->down;
        }
        else
        {
          final++;
        }
        wasPrinted[i] = 0;
      }
    }

    // 2nd: Print phase.
    for (i = 1; i <= n - 1; i++)
    {
      //printf("|");
      if (current[i]->right == current[i + 1])
      {
        printf(GREEN "[%d %d]" COLOR_RESET, current[i]->routeNum, current[i]->vals[1]);
        wasPrinted[i] = 1;
        wasPrinted[i + 1] = 1;
      }
      else
      {
        printf("[0 0]");
      }
    }
    printf("\n");

    // 3rd: Check final or not.
    for (i = 1; i <= n; i++)
    {
      if (current[i] == &lower[i])
      {
        final++;
      }
    }
  }

  printf("\n");
}

char *printVertex(void *v)
{
  char *s = calloc(100000, sizeof(char));
  strcpy(s, "VERTEX:\n");
  if (v == NULL)
  {
    strcat(s, "Vertex is NULL\n");
    //printf("Why ! %s\n", s);
    return s;
  }

  vertex *vv = (vertex *)v;

  char temp[1000];
  strcat(s, "Route Number:");
  sprintf(temp, "%d", vv->routeNum);
  strcat(s, temp);
  strcat(s, "\n");
  //strcpy(temp, "\0");
  sprintf(temp, "%d", vv->line);
  strcat(s, "Line Number:");
  strcat(s, temp);
  strcat(s, "\n");
  //strcpy(temp, "\0");

  sprintf(temp, "%d", vv->vals[0]);
  strcat(s, "Values:");
  strcat(s, temp);
  strcat(s, " ");
  //strcpy(temp, "\0");
  sprintf(temp, "%d", vv->vals[1]);
  strcat(s, temp);
  strcat(s, "\n");
  //strcpy(s, "\0");

  /*char *down = printVertex(vv->down);
  char *up = printVertex(vv->up);
  char *right = printVertex(vv->right);
  char *left = printVertex(vv->left);

  strcat(s, "Left Vertex\n");
  strcat(s, down);

  strcat(s, "Right Vertex\n");
  strcat(s, right);

  strcat(s, "Up Vertex\n");
  strcat(s, up);

  strcat(s, "Down Vertex\n");
  strcat(s, down);

  strcat(s, "\n");

  free(up);
  free(down);
  free(left);
  free(right);*/
  //printf("Why ! %s\n", s);

  return s;
}

char *printBar(void *b)
{
  return NULL;
}