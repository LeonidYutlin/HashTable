#include "dump.h"
#include "utils.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define IMG_PATH_BUF_SZ  128
#define DOT_PATH_BUF_SZ  128
#define HTML_PATH_BUF_SZ 128
#define DOT_CMD_BUF_SZ   512

static const char* BG_COLOR      = "#FFFFFF";
static const char* BAD_OUTLINE   = "#602222";
static const char* BAD_FILL      = "#F02222";
static const char* DEFAULT_CELL  = "#646CBD";
static const char* TABLE_OUTLINE = "#101510";
static const char* ADDRESS_FILL  = "#10151034";
static const char* NEXT_FILL     = "#10151034";
static const char* PREV_FILL     = "#10151034";
static const char* INDEX_FILL    = "#10151034";
static const char* VALUE_FILL    = "#10151034";
static const char* OK_EDGE       = "#2222E0";
static const char* BAD_EDGE      = "#E02222";
static const char* FREE_EDGE     = "#20B412";
static const char* FREE_OUTLINE  = "#26721F";
static const char* FREE_FILL     = "#64BD6C";
static const char* TAIL_OUTLINE  = "#666666";
static const char* TAIL_FILL     = "#DDDDDD";
static const char* HEAD_OUTLINE  = "#666666";
static const char* HEAD_FILL     = "#DDDDDD";

static int listTextDump(FILE* f, List* lst,
                        const char* commentary, 
                        const char* filename, int line,
                        uint callCount);
static int listGraphDump(FILE* f, List* lst, uint callCount);

#define WARNING_PREFIX(condition) condition ? "<b><body><font color=\"red\">[!]</font></body></b>" : ""

Error listDump_(FILE* f, List* lst, const char* commentary, 
                const char* filename, int line) {
    if (!f || !filename || !commentary)
      return BadArgs;

    static uint callCount = 0;
    ++callCount;

    listVerify(lst);
    listLoopCheck(lst);

    if (listTextDump(f, lst, commentary, 
                     filename, line, callCount))
        return OK;

    return listGraphDump(f, lst, callCount);
}

FILE* initLogFile() {
    char name[HTML_PATH_BUF_SZ] = {};
    if (snTimestampedFilename(name, HTML_PATH_BUF_SZ,
                              ".log/", ".html", 0))
      return NULL;

    FILE* f = fopen(name, "w");
    if (!f) {
        return NULL;
    }

    fprintf(f, "<pre><h1>%s</h1>\n", name);
    return f;
}

static int listTextDump(FILE* f, List* lst,
                        const char* commentary, const char* filename, int line,
                        uint callCount) {
    assert(f);
    assert(filename);
    assert(commentary);

    if (!lst) {
        fprintf(f,
            "%s\n"
            "List Dump #%u called from %s:%d\n"
            "Textual Dump:\n"
            "List [NULL] {}\n",
            commentary,
            callCount, filename, line);
        return 1;
    }

    bool isDataNull = !lst->data;
    bool isNextNull = !lst->next;
    bool isPrevNull = !lst->prev;

    fprintf(f,
            "%s\n"
            "List Dump #%u called from %s:%d\n"
            "Textual Dump:\n"
            "List [%p] {\n"
            "\tcapacity = %lu\n"
            "\tisDoubleLinked = %s\n"
            "\thead = %lu\n"
            "\ttail = %lu\n"
            "\tfree = %lu\n"
            "\tfreeTail = %lu\n"
            "\t%sstatus = %u\n"
            "\t%sdata = %p\n"
            "\t%snext = %p\n"
            "\t%sprev = %p\n"
            "\t<b>|     index    |     data     |     next     |     prev     |\n",
            commentary,
            callCount, filename, line,
            lst,
            lst->capacity,
            lst->isDoubleLinked ? "true" : "false",
            lst->next[0],
            lst->prev[0],
            lst->free,
            lst->freeTail,
            WARNING_PREFIX(lst->status), lst->status,
            WARNING_PREFIX(isDataNull),  lst->data,
            WARNING_PREFIX(isNextNull),  lst->next,
            WARNING_PREFIX(isPrevNull && lst->isDoubleLinked),  lst->prev);

    if (isDataNull)
        return 1;

    for (ListIndex i = 0; i < lst->capacity; i++) {
      fprintf(f,
              "\t| "LIST_INDEX_FMT" | "LIST_UNIT_FMT" | "LIST_INDEX_FMT" | "LIST_INDEX_FMT" |\n"
              "\t----------------------------------------------\n",
              i, LIST_UNIT_FMT_ARGS(lst->data + i),
              isNextNull ? 0 : lst->next[i], isPrevNull ? 0 : lst->prev[i]);
    }
    fprintf(f, "</b>}\n");

    return 0;
}

static int listGraphDump(FILE* f, List* lst, uint callCount) {
    assert(f);
    assert(lst);

    if ((!lst->prev && lst->isDoubleLinked) || !lst->next) {
        fputs("Prev or Next are NULL in a list that requires them to be present! "
              "I'm not gonna graph dump\n", f);
        return Fail;
    }

    char dotPath[DOT_PATH_BUF_SZ] = {};
    if (snTimestampedFilename(dotPath, DOT_PATH_BUF_SZ, ".log/dot-", ".txt", callCount)) {
        fprintf(f, "<h1><b>Dot file name composition for this graph dump</h1><b>\n");
        return Fail;
    }

    FILE* dot = fopen(dotPath, "w");
    if (!dot) {
        fprintf(f, "<h1><b>Dot file open failed for this graph dump</h1><b>\n");
        return Fail;
    }

    fprintf(dot, "digraph G {\n"
                 "rankdir=TB;\n"
                 "graph [bgcolor=\"%s\", pad=0.25, nodesep=0.55, "
                        "ranksep=0.9, splines=ortho, ordering=\"in\"];\n"
                 "node [shape=hexagon, style=\"filled\", color=\"%s\", penwidth=1.4, "
                       "fillcolor=\"%s\", fontname=\"monospace\", fontsize=30];\n"
                 "edge [color=\"%s\", penwidth=1.5, weight = 0, arrowsize=0.8, arrowhead=vee];\n"
                 "free [shape=box, style=\"filled\", color=\"%s\", "
                       "fillcolor=\"%s\", label=\"FREE\", fontsize=28]\n"
                 "freeTail [shape=box, style=\"filled\", color=\"%s\", "
                       "fillcolor=\"%s\", label=\"FREE TAIL\", fontsize=28]\n"
                 "head [shape=box, style=\"filled\", color=\"%s\", "
                       "fillcolor=\"%s\", label=\"HEAD\", fontsize=28]\n"
                 "tail [shape=box, style=\"filled\", color=\"%s\", "
                       "fillcolor=\"%s\", label=\"TAIL\", fontsize=28]\n",
                 BG_COLOR,
                 BAD_OUTLINE, BAD_FILL,
                 BG_COLOR,
                 FREE_OUTLINE, FREE_FILL,
                 FREE_OUTLINE, FREE_FILL,
                 HEAD_OUTLINE, HEAD_FILL,
                 TAIL_OUTLINE, TAIL_FILL);

    bool* isFree = (bool*)calloc(lst->capacity, sizeof(bool));
    if (!isFree) {
        fprintf(dot, "Failed to allocate isFree for this graph dump!\n");
        return Fail;
    }
    for (ListIndex i = lst->free; i < lst->capacity && i != 0; i = lst->next[i]) {
        if (!lst->isDoubleLinked || (!lst->prev[i] && i != lst->next[0])) {
            if (isFree[i])
                break;
            isFree[i] = 1;
        } else {
            break;
        }
    }

    for (ListIndex i = 0; i < lst->capacity; i++) {
        if (lst->isDoubleLinked)
            fprintf(dot,
                    "node%lu"
                    "[shape=box, style=\"rounded, filled\", color=\"%s\", fillcolor=\"%s\", penwidth=2.1, fontsize=14, label="
                    "<<table border=\"0\" cellborder=\"1\" cellspacing=\"0\" cellpadding=\"4\" color=\"%s\">"
                    "<tr>"
                        "<td bgcolor=\"%s\"><b>prev:</b> %lu</td>"
                        "<td bgcolor=\"%s\"><b>idx:</b> %lu</td>"
                        "<td bgcolor=\"%s\"><b>next:</b> %lu</td>"
                    "</tr>"
                    "<tr>"
                        "<td colspan=\"6\" bgcolor=\"%s\"><b>value:</b> "LIST_UNIT_FMT"</td>"
                    "</tr>"
                    "<tr>"
                        "<td colspan=\"6\" bgcolor=\"%s\"><b>addr:</b> %p</td>"
                    "</tr>"
                    "</table>"
                    ">];\n",
                    i,
                    TABLE_OUTLINE,
                    i == 0 ? BG_COLOR : isFree[i] ? FREE_FILL : DEFAULT_CELL,
                    TABLE_OUTLINE,
                    PREV_FILL,    lst->prev[i],
                    INDEX_FILL,   i,
                    NEXT_FILL,    lst->next[i],
                    VALUE_FILL,   LIST_UNIT_FMT_ARGS(lst->data + i),
                    ADDRESS_FILL, lst->data + i);
        else
            fprintf(dot,
                    "node%lu"
                    "[shape=box, style=\"rounded, filled\", color=\"%s\", fillcolor=\"%s\", penwidth=2.1, fontsize=14, label="
                    "<<table border=\"0\" cellborder=\"1\" cellspacing=\"0\" cellpadding=\"4\" color=\"%s\">"
                    "<tr>"
                        "<td bgcolor=\"%s\"><b>idx:</b> %lu</td>"
                        "<td bgcolor=\"%s\"><b>next:</b> %lu</td>"
                    "</tr>"
                    "<tr>"
                        "<td colspan=\"5\" bgcolor=\"%s\"><b>value:</b>"LIST_UNIT_FMT"</td>"
                    "</tr>"
                    "<tr>"
                        "<td colspan=\"5\" bgcolor=\"%s\"><b>addr:</b> %p</td>"
                    "</tr>"
                    "</table>"
                    ">];\n",
                    i,
                    TABLE_OUTLINE,
                    i == 0 ? BG_COLOR : isFree[i] ? FREE_FILL : DEFAULT_CELL,
                    TABLE_OUTLINE,
                    INDEX_FILL,   i,
                    NEXT_FILL,    lst->next[i],
                    VALUE_FILL,   LIST_UNIT_FMT_ARGS(lst->data + i),
                    ADDRESS_FILL, lst->data + i);
    }

    fprintf(dot, "{rank=same; ");
    for (ListIndex i = 0; i < lst->capacity; ++i)
        fprintf(dot, "node%lu; ", i);
    fprintf(dot, "}\n");

    // invisible arrows
    for (ListIndex i = 0; i + 1 < lst->capacity; i++)
        fprintf(dot, "node%lu -> node%lu [minlen=1, maxlen=1, weight=10, penwidth=0, "
                                         "arrowhead=none, constraint=true]\n",
                     i, i + 1);

    bool* isBroken = (bool*)calloc(lst->capacity, sizeof(bool));
    if (!isBroken) {
        fprintf(dot, "Failed to allocate isBroken for this graph dump!\n");
        free(isFree);
        return Fail;
    }

    // main
    for (ListIndex i = 1; i < lst->capacity; i++) {
        if (isFree[i]) {
            if (lst->next[i])
                fprintf(dot, "node%lu -> node%lu [color=\"%s\"]\n", i, lst->next[i],
                        lst->next[i] < lst->capacity && isFree[lst->next[i]] ? FREE_EDGE : BAD_EDGE);
            continue;
        }
        if (lst->isDoubleLinked && !((!lst->prev[i]) == (i == lst->next[0]))) {
            fprintf(dot, "node%lu -> node%lu [color=\"%s\"]\n", i, lst->prev[i], BAD_EDGE);
        }
        if (!lst->next[i] && i == lst->prev[0]) continue;
        if (!lst->isDoubleLinked) {
            fprintf(dot, "node%lu -> node%lu [color=\"%s\"]\n", i, lst->next[i],
                    lst->next[i] < lst->capacity && lst->next[i] && !isFree[lst->next[i]]
                    ? OK_EDGE : BAD_EDGE);
        } else if (lst->next[i] < lst->capacity && i == lst->prev[lst->next[i]]) {
            fprintf(dot, "node%lu -> node%lu [dir=both arrowtail=vee color=\"%s\"]\n", i, lst->next[i], OK_EDGE);
        } else {
            isBroken[i] = 1;
            fprintf(dot, "node%lu -> node%lu [color=\"%s\"]\n", i, lst->next[i], BAD_EDGE);
            if (lst->next[i] < lst->capacity && lst->prev[lst->next[i]])
                fprintf(dot, "node%lu -> node%lu [color=\"%s\"]\n", lst->next[i], lst->prev[lst->next[i]], BAD_EDGE);
        }
    }

    // broken ones
    if (lst->isDoubleLinked) {
        for (ListIndex i = 1; i < lst->capacity; i++) {
            if (lst->prev[i] < lst->capacity && isBroken[lst->prev[i]])
                fprintf(dot, "node%lu -> node%lu [color=\"%s\"]\n", i, lst->prev[i], BAD_EDGE);
        }
    }

    free(isFree);
    free(isBroken);
    isBroken = NULL;
    isFree = NULL;

    fprintf(dot, "free     -> node%lu [color=\"%s\"]\n", lst->free, FREE_EDGE);
    fprintf(dot, "freeTail -> node%lu [color=\"%s\"]\n", lst->freeTail, FREE_EDGE);
    fprintf(dot, "head     -> node%lu [color=\"%s\"]\n", lst->next[0], OK_EDGE);
    fprintf(dot, "tail     -> node%lu [color=\"%s\"]\n", lst->prev[0], OK_EDGE);

    fprintf(dot, "}\n");
    fclose(dot);

    fputs("Graphical Dump:\n", f);

    char cmd[DOT_CMD_BUF_SZ] = {0};
    char imgPath[IMG_PATH_BUF_SZ] = {0};
    if (snTimestampedFilename(imgPath, IMG_PATH_BUF_SZ, 
                              ".log/graph-", ".svg", callCount)) {
        fprintf(f, "<h1><b>Image file path composition failed for this graph dump!</h1><b>\n");
        return Fail;
    }
    snprintf(cmd, DOT_CMD_BUF_SZ, "dot -T svg \"%s\" -o \"%s\"", dotPath, imgPath);
    system(cmd);

    fprintf(f, "<img src=\"./%s\"></img>\n", imgPath + strlen(".log/"));

    return OK;
}
