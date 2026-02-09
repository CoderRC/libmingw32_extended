#ifndef __436F64657252436C69626D696E677733325F657874656E646564_H
#define __436F64657252436C69626D696E677733325F657874656E646564_H

#ifdef __cplusplus
extern "C"
{
#endif

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
  struct __436F64657252436C69626D696E677733325F657874656E646564
  {
    /* DLL handle - NULL means not initialized */
    void *ws2_32_handle;
  };

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
