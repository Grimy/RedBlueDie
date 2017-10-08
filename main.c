#include <stdio.h>
#include <stdlib.h>

#define Q_SIZE 64000
#define M_SIZE 100
#define ITERATIONS 32000

#define MAP_WIDTH 10
#define MAP_HEIGHT 16

typedef struct {
    int x, y;
    int armor;
    char map[MAP_WIDTH][MAP_HEIGHT];
    char moves[M_SIZE];
    int movesSize;
} State;

typedef struct {
    int priority; // Distance - armor * 4
    State data;
} Node;

typedef struct {
    Node nodes[Q_SIZE];
    int size;
} Queue;

typedef struct {
    char color;
    int x, y;
} Flower;

typedef struct {
    int startX, startY;
    int endX, endY;
    //Life lifes[L_SIZE];
} Map;

static Map map;

int main(void)
{
    Queue q = {};
    play(&q);
    return 0;
}

void push(Queue *q, int priority, State *data) {
    if (q->size == Q_SIZE) {
        printf("Cannot push to the queue!\n");
        exit(0);
        return;
    }

    int i = q->size - 1;
    while (i >= 0 && priority < q->nodes[i].priority) {
        q->nodes[i + 1] = q->nodes[i];
        --i;
    }
    q->nodes[i + 1] = (Node) { .priority = priority, .data = *data };
    ++(q->size);
}

void pop(Queue *q, State *data) {
    if (q->size == 0) {
        printf("Cannot pop from the queue!\n");
        return;
    }

    *data = q->nodes[0].data;
    --(q->size);
    for (int i = 0; i < q->size; i++) {
        q->nodes[i] = q->nodes[i + 1];
    }
}

char get(State *s, int x, int y) {
    if (x < 0 || x > MAP_WIDTH - 1 || y < 0 || y > MAP_HEIGHT - 1)
        return ' ';
    return s->map[x][y];
}

int canMove(const State *s, char dir) {
    int newX = s->x, newY = s->y;
    switch (dir) {
    case 'f':
        newY += 1;
        break;
    case 'b':
        newY -= 1;
        break;
    case 'l':
        newX -= 1;
        break;
    case 'r':
        newX += 1;
        break;
    }

    if (newX < 0 || newX > MAP_WIDTH - 1 || newY < 0 || newY > MAP_HEIGHT - 1)
        return 0;

    switch (get(s, newX, newY)) {
    case '#':
    case 'R':
    case 'B':
        return 0;
    default:
        return 1;
    }
}

int move(State *s, char dir) {
    char lineDmg, diagDmg;
    switch (dir) {
    case 'f':
        ++(s->y);
        lineDmg = 'R';
        diagDmg = 'B';
        break;
    case 'b':
        --(s->y);
        lineDmg = 'R';
        diagDmg = 'B';
        break;
    case 'l':
        --(s->x);
        lineDmg = 'B';
        diagDmg = 'R';
        break;
    case 'r':
        ++(s->x);
        lineDmg = 'B';
        diagDmg = 'R';
        break;
    }

    for (int i = -2; i <= 2; i++) {
        if (i != 0) {
            if (get(s, s->x + i, s->y) == lineDmg)
                --(s->armor);
            if (get(s, s->x, s->y + i) == lineDmg)
                --(s->armor);
            if (get(s, s->x + i, s->y + i) == diagDmg)
                --(s->armor);
            if (get(s, s->x + i, s->y - i) == diagDmg)
                --(s->armor);
        }
    }

    if (s->armor > 0) {
        if (s->map[s->x][s->y] == 'L') {
            ++(s->armor);
            s->map[s->x][s->y] = ' ';
        }
    }

    s->moves[s->movesSize++] = dir;
}

int priority(State *s) {
    int distance = abs(s->x - map.endX) + abs (s->y - map.endY);
    return distance + s->movesSize - s->armor;
}

void initMap(State *s) {
    map.startX = 4;
    map.startY = 1;
    map.endX = 4;
    map.endY = 14;
    s->x = map.startX;
    s->y = map.startY,
    s->armor = 1;
    for (int i = 0; i < MAP_WIDTH; i++) {
        for (int j = 0; j < MAP_HEIGHT; j++) {
            s->map[i][j] = ' ';
        }
    }

    /* Ends */
    s->map[3][14] = 'E';
    s->map[5][14] = 'E';
    s->map[4][13] = 'E';
    s->map[4][15] = 'E';

    /* Walls */
    s->map[5][0] = '#';
    s->map[6][0] = '#';
    s->map[7][0] = '#';
    s->map[9][0] = '#';
    s->map[9][8] = '#';
    s->map[9][9] = '#';
    s->map[9][10] = '#';
    s->map[9][11] = '#';
    s->map[9][13] = '#';
    s->map[9][14] = '#';
    s->map[9][15] = '#';

    /* Blue flowers */
    s->map[7][4] = 'B';
    s->map[5][7] = 'B';
    s->map[4][10] = 'B';
    s->map[2][14] = 'B';

    /* Red flowers */
    s->map[2][3] = 'R';
    s->map[1][5] = 'R';
    s->map[6][6] = 'R';
    s->map[0][8] = 'R';
    s->map[8][8] = 'R';
    s->map[1][12] = 'R';
    s->map[6][13] = 'R';

    /* Lifes */
    s->map[0][2] = 'L';
    s->map[8][3] = 'L';
    s->map[4][6] = 'L';
    s->map[3][9] = 'L';
    s->map[7][11] = 'L';
}

int play(Queue *q) {
    State currentState = {};
    initMap(&currentState);

    State movedState;
    push(q, priority(&currentState), &currentState);

    for (int count = 0; count < ITERATIONS; count++) {
    //while (1) {
        pop(q, &currentState);
        if (currentState.map[currentState.x][currentState.y] == 'E') {
            printf("Solution: %s with %d armor\n", currentState.moves, currentState.armor);
            //continue;
            return;
        }

        if (canMove(&currentState, 'f')) {
            movedState = currentState;
            move(&movedState, 'f');
            if (movedState.armor > 0)
                push(q, priority(&movedState), &movedState);
        }

        if (canMove(&currentState, 'l')) {
            movedState = currentState;
            move(&movedState, 'l');
            if (movedState.armor > 0)
                push(q, priority(&movedState), &movedState);
        }

        if (canMove(&currentState, 'r')) {
            movedState = currentState;
            move(&movedState, 'r');
            if (movedState.armor > 0)
                push(q, priority(&movedState), &movedState);
        }

        if (canMove(&currentState, 'b')) {
            movedState = currentState;
            move(&movedState, 'b');
            if (movedState.armor > 0)
                push(q, priority(&movedState), &movedState);
        }
    }

    printf("Best: %s with %d armor\n", currentState.moves, currentState.armor);
}
