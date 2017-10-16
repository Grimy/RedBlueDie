#include <stdio.h>
#include <stdlib.h>

#define Q_SIZE 100
#define M_SIZE 100
#define ITERATIONS 100000

#define MAP_WIDTH 10
#define MAP_HEIGHT 16

typedef struct {
    int x, y;
    int armor;
    char map[MAP_WIDTH][MAP_HEIGHT];
    char moves[M_SIZE]; int movesSize;
} State;

typedef struct {
    int priority; // 2*distance + moves - 2*armor
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

#define END_X 4
#define END_Y 14

static Queue q = {
    .nodes = {{
        .priority = 0,
        .data = {
            .x = 4, .y = 1,
            .armor = 1,
            .map = {
                "  L     R       ",
                "     R      R   ",
                "   R          B ",
                "         L    E ",
                "      L   B  E E",
                "#      B      E ",
                "#     R      R  ",
                "#   B      L    ",
                "   L    R       ",
                "#       #### ###",
            },
        },
    }},
    .size = 1,
};

void push(Queue *q, int priority, State *data) {
    if (q->size == Q_SIZE) {
        --q->size;
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

char get(const State *s, int x, int y) {
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

int priority(const State *s) {
    int distance = abs(s->x - END_X) + abs(s->y - END_Y);
    return 2*distance + s->movesSize - 2*s->armor;
}

int play(Queue *q) {
    State currentState, movedState;

    for (int count = 0; count < ITERATIONS; count++) {
        pop(q, &currentState);
        if (currentState.map[currentState.x][currentState.y] == 'E') {
            printf("Solution: %s with %d armor\n", currentState.moves, currentState.armor);
            //continue;
            return 0;
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

int main(void)
{
    play(&q);
    printf("AI finished!\n");
    return 0;
}
