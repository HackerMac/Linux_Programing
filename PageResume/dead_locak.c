#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static const char* const caller[2] = {"mutex_thread", "signal handler"};
static pthread_t mutex_tid;
static pthread_t sleep_tid;
static volatile int signal_handler_exit = 0;

static void hold_mutex(int c) {
    printf("enter hold_mutex [caller %s]\n", caller[c]);
    pthread_mutex_lock(&mutex);
    while (!signal_handler_exit && c != 1) {
        sleep(5);
    }
    pthread_mutex_unlock(&mutex);
    printf("leave hold_mutex [caller %s]\n", caller[c]);
}

static void* mutex_thread(void* arg) {
    hold_mutex(0);
}

static void* sleep_thread(void* arg) {
    sleep(10);
}

static void signal_handler(int signum) {
    hold_mutex(1);
    signal_handler_exit = 1;
}

int main () {
    signal(SIGUSR1, signal_handler);
    pthread_create(&mutex_tid, NULL, mutex_thread, NULL);
    pthread_create(&sleep_tid, NULL, sleep_thread, NULL);
    pthread_kill(sleep_tid, SIGUSR1);
    pthread_join(mutex_tid, NULL);
    pthread_join(sleep_tid, NULL);
    return 0;
}


struct files_struct init_files = {
    .count = ATOMIC_INIT(1),
    .fdt = &init_files.fdtab,
    .fdtab = {
        .max_fds = NR_OPEN_DEFAULT,
        .fd = &init_files.fd_array[0],.close_on_exec = (fd_set *)&init_files.close_on_exec_init,
        .open_fds = (fd_set *)&init_files.open_fds_init,
    },
    .file_lock = __SPIN_LOCK_UNLOCKED(init_task.file_lock),
};

newf = kmem_cache_alloc(files_cachep, GFP_KERNEL);
if (!newf)
    goto out;
atomic_set(&newf->count, 1);
spin_lock_init(&newf->file_lock);
newf->next_fd = 0;
new_fdt = &newf->fdtab;
new_fdt->max_fds = NR_OPEN_DEFAULT;
new_fdt->close_on_exec = (fd_set *)&newf->close_on_exec_init;
new_fdt->open_fds = (fd_set *)&newf->open_fds_init;
new_fdt->fd = &newf->fd_array[0];
new_fdt->next = NULL;