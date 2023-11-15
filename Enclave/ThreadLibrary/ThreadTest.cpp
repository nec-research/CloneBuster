/* CloneBuster
	  
File:     ThreadTest.cpp
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


#include "ThreadTest.h"

#define BUFFER_SIZE 50
#define USE_MEAN 1

long int global_counter = 0;
int monitor_running = 0;
int monitor_ready = 0;
int sampler_ready = 0;
pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_start = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condp = PTHREAD_COND_INITIALIZER;
pthread_cond_t condSets = PTHREAD_COND_INITIALIZER;
unsigned char secret_data[BUFFER_SIZE] = {0};
char memory_map[RESERVED_MEMORY];
long int eviction_set[MONITORED_SETS*CACHE_SET_SIZE * CACHE_SLICES];
long int eviction_set_1[CACHE_SET_SIZE * CACHE_SLICES];
long int samples_data[MONITORED_SETS * CACHE_SET_SIZE * CACHE_SLICES * SAMPLES_SIZE];
int ram_border = 0;
uint8_t *secret_data_read = NULL;
char aad_mac_text[BUFSIZ] = "aad mac text";
long int *base_address = NULL;

/*
* Aux function to access data
*/

/*Get the value of a memory location*/
long int mem_access(long int *v)
{
    long int rv = 0;
    asm volatile(
        "movq (%1), %0"
        : "+r"(rv)
        : "r"(v)
        :);
    return rv;
}

/*Flush data from the cache*/
void flush_data(long int *pos_data)
{
    asm __volatile__(
        " clflush 0(%0) \n"
        :
        : "c"(pos_data));
}

/*Timed access*/
long int access_timed(long int *pos_data, long int *t1)
{
    long int t = global_counter;
    lfence();
    long int lect = mem_access(pos_data);
    lfence();
    *t1 = (global_counter - t);
    return lect;
}

/*Timed access*/
long int access_timed_1(long int *pos_data)
{
    long int t = global_counter;
    lfence();
    long int lect = mem_access(pos_data);
    lfence();
    return (global_counter - t);
}

/*Timed access*/
long int calibrate_miss(long int *pos_data)
{
    long int t1, t = global_counter;
    lfence();
    mem_access(pos_data);
    lfence();
    t1 = global_counter - t;
    lfence();
    flush_data(pos_data);
    return t1;
}

long int prime_ev_set(unsigned long int pos_data)
{
    unsigned long *begin = (unsigned long *)pos_data;
    unsigned long int t1 = global_counter;
    mfence();
    lfence();
    do
    {
        begin = (unsigned long *)(*begin);
    } while (begin != (unsigned long *)pos_data);
    lfence();
    mfence();
    return (global_counter - t1);
}

/*Timed access*/
long int two_access_timed(long int *pos_data1, long int *pos_data2)
{

    asm __volatile__(
        " clflush 0(%0) \n"
        " clflush 0(%1) \n"
        " lfence \n"
        " mfence \n"
        :
        : "c"(pos_data1), "r"(pos_data2));
    long int t1 = global_counter;
    asm __volatile__(
        " mfence \n"
        " lfence \n"
        " movq 0(%0), %%rbx \n"
        " movq 0(%1), %%rdx \n"
        " lfence \n"
        " mfence \n"
        :
        : "c"(pos_data1), "r"(pos_data2));
    return (global_counter - t1);
}

long int measure_access(long int *pos_data)
{
    long int time = global_counter;
    asm __volatile__(
        " movl (%0), %%eax \n"
        :
        : "c"(pos_data)
        : "%esi", "%edx");
    time = global_counter - time;
    return time;
}

/*Flush desired set*/
void flush_desired_set(long int address_array[], int len)
{
    int i;
    for (i = 0; i < len; ++i)
    {
        flush_data((long int *)address_array[i]);
    }
}

/*Test if an element is evicted by an array of candidates*/
int probe_candidate(int number_elements, long int array_set[], long int *dir_candidate)
{
    int i;
    mem_access(dir_candidate);
    lfence();
    for (i = 0; i < number_elements; ++i)
    {
        mem_access((long int *)array_set[i]);
        //lfence();
    }
    lfence();
    i = (int)access_timed_1(dir_candidate);
    return i;
}

/*Test if an element is evicted by an array of candidates*/
int probe_candidate_sorted(int number_elements, long int array_set[], long int *dir_candidate)
{
    int i;
    long u = mem_access(dir_candidate);
    lfence();
    for (i = 0; i < number_elements; ++i)
    {
        u = mem_access((long int *)array_set[i]);
        lfence();
        mfence();
    }
    lfence();
    i = (int)access_timed_1(dir_candidate);
    return i;
}

int probe_candidate_flush(int number_elements, long int array_set[], long int *dir_candidate)
{
    int i, j;
    for (j = 0; j < 2; ++j)
    {
        long u = mem_access(dir_candidate);
        for (i = 0; i < number_elements; ++i)
        {
            u = mem_access((long int *)array_set[i]);
        }
    }
    lfence();
    flush_data(dir_candidate);
    flush_desired_set(array_set, number_elements);
    lfence();
    mfence();
    long u = mem_access(dir_candidate);
    lfence();
    mfence();
    for (i = 0; i < number_elements; ++i)
    {
        u = mem_access((long int *)array_set[i]);
        lfence();
    }
    lfence();
    mfence();
    i = (int)access_timed_1(dir_candidate);
    return i;
}

void randomize_set(long int array[], int len)
{
    int i;
    int random_number;
    uint32_t val;
    sgx_read_rand((unsigned char *)&val, 4);
    for (i = 0; i < len; ++i)
    {
        random_number = val % (len);
        long int original = array[i];
        array[i] = array[random_number];
        array[random_number] = original;
    }
}

int check_inside(long int dato, long int array_data[], int array_length)
{
    int i;
    int si = 0;
    for (i = 0; i < array_length; ++i)
    {
        if (dato == array_data[i])
        {
            si = 1;
            break;
        }
    }
    return si;
}

/*Function for hammering data*/
long int hammer(long int *pos1, long int *pos2, int rounds)
{
    int i = 0;
    long int total_time = 0;
    long int t1, t2;
    for (i = 0; i < rounds; ++i)
    {
        total_time += two_access_timed(pos1, pos2);
    }
    return total_time;
}

/*
 * Aux function for quick masking and getting the bits:
 */
int parity(unsigned long int v)
{
    v ^= v >> 1;
    v ^= v >> 2;
    v = (v & 0x1111111111111111UL) * 0x1111111111111111UL;
    return (v >> 60) & 1;
}



/*
 * Spoiler peaks:
 */

void address_aliasing(int w, int x, int ini, long int meas[PAGE_COUNT])
{
    int i, j, p;
    long int dif;
    /*Important! start in a physical address ending in 0x000*/
    /*int ini = 0;
    for (i = ini; i < ini + 0xFFF; i++)
    {
        if ((((long int)(&memory_map[i])) & 0xFFF) == 0xFFF)
        {
            ini = i + 1;
            break;
        }
    }*/
    long int *test_address1 = (long int *)(&memory_map[x * PAGE_SIZE + ini]);

    memset(meas, 0, PAGE_COUNT * sizeof(long int));

    for (p = w; p < PAGE_COUNT; p++)
    {
        dif = 0;
        for (j = 0; j < 10; j++)
        {
            for (i = w; i >= 0; i--)
            {
                memory_map[(p - i) * PAGE_SIZE + ini] = 0;
            }

            dif += measure_access(test_address1);
        }
        meas[p] = dif;
    }
}

/*
 * Simple peak searching:
 */

int peak_search(long int simple_set[PAGE_COUNT], long int meas[PAGE_COUNT])
{
    int i, j = 0;
    int sum = 0, av, ant = 0;
    int max = 0,lim = 600;
#if USE_MEAN
    for (i = 1; i < 1000; i++)
    {
        if (meas[i] > 0)
        {
            sum += meas[i];
            if (meas[i] > max)
            {
                max = meas[i];
            }
        }
    }
    av = sum / (1000);
    //max = max / (1000);
    lim = (0.6 * (max - av));
    if (lim > 800)
    {
        lim = 800;
    }

#else
    av = 2200;
    lim = 600;
#endif
    for (i = 1; i < PAGE_COUNT; i++)
    {
        if (meas[i] > (av + lim))
        {
            if ((i - ant) > WINDOW_SIZE)
            {
                simple_set[i]++;
                ant = i;
                j++;
            }
        }
    }
    return lim;
}

/*
 * increase_counter:
 */
long int increase_counter(void)
{
    long int ret = 0;
    pthread_mutex_lock(&global_mutex);
    /* mutually exclusive adding */
    long int tmp = global_counter;
    global_counter = ++tmp;
    ret = global_counter;
    pthread_mutex_unlock(&global_mutex);
    return ret;
}

/*
 * retrieve the counter value:
 */
long int check_counter(void)
{
    long int ret = 0;
    pthread_mutex_lock(&global_mutex);
    ret = global_counter;
    pthread_mutex_unlock(&global_mutex);
    return ret;
}

/*
 * Check if the secondary thread is running
 */

int check_running(void)
{
    int ret = 0;
    pthread_mutex_lock(&global_mutex);
    ret = monitor_running;
    pthread_mutex_unlock(&global_mutex);
    return ret;
}

/*
 * Check if the secondary thread is ready
 */

int check_ready(void)
{
    int ret = 0;
    pthread_mutex_lock(&global_mutex);
    ret = monitor_ready;
    pthread_mutex_unlock(&global_mutex);
    return ret;
}

/*
 * Check if everything is ready for data collection 
 */

int check_sampler(void)
{
    int ret = 0;
    pthread_mutex_lock(&global_mutex);
    ret = sampler_ready;
    pthread_mutex_unlock(&global_mutex);
    return ret;
}

/*
* Fast counter based on the SGX-Malware paper
* Ideally this process will run forever
*/

void *fast_counter(void *)
{
    pthread_mutex_lock(&global_mutex);
    monitor_running = 1;
    pthread_mutex_unlock(&global_mutex);

    global_counter = 0;

    asm("movq %0, %%rcx\n\t"
        " xor %%rax, %%rax \n\t"
        " 1: inc %%rax \n\t"
        " movq %%rax, (%%rcx) \n\t"
        " jmp 1b \n\t"
        :                      /*  No output */
        : "r"(&global_counter) /* input */
        : "%rcx", "memory"     /* clobbered register */
    );
}

/*
 * Monitor function
 */

void *counter(void *)
{
    pthread_mutex_lock(&global_mutex);
    monitor_running = 1;
    pthread_mutex_unlock(&global_mutex);

    while (global_counter < 40)
    {
        unsigned int tsx = _xbegin();
        if (tsx == _XBEGIN_STARTED)
        {
            while (1)
            {
            }
        }
        else
        {
            increase_counter();
        }
    }
    pthread_mutex_lock(&mutex_start);
    monitor_ready = 1;
    pthread_mutex_unlock(&mutex_start);
    pthread_cond_signal(&condp);
    while (global_counter < 400000000)
    {
        increase_counter();
    }
}
