/*
 atomic.h for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#ifndef _ATOMIC_H_
#define _ATOMIC_H_

typedef struct atomic_s { 
    volatile int counter;
} atomic_t;


#define ATOMIC_INIT(i)	{ (i) }
#define atomic_read(v)		((v)->counter)
#define atomic_set(v,i)		(((v)->counter) = (i))

static __inline__ void atomic_add(int i, atomic_t *v)
{
    __asm__ __volatile__(
            "lock; addl %1,%0"
            :"=m" (v->counter)
            :"ir" (i), "m" (v->counter));
}

static __inline__ void atomic_sub(int i, atomic_t *v)
{
    __asm__ __volatile__(
            "lock; subl %1,%0"
            :"=m" (v->counter)
            :"ir" (i), "m" (v->counter));
}

static __inline__ int atomic_sub_and_test(int i, atomic_t *v)
{
    unsigned char c;

    __asm__ __volatile__(
            "lock; subl %2,%0; sete %1"
            :"=m" (v->counter), "=qm" (c)
            :"ir" (i), "m" (v->counter) : "memory");
    return c;
}

static __inline__ void atomic_inc(atomic_t *v)
{
    __asm__ __volatile__(
            "lock; incl %0"
            :"=m" (v->counter)
            :"m" (v->counter));
}

static __inline__ void atomic_dec(atomic_t *v)
{
    __asm__ __volatile__(
            "lock; decl %0"
            :"=m" (v->counter)
            :"m" (v->counter));
}

static __inline__ int atomic_dec_and_test(atomic_t *v)
{
    unsigned char c;

    __asm__ __volatile__(
            "lock; decl %0; sete %1"
            :"=m" (v->counter), "=qm" (c)
            :"m" (v->counter) : "memory");
    return c != 0;
}

static __inline__ int atomic_inc_and_test(atomic_t *v)
{
    unsigned char c;

    __asm__ __volatile__(
            "lock; incl %0; sete %1"
            :"=m" (v->counter), "=qm" (c)
            :"m" (v->counter) : "memory");
    return c != 0;
}

#endif
