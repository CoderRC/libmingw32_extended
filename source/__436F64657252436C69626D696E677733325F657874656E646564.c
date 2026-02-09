#include <__436F64657252436C69626D696E677733325F657874656E646564.h>
#include <windows.h>

/* Global shared state variable - initialized to 0 by default */
struct __436F64657252436C69626D696E677733325F657874656E646564
  __436F64657252436C69626D696E677733325F657874656E646564 = { 0 };

/* Static CRITICAL_SECTION for thread synchronization */
static CRITICAL_SECTION
  __436F64657252436C69626D696E677733325F657874656E646564_critical_section;
static int
  __436F64657252436C69626D696E677733325F657874656E646564_critical_section_initialized
  = 0;

/**
 * Initialize the global lock for thread safety
 * Uses Windows CRITICAL_SECTION for efficient thread synchronization
 */
void
__436F64657252436C69626D696E677733325F657874656E646564_init_lock (void)
{
  if (!__436F64657252436C69626D696E677733325F657874656E646564_critical_section_initialized)
    {
      InitializeCriticalSection
	(&__436F64657252436C69626D696E677733325F657874656E646564_critical_section);
      __436F64657252436C69626D696E677733325F657874656E646564_critical_section_initialized
	= 1;
    }
}

/**
 * Delete the global lock
 * Should be called during cleanup
 */
void
__436F64657252436C69626D696E677733325F657874656E646564_delete_lock (void)
{
  if (__436F64657252436C69626D696E677733325F657874656E646564_critical_section_initialized)
    {
      DeleteCriticalSection
	(&__436F64657252436C69626D696E677733325F657874656E646564_critical_section);
      __436F64657252436C69626D696E677733325F657874656E646564_critical_section_initialized
	= 0;
    }
}

/**
 * Enter the critical section for thread-safe access
 * Initializes the critical section lazily if not already initialized
 */
void
__436F64657252436C69626D696E677733325F657874656E646564_lock (void)
{
  if (!__436F64657252436C69626D696E677733325F657874656E646564_critical_section_initialized)
    {
      __436F64657252436C69626D696E677733325F657874656E646564_init_lock ();
    }
  EnterCriticalSection
    (&__436F64657252436C69626D696E677733325F657874656E646564_critical_section);
}

/**
 * Leave the critical section
 */
void
__436F64657252436C69626D696E677733325F657874656E646564_unlock (void)
{
  if (__436F64657252436C69626D696E677733325F657874656E646564_critical_section_initialized)
    {
      LeaveCriticalSection
	(&__436F64657252436C69626D696E677733325F657874656E646564_critical_section);
    }
}
