/* CloneBuster
	  
File:     Enclave.cpp
Authors:  Samira Briongos (samira.briongos@neclab.eu)
            

NEC Laboratories Europe GmbH, Copyright (c) 2023, All rights reserved.  

       THIS HEADER MAY NOT BE EXTRACTED OR MODIFIED IN ANY WAY.
 
       PROPRIETARY INFORMATION ---  

SOFTWARE LICENSE AGREEMENT

ACADEMIC OR NON-PROFIT ORGANIZATION NONCOMMERCIAL RESEARCH USE ONLY

BY USING OR DOWNLOADING THE SOFTWARE, YOU ARE AGREEING TO THE TERMS OF THIS
LICENSE AGREEMENT.  IF YOU DO NOT AGREE WITH THESE TERMS, YOU MAY NOT USE OR
DOWNLOAD THE SOFTWARE.

This is a license agreement ("Agreement") between your academic institution
or non-profit organization or self (called "Licensee" or "You" in this
Agreement) and NEC Laboratories Europe GmbH (called "Licensor" in this
Agreement).  All rights not specifically granted to you in this Agreement
are reserved for Licensor. 

RESERVATION OF OWNERSHIP AND GRANT OF LICENSE: Licensor retains exclusive
ownership of any copy of the Software (as defined below) licensed under this
Agreement and hereby grants to Licensee a personal, non-exclusive,
non-transferable license to use the Software for noncommercial research
purposes, without the right to sublicense, pursuant to the terms and
conditions of this Agreement. NO EXPRESS OR IMPLIED LICENSES TO ANY OF
LICENSOR'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE. As used in this
Agreement, the term "Software" means (i) the actual copy of all or any
portion of code for program routines made accessible to Licensee by Licensor
pursuant to this Agreement, inclusive of backups, updates, and/or merged
copies permitted hereunder or subsequently supplied by Licensor,  including
all or any file structures, programming instructions, user interfaces and
screen formats and sequences as well as any and all documentation and
instructions related to it, and (ii) all or any derivatives and/or
modifications created or made by You to any of the items specified in (i).

CONFIDENTIALITY/PUBLICATIONS: Licensee acknowledges that the Software is
proprietary to Licensor, and as such, Licensee agrees to receive all such
materials and to use the Software only in accordance with the terms of this
Agreement.  Licensee agrees to use reasonable effort to protect the Software
from unauthorized use, reproduction, distribution, or publication. All
publication materials mentioning features or use of this software must
explicitly include an acknowledgement the software was developed by NEC
Laboratories Europe GmbH.

COPYRIGHT: The Software is owned by Licensor.  

PERMITTED USES:  The Software may be used for your own noncommercial
internal research purposes. You understand and agree that Licensor is not
obligated to implement any suggestions and/or feedback you might provide
regarding the Software, but to the extent Licensor does so, you are not
entitled to any compensation related thereto.

DERIVATIVES: You may create derivatives of or make modifications to the
Software, however, You agree that all and any such derivatives and
modifications will be owned by Licensor and become a part of the Software
licensed to You under this Agreement.  You may only use such derivatives and
modifications for your own noncommercial internal research purposes, and you
may not otherwise use, distribute or copy such derivatives and modifications
in violation of this Agreement.

BACKUPS:  If Licensee is an organization, it may make that number of copies
of the Software necessary for internal noncommercial use at a single site
within its organization provided that all information appearing in or on the
original labels, including the copyright and trademark notices are copied
onto the labels of the copies.

USES NOT PERMITTED:  You may not distribute, copy or use the Software except
as explicitly permitted herein. Licensee has not been granted any trademark
license as part of this Agreement.  Neither the name of NEC Laboratories
Europe GmbH nor the names of its contributors may be used to endorse or
promote products derived from this Software without specific prior written
permission.

You may not sell, rent, lease, sublicense, lend, time-share or transfer, in
whole or in part, or provide third parties access to prior or present
versions (or any parts thereof) of the Software.

ASSIGNMENT: You may not assign this Agreement or your rights hereunder
without the prior written consent of Licensor. Any attempted assignment
without such consent shall be null and void.

TERM: The term of the license granted by this Agreement is from Licensee's
acceptance of this Agreement by downloading the Software or by using the
Software until terminated as provided below.  

The Agreement automatically terminates without notice if you fail to comply
with any provision of this Agreement.  Licensee may terminate this Agreement
by ceasing using the Software.  Upon any termination of this Agreement,
Licensee will delete any and all copies of the Software. You agree that all
provisions which operate to protect the proprietary rights of Licensor shall
remain in force should breach occur and that the obligation of
confidentiality described in this Agreement is binding in perpetuity and, as
such, survives the term of the Agreement.

FEE: Provided Licensee abides completely by the terms and conditions of this
Agreement, there is no fee due to Licensor for Licensee's use of the
Software in accordance with this Agreement.

DISCLAIMER OF WARRANTIES:  THE SOFTWARE IS PROVIDED "AS-IS" WITHOUT WARRANTY
OF ANY KIND INCLUDING ANY WARRANTIES OF PERFORMANCE OR MERCHANTABILITY OR
FITNESS FOR A PARTICULAR USE OR PURPOSE OR OF NON- INFRINGEMENT.  LICENSEE
BEARS ALL RISK RELATING TO QUALITY AND PERFORMANCE OF THE SOFTWARE AND
RELATED MATERIALS.

SUPPORT AND MAINTENANCE: No Software support or training by the Licensor is
provided as part of this Agreement.  

EXCLUSIVE REMEDY AND LIMITATION OF LIABILITY: To the maximum extent
permitted under applicable law, Licensor shall not be liable for direct,
indirect, special, incidental, or consequential damages or lost profits
related to Licensee's use of and/or inability to use the Software, even if
Licensor is advised of the possibility of such damage.

EXPORT REGULATION: Licensee agrees to comply with any and all applicable
export control laws, regulations, and/or other laws related to embargoes and
sanction programs administered by law.

SEVERABILITY: If any provision(s) of this Agreement shall be held to be
invalid, illegal, or unenforceable by a court or other tribunal of competent
jurisdiction, the validity, legality and enforceability of the remaining
provisions shall not in any way be affected or impaired thereby.

NO IMPLIED WAIVERS: No failure or delay by Licensor in enforcing any right
or remedy under this Agreement shall be construed as a waiver of any future
or other exercise of such right or remedy by Licensor.

GOVERNING LAW: This Agreement shall be construed and enforced in accordance
with the laws of Germany without reference to conflict of laws principles.
You consent to the personal jurisdiction of the courts of this country and
waive their rights to venue outside of Germany.

ENTIRE AGREEMENT AND AMENDMENTS: This Agreement constitutes the sole and
entire agreement between Licensee and Licensor as to the matter set forth
herein and supersedes any previous agreements, understandings, and
arrangements between the parties relating hereto.

       THIS HEADER MAY NOT BE EXTRACTED OR MODIFIED IN ANY WAY.
 */


#include "Enclave.h"
#include "Enclave_t.h" /* print_string */

#include "ThreadLibrary/ThreadTest.h"
#include "arch.h"

#include <stdarg.h>
#include <stdio.h> /* vsnprintf */
#include <string.h>

#define ATTACK 0
#define REPS 120

#define DEBUG 0
#define DEBUG_CACHE 0
#define OFFSET 27
#define TIME_FLUSH 300
#define MARGIN 180
#define INTERLEAVED 1
#define FULL_SET 0
#define LINE_OFF 8
#define CONTINUOS 0

typedef uint64_t sys_word_t;
typedef struct _thread_data_t
{
    sys_word_t self_addr;
    sys_word_t last_sp;          /* set by urts, relative to TCS */
    sys_word_t stack_base_addr;  /* set by urts, relative to TCS */
    sys_word_t stack_limit_addr; /* set by urts, relative to TCS */
    sys_word_t first_ssa_gpr;    /* set by urts, relative to TCS */
    sys_word_t stack_guard;      /* GCC expects start_guard at 0x14 on x86 and 0x28 on x64 */

    sys_word_t flags;
    sys_word_t xsave_size; /* in bytes (se_ptrace.c needs to know its offset).*/
    sys_word_t last_error; /* init to be 0. Used by trts. */

#ifdef TD_SUPPORT_MULTI_PLATFORM
    sys_word_t m_next; /* next TD used by trusted thread library (of type "struct _thread_data *") */
#else
    struct _thread_data_t *m_next;
#endif
    sys_word_t tls_addr;  /* points to TLS pages */
    sys_word_t tls_array; /* points to TD.tls_addr relative to TCS */
#ifdef TD_SUPPORT_MULTI_PLATFORM
    sys_word_t exception_flag; /* mark how many exceptions are being handled */
#else
    intptr_t exception_flag;
#endif
    sys_word_t cxx_thread_info[6];
    sys_word_t stack_commit_addr;
} thread_data_t;

#ifdef __cplusplus
extern "C"
{
#endif

    thread_data_t *get_thread_data(void);

#ifdef __cplusplus
}
#endif

pthread_t counter_task;
pthread_t monitor_task;
uint32_t data_size;
long int timeini;

thread_data_t *thread_data;
ssa_gpr_t *ssa_gpr = NULL;

size_t *regs;
char *stack;
size_t samples_rip[MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES * SAMPLES_SIZE];
//int samples_exit[MONITORED_SETS * WAYS_FILLED * CACHE_SLICES * SAMPLES_SIZE];
//long int samples_time[MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES * SAMPLES_SIZE];
long int measurements[PAGE_COUNT];
long int array_addresses[SPOILER_SETS][SPOILER_PEAKS];
long int set_addresses[SPOILER_SETS * SPOILER_PEAKS];
long int final_eviction_set[MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES];
long int eviction_add[MONITORED_SETS * CACHE_SLICES];
long int eviction_add_1[CACHE_SLICES];
int mon_ways;

/* 
 * printf: 
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */
int printf(const char *fmt, ...)
{
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
    return (int)strnlen(buf, BUFSIZ - 1) + 1;
}

/* 
 * printf: 
 *   Invokes OCALL to display the enclave buffer with a timestamp.
 */
int printimf(const char *fmt, ...)
{
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_timed_string(buf);
    return (int)strnlen(buf, BUFSIZ - 1) + 1;
}

/* 
 * printf: 
 *   Invokes OCALL to display the enclave buffer with a timestamp.
 */
int printFile(const char *fmt, ...)
{
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_write_plain_string(buf);
    return (int)strnlen(buf, BUFSIZ - 1) + 1;
}

int get_eax(void)
{
    //printf("");
    int i = 0;
    size_t exitreason = regs[17];
    //regs[17]= 0xFF;
    if ((exitreason != 0) || (regs[20] != 0))
    {
        thread_data = get_thread_data();
        regs = (size_t *)(thread_data->first_ssa_gpr);
        if (DEBUG)
        {
            printf("exit %zx %zx\n", regs[20], regs[17]);
        }
        regs[20] = 0;
        regs[17] = 0;
        i = 1;
    }
    return i;
}

size_t get_rip(void)
{
    thread_data = get_thread_data();
    ssa_gpr = reinterpret_cast<ssa_gpr_t *>(thread_data->first_ssa_gpr);
    //printf("EXIT %i",ssa_gpr->exit_info);
    regs = (size_t *)(thread_data->first_ssa_gpr);
    return regs[17];
}

void build_ev_set(int offset, long int ev_set[MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES], long int ev_set_1[CACHE_SET_SIZE * CACHE_SLICES], long int f_ev_set[MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES], int mways)
{
    long int original_address; // = (long int)&(memory_map[ram_border]) + ((slice << 6) & 0x0000FFC0);
    int i, j, k;
    long int pos2;
    for (i = 0; i < MONITORED_SETS * CACHE_SLICES; i++)
    {
        original_address = eviction_add[i];
        eviction_add[i] = original_address;
    }
    //Save the final set
    for (i = 0; i < MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES; i++)
    {

        original_address = ev_set[i];
        f_ev_set[i] = original_address;
        //original_address = ev_set_1[i];
        //f_ev_set[2 * i + 1] = ((original_address) & (0xFFFFFFFFFFFFF000)) + ((offset << 6) & (0x00000FC0));
    }

    if (DEBUG){
        for (i = 0; i < MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES; i++)
        {
            printf("EVV %lx %i\n", f_ev_set[i], i);
        }
    }
    /*Write this data in sets*/
    if (FULL_SET)
    {

        if (!INTERLEAVED)
        {
            //It needs to start with the address of each set
            for (i = 0; i < MONITORED_SETS; i++)
            {
                for (k = 0; k < CACHE_SLICES; k++)
                {
                    for (j = 0; j < mways; j++)
                    {
                        long int *pos1 = (long int *)f_ev_set[i * CACHE_SLICES * CACHE_SET_SIZE + k * CACHE_SET_SIZE + j];
                        if (j == (mways - 1))
                        {
                            pos2 = f_ev_set[(i * CACHE_SLICES * CACHE_SET_SIZE + k * CACHE_SET_SIZE) % (MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES)];
                        }
                        else
                        {
                            pos2 = f_ev_set[(i * CACHE_SLICES * CACHE_SET_SIZE + k * CACHE_SET_SIZE + j + 1) % (MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES)];
                        }
                        *pos1 = pos2;
                    }
                }
            }
        }
        else
        {
            for (j = 0; j < mways; j++)
            {
                for (k = 0; k < CACHE_SLICES; k++)
                {
                    for (i = 0; i < MONITORED_SETS; i++)
                    {
                        long int *pos1 = (long int *)f_ev_set[i * CACHE_SLICES * CACHE_SET_SIZE + k * CACHE_SET_SIZE + j];
                        if (i == (MONITORED_SETS - 1))
                        {
                            pos2 = f_ev_set[(k * CACHE_SET_SIZE + j) % (MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES)];
                        }
                        else
                        {
                            pos2 = f_ev_set[((i + 1) * CACHE_SLICES * CACHE_SET_SIZE + k * CACHE_SET_SIZE + j) % (MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES)];
                        }
                        *pos1 = pos2;
                    }
                }
            }
        }
        /*         for (i = 0; i < 2 * CACHE_SLICES; i++)
        {
            for (j = 0; j < WAYS_FILLED; j++)
            {
                long int *pos1 = (long int *)f_ev_set[i * CACHE_SET_SIZE + j];
                printf("%lx %i ", (*pos1), i * WAYS_FILLED + j);
                pos1 = (long int *)(*pos1);
                //printf("EVV2 %i \n",i);
            }
        }
        printf("\n"); */
    }
    else
    {
        if (!INTERLEAVED)
        {
            for (i = 0; i < MONITORED_SETS; i++)
            {
                for (k = 0; k < CACHE_SLICES; k++)
                {
                    for (j = 0; j < mways; j++)
                    {
                        long int *pos1 = (long int *)f_ev_set[i * CACHE_SLICES * CACHE_SET_SIZE + k * CACHE_SET_SIZE + j];
                        if (j == (mways - 1))
                        {
                            if (k == (CACHE_SLICES - 1))
                            {
                                if (i == (MONITORED_SETS - 1))
                                {
                                    pos2 = f_ev_set[0];
                                }
                                else
                                {
                                    pos2 = f_ev_set[((i + 1) * CACHE_SLICES * CACHE_SET_SIZE) % (MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES)];
                                }
                            }
                            else
                            {
                                pos2 = f_ev_set[((i)*CACHE_SLICES * CACHE_SET_SIZE + (k + 1) * CACHE_SET_SIZE) % (MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES)];
                            }
                        }
                        else
                        {
                            pos2 = f_ev_set[((i)*CACHE_SLICES * CACHE_SET_SIZE + k * CACHE_SET_SIZE + j + 1) % (MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES)];
                        }
                        *pos1 = pos2;
                    }
                }
            }
            /* for (i = 0; i < MONITORED_SETS * CACHE_SLICES; i++)
            {
                for (j = 0; j < WAYS_FILLED; j++)
                {
                    long int *pos1 = (long int *)f_ev_set[i * CACHE_SET_SIZE + j];
                    if (j == (WAYS_FILLED - 1))
                    {
                        pos2 = f_ev_set[((i + 1) * CACHE_SET_SIZE) % (2 * CACHE_SET_SIZE * CACHE_SLICES)];
                    }
                    else
                    {
                        pos2 = f_ev_set[(i * CACHE_SET_SIZE + j + 1) % (2 * CACHE_SET_SIZE * CACHE_SLICES)];
                    }
                    //long int *pos1 = (long int *)f_ev_set[i * CACHE_SET_SIZE + j];
                    //long int pos2 = f_ev_set[(i + 1) % (CACHE_SET_SIZE * CACHE_SLICES)];
                    *pos1 = pos2;
                    //printf("EVV2 %lx %i\n", (*pos1), i);
                }
            } */
        }
        else
        {
            for (j = 0; j < mways; j++)
            {
                for (k = 0; k < CACHE_SLICES; k++)
                {
                    for (i = 0; i < MONITORED_SETS; i++)
                    {
                        //printf("Index 1: %d ", i * CACHE_SLICES * CACHE_SET_SIZE + k * CACHE_SET_SIZE + j);
                        long int *pos1 = (long int *)f_ev_set[i * CACHE_SLICES * CACHE_SET_SIZE + k * CACHE_SET_SIZE + j];
                        if (i == (MONITORED_SETS - 1))
                        {
                            if (k == (CACHE_SLICES - 1))
                            {
                                if (j == (mways - 1))
                                {
                                    pos2 = f_ev_set[0];
                                    //printf("Index 2: %d ", 0);
                                }
                                else
                                {
                                    pos2 = f_ev_set[(j + 1) % (MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES)];
                                    //printf("Index 2: %d ", (j + 1) % (MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES));
                                }
                            }
                            else
                            {
                                pos2 = f_ev_set[((k + 1) * CACHE_SET_SIZE + j) % (MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES)];
                                //printf("Index 2: %d ", ((k + 1) * CACHE_SET_SIZE + j) % (MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES));
                            }
                        }
                        else
                        {
                            pos2 = f_ev_set[((i + 1) * CACHE_SLICES * CACHE_SET_SIZE + k * CACHE_SET_SIZE + j) % (MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES)];
                            //printf("Index 2: %d ", ((i + 1) * CACHE_SLICES * CACHE_SET_SIZE + k * CACHE_SET_SIZE + j) % (MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES));
                        }
                        *pos1 = pos2;
                    }
                }
            }
        }
        long unsigned int *pos1 = (long unsigned int *)f_ev_set[0];
        long unsigned int begin = f_ev_set[0];
        i = 0;
        do
        {
            if (DEBUG)
                printf("%lx %i ", (*pos1), i);
            pos1 = (long unsigned int *)(*pos1);
            i++;
        } while (pos1 != (long unsigned int *)begin);
        //printf("\n");
    }
}


int build_array(int border, int init, int off, int spoiler_trials)
{
    int res = 1;
    int trials = spoiler_trials;
    int i, j, k, samps, c, l = 0;
    int expected_size = CACHE_SLICES+CACHE_SLICES/2;
    for (j = border; j < (border + SPOILER_SETS); j++)
    {
        trials = spoiler_trials;
        long int s_set[PAGE_COUNT] = {0};
        for (k = 1; k <= spoiler_trials; k++)
        {
            address_aliasing(WINDOW_SIZE, j, init + off, measurements);
            samps = peak_search(s_set, measurements);
        }
        c = 0;
        for (i = 0; i < PAGE_COUNT; i++)
        {
            if (s_set[i] > 0.3 * spoiler_trials)
            {
                array_addresses[l][c] = (long int)(&memory_map[i * PAGE_SIZE + init + off]);
                //if (j == border)
                if (DEBUG)
                {
                    printf("Peak %i %li %i %li\n", i, s_set[i], j - border, measurements[i]);
                }
                c++;
                if (c >= SPOILER_PEAKS)
                {
                    break;
                }
            }
        }
        if (c < (expected_size))
        {
            j--;
            l--;
            trials += 10;
            res++;
            if (res > 2)
            {
		if (l>18){
                    expected_size--;
                    res = 1;
		}
            }
            if (expected_size < 3)
            {
                return 0;
            }
        }
        else
        {
            trials = spoiler_trials;
        }
        l++;
        printf("Peak %i %i with %i samples\n", j - border, c, samps);
    }
    return res;
}

void filter_spoiler(int reduced_set[SPOILER_SETS], int pre_set[SPOILER_SETS], long int ev_set[CACHE_SET_SIZE * CACHE_SLICES])
{
    int l, c, rep, cont1;
    for (l = 0; l < SPOILER_SETS; l++)
    {
        reduced_set[l] = pre_set[l];
    }

    long int set_add[SPOILER_PEAKS];
    long int t_add;
    int cont2, s, tmes;
    for (l = 0; l < SPOILER_SETS; l++)
    {
        //Check for the reduction
        if (reduced_set[l] >= 0)
        {
            cont1 = 0;
            for (c = 0; c < SPOILER_PEAKS; c++)
            {
                if (array_addresses[l][c] > 0)
                {
                    //printf("Ad %lx %i\n", array_addresses[l][c], cont);
                    set_add[cont1] = (array_addresses[l][c]);
                    cont1++;
                }
            }
            //Check if the addresses are evicted by the
            cont2 = 0;
            for (c = 0; c < cont1; c++)
            {
                t_add = set_add[c];
                s = 0;
                if (check_inside(t_add, ev_set, CACHE_SET_SIZE * CACHE_SLICES))
                {
                    cont2 = cont1;
                    break;
                }
                else
                {
                    for (rep = 0; rep < 100; rep++)
                    {
                        flush_data((long int *)t_add);
                        flush_desired_set(ev_set, CACHE_SET_SIZE * CACHE_SLICES);
                        lfence();
                        tmes = probe_candidate_sorted(CACHE_SET_SIZE * CACHE_SLICES, ev_set, (long int *)t_add);
                        if (tmes > TIME_FLUSH)
                        {
                            s++;
                        }
                    }
                    if (s > 70)
                    {
                        cont2++;
                    }
                }
            }
            double ratio = (100.0 * cont2) / (double)cont1;
            if (ratio > 70)
            {
                reduced_set[l] = -1;
                printf("Removed %d %f\n", l, ratio);
            }
        }
    }

    //Now copy the result
    for (l = 0; l < SPOILER_SETS; l++)
    {
        pre_set[l] = reduced_set[l];
    }
}

void spoiler_cache(int reduced_set[SPOILER_SETS], int pre_set[SPOILER_SETS], long int ev_set[SPOILER_SETS * SPOILER_PEAKS], int counter)
{
    int l, c, rep, cont1;
    for (l = 0; l < SPOILER_SETS; l++)
    {
        reduced_set[l] = -1;
    }

    long int set_add[SPOILER_PEAKS];
    long int t_add;
    int cont2, s, tmes;
    for (l = 0; l < SPOILER_SETS; l++)
    {
        //Check for the reduction
        if (pre_set[l] >= 0)
        {
            cont1 = 0;
            for (c = 0; c < SPOILER_PEAKS; c++)
            {
                if (array_addresses[l][c] > 0)
                {
                    //printf("Ad %lx %i\n", array_addresses[l][c], cont);
                    set_add[cont1] = (array_addresses[l][c]);
                    cont1++;
                }
            }
            //Check if the addresses are evicted by the
            cont2 = 0;
            for (c = 0; c < cont1; c++)
            {
                t_add = set_add[c];
                s = 0;
                if (check_inside(t_add, ev_set, counter))
                {
                    cont2 = cont1;
                    break;
                }
                else
                {
                    for (rep = 0; rep < 100; rep++)
                    {
                        flush_data((long int *)t_add);
                        flush_desired_set(ev_set, counter);
                        lfence();
                        tmes = probe_candidate_sorted(counter, ev_set, (long int *)t_add);
                        if (tmes > TIME_FLUSH)
                        {
                            s++;
                        }
                    }
                    if (s > 70)
                    {
                        cont2++;
                    }
                }
            }
            double ratio = (100.0 * cont2) / (double)cont1;
            //printf("Ratio %f\n", ratio);
            if (ratio > 30)
            {
                reduced_set[l] = l;
                printf("Included %d %f\n", l, ratio);
            }
        }
    }

    //Now copy the result
}

void get_reduced_set(int reduced_set[SPOILER_SETS], int pre_set[SPOILER_SETS], long int test_address, int r, int position, int startk)
{
    int cont1 = 0, c2;
    int tmes = 0;
    int k, l, c, v = 0;
    int rem_sets = 0;
    int stuck = 0, last_red;
    int loops = 0;
    int avail = CACHE_SET_SIZE * CACHE_SLICES * MONITORED_SETS;
    int last_avail = CACHE_SET_SIZE * CACHE_SLICES * MONITORED_SETS;
    k = startk;
    for (l = 0; l < SPOILER_SETS; l++)
    {
        reduced_set[l] = pre_set[l];
        if (reduced_set[l] >= 0)
        {
            rem_sets++;
        }
    }
    ///Now reduce the sets
    int num_sets = rem_sets;
    last_red = rem_sets;
    int range = 0;
    int range1 = 0;
    //while ((num_sets > r) && (avail > (CACHE_SET_SIZE * CACHE_SLICES + range)))
    while (avail > (CACHE_SET_SIZE * CACHE_SLICES + range))
    {
        k++;
        k = k % SPOILER_SETS;
        if (k == (position))
        {
            k++;
        }
        while (reduced_set[k] == -1)
        {
            k++;
            k = k % SPOILER_SETS;
            if (k == (position))
            {
                k++;
            }
        }
        if (k == startk)
        {
            loops++;
            printf("Loops start %i %i\n", last_avail, avail);
        }

        if (loops > 2)
        {
            printf("Loops reset %i \n", last_avail);
            loops = 0;
            for (l = 0; l < SPOILER_SETS; l++)
            {
                reduced_set[l] = pre_set[l];
            }
            //reps++;
            cont1 = 0;
            num_sets = rem_sets;
            stuck = 0;
            last_red = num_sets;
            last_avail = CACHE_SET_SIZE * CACHE_SLICES * MONITORED_SETS;
            avail = CACHE_SET_SIZE * CACHE_SLICES * MONITORED_SETS;
        }
        //Check for stuck and repeat
        if (last_red != num_sets)
        {
            stuck++;
            if (stuck > SPOILER_SETS)
            {
                printf("Stuck %i num sets, cont %i ran %i %i \n",num_sets,cont1,range,range1);
                for (l = 0; l < SPOILER_SETS; l++)
                {
                    reduced_set[l] = pre_set[l];
                }
                //reps++;
                cont1 = 0;
                num_sets = rem_sets;
                stuck = 0;
                range1 = 0;
                last_avail = CACHE_SET_SIZE * CACHE_SLICES * MONITORED_SETS;
                avail = CACHE_SET_SIZE * CACHE_SLICES * MONITORED_SETS;
            }
            last_red = num_sets;
        }
        else
        {
            stuck--;
            if (stuck < 0)
            {
                stuck = 0;
            }
        }
        v = 0;
        reduced_set[k] = -1;
        num_sets--;
        last_red = num_sets;
        cont1 = 0;
        c2 = 0;
        for (l = 0; l < SPOILER_SETS; l++)
        {
            //Check for the reduction
            if (reduced_set[l] >= 0)
            {
                for (c = 0; c < SPOILER_PEAKS; c++)
                {
                    if ((array_addresses[l][c] > 0) && (array_addresses[l][c] != test_address))
                    {
                        //printf("Ad %lx %i\n", array_addresses[l][c], cont);
                        set_addresses[cont1] = (array_addresses[l][c]);
                        cont1++;
                    }
                }
                c2++;
            }
        }
        if (num_sets != c2)
        {
            printf("Num set error \n");
        }
        num_sets = c2;
        for (l = 0; l < 100; l++)
        {
            flush_data((long int *)test_address);
            flush_desired_set(set_addresses, cont1);
            lfence();
            tmes = probe_candidate_sorted(cont1, set_addresses, (long int *)test_address);
            if (tmes > TIME_FLUSH)
            {
                v++;
            }
        }
        //If it is not removed, then we should keep it
        if (v < 90)
        {
            reduced_set[k] = k;
            num_sets++;
            if (stuck>1)
            {
                if (range1<2*SPOILER_PEAKS)
                {
                    range1++;
                }
            }
            if (cont1 <= CACHE_SET_SIZE * CACHE_SLICES + range1)
            {
                //printf("Values count %i %i %i %i\n", avail, last_avail, cont1,stuck);
                if (last_avail >= CACHE_SET_SIZE * CACHE_SLICES)
                {
                    range = last_avail - CACHE_SET_SIZE * CACHE_SLICES + 1;
                }
                if (stuck>6)
                {
                    range = last_avail - CACHE_SET_SIZE * CACHE_SLICES - 2;
                }
            }
            avail = last_avail;
        }
        else
        {
            last_avail = cont1;
        }
        if (DEBUG)
        {
            printf("Times %i %i %i %i %i %i \n", tmes, cont1, k, v, num_sets, stuck);
        }
    }

    cont1 = 0;
    for (l = 0; l < SPOILER_SETS; l++)
    {
        //Check for the reduction
        if (reduced_set[l] >= 0)
        {
            for (c = 0; c < SPOILER_PEAKS; c++)
            {
                if ((array_addresses[l][c] > 0) && (array_addresses[l][c] != test_address))
                {
                    //printf("Ad %lx %i\n", array_addresses[l][c], cont);
                    set_addresses[cont1] = (array_addresses[l][c]);
                    cont1++;
                }
            }
        }
    }

    spoiler_cache(reduced_set, pre_set, set_addresses, cont1);

    //Maybe even refill
    //Set addresses and cont

    printf("Reduced set %i %i %i %i %i\n", num_sets, cont1, c2, position, avail);
    for (l = 0; l < SPOILER_SETS; l++)
    {
        printf("%i ", reduced_set[l]);
    }
    printf("\n");
}

int test_selection(int reps, int reduced_set[SPOILER_SETS], int *lim, long int test_address)
{
    int tmes = 0;
    int cont = 0;
    int l, c, v = 0;
    /// Repeat the same test to find the data for the whole ev_set
    for (l = 0; l < SPOILER_SETS; l++)
    {
        if (reduced_set[l] >= 0)
        {
            for (c = 0; c < SPOILER_PEAKS; c++)
            {
                if ((array_addresses[l][c] > 0) && (array_addresses[l][c] != test_address))
                {
                    //printf("Ad %li %i\n", array_addresses[l][c], cont);
                    set_addresses[cont] = (array_addresses[l][c]);
                    cont++;
                }
            }
        }
    }
    *lim = cont;

    //Ensure the selected set evicts the target
    v = 0;
    for (l = 0; l < 1000; l++)
    {
        flush_data((long int *)test_address);
        flush_desired_set(set_addresses, cont);
        lfence();
        tmes = probe_candidate_sorted(cont, set_addresses, (long int *)test_address);
        if (tmes > TIME_FLUSH)
        {
            v++;
        }
    }
    //printf("The filtered set goes to %i \n", v);
    return v;
}

int get_min_set(long int test_address, int cont, long int ev_set[CACHE_SET_SIZE * CACHE_SLICES], int index)
{
    int tmes = 0;
    int k, l, v = 0;
    int stuck = 0, last_red, reps;
    int num_sets = SPOILER_SETS;
    last_red = SPOILER_SETS;
    reps = 0;
    long int filtered_set[cont];
    int cont1, conf;

LAB:
    //reps = 0;
    cont1 = 1;
    conf = 0;
    randomize_set(set_addresses, cont);
    v = 0;
    while (conf < 10)
    {
        reps++;
        //Get the test set
        l = 0;
        k = 0;
        while (l < cont1)
        //for (l = 0; l < cont1; l++)
        {
            if (!check_inside(set_addresses[k], ev_set, index * CACHE_SET_SIZE))
            {
                filtered_set[l] = set_addresses[k];
                l++;
            }
            k++;
        }
        //Test it
        for (l = 0; l < 1000; l++)
        {
            //flush_data((long int *)test_address);
            //flush_desired_set(set_addresses, cont);
            //lfence();
            tmes = probe_candidate_flush(cont1, filtered_set, (long int *)test_address);
            if (tmes > TIME_FLUSH)
            {
                v++;
            }
            //printf("%i ",tmes);
        }
        if (v > 900)
        {
            cont1--;
            conf++;
            //printf("Confidence pass %i %i\n", cont1, conf);
        }
        else
        {
            conf = 0;
        }
        cont1++;
        if (cont1 > cont)
        {
            printf("Noisy %i\n", v);
            reps++;
            if (reps > 20)
            {
                return -1;
            }
            goto LAB;
        }
    }

    //printf("First pass %i %i\n", cont1, cont);
    ///Now reduce the set

    //randomize_set(filtered_set, cont1);
    long int filtered_set1[cont1];
    int min_set[cont1];
    //long int ttest;

    int ch = 0, cont2 = 0;
    for (l = 0; l < cont1; l++)
    {
        min_set[l] = l;
        filtered_set1[l] = 0;
    }
    k = 0;
    num_sets = cont1;
    last_red = cont1;
    stuck = 0;
    while (num_sets > CACHE_SET_SIZE)
    {
        k++;
        k = k % cont1;
        while (min_set[k] == -1)
        {
            k++;
            k = k % cont1;
        }
        //Check for stuck and repeat
        if (last_red != num_sets)
        {
            stuck++;
            if (stuck > cont1 + 2 * CACHE_SET_SIZE)
            {
                for (l = 0; l < cont1; l++)
                {
                    min_set[l] = l;
                    filtered_set1[l] = 0;
                }
                if (DEBUG)
                {
                    printf("Final %i %i %i \n", cont2, v, ch);
                }
                randomize_set(filtered_set, cont1);
                num_sets = cont1;
                stuck = 0;
                ch++;
                if (ch > 20)
                {
                    goto LAB;
                }
            }
            last_red = num_sets;
        }
        else
        {
            stuck--;
            if (stuck < 0)
            {
                stuck = 0;
            }
        }
        cont2 = 0;
        v = 0;
        min_set[k] = -1;
        num_sets--;
        last_red--;
        for (l = 0; l < cont1; l++)
        {
            //Check for the reduction
            if (min_set[l] >= 0)
            {
                filtered_set1[cont2] = filtered_set[l];
                cont2++;
            }
        }
        for (l = 0; l < 100; l++)
        {
            //ttest = calibrate_miss((long int *)test_address);
            //lfence();
            //flush_desired_set(filtered_set, cont1);
            //lfence();
            tmes = probe_candidate_flush(cont2, filtered_set1, (long int *)test_address);
            if (tmes > TIME_FLUSH)
            {
                v++;
            }
            //lfence();
            //flush_data((long int *)test_address);
        }
        //printf("v %i %i\n",v,cont2);
        //printf("%li \n",ttest);
        if (DEBUG)
        {
            printf("Final %i %i %i %i %i %i %i %i\n", cont1, tmes, cont2, k, v, num_sets, ch, stuck);
        }
        if (v < 90)
        {
            min_set[k] = k;
            num_sets++;
            //printf("Tested %i %i %i \n", k, v, num_sets);
            //ch++;
        }
        else
        {
            if (DEBUG){
                if (cont2 == CACHE_SET_SIZE)
                {
                    printf("Ops!! \n");
                }
            }
        }
    }
    v = 0;
    for (l = 0; l < 2000; l++)
    {
        tmes = probe_candidate_flush(CACHE_SET_SIZE, filtered_set1, (long int *)test_address);
        if (tmes > TIME_FLUSH)
        {
            v++;
        }
        //printf("%i ", tmes);
    }
    if (DEBUG)
    {
        printf("\n %i \n", v);
    }
    for (l = 0; l < CACHE_SET_SIZE; l++)
    {
        ev_set[index * CACHE_SET_SIZE + l] = filtered_set1[l];
        //printf("%lx ", filtered_set1[l]);
    }
    return 0;
    //printf("\n");
}

void complete_eviction_set(int orig_cont, long int t_address, int red_set[SPOILER_SETS], int pre_red_set[SPOILER_SETS],
                           long int evic_add[CACHE_SLICES], long int evict_set[CACHE_SLICES * CACHE_SET_SIZE], int index,
                           long int big_ev_set[MONITORED_SETS * CACHE_SLICES * CACHE_SET_SIZE], int lcont)
{
    int i = 1;
    int j = 0, v = 0;
    int k = 1, l = 0;
    int k1 = 1;
    int tmes, res, cont;
    int start_set = 20;
    cont = orig_cont;
    long int test_address = t_address;
    long int old_address = t_address;
    long int test_address1;
    int st = 0;
    //printf("Test complete address %lx \n", test_address);
    int valid = 1;
    int ind = index;
    while (i < CACHE_SLICES)
    {
    NEW_ADD:
        valid = 1;
        while (valid)
        {
            test_address1 = array_addresses[ind][j];
            if ((!check_inside(test_address1, evic_add, i)) && (!check_inside(test_address1, evict_set, i * CACHE_SET_SIZE)))
            {
                if (test_address1 != 0)
                {
                    valid = 0;
                    test_address = test_address1;
                }
                else
                {
                    test_address1 = 0;
                    while (test_address1 == 0)
                    {
                        test_address1 = array_addresses[ind][k1];
                        if ((!check_inside(test_address1, evic_add, i)) && (!check_inside(test_address1, evict_set, i * CACHE_SET_SIZE)))
                        {
                            if (test_address1 != 0)
                            {
                                j = SPOILER_PEAKS;
                                test_address = test_address1;
                            }
                            else
                            {
                                k1 = 0;
                                test_address1 = 0;
                            }
                        }
                        else
                        {
                            test_address1 = 0;
                        }
                        k1++;
                        k1 = k1 % (SPOILER_PEAKS);
                    }
                }
            }
            j++;
            if (j >= (SPOILER_PEAKS))
            {
                st++;
                if (st > 10){
                    for (st=ind+1;st<SPOILER_PEAKS;st++)
                    {
                        if(red_set[st]!=(-1)){
                            ind = st;
                            break;
                        }
                    }
                    st = 0;
                    printf("New index %i \n",ind);
                    j = 0;
                    goto NEW_ADD;
                }
                v = 0;
                while (v < 900)
                {
                    get_reduced_set(red_set, pre_red_set, test_address, 6, ind, start_set);
                    v = test_selection(1000, red_set, &cont, old_address);
                    printf("V %i %i %lx %i %i\n", v, k1, test_address, start_set,ind);
                    start_set++;
                    start_set = start_set % SPOILER_SETS;
                    mfence();
                    lfence();
                    v = test_selection(1000, red_set, &cont, old_address);
                    /* if (v > 900)
                    {
                        for (k = 0; k < SPOILER_SETS; k++)
                        {
                            if (red_set[(ind + SPOILER_SETS - k - 1) % SPOILER_SETS] >= 0)
                            {
                                ind = k;
                                break;
                            }
                        }
                    } */
                    l++;
                    if (l > 2)
                    {
                        test_address1 = 0;
                        while (test_address1 == 0)
                        {
                            k1++;
                            k1 = k1 % (SPOILER_PEAKS);
                            test_address1 = array_addresses[ind][k1];
                            if ((!check_inside(test_address1, evic_add, i)) && (!check_inside(test_address1, evict_set, i * CACHE_SET_SIZE)))
                            {
                                if (test_address1 != 0)
                                {
                                    j = SPOILER_PEAKS;
                                    test_address = test_address1;
                                }
                                else
                                {
                                    k1 = 0;
                                    test_address1 = 0;
                                }
                            }
                            else
                            {
                                test_address1 = 0;
                            }
                        }
                        l = 0;
                    }
                }
                j = 0;
            }
            j = j % (SPOILER_PEAKS);
        }
        printf("Test address %lx %i %i \n", test_address, i, j);
        //first check if it is evicted with the current eviction set (the complete one)
        v = 0;
        for (l = 0; l < 2000; l++)
        {
            tmes = probe_candidate_flush(lcont * CACHE_SET_SIZE * CACHE_SLICES, big_ev_set, (long int *)test_address);
            if (tmes > TIME_FLUSH)
            {
                v++;
            }
        }
        if (v > 200)
        {
            printf("Other set new %i\n", v);
            goto NEW_ADD;
        }
        //first check if it is evicted with the current eviction set
        v = 0;
        for (l = 0; l < 2000; l++)
        {
            tmes = probe_candidate_flush(i * CACHE_SET_SIZE, evict_set, (long int *)test_address);
            if (tmes > TIME_FLUSH)
            {
                v++;
            }
        }
        if (v > 200)
        {
            printf("Candidate invalid, evicted %i out of 2000 \n", v);
            goto NEW_ADD;
        }
        //printf("Valid %i\n", v);
        v = 0;
        k = 0;
        while (v < 1500)
        {
            v = 0;
            res = get_min_set(test_address, cont, evict_set, i);
            if (res != 0)
            {
                goto NEW_ADD;
            }
            printf("Valid\n");
            for (l = 0; l < 2000; l++)
            {
                tmes = probe_candidate_flush((i + 1) * CACHE_SET_SIZE, evict_set, (long int *)test_address);
                if (tmes > TIME_FLUSH)
                {
                    v++;
                }
                //printf("%i ", tmes);
            }
            //printf("The simp go to %i %i %i\n", v, i, k);
            k++;
            if (k > 20)
            {
                goto NEW_ADD;
            }
        }
        evic_add[i] = test_address;
        i++;
    }
}

void gen_plot(int rounds)
{

    int ini = 0 * RESERVED_RAM;
    ram_border = ini + 64;
    ini = (ram_border)&0x00000FFF;
    int val = (ram_border - ini) >> 12;
    int i, j, k;

    printf("Initial values for the spoiler array %i %i %lx\n", val, ini, &memory_map[ram_border]);
    uint32_t rd;
    sgx_read_rand((unsigned char *)&rd, 4);
    int offset = OFFSET;
    offset = offset * 64;
    int res = 0;
    //Build a "test set"
    int cont = 0, v = 0;
    int tmes = 0;
    int start_set = 20;
    int l = 0;

    res = build_array(val, ini, offset, 100);
    printf("Res %i \n", res);
    while (res == 0)
    {
        sgx_read_rand((unsigned char *)&rd, 4);
        offset = OFFSET;
        offset = offset * 64;
        res = build_array(val, ini, offset, 100);
    }
    int reduced_set[SPOILER_SETS];
    int pre_reduced_set[SPOILER_SETS];
    for (l = 0; l < SPOILER_SETS; l++)
    {
        pre_reduced_set[l] = l;
    }
    //printf("DATA %i %i %i\n", val, ini, offset);

    //Build the evictions sets,

    //FOR THE FIRST ADDRESS
    int set_num = val;
    int ind = 0;
    long int ev_set_test[CACHE_SLICES * CACHE_SET_SIZE];
    long int add_lim[CACHE_SLICES];
    int crep = 0;
    while (crep < MONITORED_SETS)
    //for (set_num = val; set_num < val + MONITORED_SETS; set_num++)
    {
        if (pre_reduced_set[ind] >= 0)
        {
            //FOR THE SECOND ADDRESS
            printf("Build eviction set %i %i\n", ind, crep);
            long int test_address = (long int)(&memory_map[set_num * PAGE_SIZE + ini + offset]);
            printf("Addresses %lx %lx %i %i\n\n", test_address, (long int)(&memory_map[set_num * PAGE_SIZE + ini]), offset, val);
            //printf("Origintal value %i %i %lx\n", val, ini, &memory_map[ram_border]);

            for (k = 0; k < CACHE_SLICES * CACHE_SET_SIZE; k++)
            {
                ev_set_test[k] = 0;
            }

        BAD_SPOT:
            v = 0;
            get_reduced_set(reduced_set, pre_reduced_set, test_address, 6, ind, start_set);
            start_set++;
            v = test_selection(1000, reduced_set, &cont, test_address);
            while (v < 900)
            {
                printf("Bad Spot \n");
                get_reduced_set(reduced_set, pre_reduced_set, test_address, 6, ind, start_set);
                v = test_selection(1000, reduced_set, &cont, test_address);
                start_set++;
                start_set = start_set % SPOILER_SETS;
            }

            while (v < 1500)
            {
                res = get_min_set(test_address, cont, ev_set_test, 0);
                if (res != 0)
                {
                    goto BAD_SPOT;
                }
                v = 0;
                for (l = 0; l < 2000; l++)
                {
                    tmes = probe_candidate_flush(CACHE_SET_SIZE, ev_set_test, (long int *)test_address);
                    if (tmes > TIME_FLUSH)
                    {
                        v++;
                    }
                    //printf("%i ", tmes);
                }
                //printf("The final test goes to %i \n", v);
            }
            //printf("The final test goes to %i \n", v);
            add_lim[0] = test_address;
            //Repeat for each of the ways
            complete_eviction_set(cont, test_address, reduced_set, pre_reduced_set, add_lim, ev_set_test, ind, eviction_set, crep);
            //Remove the data belonging to those sets from the
            filter_spoiler(reduced_set, pre_reduced_set, ev_set_test);

            memcpy(&(eviction_set[crep * CACHE_SLICES * CACHE_SET_SIZE]), ev_set_test, (CACHE_SLICES * CACHE_SET_SIZE) * sizeof(long int));
            memcpy(&(eviction_add[crep * CACHE_SLICES]), add_lim, (CACHE_SLICES) * sizeof(long int));
            crep++;
        }
        //Increase the index value
        ind++;
        set_num++;
    }

    for (i = 0; i < MONITORED_SETS * CACHE_SLICES; i++)
    {
        for (j = 0; j < CACHE_SET_SIZE; j++)
        {
            printf("%lx ", eviction_set[i * CACHE_SET_SIZE + j]);
        }
        printf("\n");
    }

    build_ev_set(OFFSET, eviction_set, eviction_set_1, final_eviction_set, mon_ways);
    pthread_mutex_lock(&mutex_start);
    monitor_ready = 1;
    pthread_mutex_unlock(&mutex_start);
    pthread_cond_signal(&condp);
    printf("Done\n");
}


void collect_samples_cache(long int address, long int samples[SAMPLES_SIZE * MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES],
                           size_t s_rip[SAMPLES_SIZE * MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES])
{
    int i, k = 0;
    if (FULL_SET)
    {
        int j = 0;
        //Prefetch
        for (i = 0; i < MONITORED_SETS * CACHE_SLICES; i++)
        {
            long int begin = final_eviction_set[(i * CACHE_SET_SIZE)];
            prime_ev_set(begin);
        }
        for (i = 0; i < MONITORED_SETS * SAMPLES_SIZE * CACHE_SET_SIZE * CACHE_SLICES; i++)
        {
            long int begin = final_eviction_set[(j * CACHE_SET_SIZE) % (MONITORED_SETS * CACHE_SLICES * CACHE_SET_SIZE)];
            samples[i] = prime_ev_set(begin);
            //access_timed(begin, &samples[i]);
            //begin = (long int *)(*begin);
            s_rip[i] = get_rip();
            lfence();
            get_eax();
            j++;
            //mem_access((long int *)eviction_add[i % CACHE_SLICES]);
        }
    }
    else
    {
        long int *begin = (long int *)address;
        prime_ev_set(address);
        prime_ev_set(address);
	//prime_ev_set(address);
        lfence();
        mfence();
        if (CONTINUOS && (mon_ways<=8))
        {
            long int ts;
            while(1)
            {
                access_timed(begin, &ts);
                begin = (long int *)(*begin);
                if ((i % MON_WINDOW) == 0)
                {
                    get_rip();
                    lfence();
                    get_eax();
                }
                i++;
                //mem_access((long int *)eviction_add[i % CACHE_SLICES]);
            }
        }
        else
        {
            for (i = 0; i < MONITORED_SETS * SAMPLES_SIZE * CACHE_SET_SIZE * CACHE_SLICES; i++)
            {
                access_timed(begin, &samples[i]);
                begin = (long int *)(*begin);
                if ((i % MON_WINDOW) == 0)
                {
                    s_rip[k] = get_rip();
                    lfence();
                    get_eax();
                    k++;
                }
                //mem_access((long int *)eviction_add[i % CACHE_SLICES]);
            }
        }
    }
}

void collect_samples_attack(long int address)
{
    long int s[MON_WINDOW];
    size_t s_r[MON_WINDOW];
    long int s_t[MON_WINDOW];
    int i;
    if (FULL_SET)
    {
        int j = 0;
        //Prefetch
        for (i = 0; i < MONITORED_SETS * CACHE_SLICES; i++)
        {
            long int begin = final_eviction_set[(i * CACHE_SET_SIZE)];
            prime_ev_set(begin);
        }
        i = 0;
        while (1)
        {
            long int begin = final_eviction_set[(j * CACHE_SET_SIZE) % (MONITORED_SETS * CACHE_SLICES * CACHE_SET_SIZE)];
            s_t[i] = global_counter - timeini;
            timeini = global_counter;
            s[i] = prime_ev_set(begin);
            s_r[i] = get_rip();
            lfence();
            j++;
            i = (i + 1) % MON_WINDOW;
            //mem_access((long int *)eviction_add[i % CACHE_SLICES]);
        }
    }
    else
    {
        i = 0;
        long int *begin = (long int *)address;
        prime_ev_set(address);
        prime_ev_set(address);
        mfence();
        //while (i< (REPS*200)*(MONITORED_SETS * SAMPLES_SIZE * CACHE_SET_SIZE * CACHE_SLICES))
        while (1)
        {
	    access_timed(begin, &s[0]);
	    begin = (long int *)(*begin);
	    // if ((i % MON_WINDOW) == 0)
        //     {
        //         get_rip();
        //         lfence();
	    //     get_eax();
 	    // }
            i++;
        }
    }
}


/*
 * Monitor function
 */

void *monitor_sets_old(void *)
{
    int i, j = 0;
    pthread_mutex_lock(&mutex_start);
    sampler_ready = 0;
    pthread_mutex_unlock(&mutex_start);
    long int address = final_eviction_set[0];
    if (ATTACK)
    {
        collect_samples_attack(address);
    }
    else
    {
        while (j < REPS)
        {
            collect_samples_cache(address, samples_data, samples_rip);
            if (DEBUG_CACHE)
            {
                if ((INTERLEAVED) || (FULL_SET))
                {
                    int k;
                    for (i = 0; i < SAMPLES_SIZE * mon_ways; i++)
                    {
                        printf("Tim ");
                        for (k = 0; k < 2 * CACHE_SLICES; k++)
                        {
                            printf("%li %zx ", samples_data[i * CACHE_SLICES + k], samples_rip[i * CACHE_SLICES + k]);
                        }
                        printf("%i %i \n", j, i);
                    }
                }
                else
                {
                    for (i = 0; i < 2 * SAMPLES_SIZE * mon_ways * CACHE_SLICES; i++)
                    {
                        printf("Tim %li %zx %i %i \n", samples_data[i], samples_rip[i], j, i);
                    }
                }
            }
            j++;
        }
    }

    pthread_mutex_lock(&mutex_start);
    sampler_ready = 1;
    pthread_mutex_unlock(&mutex_start);
    pthread_cond_signal(&condSets);
}

/*
 * Monitor function for output
 */

void *monitor_sets_file(void *)
{
    int i, j = 0, cont = 0;
    long int address = final_eviction_set[0];
    prime_ev_set(address);
    prime_ev_set(address);
    pthread_mutex_lock(&mutex_start);
    sampler_ready = 0;
    pthread_mutex_unlock(&mutex_start);
    while (j < REPS)
    {
        printFile("Start profile %li ", global_counter);
        collect_samples_cache(address, samples_data, samples_rip);
        printFile("End profile %li ", global_counter);
        if ((INTERLEAVED) || (FULL_SET))
        {
            int k;
            int cx;
            int cx2;
            int val;
            int lim = (MONITORED_SETS * SAMPLES_SIZE * CACHE_SET_SIZE * CACHE_SLICES) -
                      (MONITORED_SETS * SAMPLES_SIZE * CACHE_SET_SIZE * CACHE_SLICES) % MON_WINDOW;
            for (i = 0; i < lim; i += MON_WINDOW)
            {
                //printFile("Tim ");
                cont = 0;
                cx = 0;
                cx2 = 0;
                char buf[MON_WINDOW * (sizeof(int) + 1)];
                char buf_long[MON_WINDOW * (sizeof(long int) + 1)];
                for (k = 0; k < MON_WINDOW; k++)
                {
                    //if ((samples_rip[int(i + k]) != 0)
                    //{
                    //    cont++;
                    //}
                    cont = (int)samples_rip[int(i / MON_WINDOW)];
                    val = -1;
                    if (samples_data[i + k] != 0)
                    {
                        if (samples_data[i + k] > (TIME_FLUSH+50))
                        {
                            val = 1;
                        }
                        else
                        {
                            val = 0;
                        }
                    }
                    cx += snprintf(buf + cx, MON_WINDOW * (sizeof(int) + 1) - cx - 1, "%d ", val);
                    cx2 += snprintf(buf_long + cx2, MON_WINDOW * (sizeof(long int) + 1) - cx2 - 1, "%lu ", samples_data[i + k]);
                    //printFile("Tim %li %zx %lu ", samples_data[i * CACHE_SLICES + k], samples_rip[i * CACHE_SLICES + k], samples_time[i * CACHE_SLICES + k]);
                }
                printFile("Tim %s tim %s cont %i %i cx %i", buf, buf_long, cont, j, cx);
                //printFile("%i %i ", j, i);
            }
        }
        else
        {
            for (i = 0; i < MONITORED_SETS * SAMPLES_SIZE * mon_ways * CACHE_SLICES; i++)
            {
                printFile("Tim %li %zx %i %i \n", samples_data[i], samples_rip[i], j, i);
            }
        }
        j++;
    }
    pthread_mutex_lock(&mutex_start);
    sampler_ready = 1;
    pthread_mutex_unlock(&mutex_start);
    pthread_cond_signal(&condSets);

    /* pthread_cancel(counter_task);
    pthread_mutex_lock(&global_mutex);
    monitor_running = 0;
    pthread_mutex_unlock(&global_mutex); */
}


void printf_helloworld()
{
    mon_ways = WAYS_FILLED;
    //Info about the thread reading SSA
    thread_data = get_thread_data();
    regs = (size_t *)(thread_data->first_ssa_gpr);
    stack = (char *)(thread_data->first_ssa_gpr);
    regs[20] = 0;

    // Ensure the counter is running
    if (!(check_running()))
        pthread_create(&counter_task, NULL, fast_counter, NULL);
    //Assign the secret
    if (!(check_ready()))
    {
        gen_plot(30);
        pthread_mutex_lock(&mutex_start);
        while (!(check_ready()))
        {
            pthread_cond_wait(&condp, &mutex_start);
        }
        pthread_mutex_unlock(&mutex_start);
        pthread_create(&monitor_task, NULL, monitor_sets_old, NULL);
    }
    //increase_counter();
    long int examp = global_counter;
    if (DEBUG){
        printf("Hello World %i \n", (int8_t)secret_data[0]);
        printf("Counter %li %li\n", global_counter, examp);
    }
}

void printfile_measurements(int *ways)
{
    // Ensure the counter is running and if not get everything ready
    mon_ways = WAYS_FILLED;
    //Info about the thread reading SSA
    thread_data = get_thread_data();
    regs = (size_t *)(thread_data->first_ssa_gpr);
    stack = (char *)(thread_data->first_ssa_gpr);
    regs[20] = 0;

    // Ensure the counter is running
    if (!(check_running()))
        pthread_create(&counter_task, NULL, fast_counter, NULL);
    //Assign the secret
    if (!(check_ready()))
    {
        gen_plot(30);
        pthread_mutex_lock(&mutex_start);
        while (!(check_ready()))
        {
            pthread_cond_wait(&condp, &mutex_start);
        }
        pthread_mutex_unlock(&mutex_start);
        pthread_create(&monitor_task, NULL, monitor_sets_old, NULL);
        printf("Eviction sets have been created and the app is ready for input\n");
    }
    else {
        int inter = (*(ways));
        if ((inter > CACHE_SET_SIZE) || (inter < 3))
        {
            inter = CACHE_SET_SIZE;
        }
        //pthread_cancel(monitor_task);
        printf("Pre measurements %i\n", inter);
        pthread_mutex_lock(&mutex_start);
        while (!(check_sampler()))
        {
            pthread_cond_wait(&condSets, &mutex_start);
        }
        pthread_mutex_unlock(&mutex_start);
        if (mon_ways != inter)
        {
            mon_ways = inter;
            printf("New ways %d \n", mon_ways);
            build_ev_set(OFFSET, eviction_set, eviction_set_1, final_eviction_set, mon_ways);
        }
        printf("New measurements \n");
        if (ATTACK)
        {
        pthread_create(&monitor_task, NULL, monitor_sets_old, NULL);
        }
        else{
            pthread_create(&monitor_task, NULL, monitor_sets_file, NULL);
        }
    }
}
