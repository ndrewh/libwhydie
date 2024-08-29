#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "procmaps.h"
#include <string.h>
#include <signal.h>

#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include <sys/ucontext.h>

static int get_backtrace (char *buf, int size);

static void handler(int signo) {
    char buf[0x1000];
    get_backtrace(buf, 0x1000);
    fprintf(stderr, "%s", buf);
    signal(signo, SIG_DFL);
}

__attribute__((constructor))
void init() {
    signal(SIGSEGV, handler);
    signal(SIGABRT, handler);
    signal(SIGBUS, handler);
}

static void find_module(procmaps_row_t **maps, void* pc, char **img_name, int *off) {
  for (int i=0;; i++) {
    procmaps_row_t *cur = maps[i];
    if (!cur) break;
    if (cur->addr_begin <= (uintptr_t)pc && cur->addr_end > (uintptr_t)pc) {
      *img_name = cur->pathname;
      *off = (int)((uintptr_t)pc - cur->addr_begin);
      break;
    }
  }
}

static int get_backtrace (char *buf, int size) {
  unw_cursor_t cursor;
  unw_context_t context;

  unw_getcontext(&context);
  unw_init_local(&cursor, &context);

  procmaps_row_t **maps = contruct_procmaps(getpid());

  char *bufcur = buf;
  uintptr_t ip, sp;
  do {
    char sym[256];
    unw_word_t offset;

    unw_get_reg(&cursor, UNW_REG_IP, &ip);
    unw_get_reg(&cursor, UNW_REG_SP, &sp);

    int off = 0;
    char *name = NULL;
    find_module(maps, (void*)ip, &name, &off);
    int res;
    if (name) {
        char *d = strdup(name);
        char *modname = strrchr(d, '/');
        if (modname) {
            modname++;
        } else {
            modname = name;
        }
        res = snprintf(bufcur, size, "[%s+0x%x]\t", modname, off);
        free(d);
    } else {
        res = snprintf(bufcur, size, "\t");
    }

    if (res > 0 && res <= size) {
        bufcur += res;
        size -= res;
    }

    if (unw_get_proc_name(&cursor, sym, sizeof(sym), &offset) == 0) {
        res = snprintf(bufcur, size,"(%s+0x%lx)\tip = %lx, sp = %lx\n", sym, offset, (uint64_t) ip, (uint64_t) sp);
    } else {
        res = snprintf(bufcur, size, "\t\tip = %lx, sp = %lx\n", (uint64_t) ip, (uint64_t) sp);
    }

    if (res > 0 && res <= size) {
        bufcur += res;
        size -= res;
    }
  } while (unw_step(&cursor) > 0);

  return 0;
}
