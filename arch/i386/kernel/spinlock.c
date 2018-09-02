/*
 spinlock.c for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#include <asm/spinlock.h>
#include <asm/x86.h>
#include <linux/os.h>

void spinlock_init(spinlock_t *lock)
{
        lock->m_locked = 0;
}

void spinlock_lock(spinlock_t *lock)
{
        while (xchg(&lock->m_locked, 1) != 0)
                ;
        __sync_synchronize();
        wmb();
}

uint32 holding(spinlock_t *lock)
{
        return (lock->m_locked == 1);
}

void spinlock_unlock(spinlock_t *lock)
{
        if (!holding(lock)) {
                kprintf(RED, "Not holding the lock when try to unlock\n");
                return;
        }

        __sync_synchronize();
        wmb();

        __asm__ volatile("movl $0, %0" : "+m" (lock->m_locked));
}

/*
void spinlock_lock_irq()
{
        cli();
        lock();
}

void spinlock_unlock_irq()
{
        unlock();
        sti();
}
*/

void spin_lock_irqsave(spinlock_t *lock, uint32 flags)
{
        uint32 f;
        local_irq_save(f);
        spinlock_lock(lock);
        flags = f;
}

void spin_unlock_irqrestore(spinlock_t *lock, uint32 flags)
{
        spinlock_unlock(lock);
        local_irq_restore(flags);
}
