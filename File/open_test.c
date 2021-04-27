#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main(void) {
    int fd = open("test_open.txt", O_CREAT, 0644, "test");
    close(fd);
    return 0;
}

int expand_files(struct files_struct *files, int nr) {
    struct fdtable *fdt;
    fdt = files_fdtable(files);
    /*
    * N.B. For clone tasks sharing a files structure, this test
    * will limit the total number of files that can be opened.
    */
    if (nr >= rlimit(RLIMIT_NOFILE))
    return -EMFILE;
    /* Do we need to expand? */
    if (nr < fdt->max_fds)
    return 0;
    /* Can we expand? */
    if (nr >= sysctl_nr_open)
    return -EMFILE;
    /* All good, so we try */
    return expand_fdtable(files, nr);
}

struct file *get_empty_filp(void) {
    const struct cred *cred = current_cred();
    static long old_max;
    struct file * f;
    /*
    * Privileged users can go above max_files
    */
    /* 这里对打开文件的个数进行检查,非特权用户不能超过系统的限制
    */
    if (get_nr_files() >= files_stat.max_files && !capable(CAP_SYS_ADMIN)) {
    /*
    再次检查per cpu 的文件个数的总和,
    为什么要做两次检查呢。后文会详细介绍
    */
    if (percpu_counter_sum_positive(&nr_files) >= files_stat.max_files)
    goto over;
    }
    /* 未到达上限,申请一个新的
    file 结构
    */
    f = kmem_cache_zalloc(filp_cachep, GFP_KERNEL);
    if (f == NULL)
    goto fail;
    /* 增加
    file 结构计数
    */
    percpu_counter_inc(&nr_files);
    f->f_cred = get_cred(cred);
    if (security_file_alloc(f))
    goto fail_sec;
    INIT_LIST_HEAD(&f->f_u.fu_list);
    atomic_long_set(&f->f_count, 1);
    rwlock_init(&f->f_owner.lock);
    spin_lock_init(&f->f_lock);
    eventpoll_init_file(f);
    /* f->f_version: 0 */
    return f;
    over:
    /* 用完了
    file 配额,打印
    log 报错
    */
    /* Ran out of filps - report that */
    if (get_nr_files() > old_max) {
    pr_info("VFS: file-max limit %lu reached\n", get_max_files());
    old_max = get_nr_files();
    }
    goto fail;
    fail_sec:
    file_free(f);
    fail:
    return NULL;
}


SYSCALL_DEFINE(pread64)(unsigned int fd, char __user *buf,
size_t count, loff_t pos) {
    struct file *file;
    ssize_t ret = -EBADF;
    int fput_needed;
    if (pos < 0)
    return -EINVAL;
    file = fget_light(fd, &fput_needed);
    if (file) {
    ret = -ESPIPE;
    if (file->f_mode & FMODE_PREAD)
    ret = vfs_read(file, buf, count, &pos);
    fput_light(file, fput_needed);
    }
    return ret;
}

struct stat* buf;

struct stat {
    dev_t st_dev;           // ID of device containing file
    ino_t st_ino;           // inode number
    mode_t st_mode;         // protection
    nlink_t st_nlink;       // number of hard links
    uid_t st_uid;           // user ID of owner
    gid_t st_gid;           // group ID of owner
    dev_t st_rdev;          // device ID (if special file)
    off_t st_size;          // total size, in bytes
    blksize_t st_blksize;   // blocksize for file system I/O
    blkcnt_t st_blocks;     // number of 512B blocks allocated
    time_t st_atime;        // time of last access
    time_t st_mtime;        // time of last modification
    time_t st_ctime;        // time of last status change
};