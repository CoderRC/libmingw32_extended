#ifndef __436F64657252436C69626D696E677733325F657874656E646564_H
#define __436F64657252436C69626D696E677733325F657874656E646564_H

#ifdef __cplusplus
extern "C"
{
#endif
#include <stddef.h>

/*
 * Thread-safe shared struct state for global operations
 *
 * This structure holds the global operations values.
 * Use pointers to determine different types of initialization status:
 * - NULL: Not initialized
 * - Non-NULL: pointer value is loaded and initialized
 *
 * Thread safety pattern:
 * 1. Check condition (e.g., pointer == NULL)
 * 2. If condition true, __436F64657252436C69626D696E677733325F657874656E646564_lock
 * 3. Re-check condition after __436F64657252436C69626D696E677733325F657874656E646564_lock
 * 4. Perform initialization
 * 5. __436F64657252436C69626D696E677733325F657874656E646564_unlock
 */
  struct passwd;
  struct __436F64657252436C69626D696E677733325F657874656E646564
  {
    /* DLL handle - NULL means not initialized */
    void *ws2_32_handle;
    void *netapi32_handle;
    /* Iteration state for getpwent() */
    size_t passwd_index;
    /* Persistent storage */
    struct passwd * current_passwd;
    /* sys/msg.h: shared queue table - NULL means not attached */
    struct __436F64657252436C69626D696E677733325F657874656E646564_msg_table
      *msg_table;
    void *msg_file_handle;
    void *msg_mapping_handle;
    void *msg_mutex_handle;
    void *msg_event_handle;
  };

/*
 * sys/msg.h shared queue table
 *
 * Message queues outlive the process that created them, like System V
 * queues: the table lives in a file in the user's temp directory that
 * every process maps. A named mutex guards it across processes and a
 * named event wakes callers blocked in msgsnd() or msgrcv(). The table
 * is reset when the machine has rebooted since it was written.
 *
 * Every field has a fixed width so 32-bit and 64-bit programs share it.
 */
#define __436F64657252436C69626D696E677733325F657874656E646564_MSG_QUEUES 32
#define __436F64657252436C69626D696E677733325F657874656E646564_MSG_BYTES 16384
#define __436F64657252436C69626D696E677733325F657874656E646564_MSG_COUNT 512
#define __436F64657252436C69626D696E677733325F657874656E646564_MSG_HEADER 8
#define __436F64657252436C69626D696E677733325F657874656E646564_MSG_DATA \
  (__436F64657252436C69626D696E677733325F657874656E646564_MSG_BYTES \
   + __436F64657252436C69626D696E677733325F657874656E646564_MSG_COUNT \
   * __436F64657252436C69626D696E677733325F657874656E646564_MSG_HEADER)

  struct __436F64657252436C69626D696E677733325F657874656E646564_msg_queue
  {
    int in_use;			/* 1 while the identifier exists */
    int key;			/* key_t passed to msgget() */
    unsigned int seq;		/* Bumped on removal so stale ids fail */
    unsigned int uid, gid, cuid, cgid;
    unsigned int mode;		/* Low 9 bits are the permissions */
    unsigned int qnum;		/* Messages on the queue */
    unsigned int qbytes;	/* Byte limit for message text */
    unsigned int cbytes;	/* Bytes of message text on the queue */
    unsigned int used;		/* Bytes of data[] in use */
    unsigned int lspid, lrpid;
    long long stime, rtime, ctime;
    /* Messages back to back: int mtype, unsigned int size, then text */
    unsigned char
      data[__436F64657252436C69626D696E677733325F657874656E646564_MSG_DATA];
  };

  struct __436F64657252436C69626D696E677733325F657874656E646564_msg_table
  {
    unsigned int magic;
    unsigned int version;
    unsigned int size;
    unsigned int generation;	/* Bumped on every change */
    long long boot_time;	/* Seconds since 1970, to detect a reboot */
    struct __436F64657252436C69626D696E677733325F657874656E646564_msg_queue
      queues[__436F64657252436C69626D696E677733325F657874656E646564_MSG_QUEUES];
  };

/**
 * Attach the shared queue table and take the cross-process lock.
 * Returns the table, or NULL with errno set.
 */
  struct __436F64657252436C69626D696E677733325F657874656E646564_msg_table
    *__436F64657252436C69626D696E677733325F657874656E646564_msg_lock (void);

/**
 * Release the cross-process lock. A non-zero changed wakes blocked callers.
 */
  void __436F64657252436C69626D696E677733325F657874656E646564_msg_unlock
    (int changed);

/**
 * Release the lock, wait until the table changes or a short timeout
 * passes, then take the lock again. Returns the table, or NULL.
 */
  struct __436F64657252436C69626D696E677733325F657874656E646564_msg_table
    *__436F64657252436C69626D696E677733325F657874656E646564_msg_wait (void);

/**
 * Look up a queue by identifier while locked. Returns NULL when msqid
 * does not name an existing queue.
 */
  struct __436F64657252436C69626D696E677733325F657874656E646564_msg_queue
    *__436F64657252436C69626D696E677733325F657874656E646564_msg_find
    (struct __436F64657252436C69626D696E677733325F657874656E646564_msg_table
     *table, int msqid);

/**
 * Non-zero when the caller holds the permission want (4 read, 2 write)
 */
  int __436F64657252436C69626D696E677733325F657874656E646564_msg_allowed
    (const struct
     __436F64657252436C69626D696E677733325F657874656E646564_msg_queue *queue,
     int want);

/**
 * Non-zero when the process runs elevated (appropriate privileges)
 */
  int __436F64657252436C69626D696E677733325F657874656E646564_msg_privileged
    (void);

/* Global shared state variable - initialized to 0 by default */
  extern struct __436F64657252436C69626D696E677733325F657874656E646564
    __436F64657252436C69626D696E677733325F657874656E646564;

/**
 * Initialize the global lock for thread safety
 * Must be called once before any global operations in multi-threaded scenarios
 */
  void
    __436F64657252436C69626D696E677733325F657874656E646564_init_lock (void);

/**
 * Delete the global lock
 * Should be called during cleanup
 */
  void
    __436F64657252436C69626D696E677733325F657874656E646564_delete_lock (void);

/**
 * Enter the lock for thread-safe access
 */
  void __436F64657252436C69626D696E677733325F657874656E646564_lock (void);

/**
 * Leave the lock
 */
  void __436F64657252436C69626D696E677733325F657874656E646564_unlock (void);

#ifdef __cplusplus
}
#endif

#endif				/* __436F64657252436C69626D696E677733325F657874656E646564_H */
