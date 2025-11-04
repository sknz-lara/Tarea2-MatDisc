#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "castle.h"
#include "draft.h"

typedef struct {
    int p1, x1, y1;
    int p2, x2, y2;
} Link;

static const Node* cnode(const Graph *g, int p, int x, int y) {
    if (!g || !g->nodes) return NULL;
    if (p < 0 || p >= g->P || x < 0 || x >= g->X || y < 0 || y >= g->Y) return NULL;
    return &g->nodes[p * (g->Y * g->X) + y * g->X + x];
}

static int has_neighbor(const Node *a, const Node *b) {
    if (!a || !b) return 0;
    for (int i = 0; i < a->count; ++i)
        if (a->neighbors[i] == b) return 1;
    return 0;
}

static int wall_between_right(const Graph *g, int p, int x, int y) {
    if (x + 1 >= g->X) return 1;
    const Node *A = cnode(g, p, x, y);
    const Node *B = cnode(g, p, x + 1, y);
    return !(has_neighbor(A, B) && has_neighbor(B, A));
}

static int wall_between_down(const Graph *g, int p, int x, int y) {
    if (y + 1 >= g->Y) return 1;
    const Node *A = cnode(g, p, x, y);
    const Node *B = cnode(g, p, x, y + 1);
    return !(has_neighbor(A, B) && has_neighbor(B, A));
}

static void print_hline(FILE *f, const Graph *g, int p, int y) {
    for (int x = 0; x < g->X; ++x) {
        fputc('+', f);
        int draw_wall;
        if (y < 0 || y >= g->Y - 1)
            draw_wall = 1;
        else
            draw_wall = wall_between_down(g, p, x, y);
        fputs(draw_wall ? "---" : "   ", f);
    }
    fputs("+\n", f);
}

static int same_cell(const Node *a, const Node *b) {
    return (a->x == b->x && a->y == b->y);
}

static int manhattan(int dx, int dy) {
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    return dx + dy;
}

static char portal_label_from_index(int i) {
    return 'A' + (i % 26);
}

static char stair_label_from_index(int i) {
    return 'a' + (i % 26);
}

int draft_render_txt(const Graph *g, const char *filename) {
    if (!g || !filename) return -1;
    FILE *f = fopen(filename, "w");
    if (!f) return -1;

    Link portals[256];
    Link stairs[256];
    int np = 0, ns = 0;

    for (int i = 0; i < g->n_nodes; ++i) {
        const Node *a = &g->nodes[i];
        for (int j = 0; j < a->count; ++j) {
            const Node *b = a->neighbors[j];
            if (!b) continue;
            if (a->id >= b->id) continue; /* evitar duplicar pares */

            int dp = b->p - a->p;
            int dx = b->x - a->x;
            int dy = b->y - a->y;

            int is_stair = (abs(dp) == 1) && same_cell(a, b);
            int is_same_floor = (dp == 0);
            int is_orthogonal_adj = is_same_floor && (manhattan(dx, dy) == 1);

            if (is_stair) {
                if (ns < (int)(sizeof(stairs)/sizeof(stairs[0])))
                    stairs[ns++] = (Link){a->p, a->x, a->y, b->p, b->x, b->y};
            } else {
                int is_portal = 0;
                if (!is_same_floor) {
                    if (!(abs(dp) == 1 && same_cell(a, b))) is_portal = 1;
                    else is_portal = 0;
                } else {
                    if (!is_orthogonal_adj) is_portal = 1;
                }
                if (is_portal) {
                    if (np < (int)(sizeof(portals)/sizeof(portals[0])))
                        portals[np++] = (Link){a->p, a->x, a->y, b->p, b->x, b->y};
                }
            }
        }
    }

    for (int p = 0; p < g->P; ++p) {
        fprintf(f, "=== PISO %d ===\n", p);
        print_hline(f, g, p, -1);

        for (int y = 0; y < g->Y; ++y) {
            fputc('|', f);
            for (int x = 0; x < g->X; ++x) {
                const Node *n = cnode(g, p, x, y);
                char cell[4] = "   ";

                if (n && n->enemy_cost > 0) {
                    char tmp[16];
                    snprintf(tmp, sizeof(tmp), "%d", n->enemy_cost);
                    int len = (int)strlen(tmp);
                    if (len >= 3) {
                        cell[0] = tmp[len - 3];
                        cell[1] = tmp[len - 2];
                        cell[2] = tmp[len - 1];
                    } else if (len == 2) {
                        cell[0] = ' ';
                        cell[1] = tmp[0];
                        cell[2] = tmp[1];
                    } else {
                        cell[0] = ' ';
                        cell[1] = tmp[0];
                        cell[2] = ' ';
                    }
                } else {
                    int marked = 0;
                    for (int i = 0; i < np; ++i) {
                        if ((portals[i].p1 == p && portals[i].x1 == x && portals[i].y1 == y) ||
                            (portals[i].p2 == p && portals[i].x2 == x && portals[i].y2 == y)) {
                            cell[0] = ' ';
                            cell[1] = portal_label_from_index(i);
                            cell[2] = ' ';
                            marked = 1;
                            break;
                        }
                    }
                    if (!marked) {
                        for (int i = 0; i < ns; ++i) {
                            if ((stairs[i].p1 == p && stairs[i].x1 == x && stairs[i].y1 == y) ||
                                (stairs[i].p2 == p && stairs[i].x2 == x && stairs[i].y2 == y)) {
                                cell[0] = ' ';
                                cell[1] = stair_label_from_index(i);
                                cell[2] = ' ';
                                marked = 1;
                                break;
                            }
                        }
                    }
                }

                fputs(cell, f);
                fputc(wall_between_right(g, p, x, y) ? '|' : ' ', f);
            }
            fputc('\n', f);
            print_hline(f, g, p, y);
        }
        fputc('\n', f);
    }

    fclose(f);
    return 0;
}
