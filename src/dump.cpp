#include "dump.h"
#include "listdef.h"
#undef listDump

#include <assert.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

static const size_t MAX_TIMESTAMPED_FILE_PATH_LENGTH = 128;
static const size_t MAX_IMAGE_FILE_PATH_LENGTH = 128;
static const size_t MAX_DOT_COMMAND_LENGTH = 512;

static const char* QUOTES[] = {
    "Canary Canary Do Thy Hear Me ?",
    "SegFault? Not on my watch, bucko",
    "Aaaaand what has went wrong now?",
    "None of these words are in the Bible",
    "Et tu, Brute?",
    "Appledog",
    "Is my list gonna make it? Prolly not",
    "OUR FOOD KEEPS BLOWING U-",
    "One must imagine listDump happy...",
    "Boy oh boy I sure hope nothing bad has happened to my list!",
    "I ATE IT ALL",
    "Ouch! That hurt!",
    "So what are you gonna say at my funeral now that you've killed me",
    "Mmm spaghetti connections",
    "Yarn all around me!",
    "Lists got too silly",
    "Crawling back to me, hm?",
    "Linked list? How about you link... uhhh",
    "Let me break it down for you",
    "So, this is actually quite simple!",
    "Me when I when when you when I when he at the",
    "You. Me. Gas station",
    "Everything's connected",
    "Redesign your graph. We know what we're doing.",
    "\"This task is quite simple\" My ass",
    "7 handshakes theory frfr",
    "I probably should not be writing this instead of the actual code",
    "She data on my structures till i linearize"
};

static char* getTimestampedString(const char* prefix, const char* suffix, unsigned int count = 0);
static int listTextDump(FILE* f, List* lst,
                        const char* commentary, const char* filename, int line,
                        unsigned int callCount);
static int listGraphDump(FILE* f, List* lst, unsigned int callCount);

#define WARNING_PREFIX(condition) condition ? "<b><body><font color=\"red\">[!]</font></body></b>" : ""

void listDump(FILE* f, List* lst, const char* commentary, const char* filename, int line) {
    assert(f);
    assert(filename);
    assert(commentary);

    static unsigned int callCount = 0;
    ++callCount;

    listVerify(lst);
    listLoopCheck(lst);

    if (listTextDump(f, lst, commentary, filename, line, callCount))
        return;

    listGraphDump(f, lst, callCount);
}

FILE* initLogFile() {
    time_t timeAbs = time(NULL);
    char* name = getTimestampedString(".log/", ".html");
    if (!name)
        return NULL;

    FILE* f = fopen(name, "w");
    if (!f) {
        free(name);
        return NULL;
    }
    srand((unsigned int)timeAbs);

    fprintf(f,
            "<pre><h1>%s</h1>\n"
            "<p><h3><i>[Q] %s\n</i></h3></p>",
            name + strlen(".log/"),
            QUOTES[(unsigned long)random()
                        % (sizeof(QUOTES) / sizeof(char *))]);
    free(name);
    return f;
}

static int listTextDump(FILE* f, List* lst,
                        const char* commentary, const char* filename, int line,
                        unsigned int callCount) {
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
        return -1;
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
            "\t%sstatus = %d\n"
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
            WARNING_PREFIX(lst->status), lst->status,
            WARNING_PREFIX(isDataNull),  lst->data,
            WARNING_PREFIX(isNextNull),  lst->next,
            WARNING_PREFIX(isPrevNull && lst->isDoubleLinked),  lst->prev);

    if (isDataNull)
        return -1;

    for (ListIndex i = 0; i < lst->capacity; i++) {
        fprintf(f,
                "\t| %-12lu | %-12d | %-12lu | %-12lu |\n"
                "\t|              |              |              |              |\n",
                i, lst->data[i],
                isNextNull ? 0 : lst->next[i],
                isPrevNull ? 0 : lst->prev[i]);
    }
    fprintf(f, "</b>}\n");

    return 0;
}

static int listGraphDump(FILE* f, List* lst, unsigned int callCount) {
    assert(f);
    assert(lst);

    if ((!lst->prev && lst->isDoubleLinked) || !lst->next) {
        fputs("Prev or Next are NULL in a list that requires them to be present! Im not gonna graph dump\n", f);
        return -1;
    }

    const char* BG_COLOR      = "#FFFFFF";
    const char* BAD_OUTLINE   = "#602222";
    const char* BAD_FILL      = "#F02222";
    const char* DEFAULT_CELL  = "#646CBD";
    const char* TABLE_OUTLINE = "#101510";
    const char* ADDRESS_FILL  = "#10151034";
    const char* NEXT_FILL     = "#10151034";
    const char* PREV_FILL     = "#10151034";
    const char* INDEX_FILL    = "#10151034";
    const char* VALUE_FILL    = "#10151034";
    const char* OK_EDGE       = "#2222E0";
    const char* BAD_EDGE      = "#E02222";
    const char* FREE_EDGE     = "#20B412";
    const char* FREE_OUTLINE  = "#26721F";
    const char* FREE_FILL     = "#64BD6C";
    const char* TAIL_OUTLINE  = "#666666";
    const char* TAIL_FILL     = "#DDDDDD";
    const char* HEAD_OUTLINE  = "#666666";
    const char* HEAD_FILL     = "#DDDDDD";

    char* dotPath = getTimestampedString(".log/dot-", ".txt", callCount);
    if (!dotPath) {
        fprintf(f, "<h1><b>Dot file name composition for this graph dump</h1><b>\n");
        return -1;
    }

    FILE* dot = fopen(dotPath, "w");
    if (!dot) {
        fprintf(f, "<h1><b>Dot file open failed for this graph dump</h1><b>\n");
        free(dotPath);
        return -1;
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
                 "head [shape=box, style=\"filled\", color=\"%s\", "
                       "fillcolor=\"%s\", label=\"HEAD\", fontsize=28]\n"
                 "tail [shape=box, style=\"filled\", color=\"%s\", "
                       "fillcolor=\"%s\", label=\"TAIL\", fontsize=28]\n",
                 BG_COLOR,
                 BAD_OUTLINE, BAD_FILL,
                 BG_COLOR,
                 FREE_OUTLINE, FREE_FILL,
                 HEAD_OUTLINE, HEAD_FILL,
                 TAIL_OUTLINE, TAIL_FILL);

    bool* isFree = (bool*)calloc(lst->capacity, sizeof(bool));
    if (!isFree) {
        fprintf(dot, "Failed to allocate isFree for this graph dump!\n");
        free(dotPath);
        return -1;
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
                        "<td colspan=\"6\" bgcolor=\"%s\"><b>value:</b> %d</td>"
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
                    VALUE_FILL,   lst->data[i],
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
                        "<td colspan=\"5\" bgcolor=\"%s\"><b>value:</b> %d</td>"
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
                    VALUE_FILL,   lst->data[i],
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
        free(dotPath);
        return -1;
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

    fprintf(dot, "free -> node%lu [color=\"%s\"]\n", lst->free, FREE_EDGE);
    fprintf(dot, "head -> node%lu [color=\"%s\"]\n", lst->next[0], OK_EDGE);
    fprintf(dot, "tail -> node%lu [color=\"%s\"]\n", lst->prev[0], OK_EDGE);

    fprintf(dot, "}\n");
    fclose(dot);

    fputs("Graphical Dump:\n", f);

    char cmd[MAX_DOT_COMMAND_LENGTH] = {0};
    char* imgPath = getTimestampedString(".log/graph-", ".svg", callCount);
    if (!imgPath) {
        fprintf(f, "<h1><b>Image file path composition failed for this graph dump!</h1><b>\n");
        free(dotPath);
        return -1;
    }
    snprintf(cmd, MAX_DOT_COMMAND_LENGTH, "dot -T svg \"%s\" -o \"%s\"", dotPath, imgPath);
    system(cmd);

    fprintf(f, "<img src=\"./%s\"></img>\n", imgPath + strlen(".log/"));
    free(dotPath);
    free(imgPath);

    return 0;
}

static char* getTimestampedString(const char* prefix, const char* suffix, unsigned int count) {
    time_t timeAbs = time(NULL);
    tm* localTime = localtime(&timeAbs);
    char* name    = (char*)calloc(MAX_TIMESTAMPED_FILE_PATH_LENGTH, sizeof(char));
    char* pattern = (char*)calloc(MAX_TIMESTAMPED_FILE_PATH_LENGTH, sizeof(char));
    if (!pattern || !name) {
        free(name);
        free(pattern);
        return NULL;
    }

    if (count) {
        snprintf(pattern, MAX_TIMESTAMPED_FILE_PATH_LENGTH,
                 "%s%%d-%%m-%%Y-%%H:%%M:%%S-%u%s",
                 prefix,
                 count,
                 suffix);
    } else {
        snprintf(pattern, MAX_TIMESTAMPED_FILE_PATH_LENGTH,
                 "%s%%d-%%m-%%Y-%%H:%%M:%%S%s",
                 prefix,
                 suffix);
    }

    if (!strftime(name, MAX_TIMESTAMPED_FILE_PATH_LENGTH, pattern, localTime)){
        free(name);
        free(pattern);
        return NULL;
    }
    free(pattern);
    return name;
}
