/*
 spinlock.h for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include <linux/types.h>


#define local_irq_save(x)	    __asm__ __volatile__("pushfl ; popl %0 ; cli":"=g" (x): /* no input */ :"memory")
#define restore_flags(x) 	    __asm__ __volatile__("pushl %0 ; popfl": /* no output */ :"g" (x):"memory", "cc")
#define local_irq_restore(x)	restore_flags(x)

typedef struct spinlock {
    uint32  m_locked;
    uint32  m_flags;
} spinlock_t;

typedef struct locker {
    uint32      m_flags;
    spinlock_t *m_lock;
} locker_t;

extern void spinlock_init(spinlock_t *lock);
extern void spin_lock_irqsave(spinlock_t *lock, uint32 flags);
extern void spin_unlock_irqsave(spinlock_t *lock, uint32 flags);

#endif
