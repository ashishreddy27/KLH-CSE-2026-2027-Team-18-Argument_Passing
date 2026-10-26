// Multi-threaded Log Analyzer (pthreads) - OSSP project
// Build: gcc -O2 -pthread -o log_analyzer log_analyzer.c
// Usage: ./log_analyzer <logfile> [threads=4] [chunk_lines=20000]
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <time.h>

typedef struct { long lines, crit, err, warn, info, susp, pat[6]; } Stats;
typedef struct { char *start, *end; } Chunk;

static const char *PAT_NAME[6] = {"Failed login", "Invalid user", "SQL injection",
                                  "Path traversal", "Port scan", "Malware"};
static const char *PAT_KEY[6]  = {"failed password", "invalid user", "union select",
                                  "../", "port scan", "malware"};

static Chunk *chunks; static long nchunks, next_chunk = 0;
static Stats global;                                   // shared result
static pthread_mutex_t queue_lock  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t result_lock = PTHREAD_MUTEX_INITIALIZER;

static void classify(const char *ln, Stats *s) {
    s->lines++;
    if (strstr(ln, "[CRITICAL]")) s->crit++;
    else if (strstr(ln, "[ERROR]")) s->err++;
    else if (strstr(ln, "[WARN]")) s->warn++;
    else s->info++;
    int hit = 0;
    for (int i = 0; i < 6; i++)
        if (strcasestr(ln, PAT_KEY[i])) { s->pat[i]++; hit = 1; }
    if (hit) s->susp++;
}

static void *worker(void *arg) {
    long id = (long)arg, done = 0;
    Stats local = {0};                                 // private: no data races
    for (;;) {
        pthread_mutex_lock(&queue_lock);               // critical section 1: task queue
        long i = (next_chunk < nchunks) ? next_chunk++ : -1;
        pthread_mutex_unlock(&queue_lock);
        if (i < 0) break;
        char *p = chunks[i].start, *end = chunks[i].end;
        while (p < end) { classify(p, &local); p += strlen(p) + 1; }
        done++;
    }
    pthread_mutex_lock(&result_lock);                  // critical section 2: merge
    global.lines += local.lines; global.crit += local.crit; global.err += local.err;
    global.warn += local.warn; global.info += local.info; global.susp += local.susp;
    for (int i = 0; i < 6; i++) global.pat[i] += local.pat[i];
    pthread_mutex_unlock(&result_lock);
    printf("  thread %ld processed %ld chunks (%ld lines)\n", id + 1, done, local.lines);
    return NULL;
}

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: %s <logfile> [threads] [chunk_lines]\n", argv[0]); return 1; }
    int T = argc > 2 ? atoi(argv[2]) : 4; long CL = argc > 3 ? atol(argv[3]) : 20000;
    if (T < 1) T = 1; if (CL < 1) CL = 20000;
    FILE *f = fopen(argv[1], "rb"); if (!f) { perror("open"); return 1; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); rewind(f);
    char *buf = malloc(sz + 1); if (fread(buf, 1, sz, f) != (size_t)sz) { perror("read"); return 1; }
    buf[sz] = '\0'; fclose(f);

    // split into chunks of CL lines; turn '\n' into '\0' so each line is a C string
    long cap = sz / CL + 2; chunks = malloc(cap * sizeof(Chunk));
    char *p = buf, *cs = buf; long ln = 0;
    for (; p < buf + sz; p++) if (*p == '\n') {
        *p = '\0';
        if (++ln % CL == 0) { chunks[nchunks++] = (Chunk){cs, p + 1}; cs = p + 1; }
    }
    if (cs < buf + sz) chunks[nchunks++] = (Chunk){cs, buf + sz + 1};

    struct timespec a, b; clock_gettime(CLOCK_MONOTONIC, &a);
    pthread_t *th = malloc(T * sizeof(pthread_t));
    printf("Analyzing %s with %d thread(s), %ld chunks\n", argv[1], T, nchunks);
    for (long i = 0; i < T; i++) pthread_create(&th[i], NULL, worker, (void *)i);
    for (int i = 0; i < T; i++) pthread_join(th[i], NULL);
    clock_gettime(CLOCK_MONOTONIC, &b);
    double s = (b.tv_sec - a.tv_sec) + (b.tv_nsec - a.tv_nsec) / 1e9;

    printf("\n=== REPORT ===\nLines: %ld\nCRITICAL: %ld  ERROR: %ld  WARN: %ld  INFO: %ld\nSuspicious lines: %ld\n",
           global.lines, global.crit, global.err, global.warn, global.info, global.susp);
    for (int i = 0; i < 6; i++) printf("  %-15s %ld\n", PAT_NAME[i], global.pat[i]);
    printf("Time: %.3fs\n", s);
    return 0;
}
